/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTREEVIEW_H
#define FLUENTTREEVIEW_H

#include "qxfluent_global.h"

#include <QtWidgets/QTreeView>

class QStandardItem;

QX_FLUENT_BEGIN_NAMESPACE

class FluentTreeViewPrivate;

/*!
 * Fluent's tree view: the list view's rows, one level in.
 *
 * A node is thirty-two tall - a tree is denser than a list, because a tree is
 * read by following it rather than by scanning it - and every level adds
 * sixteen to its left. The node that has more under it carries the small
 * chevron at its left, which turns to point down when it is open; the pointer's
 * wash and the chosen node's accent tint and bar are the list's, because a tree
 * is a list that branches.
 *
 * The container is Qt's own tree view with a delegate of the family's, for the
 * reason the list states. What this decides is what a node looks like; which
 * node is where, and what opening one does to the others, is the framework's.
 *
 * \sa FluentListView
 */
class QX_FLUENT_EXPORT FluentTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit FluentTreeView(QWidget *parent = Q_NULLPTR);
    ~FluentTreeView() override;

    /*! Adds a node at the top level saying \a text. */
    QStandardItem *addNode(const QString &text);
    /*! Adds a node under \a parent saying \a text. */
    QStandardItem *addChild(QStandardItem *parent, const QString &text);
    /*! How many nodes the tree holds, at every level. */
    int nodeCount() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    Q_DISABLE_COPY(FluentTreeView)
    QX_DECLARE_PRIVATE(FluentTreeView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTREEVIEW_H
