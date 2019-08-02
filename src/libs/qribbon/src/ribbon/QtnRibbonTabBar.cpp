/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QApplication>
#include <QPainter>
#include <QWidgetAction>
#include <QStyleOption>
#include <qevent.h>

#include "QtnRibbonBar.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonButton.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonBackstageView.h"

#include "QtnRibbonQuickAccessBar.h"
#include "QtnRibbonSystemPopupBar.h"

QTITAN_USE_NAMESPACE


/*!
\class Qtitan::ContextHeader
\internal
*/
ContextHeader::ContextHeader(RibbonTab* tab)
{
    Q_ASSERT(tab);
    if (!tab)
        return;

    lastTab = firstTab = tab;
    color = firstTab->getContextColor();
    strTitle = firstTab->contextTextTab();
    rcRect = QRect();
    firstTab->setContextHeader(this);
}
ContextHeader::~ContextHeader()
{
    firstTab->setContextHeader(Q_NULL);
}

QTITAN_BEGIN_NAMESPACE

/* RibbonTabPrivate */
class RibbonTabPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonTab)

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
    Qt::Alignment m_align;
    short m_indent;
    short m_frameWidth;
    QString m_text;
    QString m_contextText;
    ContextColor m_contextColor;
    ContextHeader* m_contextHeader;
    QAction* m_defaultAction;

    int m_shortcutId;

    mutable uint m_isTextLabel     : 1;
    mutable uint m_textLayoutDirty : 1;
    mutable uint m_textDirty       : 1;
    mutable uint m_tabMouseOver    : 1;
};
QTITAN_END_NAMESPACE

RibbonTabPrivate::RibbonTabPrivate()
    : m_shortcutId(0)
{
}

void RibbonTabPrivate::init()
{
    QTN_P(RibbonTab);
    m_contextColor = ContextColorNone;
    m_contextHeader = Q_NULL;
    m_valid_hints   = false;
    m_margin        = 0;
    m_isTextLabel   = false;
    m_textDirty     = false;
    m_tabMouseOver  = false;
    m_select        = false;
    m_tracking      = false;
    m_minimizeDblClick = false;
    m_shortcutId = -1;
    m_align = Qt::AlignLeft | Qt::AlignVCenter /*| Qt::TextExpandTabs*/;
    m_indent = -1;
    m_defaultAction = new QWidgetAction(&p);
    p.setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred, QSizePolicy::Label));
}

