/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonapplicationbutton.h"
#include "ribbonbuttongroup.h"
#include "ribboncontrols.h"
#include "ribbonkeytip.h"
#include "ribbonmanager.h"
#include "ribbontabbar.h"
#include "ribbonquickaccessbar.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHoverEvent>
#include <QKeyEvent>
#include <QSet>
#include <QLinearGradient>
#include <QPainter>
#include <QResizeEvent>
#include <QSet>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QVariant>
#include <QEventLoop>
#include <QMouseEvent>
#include "qtcompat/qtcompat.h"
#include <QEvent>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

const int tabBarBaseLineHeight = 1;
const int tabBarMinimumWidth = 100;
const int dragAreaMinimumWidth = 20;

// Whether a widget takes part in the ribbon layout. isVisible() would also
// turn false while the top-level window is still hidden, so the very first
// layout would skip such widgets and leave them at their default position
// (e.g. the window button group ends up in the top-left corner and stays
// there until the window happens to be resized).
static bool takesPartInLayout(const QWidget *w)
{
    return w && !w->isHidden();
}

#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashDotDotLine);                                                                                 \
        _pen.setColor(QColor(219, 26, 59));                                                                            \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

/* RibbonStackedWidget */
RibbonStackedWidget::RibbonStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
    , m_isPopup(false)
{
}

RibbonStackedWidget::~RibbonStackedWidget()
{
}

bool RibbonStackedWidget::isPopup() const
{
    return m_isPopup;
}

void RibbonStackedWidget::setPopup(bool popup)
{
    if (m_isPopup == popup) {
        return;
    }
    m_isPopup = popup;
    if (popup) {
        setWindowFlags(Qt::Popup);
    } else {
        setWindowFlags(Qt::Widget);
    }
}

bool RibbonStackedWidget::event(QEvent *event)
{
    if (m_isPopup) {
        switch (event->type()) {
        case QEvent::Show:
            Q_EMIT aboutToShow();
            break;
        case QEvent::Hide:
            Q_EMIT aboutToHide();
            break;
        default:
            break;
        }
    }
    return QStackedWidget::event(event);
}

/**
 * @brief Like the tabbar moveTab function, swaps the indexes of two widgets
 * @param from
 * @param to
 * @note This operation emits the widgetRemoved(int index) signal
 */
void RibbonStackedWidget::moveWidget(int from, int to)
{
    QWidget *w = widget(from);

    removeWidget(w);
    insertWidget(to, w);
}

/* RibbonQuickAccessBarContainer */
RibbonQuickAccessBarContainer::RibbonQuickAccessBarContainer(QWidget *parent)
    : RibbonCtrlContainer(parent)
{
    m_quickAccessBar = new RibbonQuickAccessBar(this);
    // use QLayout::SetDefaultConstraint instread of QLayout::SetFixedSize.
    // Otherwise, the size cannot be set using setGeometry
    m_quickAccessBar->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_quickAccessBar->setObjectName(QStringLiteral("qx_QuickAccessBar"));
    setWidget(m_quickAccessBar);
    setTitleVisible(false);
}

RibbonQuickAccessBarContainer::~RibbonQuickAccessBarContainer()
{
}

RibbonQuickAccessBar *RibbonQuickAccessBarContainer::quickAccessBar() const
{
    return m_quickAccessBar;
}

RibbonBarPrivate::RibbonBarPrivate()
    : m_applicationButton(Q_NULLPTR)
    , m_tabBar(Q_NULLPTR)
    , m_stack(Q_NULLPTR)
    , m_iconRightBorderPosition(1)
    , m_minimumPageButton(Q_NULLPTR)
    , m_topLeftButtonGroup(Q_NULLPTR)
    , m_topRightButtonGroup(Q_NULLPTR)
    , m_bottomLeftButtonGroup(Q_NULLPTR)
    , m_bottomRightButtonGroup(Q_NULLPTR)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_quickAccessBarPosition(RibbonBar::QABRightPosition)
    , m_tabBarPosition(RibbonBar::TBLeftPosition)
    , m_ribbonStyle(RibbonBar::OfficeStyle)
    , m_lastShowStyle(RibbonBar::OfficeStyle)
    , m_pageContextColorListIndex(-1)
    , m_titleAligment(Qt::AlignCenter)
    , m_minimized(true)
    , m_titleVisible(true)
    , m_pageContextCoverTab(false)
    , m_layoutRequest(false)
    , m_blockResize(false)
    , m_keyTipMode(false)
{
    m_pageContextColorList << QColor(201, 89, 156)   // Rose red
                           << QColor(242, 203, 29)   // Yellow
                           << QColor(255, 157, 0)    // Orange
                           << QColor(14, 81, 167)    // Blue
                           << QColor(228, 0, 69)     // Red
                           << QColor(67, 148, 0)     // Green
        ;
}

void RibbonBarPrivate::init()
{
    Q_Q(RibbonBar);
    m_applicationButton = new RibbonApplicationButton(tr("app"), q);
    m_applicationButton->setObjectName(QStringLiteral("qx_RibbonApplicationButton"));
    connect(m_applicationButton, &QAbstractButton::clicked, q, &RibbonBar::applicationButtonClicked);
    m_tabBar = new RibbonTabBar(q);
    m_tabBar->setObjectName(QStringLiteral("qx_RibbonTabBar"));
    m_tabBar->setDrawBase(false);
    connect(m_tabBar, &QTabBar::currentChanged, this, &RibbonBarPrivate::onCurrentRibbonTabChanged);
    connect(m_tabBar, &QTabBar::tabBarClicked, this, &RibbonBarPrivate::onCurrentRibbonTabClicked);
    connect(m_tabBar, &QTabBar::tabBarDoubleClicked, this, &RibbonBarPrivate::onCurrentRibbonTabDoubleClicked);
    connect(m_tabBar, &QTabBar::tabMoved, this, &RibbonBarPrivate::onTabMoved);

    m_tabBarBaseLine = new QWidget(q);
    m_tabBarBaseLine->setObjectName(QStringLiteral("qx_RibbonTabBarBaseLine"));
    m_tabBarBaseLine->setFixedHeight(tabBarBaseLineHeight);
    m_tabBarBaseLine->setVisible(false);

    m_stack = new RibbonStackedWidget(q);
    m_stack->setObjectName(QStringLiteral("qx_RibbonStackedWidget"));
    connect(m_stack, &RibbonStackedWidget::aboutToHide, this, &RibbonBarPrivate::onStackWidgetHided);
    m_stack->installEventFilter(q);
    setMinimizedFlag(false);

    m_quickAccessBar = new RibbonQuickAccessBarContainer(q);
    m_quickAccessBar->setObjectName(QStringLiteral("qx_RibbonQuickAccessBar"));
    m_quickAccessBar->setIcon(q->windowIcon());
}

void RibbonBarPrivate::setApplicationButton(QAbstractButton *btn)
{
    Q_Q(RibbonBar);
    if (m_applicationButton) {
        delete m_applicationButton;
    }
    if (btn) {
        if (btn->parent() != q) {
            btn->setParent(q);
        }
        btn->move(0, q->titleBarHeight());
        connect(btn, &QAbstractButton::clicked, q, &RibbonBar::applicationButtonClicked);

        if (btn->objectName().isEmpty()) {
            btn->setObjectName(QStringLiteral("qx_RibbonApplicationButton"));
        }
        btn->setVisible(true);
    }
    m_applicationButton = btn;
}

bool RibbonBarPrivate::isPageContextVisible(RibbonPageContext *pageContext)
{
    for (int i = 0; i < m_currentShowingPageContextList.size(); ++i) {
        if (m_currentShowingPageContextList[i] == pageContext) {   // operator is overridden
            return true;
        }
    }
    return false;
}

void RibbonBarPrivate::setMinimizedFlag(bool flag)
{
    Q_Q(RibbonBar);
    if (m_minimized == flag) {
        return;
    }
    m_minimized = flag;

    if (flag) {
        m_stack->setPopup(true);
        m_stack->setFocusPolicy(Qt::NoFocus);
        m_stack->clearFocus();
        m_tabBar->setFocus();
        m_stack->hide();
        m_blockResize = true;
        q->setFixedHeight(m_tabBar->geometry().bottom());
        m_blockResize = false;
    } else {
        m_stack->setPopup(false);
        m_stack->setFocus();
        m_stack->show();
    }
}

void RibbonBarPrivate::updateMinimumButtonIcon()
{
    Q_Q(RibbonBar);
    if (m_minimumPageButton) {
        QAction *action = m_minimumPageButton->defaultAction();
        if (action) {
            action->setIcon(q->style()->standardIcon(m_minimized ? QStyle::SP_TitleBarUnshadeButton
                                                                 : QStyle::SP_TitleBarShadeButton,
                                                     0, m_minimumPageButton));
        }
    }
}

