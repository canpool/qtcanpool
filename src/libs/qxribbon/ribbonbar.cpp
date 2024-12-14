/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonapplicationbutton.h"
#include "ribbonbuttongroup.h"
#include "ribboncontrols.h"
#include "ribbonmanager.h"
#include "ribbontabbar.h"
#include "ribbonquickaccessbar.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QHoverEvent>
#include <QLinearGradient>
#include <QPainter>
#include <QResizeEvent>
#include <QSet>
#include <QStyleOptionMenuItem>
#include <QTimer>
#include <QVariant>
#include <QEventLoop>
#include <QMouseEvent>
#include <QEvent>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

const int tabBarBaseLineHeight = 1;

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
            emit aboutToShow();
            break;
        case QEvent::Hide:
            emit aboutToHide();
            break;
        default:
            break;
        }
    }
    return QStackedWidget::event(event);
}

/**
 * @brief 类似 tabbar 的 moveTab 函数，交换两个窗口的 index
 * @param from
 * @param to
 * @note 此操作会触发 widgetRemoved(int index) 信号
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
{
    m_pageContextColorList << QColor(201, 89, 156)   // 玫红
                           << QColor(242, 203, 29)   // 黄
                           << QColor(255, 157, 0)    // 橙
                           << QColor(14, 81, 167)    // 蓝
                           << QColor(228, 0, 69)     // 红
                           << QColor(67, 148, 0)     // 绿
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
    m_tabBar->setObjectName(QStringLiteral("qx_RibbonStackedWidget"));
    connect(m_stack, &RibbonStackedWidget::aboutToHide, this, &RibbonBarPrivate::onStackWidgetHided);
    m_stack->installEventFilter(q);
    setMinimizedFlag(false);

    m_quickAccessBar = new RibbonQuickAccessBarContainer(q);
    m_quickAccessBar->setObjectName(QStringLiteral("qx_RibbonQuickAccessBar"));
    m_quickAccessBar->setIcon(q->windowIcon());

    connect(qApp, &QApplication::fontChanged, this, &RibbonBarPrivate::onFontChanged);
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
    // 刷新完tabdata信息也要接着刷新ContextPage信息
    updatePageContextManagerData();
}

/**
 * @brief 刷新所有ContextPageManagerData，这个在单独一个Page删除时调用
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
    // 根据样式调整 RibbonPage 的布局形式
    QList<RibbonPage *> pages = q->pages();
    for (RibbonPage *page : qAsConst(pages)) {
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
    }
    // 阻止因高度变化而会触发resizeEvent，进而执行resizeXX
    m_blockResize = true;
    updateRibbonBarHeight();
    m_blockResize = false;
}

void RibbonBarPrivate::updateRibbonBarHeight()
{
    Q_Q(RibbonBar);
    // 根据样式调整bar的高度
    if (m_minimized) {
        // 处于最小模式下时，bar 的高度为 tabbar 的 bottom
        // 最小模式时，bar 的高度在 setMinimizedFlag 中已调整
        // q->setFixedHeight(m_tabBar->geometry().bottom());
    } else {
        q->setFixedHeight(mainBarHeight());
    }
}

void RibbonBarPrivate::paintInOfficeStyle(QPainter &p)
{
    Q_Q(RibbonBar);
    paintBackground(p);

    //! 显示上下文标签
    p.save();
    QList<PageContextManagerData> pageContextDataList = m_currentShowingPageContextList;

    QPoint pageContextPos(q->width(), -1);   // x表示左边界，y表示右边界
    QMargins border = q->contentsMargins();
    for (int i = 0; i < pageContextDataList.size(); ++i) {
        QRect contextTitleRect;
        QList<int> indexs = pageContextDataList[i].tabPageIndex;
        QColor clr = pageContextDataList[i].pageContext->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = m_tabBar->tabRect(indexs.first());
            QRect endRect = m_tabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            // contextTitleRect是相对于m_tabBar返回的矩形框，不是RibbonBar中的真实位置，所以需要移动到m_tabBar的起点坐标处
            contextTitleRect.translate(m_tabBar->x(), m_tabBar->y());
            contextTitleRect.setHeight(m_tabBar->height() - 1);   // 减1像素，避免tabbar基线覆盖
            contextTitleRect -= m_tabBar->tabMargin() / 2;
            contextTitleRect.setTop(border.top());   // 把区域顶部扩展到窗口顶部
            paintPageContextTab(p, pageContextDataList[i].pageContext->contextTitle(), contextTitleRect, clr);
            // 更新上下文标签的范围，用于控制标题栏的显示。如果pageContextPos.y()小于0，说明没有显示上下文标签
            if (contextTitleRect.left() < pageContextPos.x()) {
                pageContextPos.setX(contextTitleRect.left());
            }
            if (contextTitleRect.right() > pageContextPos.y()) {
                pageContextPos.setY(contextTitleRect.right());
            }
        }
#ifdef QX_DRAW_CONTEXT_PAGE_BORDER
        if (indexs.contains(m_tabBar->currentIndex())) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            p.drawRect(m_stack->geometry());
        }
#endif
    }
    p.restore();

    //! 显示标题等
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

    //! 显示上下文标签
    p.save();
    QList<PageContextManagerData> pageContextDataList = m_currentShowingPageContextList;

    QMargins border = q->contentsMargins();
    for (int i = 0; i < pageContextDataList.size(); ++i) {
        QRect contextTitleRect;
        QList<int> indexs = pageContextDataList[i].tabPageIndex;
        QColor clr = pageContextDataList[i].pageContext->contextColor();
        if (!indexs.isEmpty()) {
            contextTitleRect = m_tabBar->tabRect(indexs.first());
            QRect endRect = m_tabBar->tabRect(indexs.last());
            contextTitleRect.setRight(endRect.right());
            contextTitleRect.translate(m_tabBar->x(), m_tabBar->y());
            contextTitleRect.setHeight(m_tabBar->height() - 1);
            contextTitleRect -= m_tabBar->tabMargin() / 2;
            contextTitleRect.setTop(border.top());   // 把区域顶部扩展到窗口顶部
            paintPageContextTab(p, QString(), contextTitleRect, clr);
        }
#ifdef QX_DRAW_CONTEXT_PAGE_BORDER
        if (indexs.contains(m_tabBar->currentIndex())) {
            QPen pen;
            pen.setColor(clr);
            pen.setWidth(1);
            p.setPen(pen);
            p.setBrush(Qt::NoBrush);
            p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            p.drawRect(m_stack->geometry());
        }
#endif
    }
    p.restore();

    //! 显示标题等
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
    // 在tabbar下绘制一条线
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
    // 首先有5像素的实体粗线位于顶部
    QMargins border = q->contentsMargins();
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawRect(QRect(contextRect.x(), border.top(), contextRect.width(), 5));

    // 剩下把颜色变亮90%
    QColor gColor = color.lighter(190);

    // 减去之前的5像素
    contextRect -= QMargins(0, 5, 0, 0);
    if (!m_pageContextCoverTab) {
        // 设置底部位置，不覆盖 tab
        contextRect.setBottom(m_tabBar->geometry().top());
    }
    painter.fillRect(contextRect, gColor);

    // 只有在office模式下才需要绘制标题
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
        // 绘制 tab 边框
        painter.setPen(gColor);
        painter.setBrush(Qt::NoBrush);
        QRect tabRect(contextRect.bottomLeft(),
                      QPoint(contextRect.bottomRight().x() - 1, m_tabBar->geometry().bottom()));
        painter.drawRect(tabRect);
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
    if (cornerL && cornerL->isVisible()) {
        QSize cornerSize = cornerL->sizeHint();
        if (cornerSize.height() < validTitleBarHeight) {
            int detal = (validTitleBarHeight - cornerSize.height()) / 2;
            cornerL->setGeometry(x, y + detal, cornerSize.width(), cornerSize.height());
        } else {
            cornerL->setGeometry(x, y, cornerSize.width(), validTitleBarHeight);
        }
        x = cornerL->geometry().right() + 5;
    }
    if (m_quickAccessBar && m_quickAccessBar->isVisible()) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        m_quickAccessBar->setGeometry(x, y, quickAccessBarSize.width(), validTitleBarHeight);
        x += quickAccessBarSize.width() + 5;
    }
    if (m_topLeftButtonGroup && m_topLeftButtonGroup->isVisible()) {
        QSize wSize = m_topLeftButtonGroup->sizeHint();
        m_topLeftButtonGroup->setGeometry(x, y, wSize.width(), validTitleBarHeight);
    }
    // ~~~~~~~~|m_topRightButtonGroup|m_windowButtonGroup|
    int endX = q->width() - border.right();
    if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
        QSize wSize = m_windowButtonGroup->sizeHint();
        endX -= wSize.width();
        m_windowButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (m_topRightButtonGroup && m_topRightButtonGroup->isVisible()) {
        QSize wSize = m_topRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_topRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }

    // row-2
    x = border.left();
    y += validTitleBarHeight;
    // |m_applicationButton|~~~~~ m_tabBar ~~~~~|m_bottomLeftButtonGroup|m_bottomRightButtonGroup|cornerW|
    if (m_applicationButton && m_applicationButton->isVisible()) {
        m_applicationButton->setGeometry(x, y, m_applicationButton->size().width(), otherH);
        x = m_applicationButton->geometry().right();
    }
    endX = q->width() - border.right();
    QWidget *cornerW = q->cornerWidget(Qt::TopRightCorner);
    if (cornerW && cornerW->isVisible()) {
        QSize cornerSize = cornerW->sizeHint();
        endX -= cornerSize.width();
        if (cornerSize.height() < tabH) {
            int detal = (tabH - cornerSize.height()) / 2;
            cornerW->setGeometry(endX, y + detal, cornerSize.width(), cornerSize.height());
        } else {
            cornerW->setGeometry(endX, y, cornerSize.width(), otherH);
        }
    }
    if (m_bottomRightButtonGroup && m_bottomRightButtonGroup->isVisible()) {
        QSize wSize = m_bottomRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomRightButtonGroup->setGeometry(endX, y, wSize.width(), otherH);
    }
    if (m_bottomLeftButtonGroup && m_bottomLeftButtonGroup->isVisible()) {
        QSize wSize = m_bottomLeftButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomLeftButtonGroup->setGeometry(endX, y, wSize.width(), otherH);
    }
    int allowedTabBarWidth = endX - x;
    if (m_tabBarPosition == RibbonBar::TBLeftPosition) {
        m_tabBar->setGeometry(x, y, allowedTabBarWidth, tabH);
    } else { // RibbonBar::TBCenterPosition
        int minTabBarWidth = calcMinTabBarWidth();
        if (minTabBarWidth < allowedTabBarWidth) {
            // |~~~~~~~~~~~~~~~~~~ allowedTabBarWidth ~~~~~~~~~~~~~~|
            // |~~ xOffset ~~|~~~~ mintabBarWidth ~~~~|~~ xOffset ~~|
            int xOffset = (allowedTabBarWidth - minTabBarWidth) / 2;
            m_tabBar->setGeometry(x + xOffset, y, minTabBarWidth, tabH);
        } else {
            m_tabBar->setGeometry(x, y, allowedTabBarWidth, tabH);
        }
    }

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
    if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
        QSize wSize = m_windowButtonGroup->sizeHint();
        endX -= wSize.width();
        m_windowButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    QWidget *cornerR = q->cornerWidget(Qt::TopRightCorner);
    if (cornerR && cornerR->isVisible()) {
        QSize cornerSize = cornerR->sizeHint();
        endX -= cornerSize.width();
        if (cornerSize.height() < validTitleBarHeight) {
            int detal = (validTitleBarHeight - cornerSize.height()) / 2;
            cornerR->setGeometry(endX, y + detal, cornerSize.width(), cornerSize.height());
        } else {
            cornerR->setGeometry(endX, y, cornerSize.width(), validTitleBarHeight);
        }
    }
    if (m_bottomRightButtonGroup && m_bottomRightButtonGroup->isVisible()) {
        QSize wSize = m_bottomRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (m_topRightButtonGroup && m_topRightButtonGroup->isVisible()) {
        QSize wSize = m_topRightButtonGroup->sizeHint();
        endX -= wSize.width();
        m_topRightButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    if (m_bottomLeftButtonGroup && m_bottomLeftButtonGroup->isVisible()) {
        QSize wSize = m_bottomLeftButtonGroup->sizeHint();
        endX -= wSize.width();
        m_bottomLeftButtonGroup->setGeometry(endX, y, wSize.width(), validTitleBarHeight);
    }
    // ~~~~|m_quickAccessBar|m_bottomLeftButtonGroup|~~~~~
    if (m_quickAccessBarPosition == RibbonBar::QABRightPosition && m_quickAccessBar && m_quickAccessBar->isVisible()) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        endX -= quickAccessBarSize.width();
        m_quickAccessBar->setGeometry(endX, y, quickAccessBarSize.width(), validTitleBarHeight);
    }
    // ~~~~|cornerL|<m_quickAccessBar>|m_bottomLeftButtonGroup|~~~~~
    QWidget *cornerL = q->cornerWidget(Qt::TopLeftCorner);
    if (cornerL && cornerL->isVisible()) {
        QSize cornerSize = cornerL->sizeHint();
        endX -= cornerSize.width();
        if (cornerSize.height() < validTitleBarHeight) {
            int detal = (validTitleBarHeight - cornerSize.height()) / 2;
            cornerL->setGeometry(endX, y + detal, cornerSize.width(), cornerSize.height());
        } else {
            cornerL->setGeometry(endX, y, cornerSize.width(), validTitleBarHeight);
        }
    }

    // tab 的y值需要重新计算
    int tabH = q->tabBarHeight();
    if (tabH > validTitleBarHeight) {
        // 这种直接把tabH设置为validTitleBarHeight
        tabH = validTitleBarHeight;
    }
    // 如果tabH较小，则下移，让tab底部和title的底部对齐
    y += validTitleBarHeight - tabH;

    // |m_applicationButton|<m_quickAccessBar>|m_topLeftButtonGroup|m_tabBar|~~~~~~
    if (m_applicationButton && m_applicationButton->isVisible()) {
        m_applicationButton->setGeometry(x, y, m_applicationButton->size().width(), tabH);
        x = m_applicationButton->geometry().right() + 2;
    }
    if (m_quickAccessBarPosition == RibbonBar::QABLeftPosition && m_quickAccessBar && m_quickAccessBar->isVisible()) {
        QSize quickAccessBarSize = m_quickAccessBar->sizeHint();
        m_quickAccessBar->setGeometry(x, border.top(), quickAccessBarSize.width(), validTitleBarHeight);
        x = m_quickAccessBar->geometry().right() + 2;
    }
    if (m_topLeftButtonGroup && m_topLeftButtonGroup->isVisible()) {
        QSize wSize = m_topLeftButtonGroup->sizeHint();
        m_topLeftButtonGroup->setGeometry(x, y, wSize.width(), tabH);
        x = m_topLeftButtonGroup->geometry().right() + 2;
    }
    int allowedTabBarWidth = endX - x;
    // 20200831
    // tabBarWidth的宽度原来为endX - x;，现需要根据实际进行调整
    // 为了把tabbar没有tab的部分不占用，这里的宽度需要根据tab的size来进行设置，让tabbar的长度刚刚好，这样能让出
    // mainwindow的空间，接受鼠标事件，从而实现拖动等操作，否则tabbar占用整个顶栏，鼠标无法点击到mainwindow
    int minTabBarWidth = calcMinTabBarWidth();
    if (m_tabBarPosition == RibbonBar::TBLeftPosition) {
        if (minTabBarWidth < allowedTabBarWidth) {
            allowedTabBarWidth = minTabBarWidth;
        }
        m_tabBar->setGeometry(x, y, allowedTabBarWidth, tabH);
    } else { // RibbonBar::TBCenterPosition
        if (minTabBarWidth < allowedTabBarWidth) {
            // |~~~~~~~~~~~~~~~~~~ allowedTabBarWidth ~~~~~~~~~~~~~~|
            // |~~ xOffset ~~|~~~~ mintabBarWidth ~~~~|~~ xOffset ~~|
            int xOffset = (allowedTabBarWidth - minTabBarWidth) / 2;
            m_tabBar->setGeometry(x + xOffset, y, minTabBarWidth, tabH);
        } else {
            m_tabBar->setGeometry(x, y, allowedTabBarWidth, tabH);
        }
    }

    resizeStackedWidget();
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

/**
 * @brief 根据情况重置tabbar的宽度，主要针对wps模式
 */
