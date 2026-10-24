/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTABLE_H
#define MATERIALTABLE_H

#include "qxmaterial_global.h"

#include <QtWidgets/QTableView>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTablePrivate;

/*!
 * Material's table: rows of cells under a row of names.
 *
 * The names stand in a row fifty-six tall, the small print of the family in its
 * heavier cut and the page's ink at six tenths, with the hairline under them
 * that says where the table starts. A row is fifty-two tall and each of its
 * cells keeps sixteen from either side; the pointer's wash and the chosen row's
 * tint are drawn behind the cells, and what is under a row is closed off with
 * the same hairline - a table is read along its rows, so the lines that run
 * across it are the ones that matter.
 *
 * The container is Qt's own view with a delegate of the family's, the decision
 * the list and the grid already took: a table is what a table view is, and a
 * container of widgets would have to grow sorting, selection, the keyboard and
 * an accessibility interface before it could be one. What is decided here is
 * what a cell and a column name look like.
 *
 * \sa MaterialList
 */
class QX_MATERIAL_EXPORT MaterialTable : public QTableView
{
    Q_OBJECT
public:
    explicit MaterialTable(QWidget *parent = Q_NULLPTR);
    ~MaterialTable() override;

    /*! Names the columns \a labels, in order. */
    void setHeaders(const QStringList &labels);
    /*! Adds a row of cells; a row with more cells than there are columns grows
     *  the table, a shorter one leaves the rest empty. */
    void addRow(const QStringList &cells);
    /*! Replaces the rows. */
    void setRows(const QList<QStringList> &rows);
    /*! How many rows the table holds. */
    int rowCount() const;
    /*! What the cell at \a row and \a column says; empty when there is no such
     *  cell. */
    QString cellText(int row, int column) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
private:
    Q_DISABLE_COPY(MaterialTable)
    QX_DECLARE_PRIVATE(MaterialTable)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTABLE_H