QColor RibbonBarPrivate::getPageContextColor()
{
    if (m_pageContextColorList.isEmpty()) {
        m_pageContextColorListIndex = -1;
        return QColor();
    }
    ++m_pageContextColorListIndex;
    if ((m_pageContextColorListIndex >= m_pageContextColorList.size()) || (m_pageContextColorListIndex < 0)) {
        m_pageContextColorListIndex = 0;
    }
    return m_pageContextColorList.at(m_pageContextColorListIndex);
}

void RibbonBarPrivate::updateTabData()
{
    int tabcount = m_tabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData data = var.value<RibbonTabData>();
            data.index = i;
            m_tabBar->setTabData(i, QVariant::fromValue(data));
        }
    }
    // After refreshing the tabdata, also refresh the ContextPage information
    updatePageContextManagerData();
}

/**
 * @brief Refreshes all ContextPageManagerData; called when a single Page is removed
 */
void RibbonBarPrivate::updatePageContextManagerData()
{
    const int c = m_tabBar->count();

    for (PageContextManagerData &cd : m_currentShowingPageContextList) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.pageContext->pageCount(); ++i) {
            RibbonPage *page = cd.pageContext->page(i);
            for (int t = 0; t < c; ++t) {
                QVariant v = m_tabBar->tabData(t);
                if (v.isValid()) {
                    RibbonTabData data = v.value<RibbonTabData>();
                    if (data.page == page) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

void RibbonBarPrivate::updateRibbonElementGeometry()
{
    Q_Q(RibbonBar);
    // Adjust the RibbonPage layout mode according to the style
    QList<RibbonPage *> pages = q->pages();
    for (RibbonPage *page : std::as_const(pages)) {
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
    }
    // Prevent a height change from triggering resizeEvent, which would run resizeXX
    m_blockResize = true;
    updateRibbonBarHeight();
    m_blockResize = false;
}

void RibbonBarPrivate::updateRibbonBarHeight()
{
    Q_Q(RibbonBar);
    // Adjust the bar height according to the style
    if (m_minimized) {
        // In minimized mode the bar height is the bottom of the tabbar
        // In minimized mode the bar height is already adjusted in setMinimizedFlag
        // q->setFixedHeight(m_tabBar->geometry().bottom());
    } else {
        q->setFixedHeight(mainBarHeight());
    }
}

/**
 * @brief Pops up the stack in minimized mode: finish the tab bar hover state, then position, focus and show it
 */
void RibbonBarPrivate::showPopupStack()
{
    if (m_stack->isVisible() || !m_stack->isPopup()) {
        return;
    }
    // Before popping up the StackedWidget, send the tabbar a QHoverEvent so it knows the mouse has left
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
    QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToGlobal(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()));
#else
    QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToParent(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()),
                    m_tabBar->mapToGlobal(QCursor::pos()));
#endif
    QApplication::sendEvent(m_tabBar, &ehl);
    // Adjust the position before showing to avoid a wrong position after moving
    resizeStackedWidget();
    m_stack->setFocus();
    m_stack->show();
}

void RibbonBarPrivate::paintInOfficeStyle(QPainter &p)
{
    Q_Q(RibbonBar);
    paintBackground(p);

    //! Draw the context tabs
    const QMargins border = q->contentsMargins();
    const QPoint pageContextPos = paintPageContextTabs(p, true);

    //! Draw the title, etc.
    QWidget *parWindow = q->parentWidget();
    if (parWindow && m_titleVisible) {
        QRect titleRegion;
        int x = border.right();
        // |border|m_quickAccessBar|m_topLeftButtonGroup|~~~
        if (m_topLeftButtonGroup && m_topLeftButtonGroup->isVisible()) {
            x = m_topLeftButtonGroup->geometry().right() + 1;
        } else if (m_quickAccessBar && m_quickAccessBar->isVisible()) {
            x = m_quickAccessBar->geometry().right() + 1;
        }
        if (pageContextPos.y() < 0) {
            // ~~~~|m_topRightButtonGroup|m_windowButtonGroup|m_iconRightBorderPosition|border|
            int w = q->width() - m_iconRightBorderPosition - border.right() - x;
            if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
                w -= m_windowButtonGroup->width();
            }
            if (m_topRightButtonGroup && m_topRightButtonGroup->isVisible()) {
                w -= m_topRightButtonGroup->width();
            }
            titleRegion.setRect(x, border.top(), w, q->titleBarHeight());
        } else {
            // ~~~|~~~~~   ~~~~~~|PageContext|~~~~~    ~~~~|
            int leftwidth = pageContextPos.x() - m_iconRightBorderPosition - x;
            int rightwidth = q->width() - pageContextPos.y();
            if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
                rightwidth -= m_windowButtonGroup->width();
            }
            if (m_topRightButtonGroup && m_topRightButtonGroup->isVisible()) {
                rightwidth -= m_topRightButtonGroup->width();
            }
            if (rightwidth > leftwidth) {
                titleRegion.setRect(pageContextPos.y(), border.top(), rightwidth, q->titleBarHeight());
            } else {
                titleRegion.setRect(m_iconRightBorderPosition + x, border.top(), leftwidth, q->titleBarHeight());
            }
        }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
        p.save();
        p.setBrush(QColor(255, 0, 0, 120));
        p.drawRect(titleRegion);
        p.restore();
#endif
        paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
    }
}

void RibbonBarPrivate::paintInWpsLiteStyle(QPainter &p)
{
    Q_Q(RibbonBar);
    paintBackground(p);

    //! Draw the context tabs
    const QMargins border = q->contentsMargins();
    paintPageContextTabs(p, false);

    //! Draw the title, etc.
    QWidget *parWindow = q->parentWidget();
    if (parWindow && m_titleVisible) {
        // |m_tabBar|~~~        ~~~|m_windowButtonGroup|
        int start = m_tabBar->x() + m_tabBar->width();
        int width = q->width() - start;
        if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
            width -= m_windowButtonGroup->width();
        }
        // ~~~|m_quickAccessBar|bottomLeftBG|topRightBG|bottomRightBG|m_windowButtonGroup|
        if (m_quickAccessBarPosition == RibbonBar::QABRightPosition && m_quickAccessBar &&
            m_quickAccessBar->isVisible()) {
            width = m_quickAccessBar->x() - start;
        } else if (m_bottomLeftButtonGroup && m_bottomLeftButtonGroup->isVisible()) {
            width = m_bottomLeftButtonGroup->x() - start;
        } else if (m_topRightButtonGroup && m_topRightButtonGroup->isVisible()) {
            width = m_topRightButtonGroup->x() - start;
        } else if (m_bottomRightButtonGroup && m_bottomRightButtonGroup->isVisible()) {
            width = m_bottomRightButtonGroup->x() - start;
        }
        if (width > 20) {
            QRect titleRegion(start, border.top(), width, q->titleBarHeight());
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            p.save();
            p.setBrush(QColor(255, 0, 0, 120));
            p.drawRect(titleRegion);
            p.restore();
#endif
            paintWindowTitle(p, parWindow->windowTitle(), titleRegion);
        }
    }
}

void RibbonBarPrivate::paintBackground(QPainter &painter)
{
    Q_Q(RibbonBar);
    // Draw a line below the tabbar
    painter.save();
    QPen pen(m_tabBarBaseLine->palette().window().color());
    pen.setWidth(tabBarBaseLineHeight);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QMargins border = q->contentsMargins();
    const int lineY = m_tabBar->geometry().bottom();
    painter.drawLine(QPoint(border.left(), lineY), QPoint(q->width() - border.right() - 1, lineY));
    painter.restore();
}

/**
 * @brief Paints the currently visible context tabs and returns the horizontal range they cover
 *
 * The returned x is the left edge and y is the right edge; y < 0 means no context tab is shown (caller convention).
 * The tab text is drawn only in office style (decided inside paintPageContextTab), so the title is always passed here.
 *
 * @param extendToWindowTop office raises the top of each visible rect to the window top (the tab strip sits above the
 * tab bar); in wps style the tab bar is already on the first row with the same top edge, so pass false to keep it
 * unchanged.
 */