QSize RibbonTabPrivate::sizeForWidth(int w) const
{
    QTN_P(const RibbonTab);
    if(p.minimumWidth() > 0)
        w = qMax(w, p.minimumWidth());

    QSize contentsMargin(2,0);

    QRect br;

    int hextra = 2 * m_margin;
    int vextra = hextra;
    QFontMetrics fm = p.fontMetrics();

    QString text = m_text;

    int countText = text.count();
    int countContextText = m_contextText.count();

    if (countText < countContextText)
        text = m_contextText;

    if (!text.isEmpty()) 
    {
        int m_align = QStyle::visualAlignment(p.layoutDirection(), QFlag(this->m_align));
        // Add indentation
        int m = m_indent;

        if (m < 0 && m_frameWidth) // no m_indent, but we do have a frame
            m = fm.width(QLatin1Char('x')) - m_margin*2;
        if (m > 0) 
        {
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
        br = fm.boundingRect(0, 0, w ,2000, flags, text);
        if (tryWidth && br.height() < 4*fm.lineSpacing() && br.width() > w/2)
            br = fm.boundingRect(0, 0, w/2, 2000, flags, text);
        if (tryWidth && br.height() < 2*fm.lineSpacing() && br.width() > w/4)
            br = fm.boundingRect(0, 0, w/4, 2000, flags, text);
    } else
        br = QRect(QPoint(0, 0), QSize(fm.averageCharWidth(), fm.lineSpacing()));

    const QSize contentsSize(br.width() + hextra+12, br.height() + vextra);
    return (contentsSize + contentsMargin).expandedTo(p.minimumSize());
}

void RibbonTabPrivate::updateLabel()
{
    QTN_P(RibbonTab);
    m_valid_hints = false;

    if (m_isTextLabel)
    {
        QSizePolicy policy = p.sizePolicy();
        const bool wrap = m_align & Qt::TextWordWrap;
        policy.setHeightForWidth(wrap);
        // should be replaced by WA_WState_OwnSizePolicy idiom
        if (policy != p.sizePolicy())  
            p.setSizePolicy(policy);
        m_textLayoutDirty = true;
    }
    p.updateGeometry();
    p.update(p.contentsRect());
}

/*
 * Returns the rect that is available for us to draw the document
 */
QRect RibbonTabPrivate::documentRect() const
{
    QTN_P(const RibbonTab);
    Q_ASSERT_X(m_isTextLabel, "documentRect", "document rect called for tab that is not a m_text tab!");
    QRect cr = p.contentsRect();
    cr.setLeft(cr.left());
    cr.adjust(m_margin, m_margin, -m_margin, -m_margin);
    const int m_align = QStyle::visualAlignment(p.layoutDirection(), QFlag(this->m_align));
    int m = m_indent;
    if (m < 0 && m_frameWidth ) // no m_indent, but we do have a frame
        m = p.fontMetrics().width(QLatin1Char('x')) / 2 - m_margin;
    if (m > 0) 
    {
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

QRectF RibbonTabPrivate::layoutRect() const
{
    return documentRect();
}


/* RibbonTab */
RibbonTab::RibbonTab(const QString& text, QWidget* parent)
    : QWidget(parent)
    , lastTab(-1)
{
    QTN_INIT_PRIVATE(RibbonTab);
    QTN_D(RibbonTab);

    d.init();
    setTextTab(text);
    setMargin(4);
    setAlignment(Qt::AlignCenter);
}

RibbonTab::~RibbonTab()
{
    QTN_D(RibbonTab);
    QWidget* pParent = parentWidget();
    pParent->releaseShortcut(d.m_shortcutId);
    QTN_FINI_PRIVATE();
}

void RibbonTab::setSelect(bool select)
{
    QTN_D(RibbonTab);
    d.m_select = select;
    d.updateLabel();
}

bool RibbonTab::select() const
{
    QTN_D(const RibbonTab);
    return d.m_select;
}

void RibbonTab::setIndent(int indent)
{
    QTN_D(RibbonTab);
    d.m_indent = indent;
    d.updateLabel();
}

int RibbonTab::indent() const
{
    QTN_D(const RibbonTab);
    return d.m_indent;
}

int RibbonTab::margin() const
{
    QTN_D(const RibbonTab);
    return d.m_margin;
}

void RibbonTab::setMargin(int margin)
{
    QTN_D(RibbonTab);
    if (d.m_margin == margin)
        return;
    d.m_margin = margin;
    d.updateLabel();
}

Qt::Alignment RibbonTab::alignment() const
{
    QTN_D(const RibbonTab);
    return QFlag(d.m_align & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask));
}

void RibbonTab::setAlignment(Qt::Alignment alignment)
{
    QTN_D(RibbonTab);
    if (alignment == (d.m_align & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask)))
        return;
    d.m_align = (d.m_align & ~(Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask)) | 
        (alignment & (Qt::AlignVertical_Mask|Qt::AlignHorizontal_Mask));

    d.updateLabel();
}

void RibbonTab::setTextTab(const QString& text)
{
    QTN_D(RibbonTab);
    if (d.m_text == text)
        return;
    d.m_text = text;
    d.m_isTextLabel = true;
    d.m_textDirty = true;
    d.updateLabel();

    QWidget* pParent = parentWidget();
    pParent->releaseShortcut(d.m_shortcutId);
    d.m_shortcutId = pParent->grabShortcut(QKeySequence::mnemonic(text));
    pParent->setShortcutEnabled(d.m_shortcutId, isEnabled());

    QApplication::postEvent(pParent, new QEvent(QEvent::LayoutRequest));
}

void RibbonTab::setContextTextTab(const QString& contextText)
{
    QTN_D(RibbonTab);
    d.m_contextText = contextText;
}

const QString& RibbonTab::contextTextTab() const
{
    QTN_D(const RibbonTab);
    return d.m_contextText;
}

void RibbonTab::setContextTab(ContextColor color)
{
    QTN_D(RibbonTab);
    d.m_contextColor = color;
    if (d.m_contextHeader)
        d.m_contextHeader->color = color;
}

ContextColor RibbonTab::getContextColor() const
{
    QTN_D(const RibbonTab);
    return d.m_contextColor;
}

const QString& RibbonTab::textTab() const
{
    QTN_D(const RibbonTab);
    return d.m_text;
}

void RibbonTab::setContextHeader(ContextHeader* contextHeader)
{
    QTN_D(RibbonTab);
    d.m_contextHeader = contextHeader;
}

ContextHeader* RibbonTab::getContextHeader() const
{
    QTN_D(const RibbonTab);
    return d.m_contextHeader;
}

void RibbonTab::setTrackingMode(bool tracking)
{
    QTN_D(RibbonTab);
    d.m_tracking = tracking;
    repaint();
}

bool RibbonTab::isTrackingMode() const
{
    QTN_D(const RibbonTab);
    return d.m_tracking;
}

QAction* RibbonTab::defaultAction() const
{
    QTN_D(const RibbonTab);
    return d.m_defaultAction;
}

int RibbonTab::shortcut() const
{
    QTN_D(const RibbonTab);
    return d.m_shortcutId;
}

bool RibbonTab::validRect() const
{
    QTN_D(const RibbonTab);
    int wid = 0;    
    QStyleOptionHeader opt;
    opt.init(this);
    int clientWidth = opt.rect.width();
    if (!d.m_text.isEmpty())
    {
        QString text = d.m_text;
        wid = opt.fontMetrics.width(text.remove(QChar('&')));
    }
    else
        wid = 10;

    return clientWidth >= wid;
}

bool RibbonTab::event(QEvent* event)
{
    return QWidget::event(event);
}

void RibbonTab::paintEvent(QPaintEvent* event)
{
    QTN_D(RibbonTab);
    QPainter p(this);

    QStyleOptionHeader opt;
    opt.init(this);
    opt.text = p.fontMetrics().elidedText(d.m_text, Qt::ElideRight, opt.rect.adjusted(2,0,-2,0).width());
    opt.textAlignment = d.m_align;

    if (event->rect().intersects(opt.rect))
    {
        bool minimized = false;
        bool backstageVisible = false;
        if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
        {
            minimized = rb->isMinimized();
            backstageVisible = rb->isBackstageVisible();
        }

        if (!minimized && !backstageVisible)
        {
            if (d.m_select)
                opt.state |= QStyle::State_Selected;
            else
                opt.state &= ~QStyle::State_Selected;
        }
        else
        {
            if (d.m_tracking)
            {
                opt.state |= QStyle::State_Selected;
                opt.state |= QStyle::State_Sunken;
            }
            else
            {
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
    QTN_D(const RibbonTab);
    if (!d.m_valid_hints)
        (void)RibbonTab::minimumSizeHint();
    return d.m_sh;
}

QSize RibbonTab::minimumSizeHint() const
{
    QTN_D(const RibbonTab);
    if (d.m_valid_hints && d.m_sizePolicy == sizePolicy()) 
        return d.m_msh;

    ensurePolished();
    d.m_valid_hints = true;
    // wrap ? golden ratio : min doc size
    d.m_sh = d.sizeForWidth(-1);
    QSize m_msh(-1, -1);

    if (!d.m_isTextLabel) 
        m_msh = d.m_sh;
    else 
    {
        // height for one line
        m_msh.rheight() = d.sizeForWidth(QWIDGETSIZE_MAX).height();
        // wrap ? size of biggest word : min doc size
        m_msh.rwidth() = d.sizeForWidth(0).width();
        if (d.m_sh.height() < m_msh.height())
            m_msh.rheight() = d.m_sh.height();
    }
    const int nHeightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0);
    d.m_sh.setHeight(nHeightTabs-1);
    d.m_msh = m_msh;
    d.m_sizePolicy = sizePolicy();
    return m_msh;
}

void RibbonTab::enterEvent(QEvent* event)
{
    QWidget::enterEvent(event);
    QTN_D(RibbonTab);
    d.m_tabMouseOver = true;
    update();
}

void RibbonTab::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);
    QTN_D(RibbonTab);
    d.m_tabMouseOver = false;
    update();
}

void RibbonTab::mousePressEvent(QMouseEvent* event)
{
    QTN_D(RibbonTab);
    if (event->button() != Qt::LeftButton)
    {
        QWidget::mousePressEvent(event);
        return;
    }

    if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
    {
        if (rb->isBackstageVisible())
        {
            if (RibbonSystemButton* button = rb->getSystemButton())
            {
                if (RibbonBackstageView* backstage = qobject_cast<RibbonBackstageView*>(button->backstage()))
                    backstage->close();
            }
        }
    }

    bool minimized = false;
    if (RibbonBar* rb = qobject_cast<RibbonBar*>(parentWidget()->parentWidget()))
        minimized = rb->isMinimized();

    d.m_minimizeDblClick = d.m_select;

    if (d.m_minimizeDblClick && minimized && !d.m_tracking)
        d.m_minimizeDblClick = false;

    if (d.m_minimizeDblClick)
    {
        event->ignore();
        return;
    }

    emit pressTab(this);

    repaint();
    event->ignore();
}

void RibbonTab::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
        return;
    update();
}

