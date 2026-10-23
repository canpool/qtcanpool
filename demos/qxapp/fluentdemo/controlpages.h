/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CONTROLPAGES_H
#define CONTROLPAGES_H

#include <QtCore/QList>
#include <QtCore/QString>

class DemoPage;
class QWidget;

/*!
 * The control pages: one screen per control, under the headings the pane lists
 * them in.
 *
 * A control set is proved twice. The control gallery puts each control in its
 * own row so that a change in what a control paints is a change in one
 * rectangle; these pages are the other half - each control on a screen that
 * uses it for something, with a line saying what it is for.
 *
 * They arrive in groups because nineteen screens in a flat column is a list to
 * read rather than a set to steer by: a reader who came for a way to type a
 * number should not have to know that NumberBox is older than BreadcrumbBar to
 * find it. What the group is called is the pane's heading over it, and it is
 * decided here, beside the page, rather than in the window that lists them.
 *
 * A page here is built by one function and knows nothing about the window it
 * ends up in: what a page cannot do - fold the pane, apply a window material,
 * open a dialog over the window - it either leaves to Settings or asks of the
 * thing it points at.
 */
namespace ControlPages
{
/*! A heading the pane puts over its destinations, and the pages under it. */
struct ControlGroup {
    QString name;
    QList<DemoPage *> pages;
};

/*! Builds every control page. The pages are parented to \a parent. */
QList<ControlGroup> build(QWidget *parent);
}   // namespace ControlPages

#endif   // CONTROLPAGES_H
