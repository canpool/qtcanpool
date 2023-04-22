/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbontabbar.h"
#include "ribbon_def.h"
#include "ribbonbackstageview.h"
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonbutton_p.h"
#include "ribbonpage.h"
#include "ribbonquickaccessbar.h"
#include "ribbonstyle.h"
#include "ribbonsystempopupbar.h"
#include "stylehelpers.h"
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QWidgetAction>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

/* QRibbon::ContextHeader */
ContextHeader::ContextHeader(RibbonTab *tab)
{
    Q_ASSERT(tab);
    if (!tab)
        return;

    lastTab = firstTab = tab;
    color = firstTab->contextColor();
    strTitle = firstTab->contextTextTab();
    strGroupName = firstTab->contextGroupName();
    rcRect = QRect();
    firstTab->setContextHeader(this);
}
ContextHeader::~ContextHeader() { firstTab->setContextHeader(Q_NULL); }

QRIBBON_BEGIN_NAMESPACE

class RibbonTabPrivate : public QObject
{
public:
    QTC_DECLARE_PUBLIC(RibbonTab)
public:
    explicit RibbonTabPrivate();
public:
    void init();
    QSize sizeForWidth(int w) const;
    void updateLabel();
    QRect documentRect() const;
    QRectF layoutRect() const;
public:
    mutable bool m_valid_hints;
    mutable QSize m_sh;
    mutable QSizePolicy m_sizePolicy;
    mutable QSize m_msh;

    bool m_select;
    bool m_tracking;
    bool m_minimizeDblClick;
    int m_margin;
    int m_width;
    Qt::Alignment m_align;
    short m_indent;
    short m_frameWidth;
    QString m_text;
    QString m_contextText;
    QString m_contextGroupName;
    RibbonPage::ContextColor m_contextColor;
    ContextHeader *m_contextHeader;
    QAction *m_defaultAction;
    RibbonPage *m_page;

    int m_shortcutId;

    mutable uint m_isTextLabel : 1;
    mutable uint m_textLayoutDirty : 1;
    mutable uint m_textDirty : 1;
    mutable uint m_tabMouseOver : 1;
};

class RibbonTabBarPrivate : public QObject
{
public:
    QTC_DECLARE_PUBLIC(RibbonTabBar)
public:
    explicit RibbonTabBarPrivate();
public:
    void layoutWidgets();
    bool compressTabs();
    bool validIndex(int index) const { return index >= 0 && index < m_tabList.count(); }
    bool validWidth() const;

    RibbonBar *ribbonBar() const;
public:
    QList<RibbonTab *> m_tabList;
    QList<QWidget *> m_controls;
    QList<QWidget *> m_listEnabledWidgets;
    QAction *m_actionSystemPopupBar;
    bool m_validRect;
    int m_currentIndex;
    int m_margin;
    RibbonTabBar::SelectionBehavior selectionBehaviorOnRemove;
};

QRIBBON_END_NAMESPACE

RibbonTabPrivate::RibbonTabPrivate() : m_width(-1), m_page(Q_NULL), m_shortcutId(0) {}

void RibbonTabPrivate::init()
{
    QTC_Q(RibbonTab);
    m_contextColor = RibbonPage::ContextColorNone;
    m_contextHeader = Q_NULL;
    m_valid_hints = false;
    m_sh = QSize(0, 0);
    m_margin = 0;
    m_isTextLabel = false;
    m_textDirty = false;
    m_tabMouseOver = false;
    m_select = false;
    m_tracking = false;
    m_minimizeDblClick = false;
    m_shortcutId = -1;
    m_align = Qt::AlignLeft | Qt::AlignVCenter /*| Qt::TextExpandTabs*/;
    m_indent = -1;
    m_frameWidth = 1;
    m_defaultAction = new QWidgetAction(q);
    q->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::Label));
}

