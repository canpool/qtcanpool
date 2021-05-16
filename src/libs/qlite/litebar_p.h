/***************************************************************************
 **
 **  Copyright (C) 2019-2021 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef LITEBAR_P_H
#define LITEBAR_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QIcon>
#include "qlite_global.h"

class QLabel;
class QToolButton;
class QToolBar;

QLITE_BEGIN_NAMESPACE

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

    void setIconDark(bool dark);

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
    QWidget *m_titleBar;

    QToolButton *m_logoButton;
    QLabel *m_titleLabel;
    QToolBar *m_toolBar;
    QAction *m_closeAction;
    QAction *m_maximizeAction;
    QAction *m_minimizeAction;
    QIcon m_closeIcon;
    QIcon m_minimizeIcon;
    QIcon m_maximizeIcon;
    QIcon m_normalIcon;

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
    bool m_bLeftButtonTitlePressed;
};

QLITE_END_NAMESPACE

#endif // LITEBAR_P_H