QPoint RibbonBarPrivate::paintPageContextTabs(QPainter &painter, bool extendToWindowTop)
{
    Q_Q(RibbonBar);
    painter.save();
    const QMargins border = q->contentsMargins();
    QPoint pageContextPos(q->width(), -1);   // x is the left edge, y is the right edge

    for (const PageContextManagerData &data : std::as_const(m_currentShowingPageContextList)) {
        const QList<int> indexes = data.tabPageIndex;
        if (indexes.isEmpty()) {
            continue;
        }
        const QColor color = data.pageContext->contextColor();
        QRect contextTitleRect = m_tabBar->tabRect(indexes.first());
        const QRect endRect = m_tabBar->tabRect(indexes.last());
        contextTitleRect.setRight(endRect.right());
        // contextTitleRect is relative to m_tabBar, not a real position in RibbonBar, so translate it to the m_tabBar
        // origin
        contextTitleRect.translate(m_tabBar->x(), m_tabBar->y());
        contextTitleRect.setHeight(m_tabBar->height() - 1);   // subtract 1 pixel to avoid covering the tabbar base line
        contextTitleRect -= m_tabBar->tabMargin() / 2;
        contextTitleRect.setTop(border.top());   // extend the top of the region to the window top
        const QRect tabBarRect(m_tabBar->pos(), m_tabBar->size());
        if (tabBarRect.intersects(contextTitleRect)) {
            QRect visibleRect = tabBarRect.intersected(contextTitleRect);
            if (extendToWindowTop) {
                visibleRect.setTop(contextTitleRect.top());
            }
            paintPageContextTab(painter, data.pageContext->contextTitle(), visibleRect, color);
        }
        // Update the context tab range used to control the title bar. If pageContextPos.y() < 0 no context tab is shown
        pageContextPos.setX(qMin(pageContextPos.x(), contextTitleRect.left()));
        pageContextPos.setY(qMax(pageContextPos.y(), contextTitleRect.right()));
#ifdef QX_DRAW_CONTEXT_PAGE_BORDER
        if (indexes.contains(m_tabBar->currentIndex())) {
            QPen pen;
            pen.setColor(color);
            pen.setWidth(1);
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter.drawRect(m_stack->geometry());
        }
#endif
    }
    painter.restore();
    return pageContextPos;
}

void RibbonBarPrivate::paintWindowTitle(QPainter &painter, const QString &title, const QRect &titleRegion)
{
    Q_Q(RibbonBar);
    painter.save();
    painter.setPen(q->palette().color(QPalette::WindowText));
    painter.drawText(titleRegion, m_titleAligment, title);
    painter.restore();
}

void RibbonBarPrivate::paintPageContextTab(QPainter &painter, const QString &title, QRect contextRect,
                                           const QColor &color)
{
    Q_Q(RibbonBar);
    // First, a solid 5-pixel thick line at the top
    QMargins border = q->contentsMargins();
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRect(contextRect.x(), border.top(), contextRect.width(), 5));

    // Lighten the remaining color by 90%
    QColor gColor = color.lighter(190);

    // Subtract the previous 5 pixels
    contextRect -= QMargins(0, 5, 0, 0);
    if (!m_pageContextCoverTab) {
        // Set the bottom position so the tab is not covered
        contextRect.setBottom(m_tabBar->geometry().top());
    }
    painter.fillRect(contextRect, gColor);

    // The title is only drawn in office mode
    if (q->isOfficeStyle()) {
        if (!title.isEmpty()) {
            contextRect.setBottom(m_tabBar->geometry().top());
            painter.setPen(color);
            painter.drawText(contextRect, Qt::AlignCenter, title);
        }
    }
    painter.restore();

    if (!m_pageContextCoverTab) {
        painter.save();
        // Draw the tab border
        painter.setPen(gColor);
        painter.setBrush(Qt::NoBrush);
        QRect tabRect(contextRect.bottomLeft(), QPoint(contextRect.bottomRight().x(), m_tabBar->geometry().bottom()));
        painter.drawLine(tabRect.topLeft(), tabRect.bottomLeft());
        painter.drawLine(tabRect.topRight(), tabRect.bottomRight());
        painter.restore();
    }
}

void RibbonBarPrivate::resizeInOfficeStyle()
{
    Q_Q(RibbonBar);
    updateRibbonElementGeometry();

    QMargins border = q->contentsMargins();
    int x = border.left();
    int y = border.top();

    const int validTitleBarHeight = q->titleBarHeight();
    const int tabH = q->tabBarHeight();
    const int otherH = tabH - tabBarBaseLineHeight;

    // row-1
    x += m_iconRightBorderPosition + 1;
    // |cornerL|m_quickAccessBar|m_topLeftButtonGroup|~~~~~
    QWidget *cornerL = q->cornerWidget(Qt::TopLeftCorner);
    if (takesPartInLayout(cornerL)) {
        placeCornerWidget(cornerL, x, y, validTitleBarHeight, validTitleBarHeight);
        x = cornerL->geometry().right() + 5;
    }
    if (takesPartInLayout(m_quickAccessBar)) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        m_quickAccessBar->setGeometry(x, y, quickAccessBarSize.width(), validTitleBarHeight);
        x += quickAccessBarSize.width() + 5;
    }
    if (takesPartInLayout(m_topLeftButtonGroup)) {
        QSize wSize = m_topLeftButtonGroup->sizeHint();
        m_topLeftButtonGroup->setGeometry(x, y, wSize.width(), validTitleBarHeight);
    }
    // ~~~~~~~~|m_topRightButtonGroup|m_windowButtonGroup|
    int endX = q->width() - border.right();
    if (takesPartInLayout(m_windowButtonGroup)) {
        QSize wSize = m_windowButtonGroup->sizeHint();
        endX -= wSize.width();
        m_windowButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (takesPartInLayout(m_topRightButtonGroup)) {
        QSize wSize = m_topRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_topRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }

    // row-2
    x = border.left();
    y += validTitleBarHeight;
    // |m_applicationButton|~~~~~ m_tabBar ~~~~~|m_bottomLeftButtonGroup|m_bottomRightButtonGroup|cornerW|
    if (takesPartInLayout(m_applicationButton)) {
        m_applicationButton->setGeometry(x, y, m_applicationButton->size().width(), otherH);
        x = m_applicationButton->geometry().right();
    }
    endX = q->width() - border.right();
    QWidget *cornerW = q->cornerWidget(Qt::TopRightCorner);
    if (takesPartInLayout(cornerW)) {
        endX -= cornerW->sizeHint().width();
        placeCornerWidget(cornerW, endX, y, tabH, otherH);
    }
    if (takesPartInLayout(m_bottomRightButtonGroup)) {
        QSize wSize = m_bottomRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomRightButtonGroup->setGeometry(endX, y, wSize.width(), otherH);
    }
    if (takesPartInLayout(m_bottomLeftButtonGroup)) {
        QSize wSize = m_bottomLeftButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomLeftButtonGroup->setGeometry(endX, y, wSize.width(), otherH);
    }
    layoutTabBar(x, y, endX - x, tabH);

    resizeStackedWidget();
}

