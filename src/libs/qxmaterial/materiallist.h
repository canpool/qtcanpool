/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALLIST_H
#define MATERIALLIST_H

#include "qxmaterial_global.h"

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialListItem;
class MaterialListPrivate;

/*!
 * Material's list: the container the rows stand in.
 *
 * The rows are the list's to keep and to place - edge to edge, in the order
 * they were given - and the choosing is the list's to answer: a row clicked
 * becomes the chosen one, the one that was chosen before gives it up, and
 * the list says so twice, once with the click and once with the choosing.
 * A row the application chooses outside a click answers the same way, since
 * there is one chosen row at a time however the choosing arrived.
 */
class QX_MATERIAL_EXPORT MaterialList : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialList(QWidget *parent = Q_NULLPTR);
    ~MaterialList() override;

    /*! How many rows the list keeps. */
    int count() const;
    /*! The row at \a index, or null when there is none. */
    MaterialListItem *itemAt(int index) const;
    /*! The chosen row, or null while none is chosen. */
    MaterialListItem *currentItem() const;

    /*! Adds \a item to the end of the list; the list becomes its parent and
     *  answers for its clicks from then on. */
    void addItem(MaterialListItem *item);
    /*! Makes a row of \a text - of two lines when \a secondary is given -
     *  with \a icon before the words, and adds it. */
    MaterialListItem *addItem(const QString &text, const QString &secondary = QString(), const QIcon &icon = QIcon());

public Q_SLOTS:
    /*! Puts \a item in force and gives the choice up elsewhere. */
    void setCurrentItem(MaterialListItem *item);
    /*! Lets the choice go; no row is the chosen one. */
    void clearSelection();
Q_SIGNALS:
    /*! A row was clicked. The click also chooses it, which \c
     *  currentItemChanged says in its own words. */
    void itemClicked(MaterialListItem *item);
    /*! The chosen row changed - by a click, or by \c setCurrentItem. Null is
     *  the choice letting go. */
    void currentItemChanged(MaterialListItem *item);
private:
    Q_DISABLE_COPY(MaterialList)
    QX_DECLARE_PRIVATE(MaterialList)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALLIST_H
