/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALNAVIGATIONRAIL_H
#define MATERIALNAVIGATIONRAIL_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;
class QWheelEvent;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialNavigationRailPrivate;

/*!
 * Material's navigation rail: where a Material application keeps its few
 * destinations when there are only a few.
 *
 * Material has two shapes for "where you can go" and the choice between them is
 * a choice about the screen, not about taste: a rail is for the handful of
 * destinations a phone or a narrow window can show at once, and a drawer is for
 * the rest. Qt has neither, and its nearest thing - a QToolBar turned on its
 * end - is a row of buttons with a frame and a grip, drawn by the style.
 *
 * What makes a rail a rail is the indicator: the destination in force is not
 * marked by tinting the row but by a pill behind its icon, which is the same
 * answer Material gives everywhere - the thing in force wears a shape, and the
 * shape is the accent's. A destination here is a QAction, because that is what
 * an application already has, and the rail has nothing to say about what
 * happens when one is chosen beyond which one it is.
 *
 * A rail is eighty wide and does not stretch: it is a column the screen puts
 * beside the room the destinations open into, so the room is what grows. It is
 * fixed across and preferred down, which is what a parent layout is told; a
 * rail made wider than eighty by hand keeps its column in the middle of the
 * room rather than against the left edge of it.
 *
 * More destinations than the room holds does not make a wider rail - a rail is
 * a column - so the column travels: the wheel moves it, the keyboard keeps the
 * destination it walks to in sight, and what is above or below the room is
 * clipped rather than drawn outside it. Nothing below the last destination is
 * reachable by aiming at it, which is what "not in the room" has to mean.
 */
class QX_MATERIAL_EXPORT MaterialNavigationRail : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool folded READ isFolded WRITE setFolded NOTIFY foldedChanged)
public:
    explicit MaterialNavigationRail(QWidget *parent = Q_NULLPTR);
    ~MaterialNavigationRail() override;

    /*! Adds \a label as a destination and returns it. The rail owns it. */
    QAction *addDestination(const QString &label, const QIcon &icon = QIcon());
    void removeDestination(QAction *action);
    void clear();
    QList<QAction *> destinations() const;

    QAction *currentDestination() const;
    int currentIndex() const;

    /*! Hands \a widget the top of the rail, above every destination. It is
     *  where Material puts a floating action button: the one thing that is not
     *  a destination but belongs above them. */
    void setHeaderWidget(QWidget *widget);
    QWidget *headerWidget() const;

    /*!
     * Whether the rail is folded to its icons.
     *
     * A rail is a column, and a column costs a third of a narrow window - so a
     * screen that has the room for a page but not for a page and a column folds
     * the column to its icons and keeps the destinations. The words are the
     * first thing to go because the pill already says which destination is in
     * force and an icon is what the row was aimed at anyway; the label is not
     * lost, it is what the pointer is told when it rests on a folded row, and
     * it is still the name an assistive tool reads.
     */
    bool isFolded() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentDestination(QAction *action);
    /*! Fold the rail to its icons, or unfold it. A folded rail is narrower: the
     *  width it asks for is the icons' and not the words'. */
    void setFolded(bool folded);

Q_SIGNALS:
    void currentIndexChanged(int index);
    void currentDestinationChanged(QAction *action);
    void foldedChanged(bool folded);
protected:
    /*! Nothing: a rail is a column of destinations with a pill on one of them,
     *  and the wash and the ring belong to a destination. */
    QRectF chromeRect() const override;

    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialNavigationRail)
    QX_DECLARE_PRIVATE(MaterialNavigationRail)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALNAVIGATIONRAIL_H
