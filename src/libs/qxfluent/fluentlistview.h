/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTLISTVIEW_H
#define FLUENTLISTVIEW_H

#include "qxfluent_global.h"

#include <QtWidgets/QListView>

QX_FLUENT_BEGIN_NAMESPACE

class FluentListViewPrivate;

/*!
 * Fluent's list view: rows of things to choose from, forty tall each.
 *
 * A row is what WinUI calls a list item - an icon and a line of text, with the
 * wash of the pointer over it and, once chosen, the accent: a tint behind the
 * whole row and the small rounded bar at its left edge that says which row of
 * the list is the one in force. The keyboard walks the same rows, so a list
 * that is reached by tab can be worked without the pointer.
 *
 * The container is Qt's own view, not a column of widgets: a list is a thing
 * that scrolls over more rows than fit, that answers a model, a selection
 * model and an accessibility interface that the framework already knows how
 * to build. What this class decides is how a row looks - the delegate - and
 * what a row costs in height; a caller with a model of its own sets it and
 * ignores addItem().
 *
 * \sa FluentGridView
 */
class QX_FLUENT_EXPORT FluentListView : public QListView
{
    Q_OBJECT
public:
    explicit FluentListView(QWidget *parent = Q_NULLPTR);
    ~FluentListView() override;

    /*! Adds a row saying \a text, with \a icon standing at its left when one
     *  is given. Rows added this way live in the list's own model; a caller
     *  that sets a model of its own adds its rows to that instead. */
    void addItem(const QString &text, const QIcon &icon = QIcon());
    /*! Replaces the rows with \a texts. */
    void setItems(const QStringList &texts);
    /*! How many rows the list holds. */
    int count() const;
    /*! What the row at \a row says; empty when there is no such row. */
    QString itemText(int row) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    Q_DISABLE_COPY(FluentListView)
    QX_DECLARE_PRIVATE(FluentListView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTLISTVIEW_H
