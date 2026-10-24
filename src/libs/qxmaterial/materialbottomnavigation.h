/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALBOTTOMNAVIGATION_H
#define MATERIALBOTTOMNAVIGATION_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialBottomNavigationPrivate;

/*!
 * Material's bottom navigation: where a Material application keeps its few
 * destinations when the screen is a phone and the destinations are what the
 * thumb reaches.
 *
 * It is the other half of \c MaterialNavigationRail, and the two answer the
 * same question with the same answer - the destination in force wears the
 * accent's shape behind its icon - and differ only in where they stand: a rail
 * is a column at the side of a wide screen, a bottom bar is a row along the
 * bottom of a narrow one. Material's own guidance is to pick one and not to
 * show both at once, so neither of these is a variant of the other; they are
 * the same idea on two edges.
 *
 * A destination is a QAction, as it is on the rail, because that is what an
 * application already has. What the bar adds is the label: a row along the
 * bottom has room for a word where a column at the side has room for one too,
 * but a bar's icons are read at arm's length and the word is what makes them
 * more than five guesses - so the label is drawn under the icon, small, and in
 * the accent once its destination is the one in force.
 *
 * The bar is fifty-six tall and takes whatever width it is given, dividing it
 * evenly: it is a row across the bottom of the screen, and a parent that
 * hands it more room is widening the bar rather than the gap beside it. More
 * destinations than the width can hold do not make it taller - a bar is a row
 * - and the labels elide rather than the destinations being dropped.
 *
 * \sa MaterialNavigationRail
 */
class QX_MATERIAL_EXPORT MaterialBottomNavigation : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    explicit MaterialBottomNavigation(QWidget *parent = Q_NULLPTR);
    ~MaterialBottomNavigation() override;

    /*! Adds \a label as a destination and returns it. The bar owns it. */
    QAction *addDestination(const QString &label, const QIcon &icon = QIcon());
    void removeDestination(QAction *action);
    void clear();
    QList<QAction *> destinations() const;

    QAction *currentDestination() const;
    int currentIndex() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentDestination(QAction *action);

Q_SIGNALS:
    void currentIndexChanged(int index);
    void currentDestinationChanged(QAction *action);
protected:
    /*! Nothing: a bar is a row of destinations with a pill behind one of them,
     *  and the wash and the ring belong to a destination. */
    QRectF chromeRect() const override;

    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialBottomNavigation)
    QX_DECLARE_PRIVATE(MaterialBottomNavigation)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALBOTTOMNAVIGATION_H
