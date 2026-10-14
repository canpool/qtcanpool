/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#ifndef WINDOWKIT_WIN_H
#define WINDOWKIT_WIN_H

#include <QtCore/qglobal.h>
#include <QtCore/qt_windows.h>

// Qt's own registry key class, in a private header since Qt 5.14 - below the
// minimum supported version, so it is included unconditionally.
#include <QtCore/private/qwinregistry_p.h>

#include <QStringView>
#include <QPair>
#include <QString>

#include "qxwindow_global.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) (static_cast<int>(static_cast<short>(LOWORD(lp))))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) (static_cast<int>(static_cast<short>(HIWORD(lp))))
#endif

#ifndef IsMinimized
#define IsMinimized(hwnd) (::IsIconic(hwnd))
#endif

#ifndef IsMaximized
#define IsMaximized(hwnd) (::IsZoomed(hwnd))
#endif

#ifndef RECT_WIDTH
#define RECT_WIDTH(rect) ((rect).right - (rect).left)
#endif

#ifndef RECT_HEIGHT
#define RECT_HEIGHT(rect) ((rect).bottom - (rect).top)
#endif

// Maybe undocumented Windows messages
#ifndef WM_UAHDESTROYWINDOW
#define WM_UAHDESTROYWINDOW (0x0090)
#endif

#ifndef WM_UNREGISTER_WINDOW_SERVICES
#define WM_UNREGISTER_WINDOW_SERVICES (0x0272)
#endif

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif

#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif

#ifndef SM_CXPADDEDBORDER
#define SM_CXPADDEDBORDER 92
#endif

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0
#endif

#ifndef ABM_GETAUTOHIDEBAREX
#define ABM_GETAUTOHIDEBAREX 0x0000000b
#endif

QX_WINDOW_BEGIN_NAMESPACE

namespace Private
{

RTL_OSVERSIONINFOW GetRealOSVersion();

inline bool IsWindows1122H2OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0 && rovi.dwBuildNumber >= 22621);
}

inline bool IsWindows11OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0 && rovi.dwBuildNumber >= 22000);
}

inline bool IsWindows101903OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0 && rovi.dwBuildNumber >= 18362);
}

inline bool IsWindows101809OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) ||
           (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0 && rovi.dwBuildNumber >= 17763);
}

inline bool IsWindows10OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 10) || (rovi.dwMajorVersion == 10 && rovi.dwMinorVersion == 0);
}

inline bool IsWindows8Point1OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 6) || (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion >= 3);
}

inline bool IsWindows8OrGreater_Real()
{
    RTL_OSVERSIONINFOW rovi = GetRealOSVersion();
    return (rovi.dwMajorVersion > 6) || (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion >= 2);
}

inline bool IsWindows10Only_Real()
{
    return IsWindows10OrGreater_Real() && !IsWindows11OrGreater_Real();
}

}   // namespace Private

//
// Registry Helpers
//

// QWinRegistryKey is Qt's own wrapper (qwinregistry_p.h, included above for Qt
// 5.14 and later). The library used to carry its own WindowsRegistryKey for older
// Qt and aliased this name to Qt's; that alias stays so the call sites below keep
// reading the same way, but the hand-written version is gone - see 4.3 B2.
using WindowsRegistryKey = QWinRegistryKey;

//
// Version Helpers
//

static inline bool isWin8OrGreater()
{
    static const bool result = Private::IsWindows8OrGreater_Real();
    return result;
}

static inline bool isWin8Point1OrGreater()
{
    static const bool result = Private::IsWindows8Point1OrGreater_Real();
    return result;
}

static inline bool isWin10OrGreater()
{
    static const bool result = Private::IsWindows10OrGreater_Real();
    return result;
}

static inline bool isWin101809OrGreater()
{
    static const bool result = Private::IsWindows101809OrGreater_Real();
    return result;
}

static inline bool isWin101903OrGreater()
{
    static const bool result = Private::IsWindows101903OrGreater_Real();
    return result;
}

static inline bool isWin11OrGreater()
{
    static const bool result = Private::IsWindows11OrGreater_Real();
    return result;
}

static inline bool isWin1122H2OrGreater()
{
    static const bool result = Private::IsWindows1122H2OrGreater_Real();
    return result;
}

static inline bool isWin10Only()
{
    static const bool result = Private::IsWindows10Only_Real();
    return result;
};

//
// Native Event Helpers
//

inline bool isImmersiveColorSetChange(WPARAM wParam, LPARAM lParam)
{
    return !wParam && lParam && std::wcscmp(reinterpret_cast<LPCWSTR>(lParam), L"ImmersiveColorSet") == 0;
}

QX_WINDOW_END_NAMESPACE

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QMargins>
#include <QMetaType>
// for QVariant::fromValue(QMargins(...))
Q_DECLARE_METATYPE(QMargins)
#endif

#endif   // WINDOWKIT_WIN_H
