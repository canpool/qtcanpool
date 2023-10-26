/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonbackstageview.h"
#include "ribbonbackstageview_p.h"
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonstyle.h"
#include "stylehelpers.h"
#include <QApplication>
#include <QBitmap>
#include <QLayout>
#include <QPainter>
#include <QStatusBar>
#include <QStyle>
#include <QStyleOption>
#include <QWidgetAction>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

const int commandGap = 6;
const int textGap = 15;
const int imageMargin = 7;

/* RibbonBackstageCloseButton */
RibbonBackstageCloseButton::RibbonBackstageCloseButton(QWidget *parent) : QToolButton(parent) {}

QSize RibbonBackstageCloseButton::sizeHint() const
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    QList<QSize> sz = opt.icon.availableSizes();
    Q_ASSERT(sz.size() > 0);
    return sz[0];
}

void RibbonBackstageCloseButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    QPainter p(this);
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonBackstageCloseButton, &opt, &p, this);
}

void RibbonBackstageCloseButton::mousePressEvent(QMouseEvent *event)
{
    if (testAttribute(Qt::WA_UnderMouse))
        QToolButton::mousePressEvent(event);
}

/* RibbonBackstageViewMenu */
RibbonBackstageViewMenu::RibbonBackstageViewMenu(RibbonBackstageView *backstageView, QWidget *parent)
    : QWidget(parent), m_backstageView(backstageView), m_backStageCloseButton(Q_NULL), m_mouseDown(false),
      m_itemsDirty(true), m_menuWidth(132), m_maxIconWidth(0)
{
    setMouseTracking(true);
}

QSize RibbonBackstageViewMenu::sizeHint() const
{
    m_itemsDirty = true;
    updateActionRects();
    return QSize(m_menuWidth, m_backstageView->sizeHint().width());
}

void RibbonBackstageViewMenu::createBackstageCloseButton()
{
    if ((bool)style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)) {
        if (m_backStageCloseButton == Q_NULL) {
            m_backStageCloseButton = new RibbonBackstageCloseButton(this);
            QAction *actBackStageCloseButton = new QAction(this);
            actBackStageCloseButton->setIcon(QIcon(":/res/ribbonbackstageback_icon.png"));
            m_backStageCloseButton->setDefaultAction(actBackStageCloseButton);
            QSize sz = m_backStageCloseButton->sizeHint();
            m_backStageCloseButton->setGeometry(QRect(QPoint(sz.width() / 2, sz.height() / 2), sz));
            m_backStageCloseButton->setVisible(true);
            QObject::connect(m_backStageCloseButton, SIGNAL(pressed()), m_backstageView, SLOT(close()));
        }
    } else {
        delete m_backStageCloseButton;
        m_backStageCloseButton = Q_NULL;
    }
    m_itemsDirty = true;
}

QAction *RibbonBackstageViewMenu::actionAt(const QPoint &pt) const
{
    QList<QAction *> actions = m_backstageView->actions();
    for (int i = 0; i < m_actionRects.count(); ++i) {
        if (m_actionRects.at(i).contains(pt))
            return actions.at(i);
    }
    return 0;
}

QRect RibbonBackstageViewMenu::actionRect(QAction *act) const
{
    QList<QAction *> actions = m_backstageView->actions();
    int index = actions.indexOf(act);
    if (index == -1)
        return QRect();

    updateActionRects();

    // we found the action
    return m_actionRects.at(index);
}

void RibbonBackstageViewMenu::resetItemsDirty() { m_itemsDirty = true; }

void RibbonBackstageViewMenu::initStyleOption(StyleOptionRibbonBackstage *opt) const
{
    opt->init(this);
    opt->menuWidth = m_menuWidth;
    opt->rect.adjust(0, 0, -1, -1);
}