void RibbonBarPrivate::resizeInWpsLiteStyle()
{
    Q_Q(RibbonBar);
    updateRibbonElementGeometry();

    QMargins border = q->contentsMargins();
    int x = border.left();
    int y = border.top();

    const int validTitleBarHeight = q->titleBarHeight();

    // ~~~~|m_bottomLeftButtonGroup|m_topRightButtonGroup|m_bottomRightButtonGroup|cornerR|m_windowButtonGroup|
    int endX = q->width() - border.right();
    if (takesPartInLayout(m_windowButtonGroup)) {
        QSize wSize = m_windowButtonGroup->sizeHint();
        endX -= wSize.width();
        m_windowButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    QWidget *cornerR = q->cornerWidget(Qt::TopRightCorner);
    if (takesPartInLayout(cornerR)) {
        endX -= cornerR->sizeHint().width();
        placeCornerWidget(cornerR, endX, y, validTitleBarHeight, validTitleBarHeight);
    }
    if (takesPartInLayout(m_bottomRightButtonGroup)) {
        QSize wSize = m_bottomRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (takesPartInLayout(m_topRightButtonGroup)) {
        QSize wSize = m_topRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_topRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (takesPartInLayout(m_bottomLeftButtonGroup)) {
        QSize wSize = m_bottomLeftButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomLeftButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    // ~~~~|m_quickAccessBar|m_bottomLeftButtonGroup|~~~~~
    if (m_quickAccessBarPosition == RibbonBar::QABRightPosition && takesPartInLayout(m_quickAccessBar)) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        endX -= quickAccessBarSize.width();
        m_quickAccessBar->setGeometry(endX, y, quickAccessBarSize.width(), validTitleBarHeight);
    }
    // ~~~~|cornerL|<m_quickAccessBar>|m_bottomLeftButtonGroup|~~~~~
    QWidget *cornerL = q->cornerWidget(Qt::TopLeftCorner);
    if (takesPartInLayout(cornerL)) {
        endX -= cornerL->sizeHint().width();
        placeCornerWidget(cornerL, endX, y, validTitleBarHeight, validTitleBarHeight);
    }

    // The tab y value needs to be recalculated
    int tabH = q->tabBarHeight();
    if (tabH > validTitleBarHeight) {
        // In this case just set tabH to validTitleBarHeight
        tabH = validTitleBarHeight;
    }
    // If tabH is smaller, move it down so the tab bottom aligns with the title bottom
    y += validTitleBarHeight - tabH;

    // |m_applicationButton|<m_quickAccessBar>|m_topLeftButtonGroup|m_tabBar|~~~~~~
    if (takesPartInLayout(m_applicationButton)) {
        m_applicationButton->setGeometry(x, y, m_applicationButton->size().width(), tabH);
        x = m_applicationButton->geometry().right() + 2;
    }
    if (m_quickAccessBarPosition == RibbonBar::QABLeftPosition && takesPartInLayout(m_quickAccessBar)) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        m_quickAccessBar->setGeometry(x, border.top(), quickAccessBarSize.width(), validTitleBarHeight);
        x = m_quickAccessBar->geometry().right() + 2;
    }
    if (takesPartInLayout(m_topLeftButtonGroup)) {
        QSize wSize = m_topLeftButtonGroup->sizeHint();
        m_topLeftButtonGroup->setGeometry(x, y, wSize.width(), tabH);
        x = m_topLeftButtonGroup->geometry().right() + 2;
    }
    // 20200831
    // tabBarWidth used to be endX - x; it now needs to be adjusted to the actual size
    // To avoid the tabbar's tab-free part taking up space, set the width from the tab sizes so the tabbar is exactly as
    // long as needed; this frees mainwindow space to receive mouse events for dragging, otherwise the tabbar would
    // cover the whole top bar and the mouse could not click mainwindow
    layoutTabBar(x, y, endX - x - dragAreaMinimumWidth, tabH);

    resizeStackedWidget();
}

/**
 * @brief Places a corner widget in a horizontal band: center it vertically if it fits, otherwise stretch it
 * @param centredHeight reference height for centering
 * @param stretchedHeight height used when the widget is taller than the reference
 */
void RibbonBarPrivate::placeCornerWidget(QWidget *corner, int x, int y, int centredHeight, int stretchedHeight)
{
    const QSize cornerSize = corner->sizeHint();
    if (cornerSize.height() < centredHeight) {
        corner->setGeometry(x, y + (centredHeight - cornerSize.height()) / 2, cornerSize.width(), cornerSize.height());
    } else {
        corner->setGeometry(x, y, cornerSize.width(), stretchedHeight);
    }
}

/**
 * @brief Places the tab bar within the remaining width according to its position (left / centered)
 */
void RibbonBarPrivate::layoutTabBar(int x, int y, int allowedTabBarWidth, int tabH)
{
    const int minTabBarWidth = calcMinTabBarWidth();
    if (m_tabBarPosition == RibbonBar::TBLeftPosition) {
        m_tabBar->setGeometry(x, y, qMin(minTabBarWidth, allowedTabBarWidth), tabH);
    } else {   // RibbonBar::TBCenterPosition
        if (minTabBarWidth < allowedTabBarWidth) {
            // |~~~~~~~~~~~~~~~~~~ allowedTabBarWidth ~~~~~~~~~~~~~~|
            // |~~ xOffset ~~|~~~~ mintabBarWidth ~~~~|~~ xOffset ~~|
            int xOffset = (allowedTabBarWidth - minTabBarWidth) / 2;
            m_tabBar->setGeometry(x + xOffset, y, minTabBarWidth, tabH);
        } else {
            m_tabBar->setGeometry(x, y, allowedTabBarWidth, tabH);
        }
    }
}

void RibbonBarPrivate::resizeStackedWidget()
{
    Q_Q(RibbonBar);
    QMargins border = q->contentsMargins();
    if (m_stack->isPopup()) {
        QPoint absPosition = q->mapToGlobal(QPoint(border.left(), m_tabBar->geometry().bottom() + 1));
        m_stack->setGeometry(absPosition.x(), absPosition.y(), q->width() - border.left() - border.right(),
                             mainBarHeight() - m_tabBar->geometry().bottom() - border.bottom() - 1);
    } else {
        m_stack->setGeometry(border.left(), m_tabBar->geometry().bottom() + 1,
                             q->width() - border.left() - border.right(),
                             mainBarHeight() - m_tabBar->geometry().bottom() - border.bottom() - 1);
    }
}

void RibbonBarPrivate::resizeRibbon()
{
    if (isOfficeStyle()) {
        resizeInOfficeStyle();
    } else {
        resizeInWpsLiteStyle();
    }
}

int RibbonBarPrivate::minimumWidthInOfficeStyle() const
{
    Q_Q(const RibbonBar);
    QMargins border = q->contentsMargins();

    int w1 = 0;
    int w2 = 0;

    // row-1
    w1 += m_iconRightBorderPosition + 1;
    // |cornerL|m_quickAccessBar|m_topLeftButtonGroup|~~~~~~~~~|m_topRightButtonGroup|m_windowButtonGroup|
    QWidget *cornerL = q->cornerWidget(Qt::TopLeftCorner);
    if (takesPartInLayout(cornerL)) {
        w1 += cornerL->sizeHint().width() + 5;
    }
    if (takesPartInLayout(m_quickAccessBar)) {
        w1 += m_quickAccessBar->sizeHint().width() + 5;
    }
    if (takesPartInLayout(m_topLeftButtonGroup)) {
        w1 += m_topLeftButtonGroup->sizeHint().width();
    }
    if (takesPartInLayout(m_windowButtonGroup)) {
        w1 += m_windowButtonGroup->sizeHint().width();
    }
    if (takesPartInLayout(m_topRightButtonGroup)) {
        w1 += m_topRightButtonGroup->sizeHint().width();
    }
    w1 += dragAreaMinimumWidth;

    // row-2
    // |m_applicationButton|~~~~~ m_tabBar ~~~~~|m_bottomLeftButtonGroup|m_bottomRightButtonGroup|cornerW|
    if (takesPartInLayout(m_applicationButton)) {
        w2 += m_applicationButton->size().width();
    }
    QWidget *cornerW = q->cornerWidget(Qt::TopRightCorner);
    if (takesPartInLayout(cornerW)) {
        w2 += cornerW->sizeHint().width();
    }
    if (takesPartInLayout(m_bottomRightButtonGroup)) {
        w2 += m_bottomRightButtonGroup->sizeHint().width();
    }
    if (takesPartInLayout(m_bottomLeftButtonGroup)) {
        w2 += m_bottomLeftButtonGroup->sizeHint().width();
    }
    w2 += tabBarMinimumWidth;

    return qMax(w1, w2) + border.left() + border.right();
}

int RibbonBarPrivate::minimumWidthInWpsLiteStyle() const
{
    Q_Q(const RibbonBar);
    QMargins border = q->contentsMargins();

    int w = 0;

    // ~~~~|m_bottomLeftButtonGroup|m_topRightButtonGroup|m_bottomRightButtonGroup|cornerR|m_windowButtonGroup|
    if (takesPartInLayout(m_windowButtonGroup)) {
        w += m_windowButtonGroup->sizeHint().width();
    }
    QWidget *cornerR = q->cornerWidget(Qt::TopRightCorner);
    if (takesPartInLayout(cornerR)) {
        w += cornerR->sizeHint().width();
    }
    if (takesPartInLayout(m_bottomRightButtonGroup)) {
        w += m_bottomRightButtonGroup->sizeHint().width();
    }
    if (takesPartInLayout(m_topRightButtonGroup)) {
        w += m_topRightButtonGroup->sizeHint().width();
    }
    if (takesPartInLayout(m_bottomLeftButtonGroup)) {
        w += m_bottomLeftButtonGroup->sizeHint().width();
    }
    // ~~~~|m_quickAccessBar|m_bottomLeftButtonGroup|~~~~~
    if (takesPartInLayout(m_quickAccessBar)) {
        w += m_quickAccessBar->sizeHint().width();
    }
    // ~~~~|cornerL|<m_quickAccessBar>|m_bottomLeftButtonGroup|~~~~~
    QWidget *cornerL = q->cornerWidget(Qt::TopLeftCorner);
    if (takesPartInLayout(cornerL)) {
        w += cornerL->sizeHint().width();
    }
    // |m_applicationButton|<m_quickAccessBar>|m_topLeftButtonGroup|m_tabBar|~~~~~~
    if (takesPartInLayout(m_applicationButton)) {
        w += m_applicationButton->size().width();
    }
    if (takesPartInLayout(m_topLeftButtonGroup)) {
        w += m_topLeftButtonGroup->sizeHint().width();
    }
    w += dragAreaMinimumWidth;

    return w + tabBarMinimumWidth + border.left() + border.right();
}

/**
 * @brief Resets the tabbar width as needed, mainly for the wps style
 */
int RibbonBarPrivate::calcMinTabBarWidth() const
{
    const QMargins &mg = m_tabBar->tabMargin();
    int mintabBarWidth = 0;

    int cnt = m_tabBar->count();
    ++cnt;   // add sentry tab

    for (int i = 0; i < cnt; ++i) {
        mintabBarWidth += m_tabBar->tabRect(i).width();
    }
    mintabBarWidth += (mg.left() + mg.right());
    return mintabBarWidth;
}

/**
 * @brief The computed height of mainBarHeight
 *
 * RibbonBar::height and mainBarHeight may differ; this happens in the minimized state
 * @return height
 */
int RibbonBarPrivate::mainBarHeight() const
{
    return RibbonElementStyleOpt.ribbonBarHeight(m_ribbonStyle);
}

/**
 * @brief Finds the tabbar index by RibbonPage* pointer
 *
 * @param page the QObject pointer of the RibbonPage
 * @return -1 if not found
 * @note This function calls no method on RibbonPage*, so it can be called from RibbonPage's destroyed slot
 */
int RibbonBarPrivate::tabIndex(RibbonPage *page) const
{
    if (page == Q_NULLPTR) {
        return -1;
    }
    const int size = m_tabBar->count();

    for (int i = 0; i < size; ++i) {
        QVariant v = m_tabBar->tabData(i);
        if (v.isValid()) {
            RibbonTabData innerValue = v.value<RibbonTabData>();
            if (innerValue.page == page) {
                return i;
            }
        }
    }
    return -1;
}

RibbonButtonGroup *RibbonBarPrivate::createButtonGroup()
{
    Q_Q(RibbonBar);
    RibbonButtonGroup *buttonGroup = new RibbonButtonGroup(q);
    buttonGroup->setObjectName(QStringLiteral("qx_RibbonButtonGroup"));
    buttonGroup->setFrameShape(QFrame::NoFrame);
    return buttonGroup;
}

void RibbonBarPrivate::onWindowTitleChanged(const QString &title)
{
    Q_Q(RibbonBar);
    Q_UNUSED(title);
    q->update();
}

void RibbonBarPrivate::onWindowIconChanged(const QIcon &icon)
{
    if (m_quickAccessBar) {
        m_quickAccessBar->setIcon(icon);
    }
}

void RibbonBarPrivate::onPageWindowTitleChanged(const QString &title)
{
    RibbonPage *w = qobject_cast<RibbonPage *>(sender());

    if (Q_NULLPTR == w) {
        return;
    }
    for (int i = 0; i < m_tabBar->count(); ++i) {
        // There are few tabs, so efficiency is not a concern
        QVariant var = m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData data = var.value<RibbonTabData>();
            if (w == data.page) {
                m_tabBar->setTabText(i, title);
            }
        }
    }
}

void RibbonBarPrivate::onPageContextAdded(RibbonPage *page)
{
    Q_ASSERT_X(page != Q_NULLPTR, "onPageContextAdded", "add Q_NULLPTR page");
    m_stack->addWidget(page);   // here the stackedWidget appends; everywhere else should use insert
}

void RibbonBarPrivate::onCurrentRibbonTabChanged(int index)
{
    Q_Q(RibbonBar);
    QVariant var = m_tabBar->tabData(index);
    RibbonPage *page = Q_NULLPTR;

    if (var.isValid()) {
        RibbonTabData data = var.value<RibbonTabData>();
        page = data.page;
    }
    if (page) {
        if (m_stack->currentWidget() != page) {
            m_stack->setCurrentWidget(page);
        }
        if (m_minimized) {
            m_tabBar->clearFocus();
            // In minimized mode, set the tab index to -1 after each StackedWidget show,
            // so every click triggers onCurrentRibbonTabChanged
            showPopupStack();
        }
    }
    Q_EMIT q->currentRibbonTabChanged(index);
}

/**
 * @brief ribbon tab bar single click
 *
 * This implementation must forward the stackedwidget's QEvent::MouseButtonDblClick to the tabbar in the eventfilter,
 * otherwise a double click becomes two single clicks
 *
 * The single click event only responds when the same tab is clicked
 * @param index
 */
void RibbonBarPrivate::onCurrentRibbonTabClicked(int index)
{
    if (index != m_tabBar->currentIndex()) {
        // A different clicked tab is handled by the changed slot
        return;
    }
    if (index == -1) {
        return;
    }
    if (index == m_tabBar->count()) {
        return;   // sentry tab
    }
    if (m_minimized) {
        showPopupStack();
    }
}

void RibbonBarPrivate::onCurrentRibbonTabDoubleClicked(int index)
{
    Q_Q(RibbonBar);
    if (index == -1) {
        return;
    }
    if (index == m_tabBar->count()) {
        return;   // sentry tab
    }
    q->setMinimized(!m_minimized);
}

void RibbonBarPrivate::onTabMoved(int from, int to)
{
    m_stack->moveWidget(from, to);
}

void RibbonBarPrivate::onStackWidgetHided()
{
    m_tabBar->setCurrentIndex(-1);
}

void RibbonBarPrivate::onFontChanged()
{
    RibbonElementStyleOpt.recalc();
}

/* RibbonBar */
RibbonBar::RibbonBar(QWidget *parent)
    : QMenuBar(parent)
{
    QX_INIT_PRIVATE(RibbonBar)
    Q_D(RibbonBar);
    d->init();
    if (parent) {
        connect(parent, &QWidget::windowTitleChanged, d, &RibbonBarPrivate::onWindowTitleChanged);
        connect(parent, &QWidget::windowIconChanged, d, &RibbonBarPrivate::onWindowIconChanged);
    }
    setRibbonStyle(OfficeStyle);
}

RibbonBar::~RibbonBar()
{
    QX_FINI_PRIVATE();
}

QAbstractButton *RibbonBar::applicationButton()
{
    Q_D(RibbonBar);
    return d->m_applicationButton;
}

/**
 * @brief Sets the applicationButton; pass Q_NULLPTR to hide it
 *
 * There is a RibbonApplicationButton by default; pass Q_NULLPTR to cancel it, or pass a custom button
 *
 * @note RibbonBar owns the applicationButton; do not delete it externally
 * @param btn the applicationButton pointer; @ref RibbonApplicationButton may be passed
 * RibbonApplicationButton already has style settings applied
 */
void RibbonBar::setApplicationButton(QAbstractButton *btn)
{
    Q_D(RibbonBar);
    d->setApplicationButton(btn);
    resizeRibbon();
}

RibbonPage *RibbonBar::addPage(const QString &title)
{
    return insertPage(-1, title);
}

void RibbonBar::addPage(RibbonPage *page)
{
    insertPage(-1, page);
}

RibbonPage *RibbonBar::insertPage(int index, const QString &title)
{
    RibbonPage *page = new RibbonPage(this);
    page->setObjectName(title);
    page->setPageName(title);
    insertPage(index, page);
    return page;
}

void RibbonBar::insertPage(int index, RibbonPage *page)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == page) {
        return;
    }
    page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
    page->setRibbonBar(this);

    int i = d->m_tabBar->insertTab(index, page->windowTitle());
    RibbonTabData tabdata;
    tabdata.page = page;
    tabdata.index = i;
    d->m_tabBar->setTabData(i, QVariant::fromValue(tabdata));
    if (index != -1) {
        d->updateTabData();
    }

    d->m_stack->insertWidget(i, page);

    connect(page, &QWidget::windowTitleChanged, d, &RibbonBarPrivate::onPageWindowTitleChanged);
    resizeRibbon();
}

