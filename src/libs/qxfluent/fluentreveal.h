/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTREVEAL_H
#define FLUENTREVEAL_H

#include "qxfluent_global.h"

#include <QtCore/QObject>
#include <QtCore/QPointF>

class QColor;
class QEvent;
class QPainter;
class QWidget;

QX_FLUENT_BEGIN_NAMESPACE

class FluentRevealPrivate;

/*!
 * Fluent's reveal: the wash that follows the pointer.
 *
 * A hover in most toolkits is a flat tint that switches on, which says "the
 * pointer is somewhere on this" and nothing about where. Fluent's answer is a
 * wash that is strongest under the pointer and fades away from it, so the
 * surface looks lit from wherever the user is - and as the pointer travels,
 * the lit place travels with it.
 *
 * It is the hover wash, not an extra one: the same ink at the same opacity,
 * given a shape by where the pointer is. A control that wears it therefore
 * stops painting the flat one, and says so by answering \c washColor() with an
 * invalid colour.
 *
 * Tracking the pointer needs moves without a button, which is a setting on the
 * widget and not something a painter can arrange on its own: attaching this to
 * a widget turns mouse tracking on.
 */
class QX_FLUENT_EXPORT FluentReveal : public QObject
{
    Q_OBJECT
public:
    explicit FluentReveal(QWidget *widget);
    ~FluentReveal() override;

    QWidget *widget() const;
    /*! True while the pointer is on the widget; a wash that outlived the
     *  pointer would be a wash lit from somewhere the user is not. */
    bool isActive() const;
    QPointF pointer() const;

    /*! Paints the wash over \a rect, inside its corners, in \a color. Does
     *  nothing while the pointer is elsewhere. */
    void paint(QPainter *painter, const QRectF &rect, qreal radius, const QColor &color) const;
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(FluentReveal)
    QX_DECLARE_PRIVATE(FluentReveal)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTREVEAL_H