void RibbonBackstageViewMenu::initStyleOption(QStyleOptionMenuItem *option, const QAction *action) const
{
    if (!option || !action)
        return;

    option->initFrom(this);
    option->palette = palette();
    option->state = QStyle::State_None;

    if (m_backstageView->window()->isActiveWindow())
        option->state |= QStyle::State_Active;
    if (m_backstageView->isEnabled() && action->isEnabled() && (!action->menu() || action->menu()->isEnabled()))
        option->state |= QStyle::State_Enabled;
    else
        option->palette.setCurrentColorGroup(QPalette::Disabled);

    option->font = action->font().resolve(font());
    option->fontMetrics = QFontMetrics(option->font);

    if (m_backstageView->d_func()->m_currentAction && m_backstageView->d_func()->m_currentAction == action &&
        !m_backstageView->d_func()->m_currentAction->isSeparator())
        option->state |= QStyle::State_Selected | (m_mouseDown ? QStyle::State_Sunken : QStyle::State_None);

    option->menuItemType = QStyleOptionMenuItem::Normal;
    if (const QWidgetAction *pageAct = qobject_cast<const QWidgetAction *>(action)) {
        option->menuItemType = QStyleOptionMenuItem::SubMenu;
        if (pageAct->defaultWidget() == m_backstageView->getActivePage() /*m_activePage*/)
            option->state |= QStyle::State_HasFocus;
    } else if (action->isSeparator())
        option->menuItemType = QStyleOptionMenuItem::Separator;

    if (action->isIconVisibleInMenu())
        option->icon = action->icon();
    QString textAndAccel = action->text();

#ifndef QT_NO_SHORTCUT
    if (textAndAccel.indexOf(QLatin1Char('\t')) == -1) {
        QKeySequence seq = action->shortcut();
        if (!seq.isEmpty())
            textAndAccel += QLatin1Char('\t') + seq.toString();
    }
#endif
    option->text = textAndAccel;
    option->maxIconWidth = m_maxIconWidth;
    option->menuRect = rect();
}

void RibbonBackstageViewMenu::updateActionRects() const
{
    if (!m_itemsDirty)
        return;

    ensurePolished();
    QList<QAction *> actions = m_backstageView->actions();
    // let's reinitialize the buffer
    m_actionRects.resize(actions.count());
    m_actionRects.fill(QRect());

    QStyle *style = ((RibbonBackstageViewMenu *)this)->style();
    QStyleOption opt;
    opt.init(this);

    const int icone = style->pixelMetric(QStyle::PM_SmallIconSize, &opt, this);

    QFontMetrics qfm = fontMetrics();

    int actionHeight = 26;
    int pageHeight = qMax(37, qfm.height() + 10);

    for (int i = 0, count = actions.count(); i < count; i++) {
        QAction *action = actions.at(i);
        if (action->isSeparator() || !action->isVisible())
            continue;

        QIcon is = action->icon();
        if (!is.isNull())
            m_maxIconWidth = qMax<uint>(m_maxIconWidth, icone + 4);
    }

    for (int i = 0, count = actions.count(); i < count; i++) {
        QAction *action = actions.at(i);
        if (!action->isVisible() || action->isSeparator())
            continue;   // we continue, this action will get an empty QRect

        // let the style modify the above size..
        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);

        const QFontMetrics &fm = opt.fontMetrics;

        QRect br(fm.boundingRect(action->text()));
        int width = br.width() + commandGap + textGap + commandGap + 6;

        actionHeight = qMax(actionHeight, qMax(fm.height() + 6, qfm.height() + 6));
        width += m_maxIconWidth + imageMargin;
        m_menuWidth = qMax(m_menuWidth, width);
    }

    int top = m_backStageCloseButton ? m_backStageCloseButton->geometry().bottom() + 7 : 7;
    for (int i = 0, count = actions.count(); i < count; i++) {
        QAction *action = actions.at(i);
        if (!action->isVisible() /*||  action->isSeparator()*/)
            continue;   // we continue, this action will get an empty QRect

        if (qobject_cast<QWidgetAction *>(action)) {
            m_actionRects[i] = QRect(QPoint(0, top), QPoint(m_menuWidth, top + pageHeight));
            top += pageHeight + 2;
        } else if (action->isSeparator()) {
            m_actionRects[i] =
                QRect(QPoint(-commandGap, top), QPoint(m_menuWidth - commandGap * 2, top + actionHeight - 3));
            top += actionHeight - 3;
        } else {
            m_actionRects[i] = QRect(QPoint(commandGap, top), QPoint(m_menuWidth - commandGap, top + actionHeight));
            top += actionHeight + 2;
        }
    }

    if (!m_backstageView->getActivePage()) {
        for (int i = 0, count = actions.count(); i < count; i++) {
            if (QWidgetAction *action = qobject_cast<QWidgetAction *>(actions.at(i))) {
                m_backstageView->setActivePage(action->defaultWidget());
                break;
            }
        }
    }
    m_itemsDirty = false;
}

void RibbonBackstageViewMenu::paintEvent(QPaintEvent *event)
{
    updateActionRects();

    QPainter p(this);
    QRegion emptyArea = QRegion(rect());

    StyleOptionRibbonBackstage optBackstage;
    if (m_backStageCloseButton == Q_NULL) {
        optBackstage.init(m_backstageView);
        optBackstage.menuWidth = m_menuWidth;
    } else
        initStyleOption(&optBackstage);
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_Backstage, &optBackstage, &p, m_backstageView);

    QList<QAction *> acts = m_backstageView->actions();

    // draw the items that need updating..
    for (int i = 0, count = acts.count(); i < count; ++i) {
        QAction *action = acts.at(i);
        QRect adjustedActionRect = m_actionRects.at(i);
        if (!event->rect().intersects(adjustedActionRect))
            continue;

        // set the clip region to be extra safe (and adjust for the scrollers)
        QRegion adjustedActionReg(adjustedActionRect);
        emptyArea -= adjustedActionReg;
        p.setClipRegion(adjustedActionReg);

        QStyleOptionMenuItem opt;
        initStyleOption(&opt, action);
        opt.rect = adjustedActionRect;
        opt.text = action->text();

        // opt.menuItemType = QStyleOptionMenuItem::Normal;
        if (QWidgetAction *pageAct = qobject_cast<QWidgetAction *>(action)) {
            opt.menuItemType = QStyleOptionMenuItem::SubMenu;
            if (pageAct->defaultWidget() == m_backstageView->getActivePage())
                opt.state |= QStyle::State_HasFocus;
        }
        style()->drawControl(QStyle::CE_MenuItem, &opt, &p, m_backstageView);
    }
}

void RibbonBackstageViewMenu::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_Escape:
        m_backstageView->close();
        break;
    case Qt::Key_Up:
        break;
    case Qt::Key_Down:
        break;
    default:
        break;
    }
}

void RibbonBackstageViewMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    if (rect().contains(event->pos())) {
        if (QWidgetAction *act = qobject_cast<QWidgetAction *>(actionAt(event->pos()))) {
            if (act && (act->isSeparator() || !act->isEnabled()))
                return;
            if (QWidget *w = act->defaultWidget()) {
                m_backstageView->setActivePage(w);
            }
        }
    }
}

void RibbonBackstageViewMenu::mouseMoveEvent(QMouseEvent *event)
{
    if (rect().contains(event->pos())) {
        QAction *action = actionAt(event->pos());
        m_backstageView->d_func()->setCurrentAction(action ? action : Q_NULL);
        setFocus();
    }
}

void RibbonBackstageViewMenu::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    if (rect().contains(event->pos())) {
        QAction *action = actionAt(event->pos());

        if (action &&
            (action->isSeparator() || !action->isEnabled() || m_backstageView->d_func()->m_widgetItems.contains(action)))
            return;

        if (action && action == m_backstageView->d_func()->m_currentAction) {
            if (!m_backstageView->isClosePrevented())
                m_backstageView->close();
            action->activate(QAction::Trigger);
        }
    }
}

void RibbonBackstageViewMenu::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    m_backstageView->d_func()->setCurrentAction(Q_NULL);
}

/* RibbonBackstageSeparator */
RibbonBackstageSeparator::RibbonBackstageSeparator(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_MouseNoMask);
    setFrameStyle(HLine | Sunken);
}

RibbonBackstageSeparator::~RibbonBackstageSeparator() {}

void RibbonBackstageSeparator::setOrientation(Qt::Orientation orient)
{
    setFrameShape(orient == Qt::Horizontal ? HLine : VLine);
}

Qt::Orientation RibbonBackstageSeparator::orientation() const
{
    return frameShape() == HLine ? Qt::Horizontal : Qt::Vertical;
}

QRIBBON_BEGIN_NAMESPACE

class RibbonBackstageButtonPrivate : public QObject
{
public:
    QRN_DECLARE_PUBLIC(RibbonBackstageButton)
public:
    explicit RibbonBackstageButtonPrivate();
    virtual ~RibbonBackstageButtonPrivate();
public:
    bool m_tabStyle;
    bool m_flatStyle;
};