QSize RibbonTabPrivate::sizeForWidth(int w) const
{
    QTC_Q(const RibbonTab);
    if (q->minimumWidth() > 0)
        w = qMax(w, q->minimumWidth());

    QSize contentsMargin(2, 0);

    QRect br;

    int hextra = 2 * m_margin;
    int vextra = hextra;
    QFontMetrics fm = q->fontMetrics();

    QString text = m_text;

    int countText = text.count();
    int countContextText = m_contextText.count();

    if (countText < countContextText)
        text = m_contextText;

    bool itemUpperCase = (bool)q->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonItemUpperCase, 0, q);
    if (!text.isEmpty()) {
        if (itemUpperCase)
            text = text.toUpper();

        int m_align = QStyle::visualAlignment(q->layoutDirection(), QFlag(this->m_align));
        // Add indentation
        int m = m_indent;

        if (m < 0 && m_frameWidth)   // no m_indent, but we do have a frame
        {
            if (itemUpperCase)
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                m = fm.width(QLatin1Char('X')) - m_margin * 2;
            else
                m = fm.width(QLatin1Char('x')) - m_margin * 2;
#else
                m = fm.horizontalAdvance(QLatin1Char('X')) - m_margin * 2;
            else
                m = fm.horizontalAdvance(QLatin1Char('x')) - m_margin * 2;
#endif
        }
        if (m > 0) {
            if ((m_align & Qt::AlignLeft) || (m_align & Qt::AlignRight))
                hextra += m;
            if ((m_align & Qt::AlignTop) || (m_align & Qt::AlignBottom))
                vextra += m;
        }

        // Turn off center alignment in order to avoid rounding errors for centering,
        // since centering involves a division by 2. At the end, all we want is the size.
        int flags = m_align & ~(Qt::AlignVCenter | Qt::AlignHCenter);

        bool tryWidth = (w < 0) && (m_align & Qt::TextWordWrap);
        if (tryWidth)
            w = fm.averageCharWidth() * 80;
        else if (w < 0)
            w = 2000;
        w -= (hextra + contentsMargin.width());
        br = fm.boundingRect(0, 0, w, 2000, flags, text);
        if (tryWidth && br.height() < 4 * fm.lineSpacing() && br.width() > w / 2)
            br = fm.boundingRect(0, 0, w / 2, 2000, flags, text);
        if (tryWidth && br.height() < 2 * fm.lineSpacing() && br.width() > w / 4)
            br = fm.boundingRect(0, 0, w / 4, 2000, flags, text);
    } else
        br = QRect(QPoint(0, 0), QSize(fm.averageCharWidth(), fm.lineSpacing()));

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    int hframe = itemUpperCase ? fm.width(QLatin1Char('X')) * 2 : fm.width(QLatin1Char('x')) * 2;
#else
    int hframe = itemUpperCase ? fm.horizontalAdvance(QLatin1Char('X')) * 2
                               : fm.horizontalAdvance(QLatin1Char('x')) * 2;
#endif

    const QSize contentsSize(br.width() + hextra + hframe, br.height() + vextra);
    return (contentsSize + contentsMargin).expandedTo(q->minimumSize());
}

void RibbonTabPrivate::updateLabel()
{
    QTC_Q(RibbonTab);
    m_valid_hints = false;

    if (m_isTextLabel) {
        QSizePolicy policy = q->sizePolicy();
        const bool wrap = m_align & Qt::TextWordWrap;
        policy.setHeightForWidth(wrap);
        // should be replaced by WA_WState_OwnSizePolicy idiom
        if (policy != q->sizePolicy())
            q->setSizePolicy(policy);
        m_textLayoutDirty = true;
    }
    q->updateGeometry();
    q->update(q->contentsRect());
}

/*
 * Returns the rect that is available for us to draw the document
 */
QRect RibbonTabPrivate::documentRect() const
{
    QTC_Q(const RibbonTab);
    Q_ASSERT_X(m_isTextLabel, "documentRect", "document rect called for tab that is not a m_text tab!");
    QRect cr = q->contentsRect();
    cr.setLeft(cr.left());
    cr.adjust(m_margin, m_margin, -m_margin, -m_margin);
    const int m_align = QStyle::visualAlignment(q->layoutDirection(), QFlag(this->m_align));
    int m = m_indent;
    if (m < 0 && m_frameWidth)   // no m_indent, but we do have a frame
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        m = q->fontMetrics().width(QLatin1Char('x')) / 2 - m_margin;
#else
        m = q->fontMetrics().horizontalAdvance(QLatin1Char('x')) / 2 - m_margin;
#endif
    if (m > 0) {
        if (m_align & Qt::AlignLeft)
            cr.setLeft(cr.left() + m);
        if (m_align & Qt::AlignRight)
            cr.setRight(cr.right() - m);
        if (m_align & Qt::AlignTop)
            cr.setTop(cr.top() + m);
        if (m_align & Qt::AlignBottom)
            cr.setBottom(cr.bottom() - m);
    }
    return cr;
}

QRectF RibbonTabPrivate::layoutRect() const { return documentRect(); }

