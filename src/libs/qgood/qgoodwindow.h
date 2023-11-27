/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#ifndef QGOODWINDOW_H
#define QGOODWINDOW_H

#include "qgood_global.h"
#include <QMainWindow>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
typedef qintptr qgoodintptr;
#else
typedef long qgoodintptr;
#endif

#ifdef QGOOD_WINDOW_ENABLE
#include <QPointer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif // Q_OS_WIN

#if defined Q_OS_WIN || defined Q_OS_LINUX
class QGoodShadow;
#endif // Q_OS_WIN || Q_OS_LINUX

#endif // QGOOD_WINDOW_ENABLE

/** **QGoodWindow** class contains the public API's to control the behavior of the customized window. */
class QGOOD_SHARED_EXPORT QGoodWindow : public QMainWindow
{
    Q_OBJECT
public:
    /** Constructor of *QGoodWindow*.
     *
     * On Windows creates the native window, turns the `QMainWindow` as a native widget,
     * creates the shadow, initialize default values and calls the `QMainWindow`
     * parent constructor.
     *
     * On Linux creates the frame less `QMainWindow`, use the shadow only to create resize borders,
     * and the real shadow is draw by current Linux window manager.
     *
     * On macOS creates a `QMainWindow` with full access to the title bar,
     * and hide native minimize, zoom and close buttons.
     */
    explicit QGoodWindow(QWidget *parent = nullptr,
                         const QColor &clearColor = QColor(!isSystemThemeDark() ? Qt::white : Qt::black));
    ~QGoodWindow();

    /** Enum that contains caption buttons states when it's states are handled by *QGoodWindow*. */
    enum class CaptionButtonState {
        MinimizeHoverEnter,
        MinimizeHoverLeave,
        MinimizePress,
        MinimizeRelease,
        MinimizeClicked,
        MaximizeHoverEnter,
        MaximizeHoverLeave,
        MaximizePress,
        MaximizeRelease,
        MaximizeClicked,
        CloseHoverEnter,
        CloseHoverLeave,
        ClosePress,
        CloseRelease,
        CloseClicked
    };

    void themeChanged();
    /** Call this function to setup *QApplication* for *QGoodWindow* usage. */
    static void setup();
    /** Returns if the current system theme is dark or not. */
    static bool isSystemThemeDark();
    /** Returns true if system draw borders and false if your app should do it. */
    static bool shouldBordersBeDrawnBySystem();

Q_SIGNALS:
    /** On handled caption buttons, this SIGNAL report the state of these buttons. */
    void captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state);
    /** Notify that the system has changed between light and dark mode. */
    void systemThemeChanged();

public:
    /** Set title bar height for *QGoodWindow*. */
    void setTitleBarHeight(int height);
    /** Set current icon width on the left side of the title bar of *QGoodWindow*. */
    void setIconWidth(int width);
    /** On Windows, Linux and macOS, returns the actual title bar height, on other OSes returns 0. */
    int titleBarHeight() const;
    /** On Windows, Linux and macOS, return the actual icon width, on other OSes returns 0. */
    int iconWidth() const;
    /** Rect that contains the whole title bar. */
    QRect titleBarRect() const;
    /** Set the mask for the customized title bar. */
    void setTitleBarMask(const QRegion &mask);
    /** Set the location and shape of handled minimize button, relative to title bar rect. */
    void setMinimizeMask(const QRegion &mask);
    /** Set the location and shape of handled maximize button, relative to title bar rect. */
    void setMaximizeMask(const QRegion &mask);
    /** Set the location and shape of handled close button, relative to title bar rect. */
    void setCloseMask(const QRegion &mask);
    /** Get the mask for the customized title bar. */
    QRegion titleBarMask() const;
    /** Get the location and shape of handled minimize button, relative to title bar rect. */
    QRegion minimizeMask() const;
    /** Get the location and shape of handled maximize button, relative to title bar rect. */
    QRegion maximizeMask() const;
    /** Get the location and shape of handled close button, relative to title bar rect. */
    QRegion closeMask() const;

public:
    WId winId() const;
    bool isEnabled() const;

public:
    // Widget coordinates
    QRect frameGeometry() const;
    QRect geometry() const;
    QRect normalGeometry() const;

    int x() const;
    int y() const;
    QPoint pos() const;
    QSize size() const;
    int width() const;
    int height() const;
    QRect rect() const;

    QSize minimumSize() const;
    QSize maximumSize() const;
    int minimumWidth() const;
    int minimumHeight() const;
    int maximumWidth() const;
    int maximumHeight() const;
    void setMinimumSize(const QSize &size);
    void setMaximumSize(const QSize &size);
    void setMinimumWidth(int w);
    void setMinimumHeight(int h);
    void setMaximumWidth(int w);
    void setMaximumHeight(int h);

    void setFixedSize(const QSize &size);
    void setFixedSize(int w, int h);

public Q_SLOTS:
    void setWindowTitle(const QString &title);
public:
    QString windowTitle() const;
    QIcon windowIcon() const;
    void setWindowIcon(const QIcon &icon);
    qreal windowOpacity() const;
    void setWindowOpacity(qreal level);