void RibbonTab::mouseDoubleClickEvent(QMouseEvent* event)
{
    QTN_D(RibbonTab);

    if (event->button() != Qt::LeftButton)
    {
        event->setAccepted(false);
        QWidget::mouseDoubleClickEvent(event);
        return;
    }

    if (d.m_select && d.m_minimizeDblClick)
    {
        d.m_minimizeDblClick = true;
        event->setAccepted(false);
        QWidget::mouseDoubleClickEvent(event);
    }
}



QTITAN_BEGIN_NAMESPACE
/* RibbonTabBarPrivate */
class RibbonTabBarPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonTabBar)

public:
    explicit RibbonTabBarPrivate();

public:
    void layoutWidgets();
    bool compressTabs();
    bool validIndex(int index) const { return index >= 0 && index < m_tabList.count(); }
    bool validWidth() const;

    RibbonBar* getRibbonBar() const;

public:
    QList<RibbonTab*> m_tabList;
    QList<QWidget*> m_controls;
    QList<QWidget*> m_listEnabledWidgets;
    RibbonSystemButton* m_controlSystemButton;
    QAction* m_actionSystemPopupBar;
    bool m_validRect;
    int m_currentIndex;
    int m_margin;
    RibbonTabBar::SelectionBehavior selectionBehaviorOnRemove;
};
QTITAN_END_NAMESPACE