QRIBBON_END_NAMESPACE

RibbonBackstageButtonPrivate::RibbonBackstageButtonPrivate()
{
    m_tabStyle = false;
    m_flatStyle = false;
}

RibbonBackstageButtonPrivate::~RibbonBackstageButtonPrivate() {}

/* RibbonBackstageButton */
RibbonBackstageButton::RibbonBackstageButton(QWidget *parent) : QToolButton(parent)
{
    QRN_INIT_PRIVATE(RibbonBackstageButton);
}

RibbonBackstageButton::~RibbonBackstageButton() { QRN_FINI_PRIVATE(); }

bool RibbonBackstageButton::tabStyle() const
{
    Q_D(const RibbonBackstageButton);
    return d->m_tabStyle;
}

void RibbonBackstageButton::setTabStyle(bool style)
{
    Q_D(RibbonBackstageButton);
    d->m_tabStyle = style;
    update();
}

bool RibbonBackstageButton::flatStyle()
{
    Q_D(const RibbonBackstageButton);
    return d->m_flatStyle;
}

void RibbonBackstageButton::setFlatStyle(bool flat)
{
    Q_D(RibbonBackstageButton);
    d->m_flatStyle = flat;
    update();
}

/*! \reimp */
void RibbonBackstageButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(RibbonBackstageButton);

    QPainter p(this);
    StyleOptionBackstageButton option;
    initStyleOption(&option);
    option.tabStyle = d->m_tabStyle;
    option.flatStyle = d->m_flatStyle;

    style()->drawComplexControl(QStyle::CC_ToolButton, &option, &p, this);
}

/* RibbonBackstagePage */
RibbonBackstagePage::RibbonBackstagePage(QWidget *parent) : QWidget(parent) { setBackgroundRole(QPalette::Light); }

RibbonBackstagePage::~RibbonBackstagePage() {}

/*! \reimp */
void RibbonBackstagePage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    p.fillRect(opt.rect, opt.palette.color(QPalette::Light));
}

/*!
\class QRibbon::RibbonBackstageViewPrivate
\internal
*/
RibbonBackstageViewPrivate::RibbonBackstageViewPrivate()
{
    m_ribbon = Q_NULL;
    m_backstageMenu = Q_NULL;
    m_tabBarPosition = RibbonBar::TabBarTopPosition;
    m_activePage = Q_NULL;
    m_currentAction = Q_NULL;
    m_closePrevented = false;
}

RibbonBackstageViewPrivate::~RibbonBackstageViewPrivate()
{
    Q_Q(RibbonBackstageView);
    QWidget *parent = q->parentWidget();
    Q_ASSERT(parent != Q_NULL);
    parent->removeEventFilter(q);
}

void RibbonBackstageViewPrivate::init(RibbonBar *ribbonBar)
{
    Q_Q(RibbonBackstageView);
    m_ribbon = ribbonBar;

    QWidget *parent = q->parentWidget();
    Q_ASSERT(parent != Q_NULL);
    parent->installEventFilter(q);

    q->setFont(QApplication::font("QMenu"));
    q->setPalette(QApplication::palette("QMenu"));

    m_backstageMenu = new RibbonBackstageViewMenu(q, parent);
    m_backstageMenu->setPalette(q->palette());
    m_backstageMenu->setFont(q->font());
    m_backstageMenu->setVisible(false);

    q->setVisible(false);
}

void RibbonBackstageViewPrivate::layoutBackstage()
{
    Q_Q(RibbonBackstageView);
    if (!m_ribbon)
        return;

    int backstageViewTop = 0;

    m_backstageMenu->createBackstageCloseButton();

    if (m_ribbon->isVisible() &&
        !(bool)q->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs))
        backstageViewTop = m_ribbon->backstageViewTop();

    QWidget *parent = q->parentWidget();
    QRect rect = parent->rect();
    rect.setTop(rect.top() + backstageViewTop);
    QSize sz = m_backstageMenu->sizeHint();
    m_backstageMenu->setGeometry(QRect(QPoint(rect.topLeft()), QPoint(rect.left() + sz.width(), rect.bottom())));
    m_backstageMenu->raise();

    if ((bool)q->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs)) {
        backstageViewTop = m_ribbon->backstageViewTop();
        rect.setTop(rect.top() + backstageViewTop);
    }
    rect.setLeft(sz.width() + 1);

    q->setGeometry(rect);
    q->raise();
    updateGeometryPage(m_activePage);
}

