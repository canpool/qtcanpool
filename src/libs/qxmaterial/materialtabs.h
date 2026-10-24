/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTABS_H
#define MATERIALTABS_H

#include "qxmaterial_global.h"

#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTabsPrivate;

/*!
 * Material's tabs: the row that says which page of a few is in front.
 *
 * A tab is its name, and under the name that is in force stands the
 * indicator - the accent, two tall - which travels to whichever tab the
 * pointer or the keyboard chose. The name in force is the accent, the rest
 * are the page's ink at six tenths; the row says where one is without
 * saying anything about the others.
 *
 * A row that is given more tabs than it has room for keeps them readable by
 * sliding: the tab in force is brought into view - whole, unless it is wider
 * than the row itself, and then it is shown from its own beginning - and the
 * tabs either side of it are cut off at the row's own edges. That is what
 * tells tabs from the segmented button, where the choices must all be seen at
 * once.
 */
class QX_MATERIAL_EXPORT MaterialTabs : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentChanged)
public:
    explicit MaterialTabs(QWidget *parent = Q_NULLPTR);
    ~MaterialTabs() override;

    /*! Adds a tab named \a label at the end of the row. */
    void addTab(const QString &label);
    /*! How many tabs the row has. */
    int count() const;
    /*! The name of the tab at \a index, empty when there is no such tab. */
    QString tabLabel(int index) const;
    /*! Where the tab at \a index stands, in the widget's own coordinates.
     *  The row slides, so this is where the tab is now, not where it started. */
    QRect tabRect(int index) const;
    /*! Which tab is in front. */
    int currentIndex() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Puts the tab at \a index in front. A tab that is not there is no
     *  answer and is ignored. */
    void setCurrentIndex(int index);
Q_SIGNALS:
    /*! The tab in front changed. */
    void currentChanged(int index);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    /*! The width a tab with the name \a label asks for. */
    int tabWidth(const QString &label) const;
    /*! Moves the row so that the tab in force is fully in view. */
    void keepCurrentInView();
    /*! Puts the indicator under the tab in force, travelling there when
     *  motion is on and standing there at once when it is not. */
    void placeIndicator(bool travelling);
    /*! The tab the point \a pos falls in, or -1 for none. */
    int tabAt(const QPointF &pos) const;

    Q_DISABLE_COPY(MaterialTabs)
    QX_DECLARE_PRIVATE(MaterialTabs)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTABS_H