int RibbonBarPrivate::calcMinTabBarWidth() const
{
    const QMargins &mg = m_tabBar->tabMargin();
    int mintabBarWidth = 0;

    for (int i = 0; i < m_tabBar->count(); ++i) {
        mintabBarWidth += m_tabBar->tabRect(i).width();
    }
    mintabBarWidth += (mg.left() + mg.right());
    return mintabBarWidth;
}

/**
 * @brief mainBarHeight 的计算高度
 *
 * 有可能 RibbonBar::height 和 mainBarHeight 不相等，这种情况发生在 minimized 状态下
 * @return 高度
 */
int RibbonBarPrivate::mainBarHeight() const
{
    return RibbonElementStyleOpt.ribbonBarHeight(m_ribbonStyle);
}

/**
 * @brief 根据RibbonPage*指针查找tabbar的index
 *
 * @param page RibbonPage对应的QObject指针
 * @return 如果没有找到，返回-1
 * @note 此函数不会调用RibbonPage*的任何方法，因此可以在RibbonPage的destroyed槽中调用
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
        // 鉴于tab不会很多，不考虑效率问题
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
    m_stack->addWidget(page);   // 这里stackedWidget用append，其他地方都应该使用insert
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
            if (!m_stack->isVisible() && m_stack->isPopup()) {
                // 在StackedWindget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
                QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToGlobal(QCursor::pos()),
                                m_tabBar->mapToGlobal(QCursor::pos()));
#else
                QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToParent(QCursor::pos()),
                                m_tabBar->mapToGlobal(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()));
#endif
                QApplication::sendEvent(m_tabBar, &ehl);
                resizeStackedWidget();
                m_stack->setFocus();
                m_stack->show();
                // 在最小模式下，每次显示完StackedWindget后把tab的
                // 的index设置为-1，这样每次点击都会触发onCurrentRibbonTabChanged
            }
        }
    }
    emit q->currentRibbonTabChanged(index);
}

/**
 * @brief ribbon tab bar单击
 *
 * 此实现必须在eventfilter中传递stackedwidget的QEvent::MouseButtonDblClick事件到tabbar中，否则会导致双击变两次单击
 *
 * 单击事件仅用于响应点击同一个tab时
 * @param index
 */
