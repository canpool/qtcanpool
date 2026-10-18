/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"

#include <QString>

QX_DOCK_BEGIN_NAMESPACE

/*!
 * Reads the dock style sheet an application has to install for a DockWindow to
 * look like a dock rather than a row of unstyled widgets.
 *
 * The sheets behind this used to live in the library and were applied by
 * DockWindow's own constructor. That put the dock's appearance beyond the reach
 * of the application: a widget-level style sheet outranks the application-level
 * one, so an application that set a dark theme still got a DockWindow the
 * library had already styled. The sheets moved out with B12's move of the
 * ribbon themes and are now built in by the applications that want them - see
 * res/dockstyle.qrc, which is to the dock what res/themes.qrc is to the ribbon.
 *
 * They are handed out rather than applied on purpose. A sheet can only be read
 * from the executable that carries it, so which of the four is right is a
 * question only the application can answer, and it answers it by building the
 * one in and asking here for its text.
 *
 * \a focusHighlighting picks between the pair that draws a frame around the
 * focused dock widget and the pair that does not; it is the same
 * DockManager::FocusHighlighting the library used to read for itself.
 * QX_DOCK_X11 picks the _linux pair, which carries the extra rules an X11
 * desktop needs and is the only thing in the library that knows which desktop
 * that is.
 *
 * Returns an empty string when the resource is missing - a correct build always
 * has it - and says so on stderr. An empty string is deliberately not applied to
 * anything: the caller is about to hand it to setStyleSheet(), and doing that
 * with nothing would wipe whatever the application had already set.
 */
QX_DOCK_EXPORT QString dockStyleSheet(bool focusHighlighting);

QX_DOCK_END_NAMESPACE