RibbonTabBarPrivate::RibbonTabBarPrivate()
    : m_controlSystemButton(Q_NULL)
    , m_actionSystemPopupBar(Q_NULL)
    , m_validRect(true)
    , m_currentIndex(-1)
    , m_margin(1) 
    , selectionBehaviorOnRemove(RibbonTabBar::SelectRightTab)
{
}

void RibbonTabBarPrivate::layoutWidgets()
{
    QTN_P(RibbonTabBar);
    if (p.geometry().isEmpty())
        return; 

    int leftSide = 0;
    int rightSide = 0;

    // set the position of the system button
    int leftIndentTabs = 0;
    if (m_controlSystemButton)
    {
        m_controlSystemButton->move(m_margin * 4, m_margin /*+ 1*/);
        leftIndentTabs = m_controlSystemButton->sizeHint().width();
        leftIndentTabs += m_margin * 4;
        leftSide = m_controlSystemButton->geometry().right();
    }

    int nOffset = 0;
    if (QMenuBar* menuBar = qobject_cast<QMenuBar*>(p.parentWidget())) 
    {
        if (QWidget* rightCorner = menuBar->cornerWidget(Qt::TopRightCorner))
        {
            nOffset = rightCorner->size().width();
            nOffset += 1;
            rightSide = rightCorner->geometry().left();
        }
    }
    // set the position of controls right side
    QSize szSumControls;
    QRect rect = p.rect();
    for (int i = 0, count = m_controls.size(); count > i; i++) 
    {
        QWidget * widget = m_controls.at(i);
        if (widget->isVisible())
        {
            QSize szControl = widget->sizeHint();
            widget->setGeometry( QRect(QPoint(rect.right() - szControl.width() - szSumControls.width() - nOffset, 0),
                QSize(widget->sizeHint().width(), p.sizeHint().height() - m_margin)) );
            szSumControls += widget->sizeHint();
        }
    }

    rightSide = m_controls.size() > 0 ? m_controls.at(m_controls.size()-1)->geometry().left() : 
        rightSide == 0 ? p.geometry().right() : rightSide;

    int sumWidth = 0;
    QList<QRect> rectTabs;
    for (int i = 0, count = m_tabList.count(); count > i; i++)
    {
        RibbonTab* tab = m_tabList.at(i);
        tab->qtn_d().m_valid_hints = false;
        QRect rect = !tab->isHidden() ? QRect(QPoint(sumWidth + m_margin + leftIndentTabs, 0), tab->sizeHint()) : QRect();
        sumWidth += rect.width();
        rectTabs << rect;
    }

    int newWidth = 0;
    if (!p.geometry().isEmpty() && m_tabList.count() > 0)
    {
        int clientWidth = rightSide - leftSide;
        if (clientWidth < sumWidth)
            newWidth = (sumWidth-clientWidth)/m_tabList.count();
    }

    bool validRect = true;
    // calculate the width of the tabs
    sumWidth = 0;
    for (int i = 0, count = m_tabList.count(); count > i; i++) 
    {
        RibbonTab* tab = m_tabList.at(i);

        QRect rectItem = rectTabs.at(i);
        if (newWidth != 0)
            rectItem.setWidth(rectItem.width() - newWidth);

        tab->move(sumWidth + m_margin + leftIndentTabs, rectItem.y());
        tab->resize(rectItem.width(), rectItem.height());
        sumWidth += rectItem.width();

        if (!tab->isHidden() && !tab->validRect())
            validRect = false;
    }
    m_validRect = validRect;
}

