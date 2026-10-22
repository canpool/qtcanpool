/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CONTROLPAGES_H
#define CONTROLPAGES_H

#include <QtCore/QList>

class DemoPage;
class QWidget;

/*!
 * The control pages: one screen per control, in the order the pane lists them.
 *
 * A control set is proved twice. The control gallery puts each control in its
 * own row so that a change in what a control paints is a change in one
 * rectangle; these pages are the other half - each control on a screen that
 * uses it for something, with a line saying what it is for. The four the
 * library gained last come first, because a page is also how the newest of a
 * control set is found.
 *
 * A page here is built by one function and knows nothing about the window it
 * ends up in: what a page cannot do - fold the pane, apply a window material,
 * open a dialog over the window - it either leaves to Settings or asks of the
 * thing it points at.
 */
namespace ControlPages
{
/*! Builds every control page. The pages are parented to \a parent. */
QList<DemoPage *> build(QWidget *parent);
}   // namespace ControlPages

#endif   // CONTROLPAGES_H