/* RibbonTab */
RibbonTab::RibbonTab(const QString &text, QWidget *parent) : QWidget(parent), lastTab(-1)
{
    QTC_INIT_PRIVATE(RibbonTab);
    QTC_D(RibbonTab);

    d->init();
    setTextTab(text);
    setMargin(4);
    setAlignment(Qt::AlignCenter);
}

RibbonTab::~RibbonTab()
{
    QTC_D(RibbonTab);
    QWidget *pParent = parentWidget();
    pParent->releaseShortcut(d->m_shortcutId);

    if (d->m_page)
        d->m_page->setAssociativeTab(Q_NULL);
    QTC_FINI_PRIVATE();
}

void RibbonTab::setPage(RibbonPage *page)
{
    QTC_D(RibbonTab);
    d->m_page = page;
}

void RibbonTab::setSelect(bool select)
{
    QTC_D(RibbonTab);
    d->m_select = select;
    d->updateLabel();
}

bool RibbonTab::select() const
{
    QTC_D(const RibbonTab);
    return d->m_select;
}

void RibbonTab::setIndent(int indent)
{
    QTC_D(RibbonTab);
    d->m_indent = indent;
    d->updateLabel();
}

int RibbonTab::indent() const
{
    QTC_D(const RibbonTab);
    return d->m_indent;
}

int RibbonTab::margin() const
{
    QTC_D(const RibbonTab);
    return d->m_margin;
}

void RibbonTab::setMargin(int margin)
{
    QTC_D(RibbonTab);
    if (d->m_margin == margin)
        return;
    d->m_margin = margin;
    d->updateLabel();
}

Qt::Alignment RibbonTab::alignment() const
{
    QTC_D(const RibbonTab);
    return QFlag(d->m_align & (Qt::AlignVertical_Mask | Qt::AlignHorizontal_Mask));
}

void RibbonTab::setAlignment(Qt::Alignment alignment)
{
    QTC_D(RibbonTab);
    if (alignment == (d->m_align & (Qt::AlignVertical_Mask | Qt::AlignHorizontal_Mask)))
        return;
    d->m_align = (d->m_align & ~(Qt::AlignVertical_Mask | Qt::AlignHorizontal_Mask)) |
                 (alignment & (Qt::AlignVertical_Mask | Qt::AlignHorizontal_Mask));

    d->updateLabel();
}

void RibbonTab::setTextTab(const QString &text)
{
    QTC_D(RibbonTab);
    if (d->m_text == text)
        return;
    d->m_text = text;
    d->m_isTextLabel = true;
    d->m_textDirty = true;
    d->updateLabel();

    RibbonTabBar *pParent = qobject_cast<RibbonTabBar *>(parentWidget());
    Q_ASSERT(pParent != NULL);
    pParent->releaseShortcut(d->m_shortcutId);
    d->m_shortcutId = pParent->grabShortcut(QKeySequence::mnemonic(text));
    pParent->setShortcutEnabled(d->m_shortcutId, isEnabled());
    pParent->layoutWidgets();
}

void RibbonTab::setContextTextTab(const QString &contextText)
{
    QTC_D(RibbonTab);
    d->m_contextText = contextText;
}

const QString &RibbonTab::contextTextTab() const
{
    QTC_D(const RibbonTab);
    return d->m_contextText;
}

void RibbonTab::setContextGroupName(const QString &groupName)
{
    QTC_D(RibbonTab);
    d->m_contextGroupName = groupName;
}

const QString &RibbonTab::contextGroupName() const
{
    QTC_D(const RibbonTab);
    return d->m_contextGroupName;
}

void RibbonTab::setContextTab(RibbonPage::ContextColor color)
{
    QTC_D(RibbonTab);
    d->m_contextColor = color;
    if (d->m_contextHeader)
        d->m_contextHeader->color = color;
}

RibbonPage::ContextColor RibbonTab::contextColor() const
{
    QTC_D(const RibbonTab);
    return d->m_contextColor;
}

const QString &RibbonTab::textTab() const
{
    QTC_D(const RibbonTab);
    return d->m_text;
}

void RibbonTab::setContextHeader(ContextHeader *contextHeader)
{
    QTC_D(RibbonTab);
    d->m_contextHeader = contextHeader;
}

ContextHeader *RibbonTab::getContextHeader() const
{
    QTC_D(const RibbonTab);
    return d->m_contextHeader;
}

void RibbonTab::setTrackingMode(bool tracking)
{
    QTC_D(RibbonTab);
    d->m_tracking = tracking;
    update();
}