void RibbonBackstageViewPrivate::updateLayout()
{
    m_backstageMenu->resetItemsDirty();
    layoutBackstage();
}

void RibbonBackstageViewPrivate::updateGeometryPage(QWidget *widget)
{
    if (widget == Q_NULL)
        return;

    Q_Q(const RibbonBackstageView);
    QSize minimumSize = widget->minimumSize();
    int top = (bool)q->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs) ? 0 : 2;

    QRect rcContent(QPoint(0, top), QPoint(q->rect().right(), q->rect().bottom()));
    rcContent.setRight(qMax(rcContent.right(), rcContent.left() + minimumSize.width()));
    rcContent.setBottom(qMax(rcContent.bottom(), rcContent.top() + minimumSize.height()));
    widget->setGeometry(rcContent);
}

#if 0
static inline bool isAncestor(const QObject* obj, const QObject* child)
{
    while (child)
    {
        if (child == obj)
            return true;
        child = child->parent();
    }
    return false;
}
#endif

void RibbonBackstageViewPrivate::setCurrentAction(QAction *action)
{
    if (m_currentAction)
        m_backstageMenu->update(m_backstageMenu->actionRect(m_currentAction));

    m_currentAction = action;

    if (action) {
        if (!action->isSeparator())
            m_backstageMenu->update(m_backstageMenu->actionRect(action));
    }
}

QAction *RibbonBackstageViewPrivate::currentAction() const { return m_currentAction; }

QWidgetAction *RibbonBackstageViewPrivate::getAction(QWidget *w) const
{
    Q_Q(const RibbonBackstageView);
    QList<QAction *> actions = q->actions();
    for (int i = 0, count = actions.count(); i < count; i++) {
        QWidgetAction *action = dynamic_cast<QWidgetAction *>(actions.at(i));
        if (action && action->defaultWidget() == w)
            return action;
    }
    return Q_NULL;
}

/* RibbonBackstageView */
RibbonBackstageView::RibbonBackstageView(RibbonBar *ribbonBar) : QWidget(ribbonBar->parentWidget())
{
    QRN_INIT_PRIVATE(RibbonBackstageView);
    Q_D(RibbonBackstageView);
    d->init(ribbonBar);
}

RibbonBackstageView::~RibbonBackstageView() { QRN_FINI_PRIVATE(); }

void RibbonBackstageView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    Q_D(RibbonBackstageView);
    d->m_tabBarPosition = d->m_ribbon->tabBarPosition();
    d->m_ribbon->setTabBarPosition(RibbonBar::TabBarTopPosition);
    d->updateLayout();
    d->m_backstageMenu->setVisible(true);
    d->m_ribbon->d_func()->setVisibleBackstage(true);
    setFocus();
    emit aboutToShow();
}

void RibbonBackstageView::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    Q_D(RibbonBackstageView);
    d->m_ribbon->setTabBarPosition(d->m_tabBarPosition);
    d->m_backstageMenu->setVisible(false);
    d->m_ribbon->d_func()->setVisibleBackstage(false);
    emit aboutToHide();
}

bool RibbonBackstageView::isClosePrevented() const
{
    Q_D(const RibbonBackstageView);
    return d->m_closePrevented;
}

void RibbonBackstageView::setClosePrevented(bool prevent)
{
    Q_D(RibbonBackstageView);
    d->m_closePrevented = prevent;
}

QAction *RibbonBackstageView::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(icon, text, this);
    addAction(action);
    return action;
}

QAction *RibbonBackstageView::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
    return action;
}

QAction *RibbonBackstageView::addPage(QWidget *widget)
{
    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(widget);
    widget->setFont(font());
    action->setText(widget->windowTitle());
    addAction(action);
    return action;
}

