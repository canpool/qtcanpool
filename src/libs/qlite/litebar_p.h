/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef LITEBAR_P_H
#define LITEBAR_P_H

#include <QObject>
#include <QMouseEvent>
#include <QHoverEvent>

class QLabel;
class QToolButton;
class QToolBar;

namespace QLite {

class LiteBar;

class LiteCursor
{
public:
    LiteCursor();

    void reset();
    void recalculate(const QPoint &gMousePos, const QRect &frameRect);

public:
    bool m_bOnEdges              : true;
    bool m_bOnLeftEdge           : true;
    bool m_bOnRightEdge          : true;
    bool m_bOnTopEdge            : true;
    bool m_bOnBottomEdge         : true;
    bool m_bOnTopLeftEdge        : true;
    bool m_bOnBottomLeftEdge     : true;
    bool m_bOnTopRightEdge       : true;
    bool m_bOnBottomRightEdge    : true;

    int m_nBorderWidth          : 5;
};

class LiteBarPrivate : public QObject
{
    Q_OBJECT
public:
    LiteBarPrivate();
    void init();
    void installWidget(QWidget *w);

    void handleWidgetMouseEvent(QObject *obj, QEvent *event);

    // window
    bool windowTitleChange(QObject *obj);
    bool windowIconChange(QObject *obj);
    void windowSizeChange(QObject *obj);
    void windowStateChange(QObject *obj);

    void updateWindowButtons();

private:
    // mouse event
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleLeaveEvent(QEvent *event);
    void handleHoverMoveEvent(QHoverEvent *event);
    void handleMouseDblClickEvent(QMouseEvent *event);

    // widget action
    void restoreWidget(QWidget *pWidget);
    void maximizeWidget(QWidget *pWidget);
    void resizeWidget(const QPoint &gMousePos);
    void moveWidget(const QPoint &gMousePos);

    void doubleMoveWidget();

    // mouse shape
    void updateCursorShape(const QPoint &gMousePos);
    QPoint calcDragPoint(QWidget *pWindow, QMouseEvent *event) const;

    QRect validDragRect();

private slots:
    void slotSysButtonClicked();

public:
    LiteBar *q;

    QToolButton *m_logoButton;
    QLabel *m_titleLabel;
    QToolBar *m_toolBar;
    QAction *m_closeAction;
    QAction *m_maximizeAction;
    QAction *m_minimizeAction;

    QWidget *m_mainWidget;
    QPoint m_dragPos;
    LiteCursor m_pressCursor;
    LiteCursor m_moveCursor;
    bool m_bEdgePressed;
    bool m_bCursorShapeChanged;
    Qt::WindowFlags m_windowFlags;
    bool m_bWidgetResizable;
    bool m_bWidgetMovable;
    bool m_bWidgetMaximizable;

    bool m_isMaximized;
    bool m_isMinimized;

    // screen
    int m_currentScreen;

    QRect m_normalRect;
    QPoint m_movePoint;
    QPoint m_dragPoint;
    QPoint m_mousePoint;
    bool m_bLeftButtonPressed;
    bool m_bLeftButtonDbClicked;
};

} // namespace QLite

#endif // LITEBAR_P_H