/**
 * @brief Finds a page by index; returns Q_NULLPTR if the index is out of range
 * @param index index
 * @return Q_NULLPTR if the index is out of range
 * @note If context tabs exist, their pages are returned as well
 * @note A page found by index must be visible; a tab hidden with @ref hidePage cannot be found by index
 * @note @ref pageByObjectName finds all tabs that were ever added,
 * to get all tabs managed by the ribbonbar, use @ref pages
 * @see pageIndex pageByObjectName pageByName
 */
RibbonPage *RibbonBar::page(int index) const
{
    Q_D(const RibbonBar);
    QVariant var = d->m_tabBar->tabData(index);

    if (var.isValid()) {
        RibbonTabData data = var.value<RibbonTabData>();
        return data.page;
    }
    return Q_NULLPTR;
}

/**
 * @brief Finds a Page by name
 * @param title the Page name, i.e. the tab title
 * @return Q_NULLPTR if not found; if names are duplicated, the first match is returned, so avoid duplicate tab names
 * @note The title may change due to translation, so to find a page by fixed content use @ref pageByObjectName
 * @see pageByObjectName
 */
RibbonPage *RibbonBar::pageByName(const QString &title) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w && w->windowTitle() == title) {
            return w;
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Finds a Page by ObjectName
 * @param objname
 * @return Q_NULLPTR if not found; if the ObjectName is duplicated, the first match is returned, so avoid duplicate
 * ObjectNames
 * @see pageByName
 */
RibbonPage *RibbonBar::pageByObjectName(const QString &objname) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w && w->objectName() == objname) {
            return (w);
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Gets all currently visible RibbonPages; RibbonPages of hidden RibbonPageContexts are also returned
 *
 * @return
 */
QList<RibbonPage *> RibbonBar::pages(bool getAll) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();
    QList<RibbonPage *> res;

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w) {
            if (!getAll && w->isPageContext()) {
                // Skip context pages when getAll is false
                continue;
            }
            res.append(w);
        }
    }
    return res;
}

/**
 * @brief Checks whether this page is visible, i.e. the tabbar has this page
 * @param page
 * @return
 */
bool RibbonBar::isPageVisible(RibbonPage *page) const
{
    int tabindex = pageIndex(page);

    return (tabindex >= 0);
}

/**
 * @brief Shows a hidden page
 * @param page
 */
void RibbonBar::showPage(RibbonPage *page)
{
    Q_D(RibbonBar);
    for (auto i = d->m_hidedPage.begin(); i != d->m_hidedPage.end(); ++i) {
        if (i->page == page) {   // this is the one to show
            int index = d->m_tabBar->insertTab(i->index, i->page->windowTitle());
            i->index = index;
            d->m_tabBar->setTabData(index, QVariant::fromValue(*i));
            d->m_hidedPage.erase(i);   // remove
            d->updateTabData();        // update the index information
            break;
        }
    }
    raisePage(page);
}

/**
 * @brief Hides a page without deleting or removing it
 * @param page
 */
void RibbonBar::hidePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int tabcount = d->m_tabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d->m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData p = var.value<RibbonTabData>();
            if (p.page == page) {
                d->m_hidedPage.append(p);
                d->m_tabBar->removeTab(i);   // only remove the tab
                // After hiding the Page the context position changes, so it must be updated
                d->updateTabData();
                return;
            }
        }
    }
}

///
/// \brief Ensures the tab is shown and switches to the corresponding page
/// \param page tab pointer
///
void RibbonBar::raisePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int index = d->m_stack->indexOf(page);

    if (index >= 0) {
        setCurrentIndex(index);
    }
}

/**
 * @brief Removes a RibbonPage
 *
 * RibbonBar does not delete RibbonPage*, but the RibbonPage leaves RibbonBar's management
 * the tab is removed from the tabbar and the panel is removed; after this call the RibbonPage can be deleted
 * @param page
 */
void RibbonBar::removePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int index = d->tabIndex(page);
    bool isupdate = false;
    if (index >= 0) {
        d->m_tabBar->removeTab(index);
        isupdate = true;
    }
    d->m_stack->removeWidget(page);
    // Also check whether this page belongs to a context page
    for (RibbonPageContext *c : std::as_const(d->m_pageContextList)) {
        c->takePage(page);
    }
    // Refresh the index information of all tabdata
    if (isupdate) {
        d->updateTabData();
    }
    // Repaint after removal
    repaint();
    resizeRibbon();
}

void RibbonBar::clearPages()
{
    for (RibbonPage *page : pages(false)) {
        removePage(page);
        delete page;
    }
}

void RibbonBar::movePage(int from, int to)
{
    Q_D(RibbonBar);
    d->m_tabBar->moveTab(from, to);
    // Refresh the index information of all tabdata
    d->updateTabData();
    // This emits the tabMoved signal, which adjusts the window position in the stacked widget
}

int RibbonBar::pageIndex(RibbonPage *page) const
{
    Q_D(const RibbonBar);
    return d->tabIndex(page);
}

/**
 * @brief Adds a context tab
 *
 * A context tab is triggered on special occasions and must be triggered manually by the user
 *
 * Call @ref RibbonPageContext::addPage to add a RibbonPage to the context tab;
 * a RibbonPage added to a context tab is shown only while the context tab is shown
 * @param title the context tab title, shown in Office mode but not in wps mode. By default the RibbonPageContext's
 * object name is also set to title
 * @param color the context tab color; if an invalid QColor() is given, the RibbonBar default color set is used
 * @return the context tab pointer
 * @note RibbonBar owns the RibbonPageContext; do not delete it externally. To delete it, call @ref
 * destroyPageContext
 */
RibbonPageContext *RibbonBar::addPageContext(const QString &title, const QColor &color)
{
    Q_D(RibbonBar);
    RibbonPageContext *context = new RibbonPageContext(this);

    context->setObjectName(title);
    context->setContextTitle(title);
    context->setContextColor(color.isValid() ? color : d->getPageContextColor());
    addPageContext(context);
    return context;
}

void RibbonBar::addPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == context) {
        return;
    }
    connect(context, &RibbonPageContext::pageAdded, d, &RibbonBarPrivate::onPageContextAdded);
    // remove is not connected: after remove the page is not shown in the stacked widget either, and remove followed by
    // delete also deletes it from the stacked widget
    if (currentRibbonStyle() == WpsLiteStyle) {
        d->resizeInWpsLiteStyle();
    }
    d->m_pageContextList.append(context);
}

void RibbonBar::showPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (isPageContextVisible(context)) {
        return;
    }
    PageContextManagerData pageContextData;

    pageContextData.pageContext = context;
    int cnt = context->pageCount();
    for (int i = 0; i < context->pageCount(); ++i) {
        RibbonPage *page = context->page(i);
        // Setting the same mode again performs no extra work
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
        // Switching the mode changes the height; the context tab must be shown when it is displayed
        // page->setFixedHeight(pageHeight());
        int index = d->m_tabBar->addTab(page->windowTitle());
        pageContextData.tabPageIndex.append(index);

        RibbonTabData tabdata;
        tabdata.page = page;
        tabdata.index = index;
        d->m_tabBar->setTabData(index, QVariant::fromValue(tabdata));
        if (cnt > 1) {
            // When adding several pages in a row, update the size right after each page; relying on the final postEvent
            // briefly shows navigation arrows
            QRect r = d->m_tabBar->geometry();
            r.setWidth(d->calcMinTabBarWidth());
            d->m_tabBar->setGeometry(r);
        }
    }
    d->m_currentShowingPageContextList.append(pageContextData);
    // Since contexts are always appended last, updateTabData() is not needed;
    resizeRibbon();
}

void RibbonBar::hidePageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    bool needResize = false;

    for (int i = 0; i < d->m_currentShowingPageContextList.size(); ++i) {
        if (d->m_currentShowingPageContextList[i].pageContext == context) {
            const QList<int> &indexs = d->m_currentShowingPageContextList[i].tabPageIndex;
            for (int j = indexs.size() - 1; j >= 0; --j) {
                d->m_tabBar->removeTab(indexs[j]);
            }
            // After removing a ContextPage the tab indexes change, so all tabs after it must be renumbered
            needResize = true;
            d->m_currentShowingPageContextList.removeAt(i);
            break;
        }
    }
    if (needResize) {
        d->updateTabData();
        resizeRibbon();
    }
}

bool RibbonBar::isPageContextVisible(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    return d->isPageContextVisible(context);
}

void RibbonBar::setPageContextVisible(RibbonPageContext *context, bool visible)
{
    if (Q_NULLPTR == context) {
        return;
    }
    if (visible) {
        showPageContext(context);
    } else {
        hidePageContext(context);
    }
}

bool RibbonBar::isPageContextCoverTab()
{
    Q_D(RibbonBar);
    return d->m_pageContextCoverTab;
}

void RibbonBar::setPageContextCoverTab(bool cover)
{
    Q_D(RibbonBar);
    d->m_pageContextCoverTab = cover;
}

QList<RibbonPageContext *> RibbonBar::pageContextList() const
{
    Q_D(const RibbonBar);
    return d->m_pageContextList;
}

/**
 * @brief Destroys a context tab; its RibbonPages are destroyed as well
 * @param context the context tab pointer to destroy
 */
void RibbonBar::destroyPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == context) {
        return;
    }
    //! 1. If the context tab is shown, hide it first
    if (isPageContextVisible(context)) {
        hidePageContext(context);
    }
    //! 2. Remove the context tab's related content
    d->m_pageContextList.removeAll(context);

    QList<RibbonPage *> res = context->pageList();
    for (RibbonPage *page : std::as_const(res)) {
        page->hide();
        page->deleteLater();
    }
    context->deleteLater();
    resizeRibbon();
}

void RibbonBar::minimize()
{
    setMinimized(true);
}

bool RibbonBar::isMinimized() const
{
    Q_D(const RibbonBar);
    return d->m_minimized;
}

void RibbonBar::setMinimized(bool flag)
{
    Q_D(RibbonBar);
    d->setMinimizedFlag(flag);
    d->updateMinimumButtonIcon();
    QResizeEvent resizeEvent(size(), size());
    QApplication::sendEvent(this, &resizeEvent);
    Q_EMIT minimizationChanged(flag);
}

bool RibbonBar::haveShowMinimumButton() const
{
    Q_D(const RibbonBar);
    return (Q_NULLPTR != d->m_minimumPageButton);
}

void RibbonBar::showMinimumButton(bool isShow)
{
    Q_D(RibbonBar);
    if (isShow) {
        cornerButtonGroup(Qt::BottomRightCorner);   // create m_bottomRightButtonGroup
        if (Q_NULLPTR == d->m_minimumPageButton) {
            d->m_minimumPageButton = RibbonElementFactory->createHideGroupButton(this);
            d->m_minimumPageButton->ensurePolished();   // load the style icon
            QAction *action = new QAction(d->m_minimumPageButton);
            connect(action, &QAction::triggered, this, [=]() {
                this->setMinimized(!isMinimized());
            });
            d->m_minimumPageButton->setDefaultAction(action);
            d->m_bottomRightButtonGroup->addWidget(d->m_minimumPageButton);
            update();
        }
        d->updateMinimumButtonIcon();
    } else {
        if (Q_NULLPTR != d->m_minimumPageButton) {
            d->m_minimumPageButton->hide();
            d->m_minimumPageButton->deleteLater();
            d->m_minimumPageButton = Q_NULLPTR;
        }
    }
    QResizeEvent resizeEvent(size(), size());
    QApplication::sendEvent(this, &resizeEvent);
}

