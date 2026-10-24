/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALPAGINATION_H
#define MATERIALPAGINATION_H

#include "qxmaterial_global.h"

#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialPaginationPrivate;

/*!
 * Material's pagination: the row at the foot of a long list that says which
 * part of it is on screen, and takes you to another part.
 *
 * What it shows is not "one number per page" - a list of a thousand pages
 * cannot be drawn - but the window around the page in force: the first and the
 * last page always, \a siblingCount pages either side of the one in force, and
 * an ellipsis wherever pages were left out. MUI's arithmetic is what decides
 * where the ellipsis falls, and it is kept as MUI states it rather than
 * reinvented: two readers of the same list should be told the same thing by the
 * same picture.
 *
 * The arrows at the ends are always there, and are disabled at the ends of the
 * list. A bar that dropped them would be a bar whose buttons move under the
 * pointer as the page changes.
 *
 * Not here, and deliberately: MUI's outlined and rounded variants. Those are
 * ways for a button to wear a surface, and a row of them is a toggle button
 * group's business; a paginator is navigation at the foot of the page and has
 * one shape.
 *
 * The keyboard is one stop, on the page in force, with the arrows moving a page
 * at a time and Home / End going to the ends - a paginator is walked, not
 * tabbed through. The arrows are also why the previous and next buttons carry
 * names of their own: they are what a pointer reaches for and what an assistive
 * tool reads out, and neither is the keyboard's way through the bar.
 */
class QX_MATERIAL_EXPORT MaterialPagination : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
public:
    explicit MaterialPagination(QWidget *parent = Q_NULLPTR);
    ~MaterialPagination() override;

    /*! How many pages there are. A paginator with none is not drawn at all, so
     *  the count never goes below one. */
    int count() const;
    /*! The page on screen, counted from one. */
    int page() const;
    /*! How many pages either side of the one in force are shown. */
    int siblingCount() const;
    /*! How many pages at each end are always shown. */
    int boundaryCount() const;

    /*!
     * The page slots the bar is showing, in order, with \c -1 where an
     * ellipsis stands. The arrows at the ends are not among them - they are the
     * bar's own chrome, and are read back by previousRect() and nextRect().
     */
    QList<int> items() const;
    /*! How many page slots the bar is showing; \c items().count(). */
    int itemCount() const;
    /*! Where page slot \a index sits; empty for an index that is not there. */
    QRect itemRect(int index) const;
    QRect previousRect() const;
    QRect nextRect() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCount(int count);
    /*! Go to \a page; a page that is not there is refused rather than clamped,
     *  because a page nobody asked for is not a page. */
    void setPage(int page);
    void setSiblingCount(int count);
    void setBoundaryCount(int count);

Q_SIGNALS:
    /*! The page on screen changed, whoever changed it. */
    void pageChanged(int page);
    void countChanged(int count);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialPagination)
    QX_DECLARE_PRIVATE(MaterialPagination)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALPAGINATION_H