bool RibbonTabBarPrivate::validWidth() const
{
    return m_validRect;
}

RibbonBar* RibbonTabBarPrivate::getRibbonBar() const
{
    QTN_P(const RibbonTabBar);
    return qobject_cast<RibbonBar*>(p.parentWidget());
}



/* RibbonTabBar */
RibbonTabBar::RibbonTabBar(QWidget* pParent)
    : QWidget(pParent)
{
    QTN_INIT_PRIVATE(RibbonTabBar);
}

RibbonTabBar::~RibbonTabBar()
{
    QTN_FINI_PRIVATE();
}

int RibbonTabBar::addTab(const QString& text) 
{
    return insertTab(-1, text);
}

int RibbonTabBar::addTab(const QIcon& icon, const QString& text)
{
    return insertTab(-1, icon, text);
}

int RibbonTabBar::insertTab(int index, const QString& text)
{
    return insertTab(index, QIcon(), text);
}

int RibbonTabBar::insertTab(int index, const QIcon& icon, const QString& text)
{
    Q_UNUSED(icon);
    QTN_D(RibbonTabBar);
    RibbonTab* pTabLabel = new RibbonTab(text, this);
    connect(pTabLabel, SIGNAL(pressTab(QWidget*)), this, SLOT(activateTab(QWidget*)));

    if (!d.validIndex(index)) 
    {
        index = d.m_tabList.count();
        d.m_tabList.append(pTabLabel);
    } 
    else 
        d.m_tabList.insert(index, pTabLabel);

    if (d.m_tabList.count() == 1)
        setCurrentIndex(index);
    else if (index <= d.m_currentIndex)
        ++d.m_currentIndex;

    for (int i = 0; i < d.m_tabList.count(); ++i) 
    {
        if (d.m_tabList[i]->lastTab >= index)
            ++d.m_tabList[i]->lastTab;
    }

    bool setSelect = true;
    if (RibbonBar* rb = d.getRibbonBar())
        setSelect = !rb->isMinimized();

    if (setSelect)
    {
        for (int i = 0; i < d.m_tabList.count(); i++) 
            d.m_tabList.at(i)->setSelect(d.m_currentIndex == i);
    }

    if (isVisible())
        pTabLabel->show();

    d.layoutWidgets();
    return index;
}

