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
#include <QLinearGradient>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionMenuItem>
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

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
void RibbonBarPrivate::paintInTabBarBelowStyle(QPainter &p)
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

void RibbonBarPrivate::paintInTabBarInlineStyle(QPainter &p)
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
 * The tab text is drawn only when the tab bar has a row of its own (decided inside paintPageContextTab), so the title
 * is always passed here.
 *
 * @param extendToWindowTop with an own tab row the top of each visible rect is raised to the window top (the tab
 * strip sits above the tab bar); with the tab bar on the title row it already shares that top edge, so pass false to
 * keep it unchanged.
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

    // The title is only drawn when the tab bar has a row of its own
    if (q->hasOwnTabRow()) {
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

void RibbonBar::paintEvent(QPaintEvent *e)
{
    Q_D(RibbonBar);
    Q_UNUSED(e);
    QPainter p(this);
    if (hasOwnTabRow()) {
        d->paintInTabBarBelowStyle(p);
    } else {
        d->paintInTabBarInlineStyle(p);
    }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    HELP_DRAW_RECT(p, d->m_quickAccessBar->geometry());
    HELP_DRAW_RECT(p, d->m_tabBar->geometry());
    HELP_DRAW_RECT(p, d->m_stack->geometry());
#endif
}

QX_RIBBON_END_NAMESPACE