void RibbonBackstageView::setActivePage(QWidget *widget)
{
    Q_D(RibbonBackstageView);

    bool find = false;
    QList<QAction *> acts = actions();
    for (int i = 0, count = acts.count(); i < count && !find; i++) {
        if (QWidgetAction *action = dynamic_cast<QWidgetAction *>(acts.at(i))) {
            if (action->defaultWidget() == widget) {
                if (!action->isVisible()) {
                    Q_ASSERT(false);
                    return;
                }
                find = true;
            }
        }
    }

    if (!find) {
        Q_ASSERT(false);
        return;
    }

    if (d->m_activePage) {
        if (QWidgetAction *act = d->getAction(d->m_activePage))
            d->m_backstageMenu->update(d->m_backstageMenu->actionRect(act));
    }

    for (int i = 0, count = acts.count(); i < count; i++) {
        if (QWidgetAction *action = dynamic_cast<QWidgetAction *>(acts.at(i))) {
            QWidget *w = action->defaultWidget();
            if (w == widget) {
                d->m_activePage = widget;
                d->updateGeometryPage(widget);
                w->setVisible(true);
            } else
                w->setVisible(false);
        }
    }

    if (d->m_activePage) {
        if (QWidgetAction *act = d->getAction(d->m_activePage))
            d->m_backstageMenu->update(d->m_backstageMenu->actionRect(act));
    }
}

QWidget *RibbonBackstageView::getActivePage() const
{
    Q_D(const RibbonBackstageView);
    return d->m_activePage;
}

QRect RibbonBackstageView::actionGeometry(QAction *act) const
{
    Q_D(const RibbonBackstageView);
    return d->m_backstageMenu->actionRect(act);
}

QRect RibbonBackstageView::menuGeometry() const
{
    Q_D(const RibbonBackstageView);
    return d->m_backstageMenu->geometry();
}

void RibbonBackstageView::open()
{
    ensurePolished();   // Get the right font
    setVisible(true);
}

bool RibbonBackstageView::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::ParentChange: {
        if (QWidget *parent = parentWidget())
            parent->installEventFilter(this);
    } break;
    case QEvent::ParentAboutToChange: {
        if (QWidget *parent = parentWidget())
            parent->removeEventFilter(this);
    } break;
    default:
        break;
    }
    return QWidget::event(event);
}

void RibbonBackstageView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(RibbonBackstageView);
    QPainter p(this);

    StyleOptionRibbonBackstage opt;
    opt.init(this);
    opt.menuWidth = d->m_backstageMenu->sizeHint().width();
    style()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_Backstage, &opt, &p, this);
}

bool RibbonBackstageView::eventFilter(QObject *object, QEvent *event)
{
    Q_D(RibbonBackstageView);
    if (parentWidget() == object && isVisible()) {
        switch (event->type()) {
        case QEvent::Resize:
            d->updateLayout();
            break;
        default:
            break;
        }
    }
    return QWidget::eventFilter(object, event);
}

void RibbonBackstageView::actionEvent(QActionEvent *event)
{
    QWidget::actionEvent(event);

    Q_D(RibbonBackstageView);
    if (event->type() == QEvent::ActionAdded) {
        if (QWidgetAction *wa = qobject_cast<QWidgetAction *>(event->action())) {
            QWidget *widget = wa->requestWidget(this);
            if (widget)
                d->m_widgetItems.insert(wa, widget);
        }
    } else if (event->type() == QEvent::ActionRemoved) {
        if (QWidgetAction *wa = qobject_cast<QWidgetAction *>(event->action())) {
            if (QWidget *widget = d->m_widgetItems.value(wa))
                wa->releaseWidget(widget);
        }
        d->m_widgetItems.remove(event->action());
    } else if (event->type() == QEvent::ActionChanged) {
        if (QWidgetAction *action = dynamic_cast<QWidgetAction *>(event->action())) {
            if (!action->isVisible() && action->defaultWidget()) {
                action->defaultWidget()->setVisible(false);

                if (getActivePage() == action->defaultWidget()) {
                    bool activatePage = false;
                    QList<QAction *> acts = actions();
                    for (int i = 0, count = acts.count(); i < count && !activatePage; i++) {
                        if (QWidgetAction *action = dynamic_cast<QWidgetAction *>(acts.at(i))) {
                            if (action->isVisible()) {
                                if (QWidget *widget = action->defaultWidget()) {
                                    setActivePage(widget);
                                    activatePage = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        d->m_backstageMenu->resetItemsDirty();
    }
}