bool RibbonTab::isTrackingMode() const
{
    QTC_D(const RibbonTab);
    return d->m_tracking;
}

void RibbonTab::setTabWidth(int width)
{
    QTC_D(RibbonTab);
    if (width == d->m_width)
        return;
    d->m_width = width;
}

int RibbonTab::tabWidth() const
{
    QTC_D(const RibbonTab);
    return d->m_width;
}

QAction *RibbonTab::defaultAction() const
{
    QTC_D(const RibbonTab);
    return d->m_defaultAction;
}

int RibbonTab::shortcut() const
{
    QTC_D(const RibbonTab);
    return d->m_shortcutId;
}

bool RibbonTab::validRect() const
{
    QTC_D(const RibbonTab);
    int wid = 0;
    StyleRibbonOptionHeader opt;
    opt.init(this);
    opt.contextText = contextTextTab();
    int clientWidth = opt.rect.width();
    if (!d->m_text.isEmpty()) {
        QString text = d->m_text;
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        wid = opt.fontMetrics.width(text.remove(QChar('&')));
#else
        wid = opt.fontMetrics.horizontalAdvance(text.remove(QChar('&')));
#endif
    } else
        wid = 10;

    return clientWidth >= wid;
}

void RibbonTab::paintEvent(QPaintEvent *event)
{
    QTC_D(RibbonTab);
    QPainter p(this);
    StyleRibbonOptionHeader opt;
    opt.init(this);
    opt.text = p.fontMetrics().elidedText(d->m_text, Qt::ElideRight, opt.rect.adjusted(2, 0, -2, 0).width());
    opt.contextText = contextTextTab();
    opt.textAlignment = d->m_align;

    bool minimized = false;
    bool backstageVisible = false;
    if (RibbonBar *rb = qobject_cast<RibbonBar *>(parentWidget()->parentWidget())) {
        opt.tabBarPosition = rb->tabBarPosition();
        minimized = rb->isMinimized();
        backstageVisible = rb->isBackstageVisible();
    }

    int w1 = d->m_sh.width();
    int w2 = opt.rect.width();
    if (w1 != w2)
        opt.state |= QStyle::State_Small;
    else
        opt.state &= ~QStyle::State_Small;

    if (event->rect().intersects(opt.rect)) {
        if (!minimized && !backstageVisible) {
            if (d->m_select)
                opt.state |= QStyle::State_Selected;
            else
                opt.state &= ~QStyle::State_Selected;
        } else {
            if (d->m_tracking) {
                opt.state |= QStyle::State_Selected;
                opt.state |= QStyle::State_Sunken;
            } else {
                opt.state &= ~QStyle::State_Selected;
                opt.state &= ~QStyle::State_Sunken;
            }
        }

        style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonTab, &opt, &p, this);
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonTabShapeLabel, &opt, &p, this);
    }
}

QSize RibbonTab::sizeHint() const
{
    QTC_D(const RibbonTab);
    if (!d->m_valid_hints)
        (void)RibbonTab::minimumSizeHint();

    QSize size = d->m_sh;
    if (d->m_width != -1)
        size.setWidth(d->m_width);

    return size;
}

QSize RibbonTab::minimumSizeHint() const
{
    QTC_D(const RibbonTab);
    if (d->m_valid_hints && d->m_sizePolicy == sizePolicy())
        return d->m_msh;

    ensurePolished();
    d->m_valid_hints = true;
    // wrap ? golden ratio : min doc size
    d->m_sh = d->sizeForWidth(-1);
    QSize m_msh(-1, -1);

    if (!d->m_isTextLabel)
        m_msh = d->m_sh;
    else {
        // height for one line
        m_msh.rheight() = d->sizeForWidth(QWIDGETSIZE_MAX).height();
        // wrap ? size of biggest word : min doc size
        m_msh.rwidth() = d->sizeForWidth(0).width();
        if (d->m_sh.height() < m_msh.height())
            m_msh.rheight() = d->m_sh.height();
    }

    int nHeightTabs = 31;
    if (RibbonBar *rb = qobject_cast<RibbonBar *>(parentWidget()->parentWidget()))
        nHeightTabs = rb->qtc_d()->heightTabs();

    d->m_sh.setHeight(nHeightTabs - 1);
    d->m_msh = m_msh;
    d->m_sizePolicy = sizePolicy();
    return m_msh;
}

void RibbonTab::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    QTC_D(RibbonTab);
    d->m_tabMouseOver = true;
    update();
}