void RibbonBarPrivate::onCurrentRibbonTabClicked(int index)
{
    if (index != m_tabBar->currentIndex()) {
        // 点击的标签不一致通过changed槽去处理
        return;
    }
    if (m_minimized) {
        if (!m_stack->isVisible() && m_stack->isPopup()) {
            // 在StackedWindget弹出前，先给tabbar一个QHoverEvent,让tabbar知道鼠标已经移开
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
            QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToGlobal(QCursor::pos()),
                            m_tabBar->mapToGlobal(QCursor::pos()));
#else
            QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToParent(QCursor::pos()),
                            m_tabBar->mapToGlobal(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()));
#endif
            QApplication::sendEvent(m_tabBar, &ehl);
            // 弹出前都调整一下位置，避免移动后位置异常
            resizeStackedWidget();
            m_stack->setFocus();
            m_stack->show();
        }
    }
}

void RibbonBarPrivate::onCurrentRibbonTabDoubleClicked(int index)
{
    Q_Q(RibbonBar);
    if (index == -1) {
        return;
    }
    q->setMinimized(!m_minimized);
}

void RibbonBarPrivate::onTabMoved(int from, int to)
{
    m_stack->moveWidget(from, to);
}

void RibbonBarPrivate::onStackWidgetHided()
{
    // m_tabBar->repaint();
}

