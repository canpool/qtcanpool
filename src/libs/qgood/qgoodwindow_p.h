/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2021-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#ifndef QGOODWINDOW_P_H
#define QGOODWINDOW_P_H

#include "qgood_global.h"

#ifdef QGOOD_WINDOW_ENABLE

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <windows.h>

// reference to https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-getsystemmetrics
#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER 92
#endif

#ifndef WM_DWMCOMPOSITIONCHANGED
#define WM_DWMCOMPOSITIONCHANGED 0x031e
#endif

inline int BORDERWIDTH()
{
    return (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

inline int BORDERHEIGHT()
{
    return (GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#endif // _WIN32

#ifdef Q_OS_LINUX

#define BORDERWIDTH 10   // PIXELS

#define BORDERWIDTHDPI BORDERWIDTH

#define MOVERESIZE_MOVE 8   // X11 Fixed Value

#endif // Q_OS_LINUX

#if defined Q_OS_LINUX || defined Q_OS_MAC
// The positive values are mandatory on Linux and arbitrary on macOS,
// using the same for convenience.
// The negative values are arbitrary on both platforms.

#define HTNOWHERE     -1
#define HTMINBUTTON   -2
#define HTMAXBUTTON   -3
#define HTCLOSE       -4
#define HTTOPLEFT     0
#define HTTOP         1
#define HTTOPRIGHT    2
#define HTLEFT        7
#define HTRIGHT       3
#define HTBOTTOMLEFT  6
#define HTBOTTOM      5
#define HTBOTTOMRIGHT 4
#define HTCAPTION     8

#endif // Q_OS_LINUX || Q_OS_MAC

#include "qgoodwindow.h"
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#if defined Q_OS_WIN || defined Q_OS_LINUX
class QGoodWindow;
#endif

class QGoodShadow : public QWidget
{
    Q_OBJECT
public:
#if defined Q_OS_WIN || defined Q_OS_LINUX
    explicit QGoodShadow(qintptr hwnd, QGoodWindow *gw, QWidget *parent);
#endif

public Q_SLOTS:
    void showLater();
    void show();
    void hide();
    void setActive(bool active);
    int shadowWidth();
private:
    bool nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result);
    bool event(QEvent *event);
    void paintEvent(QPaintEvent *event);

#if defined Q_OS_WIN || defined Q_OS_LINUX
    QPointer<QGoodWindow> m_parent;
    QPointer<QTimer> m_timer;
#endif
#ifdef Q_OS_WIN
    HWND m_hwnd;
    bool m_active;
#endif
};

#ifdef Q_OS_MAC

class Notification
{
public:
    Notification();
    ~Notification();

    void addWindow(void *ptr);
    void removeWindow(void *ptr);
    void notification(const char *notification_name, long wid);
    void registerNotification(const QByteArray &name, WId wid);
    void unregisterNotification();
private:
    QList<void *> m_ptr_list;
};

extern Notification notification;

namespace macOSNative
{
enum class StyleType {
    NoState,
    Disabled,
    Fullscreen
};

void registerThemeChangeNotification();
void registerNotification(const char *notification_name, long wid);
void unregisterNotification();

inline void handleNotification(const char *notification_name, long wid)
{
    notification.notification(notification_name, wid);
}

void setStyle(long winid, StyleType type);

const char *themeName();
}

#endif // Q_OS_MAC

#endif // QGOOD_WINDOW_ENABLE

#endif   // QGOODWINDOW_P_H
