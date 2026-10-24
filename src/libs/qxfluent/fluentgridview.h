/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTGRIDVIEW_H
#define FLUENTGRIDVIEW_H

#include "qxfluent_global.h"

#include <QtWidgets/QListView>

QX_FLUENT_BEGIN_NAMESPACE

class FluentGridViewPrivate;

/*!
 * Fluent's grid view: the same things to choose from, laid out as tiles that
 * wrap rather than as rows that stack.
 *
 * A tile is a hundred and fifty-two wide and a hundred and twelve tall: an
 * icon standing above the name, both centred, with the pointer's wash over the
 * tile and - once chosen - the accent: a tint of it behind the tile and a
 * stroke of it around, which is how a grid says what a list says with its bar.
 * The grid grows sideways before it grows down, and reflows when the view is
 * narrowed, so the same collection reads as a gallery at any width.
 *
 * The container is Qt's own view in icon mode for the reasons the list gives:
 * a model, a selection model, the keyboard and an accessibility interface are
 * the framework's, and what is decided here is what a tile looks like.
 *
 * \sa FluentListView
 */
class QX_FLUENT_EXPORT FluentGridView : public QListView
{
    Q_OBJECT
public:
    explicit FluentGridView(QWidget *parent = Q_NULLPTR);
    ~FluentGridView() override;

    /*! Adds a tile saying \a text, with \a icon over it when one is given. */
    void addItem(const QString &text, const QIcon &icon = QIcon());
    /*! Replaces the tiles with \a texts. */
    void setItems(const QStringList &texts);
    /*! How many tiles the grid holds. */
    int count() const;
    /*! What the tile at \a row says; empty when there is no such tile. */
    QString itemText(int row) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    Q_DISABLE_COPY(FluentGridView)
    QX_DECLARE_PRIVATE(FluentGridView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTGRIDVIEW_H
