/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonbuttongroup.h"
#include "ribboncontrols.h"
#include "ribbongallery.h"
#include "ribbonmanager.h"
#include "ribbontabbar.h"
#include "ribbonutils.h"

#include <QApplication>
#include <QEvent>
#include <QLayout>
#include <QResizeEvent>
#include <QStyle>
#include <QWidget>
#include "qtcompat/qtcompat.h"

QX_RIBBON_BEGIN_NAMESPACE

const int tabBarMinimumWidth = 100;
const int dragAreaMinimumWidth = 20;
void RibbonBarPrivate::resizeInTabBarBelowStyle()
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
        // The height is the tab strip's, not the strip minus the baseline row: the button
        // carries a label of its own, and a rect one pixel shorter centres that label half
        // a pixel off every tab title beside it. The baseline is painted under the button
        // anyway, so the strip's bottom row costs nothing here.
        m_applicationButton->setGeometry(x, y, m_applicationButton->size().width(), tabH);
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

void RibbonBarPrivate::resizeInTabBarInlineStyle()
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
    const CornerBand band = cornerBandGeometry(y, centredHeight, stretchedHeight, cornerSize.height());
    corner->setGeometry(x, band.top, cornerSize.width(), band.height);
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
    if (hasOwnTabRow()) {
        resizeInTabBarBelowStyle();
    } else {
        resizeInTabBarInlineStyle();
    }
}

int RibbonBarPrivate::minimumWidthInTabBarBelowStyle() const
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

int RibbonBarPrivate::minimumWidthInTabBarInlineStyle() const
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
 * Whether a corner widget no longer sits where the ribbon put it.
 *
 * The corner widgets are the only children this class does not create: Qt puts an
 * MDI child's icon and window buttons there while that child is maximized, and an
 * application may put anything else. Two things move them behind this class's back.
 *
 * The widget itself can ask for a different size long after the bar was last laid
 * out - a longer label, a theme font, a widget that was hidden and is now shown - so
 * the placement computed by resizeRibbon() is stale from then on.
 *
 * QMenuBar can also place them itself, which is the case the width alone does not
 * catch. setCornerWidget() is not virtual, so the widgets Qt installs arrive through
 * the base class, and the base class lays corner widgets out on its own terms: across
 * the full height of the menu bar, centred on the left and top-aligned on the right
 * (Qt 5 and 6, QMenuBarPrivate::updateGeometries). That is where a menu bar's corners
 * belong - they own its whole height - and not where a ribbon's are, whose rows are
 * shorter than the bar; and Qt does it after this class last ran, because it is
 * reacting to the widget becoming visible. It gives them their size hint width while
 * it is at it, so only the row they end up on tells the two placements apart.
 *
 * This is the question the LayoutRequest handler asks, to re-run that placement
 * without paying for a pass on every layout request the bar's own widgets post.
 */
bool RibbonBarPrivate::cornerWidgetsNeedLayout() const
{
    Q_Q(const RibbonBar);
    const QMargins border = q->contentsMargins();
    const int titleH = q->titleBarHeight();
    const int tabH = q->tabBarHeight();
    const bool ownTabRow = hasOwnTabRow();

    // The bands the two resize functions hand to placeCornerWidget(): the below style
    // keeps the title row for the left corner and gives the tab row to the right one,
    // the inline style has a single row and uses it for both.
    struct CornerWidget {
        QWidget *widget;
        int top;
        int centredHeight;
        int stretchedHeight;
    };
    const CornerWidget corners[] = {
        {q->cornerWidget(Qt::TopLeftCorner), border.top(), titleH, titleH},
        {q->cornerWidget(Qt::TopRightCorner), ownTabRow ? border.top() + titleH : border.top(),
         ownTabRow ? tabH : titleH, ownTabRow ? tabH - tabBarBaseLineHeight : titleH},
    };
    for (const CornerWidget &corner : corners) {
        if (!takesPartInLayout(corner.widget)) {
            continue;
        }
        const QSize hint = corner.widget->sizeHint();
        const CornerBand band =
            cornerBandGeometry(corner.top, corner.centredHeight, corner.stretchedHeight, hint.height());
        const QRect placed = corner.widget->geometry();
        if (placed.width() != hint.width() || placed.top() != band.top || placed.height() != band.height) {
            return true;
        }
    }
    return false;
}

