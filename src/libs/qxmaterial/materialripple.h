/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALRIPPLE_H
#define MATERIALRIPPLE_H

#include "qxmaterial_global.h"

#include <QtCore/QObject>
#include <QtCore/QPointF>

class QColor;
class QEvent;
class QPainter;
class QWidget;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRipplePrivate;

/*!
 * Material's ripple: the answer a surface gives when it is pressed.
 *
 * A press in most toolkits is a flat tint that switches on, which says "this
 * was pressed" and nothing about where. Material's answer is a circle that
 * starts where the pointer landed, spreads to the far corner, and fades as it
 * goes - so the surface answers from the place the user touched it, and the
 * answer travels outwards from there.
 *
 * It is not the state layer, and it does not replace it: the flat wash is what
 * a hover gets, and the ripple is what a press gets on top of it. It is also
 * not the Fluent reveal, which is the *hover* answer and stays where the
 * pointer is; a ripple is over before the pointer has moved.
 *
 * Attaching this to a widget is all that is needed - it watches for the press
 * itself - and the widget paints it inside its own corners, at the end of its
 * surface, so that it lands under what the widget is holding.
 */
class QX_MATERIAL_EXPORT MaterialRipple : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    explicit MaterialRipple(QWidget *widget);
    ~MaterialRipple() override;

    QWidget *widget() const;

    /*! The ink it answers with; the widget's own text colour while none is
     *  set, because a ripple is the surface's own colour coming through. */
    QColor color() const;
    void setColor(const QColor &color);

    /*! True while a ripple is spreading and fading. */
    bool isSpreading() const;
    /*! Where the press landed, which is where the ripple started. */
    QPointF origin() const;
    /*! How far through spreading and fading it is, 0 to 1. */
    qreal progress() const;

    /*! Paints the ripple inside \a rect's corners, over what is already there.
     *  Does nothing while there is no ripple to paint. */
    void paint(QPainter *painter, const QRectF &rect, qreal radius) const;
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialRipple)
    QX_DECLARE_PRIVATE(MaterialRipple)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALRIPPLE_H