void RibbonTab::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    QTC_D(RibbonTab);
    d->m_tabMouseOver = false;
    update();
}

void RibbonTab::mousePressEvent(QMouseEvent *event)
{
    QTC_D(RibbonTab);
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (RibbonBar *rb = qobject_cast<RibbonBar *>(parentWidget()->parentWidget())) {
        if (rb->isBackstageVisible()) {
            if (RibbonSystemButton *button = rb->getSystemButton()) {
                if (RibbonBackstageView *backstage = qobject_cast<RibbonBackstageView *>(button->backstage()))
                    backstage->close();
            }
        }
    }

    bool minimized = false;
    if (RibbonBar *rb = qobject_cast<RibbonBar *>(parentWidget()->parentWidget()))
        minimized = rb->isMinimized();

    d->m_minimizeDblClick = d->m_select;

    if (d->m_minimizeDblClick && minimized && !d->m_tracking)
        d->m_minimizeDblClick = false;

    if (d->m_minimizeDblClick) {
        event->ignore();
        return;
    }

    emit pressTab(this);

    update();
    event->ignore();
}

void RibbonTab::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
        return;
    update();
}

void RibbonTab::mouseDoubleClickEvent(QMouseEvent *event)
{
    QTC_D(RibbonTab);

    if (event->button() != Qt::LeftButton) {
        event->setAccepted(false);
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    // When the ribbon is minimized, it supports double-clicking tab to maximize ribbon
    if (d->m_select/* && d->m_minimizeDblClick*/) {
        d->m_minimizeDblClick = true;
        event->setAccepted(false);
        QWidget::mouseDoubleClickEvent(event);
    }
}

/* RibbonTabBarPrivate */
RibbonTabBarPrivate::RibbonTabBarPrivate()
    : m_actionSystemPopupBar(Q_NULL), m_validRect(true), m_currentIndex(-1), m_margin(1),
      selectionBehaviorOnRemove(RibbonTabBar::SelectRightTab)
{
}

void RibbonTabBarPrivate::layoutWidgets()
{
    QTC_Q(RibbonTabBar);
    RibbonBar *ribBar = ribbonBar();
    Q_ASSERT(ribBar != NULL);

    if (q->geometry().isEmpty())
        return;

    int leftSide = 0;
    int rightSide = 0;

    // set the position of the system button
    int nOffset = 0;
    if (QMenuBar *menuBar = qobject_cast<QMenuBar *>(q->parentWidget())) {
        if (QWidget *rightCorner = menuBar->cornerWidget(Qt::TopRightCorner)) {
            nOffset = rightCorner->size().width();
            nOffset += 1;
            rightSide = rightCorner->geometry().left();
        }
    }
    // set the position of controls right side
    QSize szSumControls;
    QRect rect = q->rect();
    for (int i = 0, count = m_controls.size(); count > i; ++i) {
        QWidget *widget = m_controls.at(i);
        if (widget->isVisible()) {
            QSize szControl = widget->sizeHint();
            int top = ribBar->tabBarPosition() == RibbonBar::TabBarBottomPosition ? 2 : 0;
            widget->setGeometry(QRect(QPoint(rect.right() - szControl.width() - szSumControls.width() - nOffset, top),
                                      QSize(widget->sizeHint().width(), q->sizeHint().height() - m_margin)));
            szSumControls += widget->sizeHint();
        }
    }

    rightSide = m_controls.size() > 0 ? m_controls.at(m_controls.size() - 1)->geometry().left()
                : rightSide == 0      ? q->geometry().right() - m_margin * 8
                                      : rightSide;

    int sumWidth = 0;
    int sumCount = 0;
    QList<QRect> rectTabs;
    for (int i = 0, count = m_tabList.count(); count > i; i++) {
        RibbonTab *tab = m_tabList.at(i);
        tab->qtc_d()->m_valid_hints = false;
        QRect rect = !tab->isHidden() ? QRect(QPoint(sumWidth + m_margin, 0), tab->sizeHint()) : QRect();
        if (!rect.isEmpty())
            sumCount++;

        sumWidth += rect.width();
        rectTabs << rect;
    }

    int newWidth = 0;
    if (!q->geometry().isEmpty() && sumCount > 0) {
        int clientWidth = rightSide - leftSide;
        if (clientWidth < sumWidth)
            newWidth = (sumWidth - clientWidth) / sumCount;
    }

    bool validRect = true;
    // calculate the width of the tabs
    sumWidth = 0;
    for (int i = 0, count = m_tabList.count(); count > i; i++) {
        RibbonTab *tab = m_tabList.at(i);

        QRect rectItem = rectTabs.at(i);
        if (rectItem.isEmpty())
            continue;

        if (newWidth != 0)
            rectItem.setWidth(rectItem.width() - newWidth);

        tab->move(sumWidth + m_margin, rectItem.y());
        tab->resize(rectItem.width(), rectItem.height());
        sumWidth += rectItem.width();

        if (!tab->isHidden() && !tab->validRect())
            validRect = false;
    }
    m_validRect = validRect;
}

bool RibbonTabBarPrivate::validWidth() const { return m_validRect; }

RibbonBar *RibbonTabBarPrivate::ribbonBar() const
{
    QTC_Q(const RibbonTabBar);
    return qobject_cast<RibbonBar *>(q->parentWidget());
}

/* RibbonTabBar */
RibbonTabBar::RibbonTabBar(QWidget *pParent) : QWidget(pParent) { QTC_INIT_PRIVATE(RibbonTabBar); }

RibbonTabBar::~RibbonTabBar() { QTC_FINI_PRIVATE(); }

int RibbonTabBar::addTab(const QString &text) { return insertTab(-1, text); }

int RibbonTabBar::addTab(const QIcon &icon, const QString &text) { return insertTab(-1, icon, text); }

int RibbonTabBar::insertTab(int index, const QString &text) { return insertTab(index, QIcon(), text); }

int RibbonTabBar::insertTab(int index, const QIcon &icon, const QString &text)
{
    Q_UNUSED(icon);
    QTC_D(RibbonTabBar);

    QString strName = text;

    if (strName.isEmpty())
        strName = RibbonBar::tr_compatible(RibbonUntitledString);

    RibbonTab *tabLabel = new RibbonTab(strName, this);
    connect(tabLabel, SIGNAL(pressTab(QWidget *)), this, SLOT(activateTab(QWidget *)));

    if (!d->validIndex(index)) {
        index = d->m_tabList.count();
        d->m_tabList.append(tabLabel);
    } else
        d->m_tabList.insert(index, tabLabel);

    if (d->m_tabList.count() == 1)
        setCurrentIndex(index);
    else if (index <= d->m_currentIndex)
        ++d->m_currentIndex;

    for (int i = 0; i < d->m_tabList.count(); ++i) {
        if (d->m_tabList[i]->lastTab >= index)
            ++d->m_tabList[i]->lastTab;
    }

    bool setSelect = true;
    if (RibbonBar *rb = d->ribbonBar())
        setSelect = !rb->isMinimized();

    if (setSelect) {
        for (int i = 0; i < d->m_tabList.count(); i++)
            d->m_tabList.at(i)->setSelect(d->m_currentIndex == i);
    }

    if (isVisible())
        tabLabel->show();

    d->layoutWidgets();
    return index;
}

void RibbonTabBar::moveTab(int index, int newIndex)
{
    QTC_D(RibbonTabBar);
    if (d->validIndex(index) && d->validIndex(newIndex)) {
        // save pointer active tab
        RibbonTab *saveTab = d->m_tabList[d->m_currentIndex];
        // Moves the tab in the position index to the position newIndex
        d->m_tabList.move(index, newIndex);
        // set an index of active tab
        int ind = d->m_tabList.indexOf(saveTab);
        if (d->validIndex(ind) && ind != d->m_currentIndex)
            d->m_currentIndex = ind;
        // reposition tabs
        d->layoutWidgets();
    }
}

void RibbonTabBar::removeTab(int index)
{
    QTC_D(RibbonTabBar);
    if (d->validIndex(index)) {
        int newIndex = d->m_tabList[index]->lastTab;
        RibbonTab *tab = d->m_tabList[index];
        d->m_tabList.removeAt(index);
        delete tab;

        for (int i = 0; i < d->m_tabList.count(); ++i) {
            if (d->m_tabList[i]->lastTab == index)
                d->m_tabList[i]->lastTab = -1;
            if (d->m_tabList[i]->lastTab > index)
                --d->m_tabList[i]->lastTab;
        }
        if (index == d->m_currentIndex) {
            // The current tab is going away, in order to make sure
            // we emit that "current has changed", we need to reset this
            // around.
            d->m_currentIndex = -1;
            if (d->m_tabList.size() > 0) {
                switch (d->selectionBehaviorOnRemove) {
                case SelectPreviousTab:
                    if (newIndex > index)
                        newIndex--;
                    if (d->validIndex(newIndex))
                        break;
                    // else fallthrough
                case SelectRightTab:
                    newIndex = index;
                    if (newIndex >= d->m_tabList.size())
                        newIndex = d->m_tabList.size() - 1;
                    break;
                case SelectLeftTab:
                    newIndex = index - 1;
                    if (newIndex < 0)
                        newIndex = 0;
                    break;
                default:
                    break;
                }

                if (d->validIndex(newIndex)) {
                    // don't loose newIndex's old through setCurrentIndex
                    int bump = d->m_tabList[newIndex]->lastTab;
                    if (d->ribbonBar() && !d->ribbonBar()->isMinimized())
                        setCurrentIndex(newIndex);
                    d->m_tabList[newIndex]->lastTab = bump;
                }
            } else {
                // emit currentChanged(-1);
            }
        } else if (index < d->m_currentIndex) {
            if (d->ribbonBar() && !d->ribbonBar()->isMinimized())
                setCurrentIndex(d->m_currentIndex - 1);
        }
    }
}

int RibbonTabBar::currentIndex() const
{
    QTC_D(const RibbonTabBar);
    if (d->validIndex(d->m_currentIndex))
        return d->m_currentIndex;
    return -1;
}

RibbonTab *RibbonTabBar::getTab(int nIndex) const
{
    QTC_D(const RibbonTabBar);
    if (d->validIndex(nIndex))
        return d->m_tabList[nIndex];
    return Q_NULL;
}

int RibbonTabBar::getTabCount() const
{
    QTC_D(const RibbonTabBar);
    return d->m_tabList.count();
}

int RibbonTabBar::getIndex(const RibbonTab *tab) const
{
    for (int i = 0, count = getTabCount(); count > i; i++) {
        if (getTab(i) == tab)
            return i;
    }
    return -1;
}

RibbonTabBar::SelectionBehavior RibbonTabBar::selectionBehaviorOnRemove() const
{
    QTC_D(const RibbonTabBar);
    return d->selectionBehaviorOnRemove;
}

void RibbonTabBar::setSelectionBehaviorOnRemove(RibbonTabBar::SelectionBehavior behavior)
{
    QTC_D(RibbonTabBar);
    d->selectionBehaviorOnRemove = behavior;
}

void RibbonTabBar::setEnabledWidgets(bool enabled)
{
    QTC_D(RibbonTabBar);
    if (enabled) {
        for (int i = 0, count = d->m_listEnabledWidgets.size(); count > i; i++) {
            QWidget *widget = d->m_listEnabledWidgets.at(i);
            widget->setEnabled(true);
        }
        d->m_listEnabledWidgets.clear();
    } else {
        d->m_listEnabledWidgets.clear();
        for (int i = 0, count = d->m_controls.size(); count > i; i++) {
            QWidget *widget = d->m_controls.at(i);
            if (widget->isEnabled() /*&& widget->objectName() == QString("No")*/) {
                d->m_listEnabledWidgets << widget;
                widget->setEnabled(false);
            }
        }
    }
}

QAction *RibbonTabBar::addAction(const QIcon &icon, const QString &text, Qt::ToolButtonStyle style, QMenu *menu)
{
    QAction *action = new QAction(icon, text, this);
    if (menu)
        action->setMenu(menu);

    return addAction(action, style);
}

QAction *RibbonTabBar::addAction(QAction *action, Qt::ToolButtonStyle style)
{
    QTC_D(RibbonTabBar);
    if (Qt::ToolButtonTextUnderIcon == style) {
        Q_ASSERT(false);
        style = Qt::ToolButtonTextBesideIcon;
    }
    QWidget::addAction(action);
    RibbonButton *button = new RibbonButton(this);

    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolButtonStyle(style);

    QIcon icon = action->icon();
    if (!icon.isNull() && icon.availableSizes().count() > 0)
        button->setIconSize(icon.availableSizes().first());

    action->setIconText(action->text());
    button->setDefaultAction(action);
    d->m_controls.append(button);

    button->setVisible(true);
    layoutWidgets();
    return action;
}

QMenu *RibbonTabBar::addMenu(const QString &title)
{
    QTC_D(RibbonTabBar);
    QMenu *menu = new QMenu(title, this);

    RibbonButton *button = new RibbonButton(this);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);

    button->setDefaultAction(menu->menuAction());
    d->m_controls.append(button);

    button->setVisible(true);
    layoutWidgets();
    return menu;
}