void RibbonTabBar::moveTab(int index, int newIndex)
{
    QTN_D(RibbonTabBar);
    if (d.validIndex(index) && d.validIndex(newIndex))
    {
        // save pointer active tab
        RibbonTab* saveTab = d.m_tabList[d.m_currentIndex];
        // Moves the tab in the position index to the position newIndex
        d.m_tabList.move(index, newIndex);
        // set an index of active tab
        int ind = d.m_tabList.indexOf(saveTab);
        if (d.validIndex(ind) && ind != d.m_currentIndex)
            d.m_currentIndex = ind;
        // reposition tabs
        d.layoutWidgets();
    }
}

void RibbonTabBar::removeTab(int index)
{
    QTN_D(RibbonTabBar);
    if (d.validIndex(index)) 
    {
        int newIndex = d.m_tabList[index]->lastTab;
        RibbonTab* tab = d.m_tabList[index];
        d.m_tabList.removeAt(index);
        delete tab;

        for (int i = 0; i < d.m_tabList.count(); ++i) 
        {
            if (d.m_tabList[i]->lastTab == index)
                d.m_tabList[i]->lastTab = -1;
            if (d.m_tabList[i]->lastTab > index)
                --d.m_tabList[i]->lastTab;
        }
        if (index == d.m_currentIndex) 
        {
            // The current tab is going away, in order to make sure
            // we emit that "current has changed", we need to reset this
            // around.
            d.m_currentIndex = -1;
            if (d.m_tabList.size() > 0) 
            {
                switch(d.selectionBehaviorOnRemove) 
                {
                case SelectPreviousTab:
                    if (newIndex > index)
                        newIndex--;
                    if (d.validIndex(newIndex))
                        break;
                    // else fallthrough
                case SelectRightTab:
                    newIndex = index;
                    if (newIndex >= d.m_tabList.size())
                        newIndex = d.m_tabList.size() - 1;
                    break;
                case SelectLeftTab:
                    newIndex = index - 1;
                    if (newIndex < 0)
                        newIndex = 0;
                    break;
                default:
                    break;
                }

                if (d.validIndex(newIndex)) 
                {
                    // don't loose newIndex's old through setCurrentIndex
                    int bump = d.m_tabList[newIndex]->lastTab;
                    if (d.getRibbonBar() && !d.getRibbonBar()->isMinimized())
                        setCurrentIndex(newIndex);
                    d.m_tabList[newIndex]->lastTab = bump;
                }
            } 
            else 
            {
//                emit currentChanged(-1);
            }
        } 
        else if (index < d.m_currentIndex) 
        {
            if (d.getRibbonBar() && !d.getRibbonBar()->isMinimized())
                setCurrentIndex(d.m_currentIndex - 1);
        }
    }
}