/*!
 * Whether the quick access bar stands at the width it asks for.
 *
 * What the bar's own layout gives the quick access bar is the width of its size
 * hint, and a style sheet is one of the things that changes what that hint is:
 * the shipped themes pad the buttons, so the bar wants more room after one lands
 * than it did before. A sheet that reaches the window after it was shown changes
 * the hint too late for the pass that answers it - this class is told about the
 * style before the tool bar inside it is - and nothing else asks afterwards.
 *
 * So the request that follows the change is the only chance to notice, and this
 * is the question the LayoutRequest handler puts to it: like a corner widget,
 * the quick access bar is a part of this bar whose size this class does not
 * decide, so a stale one is invisible to every other trigger. The button that
 * used to disappear when a theme was installed late was this staleness, with the
 * rest of the row cut off by the widget that follows it.
 */
bool RibbonBarPrivate::quickAccessBarNeedsLayout() const
{
    if (!takesPartInLayout(m_quickAccessBar)) {
        return false;
    }
    return m_quickAccessBar->width() != m_quickAccessBar->sizeHint().width();
}

/**
 * @brief Resets the tabbar width as needed, mainly for TabBarInlineStyle
 */
int RibbonBarPrivate::calcMinTabBarWidth() const
{
    const QMargins &mg = m_tabBar->tabMargin();
    int mintabBarWidth = 0;

    const int cnt = m_tabBar->count();

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
    // resizeEvent alternative that was tried first - breaks TabBarInlineStyle: after
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

void RibbonBar::resizeRibbonNow()
{
    // The event a real resize would have delivered, sent instead of posted: the
    // same work in the same order, and resizeEvent()'s m_blockResize guard still
    // applies. This is the idiom two callers inside this class already use -
    // setMinimized() and showMinimumButton() - given a name, so a caller outside
    // it does not have to build the event itself.
    QResizeEvent resizeEvent(size(), size());
    QApplication::sendEvent(this, &resizeEvent);
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
    d->m_quickAccessBar->setIconVisible(hasOwnTabRow() && visible);
}

QSize RibbonBar::sizeHint() const
{
    Q_D(const RibbonBar);
    QSize sz = QMenuBar::sizeHint();
    int w = 0;
    if (hasOwnTabRow()) {
        w = d->minimumWidthInTabBarBelowStyle();
    } else {
        w = d->minimumWidthInTabBarInlineStyle();
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
    case QEvent::PaletteChange: {
        // The minimize button is the only glyph this class paints from a mask, so it
        // is the only one that has to be repainted here; the gallery, the group
        // option button and the quick access bar all re-tint their own icons from
        // their own changeEvent().
        d->updateMinimumButtonIcon();
    } break;
    case QEvent::StyleChange: {
        // A style sheet decides how wide a tab is - the shipped themes pad
        // ::tab by 8 to 12 pixels - and the strip gets its width from adding
        // those tabs up (calcMinTabBarWidth, then layoutTabBar), not from a
        // layout that would follow the style on its own. So a theme switch has
        // to re-run that sum: without this the bar keeps the width the previous
        // sheet asked for, the wider tabs no longer fit, and QTabBar answers by
        // scrolling - a pair of arrows appears on the right of the strip.
        //
        // Queued, like every other resize in this class, and for one more
        // reason here: the style change reaches the tab bar after it reaches
        // this widget, and the tabs report their new width only once it has.
        d->resizeRibbon();
    } break;
    case QEvent::LayoutRequest: {
        // NOTE: reacting to LayoutRequest by re-running the ribbon resize can
        // cost more than one resize pass per event, and the conditions that
        // produce a LayoutRequest here have never been enumerated - nor has
        // which of the postEvent()/sendEvent() calls elsewhere could be
        // replaced by one. The handler is therefore gated, and the gate asks
        // about the parts of this bar whose size and position this class does
        // not decide, because a request from one of them is what can leave the
        // placement stale:
        //  - the corner widgets, including the request QMenuBar posts right
        //    after it placed them on its own terms, which is the only signal
        //    this bar gets for that (cornerWidgetsNeedLayout);
        //  - the quick access bar, whose size hint a style sheet changes after
        //    the pass that would have answered it has already run
        //    (quickAccessBarNeedsLayout).
        // Both questions are of the form "is what was placed still what is
        // asked for", so a yes is answered by one more pass and a no costs
        // nothing.
        // See doc/pages/limitations.md, "LayoutRequest can add resize passes".
        if (d->cornerWidgetsNeedLayout() || d->quickAccessBarNeedsLayout()) {
            d->resizeRibbon();
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
        if (obj == d->m_stack && d->m_stack->isPopup()) {
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