public:
    bool isActiveWindow() const;
    void activateWindow();

public Q_SLOTS:
    // Widget management functions
    void show();
    void hide();

    void showMinimized();
    void showMaximized();
    void showFullScreen();
    void showNormal();

    bool close();

public:
    void move(int x, int y);
    void move(const QPoint &pos);
    void resize(int width, int height);
    void resize(const QSize &size);

    void setGeometry(int x, int y, int w, int h);
    void setGeometry(const QRect &rect);
    QByteArray saveGeometry() const;
    bool restoreGeometry(const QByteArray &geometry);
    bool isVisible() const;

    bool isMinimized() const;
    bool isMaximized() const;
    bool isFullScreen() const;

    Qt::WindowStates windowState() const;
    void setWindowState(Qt::WindowStates state);

    void setWindowFlags(Qt::WindowFlags type);
    Qt::WindowFlags windowFlags() const;

    QWindow *windowHandle() const;

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result) override;

private:
#ifdef QGOOD_WINDOW_ENABLE
#ifdef Q_OS_WIN
    // Functions
    void initGW();
    void closeGW();
    void setWindowStateWin();
    static LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void handleActivation();
    void handleDeactivation();
    void setWidgetFocus();
    void enableCaption();
    void disableCaption();
    void frameChanged();
    void sizeMoveWindow();
    void setWindowMask();
    void moveShadow();
    void screenChangeMoveWindow(QScreen *screen);
    void updateScreen(QScreen *screen);
    void setCurrentScreen(QScreen *screen);
    void updateWindowSize();
    QPoint screenAdjustedPos();
    QScreen *screenForWindow(HWND hwnd);
    QScreen *screenForPoint(const QPoint &pos);
    void showContextMenu(int x, int y);
    void showContextMenu();
    QWidget *bestParentForModalWindow();
    void moveCenterWindow(QWidget *widget);
    bool winButtonHover(qintptr button);
    void iconClicked();

    bool handleWindowsNativeMessage(void *message, qgoodintptr *result);

    HWND m_hwnd;
    QPointer<QMainWindow> m_mainWindow;
    QPointer<QGoodShadow> m_shadow;
    QPointer<QWidget> m_focusWidget;
    QPointer<QWidget> m_helperWidget;
#ifdef QT_VERSION_QT6
    QPointer<QWindow> m_helperWindow;
#endif
    QWindow *m_windowHandle;

    QPointer<QTimer> m_moveTimer;
    QPointer<QTimer> m_menuTimer;

    bool m_windowReady;
    bool m_closed;
    bool m_visible;
    bool m_activeState;
    bool m_isMenuVisible;

    bool m_isWin11OrGreater;

    bool m_selfGeneratedCloseEvent;
    bool m_selfGeneratedShowEvent;
    bool m_selfGeneratedResizeEvent;

    QRect m_normalRect;

    Qt::WindowStates m_windowState;
    Qt::WindowState m_lastState;

    QColor m_clearColor;

    int m_minWidth;
    int m_minHeight;
    int m_maxWidth;
    int m_maxHeight;
#endif // Q_OS_WIN

#ifdef Q_OS_LINUX
    // Functions
    void setCursorForCurrentPos();
    void startSystemMoveResize();
    void sizeMove();
    void sizeMoveBorders();

    // Variables
    QPointer<QGoodShadow> m_topShadow;
    QPointer<QGoodShadow> m_bottomShadow;
    QPointer<QGoodShadow> m_leftShadow;
    QPointer<QGoodShadow> m_rightShadow;

    int m_margin;
    QPoint m_cursorPos;
    bool m_resizeMove;
    bool m_resizeMoveStarted;
    Qt::WindowFlags m_windowFlags;

    friend class QGoodShadow;
#endif // Q_OS_LINUX

#ifdef Q_OS_MAC
    // Functions
    void notificationReceiver(const QByteArray &notification);

    // Variables
    QPoint m_cursorMovePos;
    bool m_mouseButtonPressed;
    bool m_onAnimateEvent;

    friend class Notification;
#endif // Q_OS_MAC

#if defined Q_OS_LINUX || defined Q_OS_MAC
    int m_lastMoveButton;
#endif // Q_OS_LINUX || Q_OS_MAC

    qintptr ncHitTest(int pos_x, int pos_y);

    void buttonEnter(qintptr button);
    void buttonLeave(qintptr button);
    bool buttonPress(qintptr button);
    bool buttonRelease(qintptr button, bool valid_click);

    QPointer<QWidget> m_parent;
    QPointer<QTimer> m_hoverTimer;

    QRegion m_titleBarMask;
    QRegion m_minMask;
    QRegion m_maxMask;
    QRegion m_clsMask;

    qreal m_pixelRatio;

    bool m_isUsingSystemBorders;
    bool m_dark;

    int m_titleBarHeight;
    int m_iconWidth;

    bool m_isCaptionButtonPressed;
    qintptr m_lastCaptionButtonHovered;
    qintptr m_captionButtonPressed;
#endif // QGOOD_WINDOW_ENABLE
};

#endif   // QGOODWINDOW_H
