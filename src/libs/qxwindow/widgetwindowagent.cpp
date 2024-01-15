/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "widgetwindowagent.h"
#include "widgetitemdelegate_p.h"
#include "widgetwindowagent_p.h"
#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QtEvents>

QX_WINDOW_BEGIN_NAMESPACE

/*!
    \class WidgetWindowAgent
    \brief WindowAgentBase is the window agent for QtWidgets.

    It provides interfaces for QtWidgets and processes some Qt events related to the QWidget
    instance.
*/

WidgetWindowAgentPrivate::WidgetWindowAgentPrivate() = default;

WidgetWindowAgentPrivate::~WidgetWindowAgentPrivate() = default;

void WidgetWindowAgentPrivate::init()
{
}

/*!
    Constructs a widget agent, it's better to set the widget to setup as \a parent.
*/
WidgetWindowAgent::WidgetWindowAgent(QObject *parent) : WidgetWindowAgent(*new WidgetWindowAgentPrivate(), parent)
{
}

/*!
    Destructor.
*/
WidgetWindowAgent::~WidgetWindowAgent() = default;

/*!
    Installs the window agent on the widget. The window agent will take over some of the window
    events, making the window look frameless.
*/
bool WidgetWindowAgent::setup(QWidget *w)
{
    Q_ASSERT(w);
    if (!w) {
        return false;
    }

    Q_D(WidgetWindowAgent);
    if (d->hostWidget) {
        return false;
    }

    w->setAttribute(Qt::WA_DontCreateNativeAncestors);
    w->setAttribute(Qt::WA_NativeWindow);

    d->setup(w, new WidgetItemDelegate());
    d->hostWidget = w;

#if defined(Q_OS_WINDOWS) && defined(QX_WINDOW_ENABLE_SYSTEM_BORDERS) && defined(QX_WINDOW_NATIVE)
    d->setupWindows10BorderWorkaround();
#endif
    return true;
}

/*!
    Returns the title bar widget.
*/
QWidget *WidgetWindowAgent::titleBar() const
{
    Q_D(const WidgetWindowAgent);
    return static_cast<QWidget *>(d->context->titleBar());
}

/*!
    Sets the title bar widget, all system button and hit-test visible widget references that
    have been set will be removed.
*/
void WidgetWindowAgent::setTitleBar(QWidget *w)
{
    Q_D(WidgetWindowAgent);
    if (!d->context->setTitleBar(w)) {
        return;
    }
#ifdef Q_OS_MAC
    setSystemButtonArea(nullptr);
#endif
    Q_EMIT titleBarChanged(w);
}

/*!
    Returns the system button of the given type.
*/
QWidget *WidgetWindowAgent::systemButton(SystemButton button) const
{
    Q_D(const WidgetWindowAgent);
    return static_cast<QWidget *>(d->context->systemButton(button));
}

/*!
    Sets the system button of the given type, the system buttons always receive mouse events so
    you don't need to call \c setHitTestVisible for them.
*/
void WidgetWindowAgent::setSystemButton(SystemButton button, QWidget *w)
{
    Q_D(WidgetWindowAgent);
    if (!d->context->setSystemButton(button, w)) {
        return;
    }
    Q_EMIT systemButtonChanged(button, w);
}

/*!
    Returns \a true if the widget can receive mouse events on title bar.
*/
bool WidgetWindowAgent::isHitTestVisible(const QWidget *w) const
{
    Q_D(const WidgetWindowAgent);
    return d->context->isHitTestVisible(w);
}

/*!
    Makes the widget able to receive mouse events on title bar if \a visible is \c true.
    You're supposed to make sure that the specified widget \a w is a child or descendant
    of the title bar widget.
*/
void WidgetWindowAgent::setHitTestVisible(const QWidget *w, bool visible)
{
    Q_D(WidgetWindowAgent);
    d->context->setHitTestVisible(w, visible);
}

void WidgetWindowAgent::addCaptionClassName(const QString &name)
{
    Q_D(WidgetWindowAgent);
    d->context->addCaptionClassName(name);
}

/*!
    \internal
*/
WidgetWindowAgent::WidgetWindowAgent(WidgetWindowAgentPrivate &d, QObject *parent) : WindowAgentBase(d, parent)
{
    d.init();
}

/*!
    \fn void WidgetWindowAgent::titleBarChanged(const QWidget *w)

    This signal is emitted when the title bar widget is replaced.
*/

/*!
    \fn void WidgetWindowAgent::systemButtonChanged(SystemButton button, const QWidget *w)

    This signal is emitted when a system button is replaced.
*/

QX_WINDOW_END_NAMESPACE