int RibbonTabBar::currentIndex() const
{
    QTN_D(const RibbonTabBar);
    if (d.validIndex(d.m_currentIndex))
        return d.m_currentIndex;
    return -1;
}

RibbonTab* RibbonTabBar::getTab(int nIndex) const
{
    QTN_D(const RibbonTabBar);
    if (d.validIndex(nIndex))
        return d.m_tabList[nIndex];
    return Q_NULL;
}

int RibbonTabBar::getTabCount() const
{
    QTN_D(const RibbonTabBar);
    return d.m_tabList.count();
}

int RibbonTabBar::getIndex(const RibbonTab* tab) const
{
    for (int i = 0, count = getTabCount(); count > i; i++)
    {
        if (getTab(i) == tab)
            return i;
    }
    return -1;
}

RibbonTabBar::SelectionBehavior RibbonTabBar::selectionBehaviorOnRemove() const
{
    QTN_D(const RibbonTabBar);
    return d.selectionBehaviorOnRemove;
}

void RibbonTabBar::setSelectionBehaviorOnRemove(RibbonTabBar::SelectionBehavior behavior)
{
    QTN_D(RibbonTabBar);
    d.selectionBehaviorOnRemove = behavior;
}

void RibbonTabBar::setEnabledWidgets(bool enabled)
{
    QTN_D(RibbonTabBar);
    if (enabled)
    {
        for (int i = 0, count = d.m_listEnabledWidgets.size(); count > i; i++) 
        {
            QWidget * widget = d.m_listEnabledWidgets.at(i);
            widget->setEnabled(true);
        }
        d.m_listEnabledWidgets.clear();
    }
    else
    {
        d.m_listEnabledWidgets.clear();
        for (int i = 0, count = d.m_controls.size(); count > i; i++) 
        {
            QWidget * widget = d.m_controls.at(i);
            if (widget->isEnabled() /*&& widget->objectName() == QString("No")*/)
            {
                d.m_listEnabledWidgets << widget;
                widget->setEnabled(false);
            }
        }
    }
}

QAction* RibbonTabBar::addAction(const QIcon& icon, const QString& text, Qt::ToolButtonStyle style, QMenu* menu)
{
    QAction* action = new QAction(icon, text, this);
    if (menu)
        action->setMenu(menu);

    return addAction(action, style);
}

QAction* RibbonTabBar::addAction(QAction* action, Qt::ToolButtonStyle style)
{
    QTN_D(RibbonTabBar);
    if ( Qt::ToolButtonTextUnderIcon == style )
    {
        Q_ASSERT(false);
        style = Qt::ToolButtonTextBesideIcon;
    }
    QWidget::addAction(action);
    RibbonButton* button = new RibbonButton(this);

    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setToolButtonStyle(style);

    QIcon icon = action->icon();
    if (!icon.isNull() && icon.availableSizes().count() > 0)
        button->setIconSize(icon.availableSizes().first());

    action->setIconText(action->text());
    button->setDefaultAction(action);
    d.m_controls.append(button);

    button->setVisible(true);
    layoutWidgets();
    return action;
}

QMenu* RibbonTabBar::addMenu(const QString& title)
{
    QTN_D(RibbonTabBar);
    QMenu* menu = new QMenu(title, this);

    RibbonButton * button = new RibbonButton(this);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);

    button->setDefaultAction(menu->menuAction());
    d.m_controls.append(button);

    button->setVisible(true);
    layoutWidgets();
    return menu;
}

void RibbonTabBar::setCurrentIndex(int index)
{
    QTN_D(RibbonTabBar);

    bool allowSetCurrentIndex = false;
    if (RibbonBar* rb = d.getRibbonBar())
        allowSetCurrentIndex = rb->isMinimized();

    if (d.validIndex(index) && (d.m_currentIndex != index || allowSetCurrentIndex)) 
    {
        d.m_currentIndex = index;

        emit currentChanged(index);

        for (int i = 0; i < d.m_tabList.count(); i++) 
            d.m_tabList.at(i)->setSelect(index == i);
        repaint();
    }
}

