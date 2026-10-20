/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#include "windowagentbase.h"
#include "windowkit_p.h"
#include "windowagentbase_p.h"
#include "windowcontext_p.h"

Q_LOGGING_CATEGORY(qxWindowLog, "qxwindow")

QX_WINDOW_BEGIN_NAMESPACE

/*!
    \class WindowAgentBase
    \brief WindowAgentBase is the base class of the specifiy window agent for QtWidgets and
    QtQuick.

    It processes some system events to remove the window's default title bar, and provides some
    shared methods for derived classes to call.
*/

WindowAgentBasePrivate::WindowAgentBasePrivate()
    : q_ptr(nullptr)
    , m_context(nullptr)
{
}

WindowAgentBasePrivate::~WindowAgentBasePrivate() = default;

void WindowAgentBasePrivate::init()
{
    // The context is created here rather than in setup(). Every accessor on
    // WindowAgentBase and its subclasses goes through m_context, and asking a
    // freshly constructed agent for its title bar is a perfectly reasonable
    // thing to do - the answer is "none", not a null pointer dereference.
    //
    // createContext() is virtual, but it is not overridden anywhere and only
    // depends on compile-time platform macros, so calling it this early is
    // well defined.
    m_context.reset(createContext());
}

WindowContext *WindowAgentBasePrivate::createContext() const
{
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
    if (!m_context) {
        m_context.reset(createContext());
    }
    m_context->setup(host, delegate);
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
    return d->m_context->windowAttribute(key);
}

/*!
    Returns whether the resize border claims a press that lands on it.

    \sa setBorderOwnsCursor()
*/
bool WindowAgentBase::borderOwnsCursor() const
{
    Q_D(const WindowAgentBase);
    return d->m_context->borderOwnsCursor();
}

/*!
    Sets whether the resize border claims a press that lands on it, and with it
    the cursor that advertises the resize.

    A frameless window draws its own border, and the border it draws is not always
    there to be grabbed: it may be no more than the room a shadow needs, with
    dragging meant to go through the title bar only. Nothing in the geometry says
    which of the two a border is, so it has to be told.

    With \a owns false the border drops out of the hit test: a press there reaches
    whatever is drawn underneath instead of starting a resize, and the cursor stays
    the arrow all the way to the window's edge. The title bar keeps its own answer,
    so dragging by the title bar is unaffected.

    Outside the window the answer is a plain border rather than the client area:
    there is nothing out there to hand a press to, and a point off the window that
    claims to be client area confuses the window manager. So a window with this off
    cannot be resized from its frame at all, which is the intent - but note that
    how much of the frame is the system's and how much is drawn here depends on
    QX_WINDOW_CONFIG_SYSTEM_BORDERS, and both halves honour the flag.

    Default is true, which is the behaviour every window has always had.

    \sa borderOwnsCursor()
*/
void WindowAgentBase::setBorderOwnsCursor(bool owns)
{
    Q_D(WindowAgentBase);
    d->m_context->setBorderOwnsCursor(owns);
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
    return d->m_context->setWindowAttribute(key, attribute);
}

/*!
    Shows the system menu, it's only implemented on Windows.
*/
void WindowAgentBase::showSystemMenu(const QPoint &pos)
{
    Q_D(WindowAgentBase);
    d->m_context->showSystemMenu(pos);
}

/*!
    Makes the window show in center of the current screen.
*/
void WindowAgentBase::centralize()
{
    Q_D(WindowAgentBase);
    d->m_context->virtual_hook(WindowContext::CentralizeHook, nullptr);
}

/*!
    Brings the window to top.
*/
void WindowAgentBase::raise()
{
    Q_D(WindowAgentBase);
    d->m_context->virtual_hook(WindowContext::RaiseWindowHook, nullptr);
}

/*!
    \internal
*/
WindowAgentBase::WindowAgentBase(WindowAgentBasePrivate &d, QObject *parent)
    : QObject(parent)
    , d_ptr(&d)
{
    d.q_ptr = this;

    d.init();
}

QX_WINDOW_END_NAMESPACE