void RibbonTabBar::setCurrentIndex(int index)
{
    QTC_D(RibbonTabBar);

    bool allowSetCurrentIndex = false;
    if (RibbonBar *rb = d->ribbonBar())
        allowSetCurrentIndex = rb->isMinimized();

    if (d->validIndex(index) && (d->m_currentIndex != index || allowSetCurrentIndex)) {
        d->m_currentIndex = index;

        emit currentChanged(index);

        for (int i = 0; i < d->m_tabList.count(); i++)
            d->m_tabList.at(i)->setSelect(index == i);
        update();
    }
}

void RibbonTabBar::currentNextTab(bool next)
{
    QTC_D(RibbonTabBar);

    int nextIndex = -1;
    if (next) {
        for (int i = currentIndex() + 1; i < d->m_tabList.count(); i++) {
            if (d->m_tabList.at(i)->isVisible()) {
                nextIndex = i;
                break;
            }
        }
    } else {
        for (int i = currentIndex() - 1; i >= 0; --i) {
            if (d->m_tabList.at(i)->isVisible()) {
                nextIndex = i;
                break;
            }
        }
    }
    if (d->validIndex(nextIndex))
        setCurrentIndex(nextIndex);
}

void RibbonTabBar::layoutWidgets()
{
    QTC_D(RibbonTabBar);
    d->layoutWidgets();
}