void RibbonTabBar::currentNextTab(bool next)
{
    QTN_D(RibbonTabBar);

    int nextIndex = -1;
    if (next)
    {
        for (int i = currentIndex() + 1; i < d.m_tabList.count(); i++) 
        {
            if (d.m_tabList.at(i)->isVisible())
            {
                nextIndex = i;
                break;
            }
        }
    }
    else
    {
        for (int i = currentIndex() - 1; i >= 0; --i) 
        {
            if (d.m_tabList.at(i)->isVisible())
            {
                nextIndex = i;
                break;
            }
        }
    }
    if (d.validIndex(nextIndex))
        setCurrentIndex(nextIndex);
}

void RibbonTabBar::layoutWidgets()
{
    QTN_D(RibbonTabBar);
    d.layoutWidgets();
}

void RibbonTabBar::refresh()
{
    QTN_D(RibbonTabBar);
    for (int i = 0; i < d.m_tabList.count(); i++) 
        d.m_tabList.at(i)->qtn_d().updateLabel();
}

bool RibbonTabBar::validWidth() const
{
    QTN_D(const RibbonTabBar);
    return d.validWidth();
}

bool RibbonTabBar::event(QEvent* event)
{
    QTN_D(RibbonTabBar);
    switch(event->type())
    {
        case QEvent::Shortcut :
            if (QShortcutEvent* se = static_cast<QShortcutEvent*>(event))
            {
                for (int i = 0; i < d.m_tabList.count(); ++i) 
                {
                    const RibbonTab* tab = d.m_tabList.at(i);
                    if (tab->shortcut() == se->shortcutId()) 
                    {
                        setCurrentIndex(i);
                        return true;
                    }
                }
            }
            break;
        case QEvent::LayoutRequest :
                layoutWidgets();
            break;
        case QEvent::ActionRemoved :
            if (QActionEvent* actionEvent = static_cast<QActionEvent*>(event))
            {
                for (int i = d.m_controls.count() - 1; i >= 0; --i) 
                {
                    if (QToolButton* widget = qobject_cast<QToolButton*>(d.m_controls.at(i)))
                    {
                        if (actionEvent->action() == widget->defaultAction())
                        {   
                             d.m_controls.removeAt(i);
                            delete widget;
                        }
                    }
                }
                layoutWidgets();
            }
            break;
        case QEvent::ActionChanged :
            if (QActionEvent* actionEvent = static_cast<QActionEvent*>(event))
            {
                for (int i = 0, count = d.m_controls.size(); count > i; i++) 
                {
                    QWidget* widget = d.m_controls.at(i);
                    if (QToolButton* button = qobject_cast<QToolButton*>(widget))
                    {
                        QAction* action = actionEvent->action();
                        if (action == button->defaultAction())
                        {
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

void RibbonTabBar::mouseDoubleClickEvent(QMouseEvent* event)
{
    event->setAccepted(false);
    QWidget::mouseDoubleClickEvent(event);
}

QSize RibbonTabBar::sizeHint() const
{
    const int heightTabs = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonTabsHeight, 0, 0);
    return QSize(0, heightTabs-2).expandedTo(QApplication::globalStrut());
}

void RibbonTabBar::activateTab(QWidget* widget)
{
    QTN_D(RibbonTabBar);

    for (int i = 0; i < d.m_tabList.count(); ++i) 
    {
        if (d.m_tabList.at(i) == widget) 
        {
            bool allow = true;
            if (RibbonBar* ribbonBar = d.getRibbonBar())
            {
                if (RibbonPage* page = ribbonBar->getPage(i))
                    page->activatingPage(allow);
            }

            if (allow)
                setCurrentIndex(i);
            return;
        }
    }
}
