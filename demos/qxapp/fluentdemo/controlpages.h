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
 * They arrive in groups because a flat column of every screen is a list to
 * read rather than a set to steer by: a reader who came for a way to type a
 * number should not have to know that NumberBox is older than BreadcrumbBar to
 * find it. What the group is called is the pane's heading over it, and it is
 * decided here, beside the page, rather than in the window that lists them.
 *
 * A page may also stand under a part of its group rather than under the group
 * itself, which is the one place these pages show a pane that folds: the part
 * is a page of its own, so the row over the others opens as well as closes.
 *
 * A page here is built by one function and knows nothing about the window it
 * ends up in: what a page cannot do - fold the pane, apply a window material,
 * open a dialog over the window - it either leaves to Settings or asks of the
 * thing it points at.
 */
namespace ControlPages
{
/*! A page the pane lists, and the category it is listed under when it has one.
 *
 *  A category is a page too - it is opened the way any other destination is -
 *  so the row that carries it is a destination of the pane and not a heading:
 *  what a heading cannot do is close. A page with an empty \c parent stands
 *  directly under the group's heading, and a group may be all of one kind.
 */
struct ControlEntry {
    DemoPage *page = Q_NULLPTR;
    QString parent;
    /*! The glyph the pane puts beside the page's name, named rather than drawn:
     *  the file is one of this demo's own (see icons.h), and which one belongs
     *  to a page is decided here for the same reason the group is - beside the
     *  page, by whoever knows what the page is for. Empty leaves the row with
     *  no icon, which is what a heading gets. */
    QString icon;
};

/*! A heading the pane puts over its destinations, and the pages under it. */
struct ControlGroup {
    QString name;
    QList<ControlEntry> pages;
};

/*! Builds every control page. The pages are parented to \a parent. */
QList<ControlGroup> build(QWidget *parent);
}   // namespace ControlPages

#endif   // CONTROLPAGES_H