int RibbonBar::tabBarHeight() const
{
    return RibbonElementStyleOpt.tabBarHeight();
}

int RibbonBar::titleBarHeight() const
{
    return RibbonElementStyleOpt.titleBarHeight();
}

RibbonButtonGroup *RibbonBar::cornerButtonGroup(Qt::Corner corner)
{
    Q_D(RibbonBar);
    RibbonButtonGroup *buttonGroup = Q_NULLPTR;

    switch (corner) {
    case Qt::TopLeftCorner:
        if (Q_NULLPTR == d->m_topLeftButtonGroup) {
            d->m_topLeftButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_topLeftButtonGroup;
        break;
    case Qt::TopRightCorner:
        if (Q_NULLPTR == d->m_topRightButtonGroup) {
            d->m_topRightButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_topRightButtonGroup;
        break;
    case Qt::BottomLeftCorner:
        if (Q_NULLPTR == d->m_bottomLeftButtonGroup) {
            d->m_bottomLeftButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_bottomLeftButtonGroup;
        break;
    case Qt::BottomRightCorner:
        if (Q_NULLPTR == d->m_bottomRightButtonGroup) {
            d->m_bottomRightButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_bottomRightButtonGroup;
        break;
    default:
        break;
    }
    if (buttonGroup) {
        buttonGroup->show();
    }
    return buttonGroup;
}

RibbonQuickAccessBar *RibbonBar::quickAccessBar() const
{
    Q_D(const RibbonBar);
    return d->m_quickAccessBar->quickAccessBar();
}

void RibbonBar::setQuickAccessBarPosition(QuickAccessBarPosition position)
{
    Q_D(RibbonBar);
    if (d->m_quickAccessBarPosition == position) {
        return;
    }
    d->m_quickAccessBarPosition = position;
    if (!d->isOfficeStyle()) {
        resizeRibbon();
    }
}

RibbonBar::QuickAccessBarPosition RibbonBar::quickAccessBarPosition() const
{
    Q_D(const RibbonBar);
    return d->m_quickAccessBarPosition;
}

void RibbonBar::setTabBarPosition(RibbonBar::TabBarPosition position)
{
    Q_D(RibbonBar);
    if (d->m_tabBarPosition == position) {
        return;
    }
    d->m_tabBarPosition = position;
    resizeRibbon();
}

RibbonBar::TabBarPosition RibbonBar::tabBarPosition() const
{
    Q_D(const RibbonBar);
    return d->m_tabBarPosition;
}

RibbonBar::RibbonStyle RibbonBar::currentRibbonStyle() const
{
    Q_D(const RibbonBar);
    return d->m_ribbonStyle;
}

/**
 * @brief Sets the ribbonbar style; this resets all elements, including the button layout,
 * especially when switching from three rows to two rows, where much is reset
 * @note This function triggers ResizeEvent automatically; no manual call is needed
 * @param v the style, see @ref RibbonBar::RibbonStyle
 */
void RibbonBar::setRibbonStyle(RibbonBar::RibbonStyle v)
{
    Q_D(RibbonBar);
    d->m_ribbonStyle = v;
    d->m_lastShowStyle = v;
    d->m_quickAccessBar->setIconVisible(isOfficeStyle() && d->m_titleVisible);
    d->updateRibbonElementGeometry();

    /*
    QSize oldSize = size();
    QSize newSize(oldSize.width(), d->mainBarHeight());
    QResizeEvent es(newSize, oldSize);
    QApplication::sendEvent(this, &es);
    */
    // sendEvent prevents RibbonQuickAccessBar from updating its size after a style switch, so use postEvent instead
    // resizeRibbon();

    // NOTE: resizeRibbon() defers its work with postEvent(), and that delay is
    // observable here. Coming from the WPS style while the bar is minimized,
    // the switch to the OFFICE style reads m_tabBar->geometry().bottom() for
    // setFixedHeight() below before the tab bar has been resized, so the height
    // stays at the WPS value and the horizontal strip holding the tab bar does
    // not display. Calling the resize directly is the workaround: the widget
    // style change needs the new geometry now, not next event loop pass.
    // See doc/pages/limitations.md, "RibbonBar resize is deferred".
    d->resizeRibbon();
    update();

    if (isMinimized()) {
        // In minimized mode the bar height is the tabbar bottom; this adjustment must happen after the resize event
        d->m_blockResize = true;
        setFixedHeight(d->m_tabBar->geometry().bottom());
        d->m_blockResize = false;
    }
    Q_EMIT ribbonStyleChanged(d->m_ribbonStyle);
}

int RibbonBar::currentIndex()
{
    Q_D(RibbonBar);
    return d->m_tabBar->currentIndex();
}

void RibbonBar::setCurrentIndex(int index)
{
    Q_D(RibbonBar);
    d->m_tabBar->setCurrentIndex(index);
}

bool RibbonBar::isOfficeStyle() const
{
    Q_D(const RibbonBar);
    return d->isOfficeStyle();
}

bool RibbonBar::isTwoRowStyle() const
{
    Q_D(const RibbonBar);
    return d->isTwoRowStyle();
}

void RibbonBar::setWindowButtonGroup(QWidget *widget)
{
    Q_D(RibbonBar);
    d->m_windowButtonGroup = widget;
}

/**
 * @brief Updates the ribbon layout data; use this for key size changes that require relayout of the ribbon's items
 *
 * @note This function is slow; avoid calling it in performance-critical paths
 */
void RibbonBar::updateRibbonGeometry()
{
    Q_D(RibbonBar);
    d->updateRibbonBarHeight();
    QList<RibbonPage *> pages = this->pages();
    for (RibbonPage *page : std::as_const(pages)) {
        page->updateItemGeometry();
    }
}

void RibbonBar::updateRibbonTheme()
{
    Q_D(RibbonBar);
    d->updateMinimumButtonIcon();
}

void RibbonBar::resizeRibbon()
{
    // NOTE: doing this work directly instead of through postEvent() - the
    // resizeEvent alternative that was tried first - breaks the WPS style: after
    // switching between frameless and framed, m_applicationButton.isVisible()
    // starts returning false and the size computation below goes wrong. The
    // event is therefore queued, which is also what makes the switch visible as
    // a single repaint rather than two. See doc/pages/limitations.md,
    // "RibbonBar resize is deferred".
    //
    // This is the only place that queues the resize: every site that changes the
    // bar's content asks through here, so the reason above has one home instead
    // of one per caller.
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

Qt::Alignment RibbonBar::windowTitleAligment() const
{
    Q_D(const RibbonBar);
    return d->m_titleAligment;
}

void RibbonBar::setWindowTitleAligment(Qt::Alignment al)
{
    Q_D(RibbonBar);
    d->m_titleAligment = al;
}

void RibbonBar::setWindowTitleVisible(bool visible)
{
    Q_D(RibbonBar);
    d->m_titleVisible = visible;
    d->m_quickAccessBar->setIconVisible(isOfficeStyle() && visible);
}

QSize RibbonBar::sizeHint() const
{
    Q_D(const RibbonBar);
    QSize sz = QMenuBar::sizeHint();
    int w = 0;
    if (isOfficeStyle()) {
        w = d->minimumWidthInOfficeStyle();
    } else {
        w = d->minimumWidthInWpsLiteStyle();
    }
    sz.setWidth(w);
    return sz;
}

QSize RibbonBar::minimumSizeHint() const
{
    return sizeHint();
}

bool RibbonBar::event(QEvent *event)
{
    Q_D(RibbonBar);
    // KeyTips (B1): bare Alt enters/exits badge mode; while in the mode every key is
    // intercepted to drive cascade navigation. This must run before the base-class
    // QMenuBar::event() so the native menu logic does not swallow Alt first.
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (d->handleKeyTipKeyEvent(ke)) {
            return true;
        }
    }
    bool res = QMenuBar::event(event);

    switch (event->type()) {
    // QGuiApplication::fontChanged() is deprecated since Qt 6; the same
    // notification arrives as an event on every widget instead.
    case QEvent::ApplicationFontChange: {
        d->onFontChanged();
    } break;
    case QEvent::LayoutRequest: {
        // NOTE: reacting to LayoutRequest by re-running the ribbon resize can
        // cost more than one resize pass per event, and the conditions that
        // produce a LayoutRequest here have never been enumerated - nor has
        // which of the postEvent()/sendEvent() calls elsewhere could be
        // replaced by one. The handler is therefore gated on m_layoutRequest,
        // which is only set while the corner widget is a QMdiArea, so the cost
        // stays bounded by that case. See doc/pages/limitations.md,
        // "LayoutRequest can add resize passes".
        if (d->m_layoutRequest) {
            d->resizeRibbon();
            d->m_layoutRequest = false;
        }
    } break;
    default:
        break;
    }

    return res;
}

bool RibbonBar::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(RibbonBar);
    if (obj) {
        // Adjust button updates in window mode for multi-document setups
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            // For QMdiArea, maximizing produces a ShowToParent event, after which effective clicks on buttons (logo,
            // max/min, close) produce MouseButtonRelease events
            if ((QEvent::ShowToParent == e->type()) || (QEvent::MouseButtonRelease == e->type())) {
                d->m_layoutRequest = true;
            }
        } else if (obj == d->m_stack && d->m_stack->isPopup()) {
            // When the stack is in popup mode and the click is outside the stack area, if it is on the ribbonTabBar,
            // ignore this click Forward the click to the ribbonTabBar to avoid flicker from the stack hiding and
            // showing again after clicking the ribbonTabBar
            if ((QEvent::MouseButtonPress == e->type()) || (QEvent::MouseButtonDblClick == e->type())) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                QPoint pos = mouseEvent->pos();
                QPoint globalPos = QtCanpoolCompat::globalMousePos(mouseEvent);
#else
                QPoint pos = mouseEvent->position().toPoint();
                QPoint globalPos = mouseEvent->globalPosition().toPoint();
#endif
                if (!d->m_stack->rect().contains(pos)) {
                    QWidget *clickedWidget = QApplication::widgetAt(globalPos);
                    if (clickedWidget == d->m_tabBar) {
                        const QPoint targetPoint = clickedWidget->mapFromGlobal(globalPos);
                        QMouseEvent *evPress =
                            new QMouseEvent(mouseEvent->type(), targetPoint, globalPos, mouseEvent->button(),
                                            mouseEvent->buttons(), mouseEvent->modifiers());
                        QApplication::postEvent(clickedWidget, evPress);
                        return true;
                    }
                }
            }
        }
    }
    return QMenuBar::eventFilter(obj, e);
}

// ===== KeyTips (B1) =====

void RibbonBarPrivate::hideKeyTips()
{
    Q_Q(RibbonBar);
    m_keyTipMode = false;
    qDeleteAll(m_keyTips);
    m_keyTips.clear();
}

static QString keyTipMnemonic(const QString &title)
{
    // Extract the mnemonic letter from a "&File" style title; empty when absent
    const int idx = title.indexOf(QLatin1Char('&'));
    if (idx >= 0 && idx + 1 < title.size()) {
        const QChar c = title.at(idx + 1);
        if (!c.isSpace()) {
            return QString(c).toUpper();
        }
    }
    return QString();
}

void RibbonBarPrivate::renderKeyTips(const QList<KeyTipTarget> &targets)
{
    Q_Q(RibbonBar);
    hideKeyTips();
    if (targets.isEmpty() || !q->isVisible()) {
        return;
    }
    // Assign badge letters: mnemonics first, then auto-fill with A-Z
    QSet<QChar> used;
    QList<QString> keys(targets.size());
    for (int i = 0; i < targets.size(); ++i) {
        const QString &pref = targets.at(i).preferred;
        if (!pref.isEmpty() && !used.contains(pref.at(0))) {
            keys[i] = pref;
            used.insert(pref.at(0));
        }
    }
    static const QString pool = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    for (int i = 0; i < targets.size(); ++i) {
        if (!keys.at(i).isEmpty()) {
            continue;
        }
        for (const QChar &c : pool) {
            if (!used.contains(c)) {
                keys[i] = QString(c);
                used.insert(c);
                break;
            }
        }
    }
    m_keyTipMode = true;
    for (int i = 0; i < targets.size(); ++i) {
        if (keys.at(i).isEmpty()) {
            continue;   // Letter pool exhausted: drop this target
        }
        const KeyTipTarget &tg = targets.at(i);
        QRect rect;
        if (tg.pageIndex >= 0) {
            const QRect tabRect = m_tabBar->tabRect(tg.pageIndex);
            rect = QRect(m_tabBar->mapTo(q, tabRect.topLeft()), tabRect.size());
        } else if (tg.widget) {
            rect = QRect(tg.widget->mapTo(q, QPoint(0, 0)), tg.widget->size());
        } else {
            continue;
        }
        RibbonKeyTip *kt = new RibbonKeyTip(q);
        kt->setKey(keys.at(i));
        kt->setTarget(tg.widget);
        kt->setProperty("pageIndex", tg.pageIndex);
        const QSize sz = kt->sizeHint();
        const QPoint pos(rect.center().x() - sz.width() / 2, rect.top() + (rect.height() - sz.height()) / 2);
        kt->setGeometry(QRect(pos, sz));
        kt->show();
        kt->raise();
        m_keyTips.append(kt);
    }
}

void RibbonBarPrivate::showKeyTips()
{
    Q_Q(RibbonBar);
    // Level-1 targets: application button + visible page tabs
    QList<KeyTipTarget> targets;
    if (m_applicationButton && m_applicationButton->isVisible()) {
        targets.append({m_applicationButton, -1, QString()});
    }
    // Enumerate by tab index (same coordinate space as tabRect/setCurrentIndex,
    // so hidden pages cannot desync the badge positions)
    const int tabCount = m_tabBar->count();
    for (int i = 0; i < tabCount; ++i) {
        RibbonPage *p = q->page(i);
        if (!p || !q->isPageVisible(p)) {
            continue;
        }
        targets.append({Q_NULLPTR, i, keyTipMnemonic(p->title())});
    }
    renderKeyTips(targets);
}

void RibbonBarPrivate::showControlKeyTips()
{
    Q_Q(RibbonBar);
    // Level-2 targets: RibbonButtons inside the current page (keep the application button)
    QList<KeyTipTarget> targets;
    if (m_applicationButton && m_applicationButton->isVisible()) {
        targets.append({m_applicationButton, -1, QString()});
    }
    RibbonPage *page = q->page(q->currentIndex());
    if (page) {
        const QList<RibbonGroup *> groups = page->groups();
        for (RibbonGroup *g : groups) {
            const QList<RibbonButton *> btns = g->ribbonButtons();
            for (RibbonButton *b : btns) {
                if (!b->isVisible()) {
                    continue;
                }
                targets.append({b, -1, keyTipMnemonic(b->text())});
            }
        }
    }
    renderKeyTips(targets);
}

bool RibbonBarPrivate::handleKeyTipKeyEvent(QKeyEvent *ke)
{
    Q_Q(RibbonBar);
    if (m_keyTipMode) {
        if (ke->key() == Qt::Key_Escape || ke->key() == Qt::Key_Alt) {
            hideKeyTips();
            return true;
        }
        const QString t = ke->text().toUpper();
        if (!t.isEmpty()) {
            for (RibbonKeyTip *kt : m_keyTips) {
                if (kt->key().toUpper() != t) {
                    continue;
                }
                const int pageIndex = kt->property("pageIndex").toInt();
                QWidget *target = kt->target();
                if (target) {
                    // Control target: application button / RibbonButton, both fire on click
                    if (QAbstractButton *btn = qobject_cast<QAbstractButton *>(target)) {
                        btn->click();
                    }
                    hideKeyTips();
                } else if (pageIndex >= 0) {
                    // Tab target: switch to that page and descend into level-2 (in-page badges)
                    q->setCurrentIndex(pageIndex);
                    showControlKeyTips();
                } else {
                    hideKeyTips();
                }
                return true;
            }
            return true;   // No match: stay in the mode, awaiting Esc/Alt or the next key
        }
        return true;
    }
    // Not in the mode: bare Alt enters keytip mode (no Ctrl/Meta, to avoid stealing system shortcuts)
    if (ke->key() == Qt::Key_Alt) {
        if (!(ke->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))) {
            showKeyTips();
            return true;
        }
    }
    return false;
}

void RibbonBar::paintEvent(QPaintEvent *e)
{
    Q_D(RibbonBar);
    Q_UNUSED(e);
    QPainter p(this);
    if (isOfficeStyle()) {
        d->paintInOfficeStyle(p);
    } else {
        d->paintInWpsLiteStyle(p);
    }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    HELP_DRAW_RECT(p, d->m_quickAccessBar->geometry());
    HELP_DRAW_RECT(p, d->m_tabBar->geometry());
    HELP_DRAW_RECT(p, d->m_stack->geometry());
#endif
}

void RibbonBar::resizeEvent(QResizeEvent *e)
{
    Q_D(RibbonBar);
    Q_UNUSED(e);
    d->hideKeyTips();
    if (d->m_blockResize) {
        return;
    }
    d->resizeRibbon();
    update();
}

QX_RIBBON_END_NAMESPACE
