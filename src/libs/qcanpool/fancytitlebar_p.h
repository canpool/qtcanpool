/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYTITLEBAR_P_H
#define FANCYTITLEBAR_P_H

//
// W A R N I N G
// -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcanpool.h"
#include <QObject>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QLabel>

#ifdef QTC_USE_NATIVE
#include <QAbstractNativeEventFilter>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QTRESULT qintptr
#else
#define QTRESULT long
#endif // QTRESULT
#endif // QTC_USE_NATIVE

QCANPOOL_BEGIN_NAMESPACE

/* FancyCursor */
class FancyCursor
{
public:
    FancyCursor();

    void reset();
    void update(const QPoint &gMousePos, const QRect &frameRect);

public:
    bool m_bOnEdges;
    bool m_bOnLeftEdge;
    bool m_bOnRightEdge;
    bool m_bOnTopEdge;
    bool m_bOnBottomEdge;
    bool m_bOnTopLeftEdge;
    bool m_bOnBottomLeftEdge;
    bool m_bOnTopRightEdge;
    bool m_bOnBottomRightEdge;

    int m_nBorderWidth;
};

/* FancyScreen */
class FancyScreen
{
public:
    FancyScreen();

    int currentScreen(const int x);
    QRect screenRect(const int current);

    static QRect normalRect();

private:
    QList<QRect> m_screenRects;
};

class FancyTitleBar;

/* FancyTitleBarPrivate */
class FancyTitleBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyTitleBarPrivate(QWidget *mainWidget);
    virtual ~FancyTitleBarPrivate();

    virtual void init();
    virtual void setDisabled(bool disable);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    void updateWindowButtons();

public:
    // window
    bool windowTitleChange(QObject *obj);
    bool windowIconChange(QObject *obj);
    void windowSizeChange(QObject *obj);
    void windowStateChange(QObject *obj);

    void restoreWidget(QWidget *pWidget);
    void maximizeWidget(QWidget *pWidget);

public slots:
    virtual void systemButtonClicked();

public:
    FancyTitleBar   *q;
    QWidget         *m_mainWidget;
    QWidget         *m_titleWidget;

    // title widget
    QToolButton     *m_logoButton;
    QLabel          *m_titleLabel;
    QToolBar        *m_toolBar;
    QAction         *m_minimizeAction;
    QAction         *m_maximizeAction;
    QAction         *m_closeAction;

    QIcon           m_maximizeIcon;
    QIcon           m_normalIcon;

    QString         m_maximizeTip;
    QString         m_normalTip;

    // main window
    Qt::WindowFlags m_windowFlags;
    bool            m_isMaximized;
    bool            m_isMinimized;
    bool            m_isDisabled;
    bool            m_bWidgetMaximizable;
    bool            m_bWidgetResizable;
    bool            m_bWidgetMovable;
    QRect           m_normalRect;
    int             m_currentScreen;
};

#ifndef QTC_USE_NATIVE

/* FancyTitleBarPrivateQt */
class FancyTitleBarPrivateQt : public FancyTitleBarPrivate
{
    Q_OBJECT
public:
    explicit FancyTitleBarPrivateQt(QWidget *mainWidget);
    virtual ~FancyTitleBarPrivateQt();

    void init();
    void setDisabled(bool disable);

    // mouse event
    void handleWidgetMouseEvent(QObject *obj, QEvent *event);

private:
    // mouse event
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleLeaveEvent(QEvent *event);
    void handleHoverMoveEvent(QHoverEvent *event);
    void handleMouseDblClickEvent(QMouseEvent *event);

    // widget action
    void resizeWidget(const QPoint &gMousePos);

    // mouse shape
    void updateCursorShape(const QPoint &gMousePos);

    QPoint windowStartPos(QWidget *pWindow, QMouseEvent *event) const;
    QRect validDragRect();

public:
    // Simulate mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

protected:
    virtual bool eventFilter(QObject *object, QEvent *event) override;

public:
    // main window
    FancyCursor     m_pressCursor;
    FancyCursor     m_moveCursor;
    bool            m_bEdgePressed;
    bool            m_bLeftButtonPressed;
    bool            m_bLeftButtonDbClicked;
    bool            m_bLeftButtonTitlePressed;
    bool            m_bCursorShapeChanged;

    QPoint          m_movePoint;
};

#else  // QTC_USE_NATIVE

class FancyTitleBarPrivateNative : public FancyTitleBarPrivate, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit FancyTitleBarPrivateNative(QWidget *mainWidget);
    virtual ~FancyTitleBarPrivateNative();

    void init();
    void setDisabled(bool disable);

#ifdef Q_OS_WINDOWS
    bool handleWindowsMessage(void *message, QTRESULT *result);
#endif

public slots:
    void systemButtonClicked();

public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QTRESULT *result);

private:
    void installNativeEventFilter();
};

#endif // QTC_USE_NATIVE

QCANPOOL_END_NAMESPACE

#endif // FANCYTITLEBAR_P_H
