/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSKELETON_H
#define MATERIALSKELETON_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSkeletonPrivate;

/*!
 * Material's skeleton: the shape of what has not arrived yet.
 *
 * A screen that is waiting can wait two ways: it can show nothing and then
 * jump, or it can show the shape of what is coming - a line where a line will
 * be, a disc where a face will be - so that when it arrives it lands where the
 * user was already looking. A skeleton is the second, and it is the difference
 * between a screen that loads and a screen that lurches.
 *
 * Material does not name this one, and the shape is drawn the way a surface
 * here is: a tint of the page's own ink. What it does is breathe, because a
 * shape that sits perfectly still is a shape the user has started to read.
 *
 * It is not content and it is not a control: it takes neither the pointer nor a
 * stop on the way through the keyboard, and it says nothing of its own - the
 * application is the one that can say what is loading.
 */
class QX_MATERIAL_EXPORT MaterialSkeleton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Shape shape READ shape WRITE setShape)
public:
    enum class Shape {
        Rectangle, /*!< A line of text, a field, a row. */
        Circle     /*!< An avatar, anything round that has a face. */
    };
    Q_ENUM(Shape)

    explicit MaterialSkeleton(Shape shape = Shape::Rectangle, QWidget *parent = Q_NULLPTR);
    ~MaterialSkeleton() override;

    Shape shape() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setShape(Shape shape);
protected:
    qreal cornerRadius() const override;
    QColor washColor() const override;
    bool hitTest(const QPointF &pos) const override;
    void paintSurface(QPainter *painter) override;
    void changeEvent(QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    void updateBreathing();

    Q_DISABLE_COPY(MaterialSkeleton)
    QX_DECLARE_PRIVATE(MaterialSkeleton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSKELETON_H