void RibbonTabBar::refresh()
{
    QTC_D(RibbonTabBar);
    for (int i = 0; i < d->m_tabList.count(); i++)
        d->m_tabList.at(i)->qtc_d()->updateLabel();
}

bool RibbonTabBar::validWidth() const
{
    QTC_D(const RibbonTabBar);
    return d->validWidth();
}

bool RibbonTabBar::event(QEvent *event)
{
    QTC_D(RibbonTabBar);
    switch (event->type()) {
    case QEvent::Shortcut:
        if (QShortcutEvent *se = static_cast<QShortcutEvent *>(event)) {
            for (int i = 0; i < d->m_tabList.count(); ++i) {
                const RibbonTab *tab = d->m_tabList.at(i);
                if (tab->shortcut() == se->shortcutId()) {
                    setCurrentIndex(i);
                    return true;
                }
            }
        }
        break;
    case QEvent::Resize:
        layoutWidgets();
        break;
    case QEvent::ActionRemoved:
        if (QActionEvent *actionEvent = static_cast<QActionEvent *>(event)) {
            for (int i = d->m_controls.count() - 1; i >= 0; --i) {
                if (QToolButton *widget = qobject_cast<QToolButton *>(d->m_controls.at(i))) {
                    if (actionEvent->action() == widget->defaultAction()) {
                        d->m_controls.removeAt(i);
                        delete widget;
                    }
                }
            }
            layoutWidgets();
        }
        break;
    case QEvent::ActionChanged:
        if (QActionEvent *actionEvent = static_cast<QActionEvent *>(event)) {
            for (int i = 0, count = d->m_controls.size(); count > i; i++) {
                QWidget *widget = d->m_controls.at(i);
                if (QToolButton *button = qobject_cast<QToolButton *>(widget)) {
                    QAction *action = actionEvent->action();
                    if (action == button->defaultAction()) {
                        widget->setVisible(action->isVisible());
                        widget->setEnabled(action->isEnabled());
                    }
                }
            }
            layoutWidgets();
        }
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void RibbonTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonTabBar, &opt, &p, this);
}

void RibbonTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    event->setAccepted(false);
    QWidget::mouseDoubleClickEvent(event);
}

QSize RibbonTabBar::sizeHint() const
{
    QTC_D(const RibbonTabBar);
    if (!d->ribbonBar())
        return QWidget::sizeHint();

    int heightTabs = d->ribbonBar()->qtc_d()->heightTabs();
    return QSize(0, heightTabs - 2).expandedTo(QApplication::globalStrut());
}

void RibbonTabBar::activateTab(QWidget *widget)
{
    QTC_D(RibbonTabBar);

    for (int i = 0; i < d->m_tabList.count(); ++i) {
        if (d->m_tabList.at(i) == widget) {
            bool allow = true;
            if (RibbonBar *ribbonBar = d->ribbonBar()) {
                if (RibbonPage *page = ribbonBar->getPage(i))
                    page->activatingPage(allow);
            }

            if (allow)
                setCurrentIndex(i);
            return;
        }
    }
}
