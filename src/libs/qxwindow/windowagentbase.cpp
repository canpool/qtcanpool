/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "windowagentbase.h"
#include "windowkit_p.h"
#include "windowagentbase_p.h"
#include "windowcontext_p.h"

Q_LOGGING_CATEGORY(qWindowKitLog, "qwindowkit")

QX_WINDOW_BEGIN_NAMESPACE

/*!
    \class WindowAgentBase
    \brief WindowAgentBase is the base class of the specifiy window agent for QtWidgets and
    QtQuick.

    It processes some system events to remove the window's default title bar, and provides some
    shared methods for derived classes to call.
*/

WindowAgentBasePrivate::WindowContextFactoryMethod WindowAgentBasePrivate::windowContextFactoryMethod = nullptr;

WindowAgentBasePrivate::WindowAgentBasePrivate() : q_ptr(nullptr), context(nullptr)
{
}

WindowAgentBasePrivate::~WindowAgentBasePrivate() = default;

void WindowAgentBasePrivate::init()
{
}

WindowContext *WindowAgentBasePrivate::createContext() const
{
    if (windowContextFactoryMethod) {
        return windowContextFactoryMethod();
    }

#if defined(Q_OS_WINDOWS) && defined(QX_WINDOW_NATIVE)
    return new WindowContextWin();
#elif defined(Q_OS_MAC) && defined(QX_WINDOW_NATIVE)
    return new WindowContextCocoa();
#else
    return new WindowContextQt();
#endif
}

void WindowAgentBasePrivate::setup(QObject *host, WindowItemDelegate *delegate)
{
    auto ctx = createContext();
    ctx->setup(host, delegate);
    context.reset(ctx);
}

/*!
    Destructor.
*/
WindowAgentBase::~WindowAgentBase() = default;

/*!
    Returns the window attribute value.

    \sa setWindowAttribute()
*/
QVariant WindowAgentBase::windowAttribute(const QString &key) const
{
    Q_D(const WindowAgentBase);
    return d->context->windowAttribute(key);
}

/*!
    Sets the platform-related attribute for the window. Available attributes:

    On Windows,
        \li \c dwm-blur: Specify a boolean value to enable or disable dwm blur effect, this
               attribute is available on Windows 10 or later.
        \li \c dark-mode: Specify a boolean value to enable or disable the dark mode, it is
               enabled by default on Windows 10 if the system borders config is enabled. This
               attribute is available on Windows 10 or later.
        \li \c acrylic-material: Specify a boolean value to enable or disable acrylic material,
               this attribute is only available on Windows 11.
        \li \c mica: Specify a boolean value to enable or disable mica material,
               this attribute is only available on Windows 11.
        \li \c mica-alt: Specify a boolean value to enable or disable mica-alt material,
               this attribute is only available on Windows 11.
        \li \c extra-margins: Specify a margin value to change the \c dwm extended area
               geometry, you shouldn't change this attribute because it may break the
               internal state.

    On macOS,
        \li \c no-system-buttons: Specify a boolean value to set the system buttons'
               visibility.
        \li \c blur-effect: You can specify a string value, "dark" to enable dark mode, "light"
               to set enable mode, "none" to disable. You can also specify a boolean value,
               \c true to enable current theme mode, \c false to disable.
        \li \c title-bar-height: Returns the system title bar height, the system button display
               area will be limited to this height. (Readonly)
*/
bool WindowAgentBase::setWindowAttribute(const QString &key, const QVariant &attribute)
{
    Q_D(WindowAgentBase);
    return d->context->setWindowAttribute(key, attribute);
}

/*!
    Shows the system menu, it's only implemented on Windows.
*/
void WindowAgentBase::showSystemMenu(const QPoint &pos)
{
    Q_D(WindowAgentBase);
    d->context->showSystemMenu(pos);
}

/*!
    Makes the window show in center of the current screen.
*/
void WindowAgentBase::centralize()
{
    Q_D(WindowAgentBase);
    d->context->virtual_hook(WindowContext::CentralizeHook, nullptr);
}

/*!
    Brings the window to top.
*/
void WindowAgentBase::raise()
{
    Q_D(WindowAgentBase);
    d->context->virtual_hook(WindowContext::RaiseWindowHook, nullptr);
}

/*!
    \internal
*/
WindowAgentBase::WindowAgentBase(WindowAgentBasePrivate &d, QObject *parent) : QObject(parent), d_ptr(&d)
{
    d.q_ptr = this;

    d.init();
}

QX_WINDOW_END_NAMESPACE