void RibbonBarPrivate::onFontChanged(const QFont &font)
{
    Q_UNUSED(font);
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
 * @brief 设置applicationButton,如果想隐藏，可以传入Q_NULLPTR
 *
 * 默认会有一个RibbonApplicationButton，如果想取消，可传入Q_NULLPTR，或者自定义的button也可以传入
 *
 * @note applicationButton的所有权归RibbonBar所有，不要在外部对applicationButton进行delete操作
 * @param btn applicationButton指针，可以传入@ref RibbonApplicationButton，
 * 已经对RibbonApplicationButton进行了样式设置
 */
void RibbonBar::setApplicationButton(QAbstractButton *btn)
{
    Q_D(RibbonBar);
    d->setApplicationButton(btn);
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

/**
 * @brief 通过索引找到page，如果超过索引范围，会返回Q_NULLPTR
 * @param index 索引
 * @return 如果超过索引范围，会返回Q_NULLPTR
 * @note 如果此时有上下文标签，上下文的标签也会返回
 * @note 通过索引查找的page必须是visible状态的page，如果通过@ref hidePage 隐藏的标签，通过索引是找不到的
 * @note 通过@ref pageByObjectName 可以找到所有加入过的标签，
 * 如果想得到ribbonbar管理的所有标签，可以通过函数@ref pages 得到
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
 * @brief 通过名字查找Page
 * @param title Page的名字，既标签的标题
 * @return 如果没有找到，将返回Q_NULLPTR，如果有重名，将返回第一个查询到的名字，因此，尽量避免重名标签
 * @note 由于翻译等原因，可能title会变化，因此如果想通过固定内容查找page，应该使用 @ref pageByObjectName
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
 * @brief 通过ObjectName查找Page
 * @param objname
 * @return 如果没有找到，将返回Q_NULLPTR，如果有同样的ObjectName，将返回第一个查询到的名字，因此，尽量避免ObjectName重名
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
 * @brief 获取当前显示的所有的RibbonPage，包含未显示的RibbonPageContext的RibbonPage也一并返回
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
                // 不是getall且是上下文时跳过
                continue;
            }
            res.append(w);
        }
    }
    return res;
}

/**
 * @brief 判断这个page是否在显示状态，也就是tabbar有这个page
 * @param page
 * @return
 */
bool RibbonBar::isPageVisible(RibbonPage *page) const
{
    int tabindex = pageIndex(page);

    return (tabindex >= 0);
}

/**
 * @brief 显示被隐藏的page
 * @param page
 */
void RibbonBar::showPage(RibbonPage *page)
{
    Q_D(RibbonBar);
    for (auto i = d->m_hidedPage.begin(); i != d->m_hidedPage.end(); ++i) {
        if (i->page == page) {   // 说明要显示
            int index = d->m_tabBar->insertTab(i->index, i->page->windowTitle());
            i->index = index;
            d->m_tabBar->setTabData(index, QVariant::fromValue(*i));
            d->m_hidedPage.erase(i);   // 移除
            d->updateTabData();        // 更新index信息
            break;
        }
    }
    raisePage(page);
}

/**
 * @brief 隐藏page,并不会删除或者取走，只是隐藏
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
                d->m_tabBar->removeTab(i);   // 仅仅把tab移除
                // 注意Page隐藏后，contex的位置就会发生变化，需要更新
                d->updateTabData();
                return;
            }
        }
    }
}

///
/// \brief 确保标签显示出来，tab并切换到对应页
/// \param page 标签指针
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
 * @brief 移除RibbonPage
 *
 * RibbonBar不会delete RibbonPage*，但这个RibbonPage会脱离RibbonBar的管理
 * 表现在tabbar会移除，面板会移除，使用此函数后可以对RibbonPage进行delete
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
    // 同时验证这个page是否是contexpage里的
    for (RibbonPageContext *c : qAsConst(d->m_pageContextList)) {
        c->takePage(page);
    }
    // 这时要刷新所有tabdata的index信息
    if (isupdate) {
        d->updateTabData();
    }
    // 移除完后需要重绘
    repaint();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}

void RibbonBar::movePage(int from, int to)
{
    Q_D(RibbonBar);
    d->m_tabBar->moveTab(from, to);
    // 这时要刷新所有tabdata的index信息
    d->updateTabData();
    // 这里会触发tabMoved信号，在tabMoved信号中调整stacked里窗口的位置
}

int RibbonBar::pageIndex(RibbonPage *page) const
{
    Q_D(const RibbonBar);
    return d->tabIndex(page);
}

/**
 * @brief 添加上下文标签
 *
 * 上下文标签是特殊时候触发的标签，需要用户手动触发
 *
 * 调用@ref RibbonPageContext::addPage 可在上下文标签中添加RibbonPage，
 * 在上下文标签添加的RibbonPage，只有在上下文标签显示的时候才会显示
 * @param title 上下文标签的标题，在Office模式下会显示，在wps模式下不显示。默认情况下RibbonPageContext的object
 * name也被设置为title
 * @param color 上下文标签的颜色，如果指定为空QColor(),将会使用RibbonBar的默认色系
 * @return 返回上下文标签指针
 * @note RibbonBar拥有RibbonPageContext的管理权，用户避免在外部直接delete,如果要删除，调用@ref
 * destroyPageContext 函数
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
    // remove并没有绑定，主要是remove后在stacked里也不会显示，remove且delete的话，stacked里也会删除
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
        // 此句如果模式重复设置不会进行多余操作
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
        // 切换模式后会改变高度，上下文标签显示时要保证显示出来
        // page->setFixedHeight(pageHeight());
        int index = d->m_tabBar->addTab(page->windowTitle());
        pageContextData.tabPageIndex.append(index);

        RibbonTabData tabdata;
        tabdata.page = page;
        tabdata.index = index;
        d->m_tabBar->setTabData(index, QVariant::fromValue(tabdata));
        if (cnt > 1) {
            // 如果连续添加多页，每添加一页就立即更新一下尺寸，如果依赖最后的postEvent会短暂出现导航箭头
            QRect r = d->m_tabBar->geometry();
            r.setWidth(d->calcMinTabBarWidth());
            d->m_tabBar->setGeometry(r);
        }
    }
    d->m_currentShowingPageContextList.append(pageContextData);
    // 由于上下文都是在最后追加，不需要调用updateTabData();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
            // 注意，删除ContextPage后，tab的序号就会改变，这时，这个tab后面的都要调整它的序号
            needResize = true;
            d->m_currentShowingPageContextList.removeAt(i);
            break;
        }
    }
    if (needResize) {
        d->updateTabData();
        QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
 * @brief 销毁上下文标签，上下文标签的RibbonPage也会随之销毁
 * @param context 需要销毁的上下文标签指针
 */
void RibbonBar::destroyPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == context) {
        return;
    }
    //! 1、如果上下文标签显示中，先隐藏
    if (isPageContextVisible(context)) {
        hidePageContext(context);
    }
    //! 2、删除上下文标签的相关内容
    d->m_pageContextList.removeAll(context);

    QList<RibbonPage *> res = context->pageList();
    for (RibbonPage *page : qAsConst(res)) {
        page->hide();
        page->deleteLater();
    }
    context->deleteLater();
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
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
    emit minimizationChanged(flag);
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
            d->m_minimumPageButton->ensurePolished();   // 载入样式图标
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
 * @brief 设置ribbonbar的风格，此函数会重新设置所有元素，包括button的布局方式，
 * 尤其是从三行变到两行的过程，重设的内容较多
 * @note 此函数会自动触发ResizeEvent，不需要手动调用
 * @param v 样式，见@ref RibbonBar::RibbonStyle
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
    // sendEvent会导致RibbonQuickAccessBar在样式切换后无法更新尺寸，改为用postEvent
    resizeRibbon();

    if (isMinimized()) {
        // 处于最小模式下时，bar的高度为tabbar的bottom,这个调整必须在resize event之后
        d->m_blockResize = true;
        setFixedHeight(d->m_tabBar->geometry().bottom());
        d->m_blockResize = false;
    }
    emit ribbonStyleChanged(d->m_ribbonStyle);
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
 * @brief 更新ribbon的布局数据，此函数适用于一些关键性尺寸变化，换起ribbon下面元素的布局
 *
 * @note 此函数调用较慢，避免在高速要求场合调用
 */
void RibbonBar::updateRibbonGeometry()
{
    Q_D(RibbonBar);
    d->updateRibbonBarHeight();
    QList<RibbonPage *> pages = this->pages();
    for (RibbonPage *page : qAsConst(pages)) {
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
    // FIXME: 此处使用resizeEvent中的d->resizeRibbon()和update()来代替postEvent会出现如下问题：
    // WPS模式下，无边框和有边框之间切换后,
    // m_applicationButton.isVisible()突然返回false，导致d->resizeRibbon()中计算尺寸异常
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

bool RibbonBar::event(QEvent *event)
{
    Q_D(RibbonBar);
    bool res = QMenuBar::event(event);

    switch (event->type()) {
    case QEvent::LayoutRequest: {
        // FIXME: 引入LayoutRequest后，如果处理不当，会增加resize次数，需要梳理触发LayoutRequest的条件，
        // 同时，需要梳理哪些postEvent或sendEvent可以由LayoutRequest代替。
        // 目前，当cornerWidget为QMdiArea时，根据有效事件来设置m_layoutRequest
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
        // 调整多文档时在窗口模式下的按钮更新
        if ((obj == cornerWidget(Qt::TopLeftCorner)) || (obj == cornerWidget(Qt::TopRightCorner))) {
            // 对于QMdiArea，最大化时会存在ShowToParent事件，之后有效点击按钮（logo、最大/小、关闭按钮）会存在MouseButtonRelease事件
            if ((QEvent::ShowToParent == e->type()) || (QEvent::MouseButtonRelease == e->type())) {
                d->m_layoutRequest = true;
            }
        } else if (obj == d->m_stack && d->m_stack->isPopup()) {
            // 在 stack 是 popup 模式时，点击 stack 区域外的时候，如果是在 ribbonTabBar 上点击, 那么忽略掉这次点击
            // 把点击下发到 ribbonTabBar, 这样可以避免在点击 ribbonTabBar 后 stack 隐藏又显示，出现闪烁
            if ((QEvent::MouseButtonPress == e->type()) || (QEvent::MouseButtonDblClick == e->type())) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                QPoint pos = mouseEvent->pos();
                QPoint globalPos = mouseEvent->globalPos();
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
    if (d->m_blockResize) {
        return;
    }
    d->resizeRibbon();
    update();
}

QX_RIBBON_END_NAMESPACE
