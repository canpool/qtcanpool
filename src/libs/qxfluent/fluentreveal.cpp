/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentreveal.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QRadialGradient>
#include <QtWidgets/QWidget>
#include <QEvent>
#include <QPointer>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentRevealPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentReveal)

    QPointer<QWidget> widget;
    QPointF pointer;
    bool active = false;

    void move(const QPointF &pos);
    void clear();
};

void FluentRevealPrivate::move(const QPointF &pos)
{
    if (active && pointer == pos) {
        return;
    }
    pointer = pos;
    active = true;
    // The lit place is the pointer's, so a move is a repaint - the surface
    // under it has changed even though nothing else about it has.
    if (widget) {
        widget->update();
    }
}

void FluentRevealPrivate::clear()
{
    if (!active) {
        return;
    }
    active = false;
    if (widget) {
        widget->update();
    }
}

FluentReveal::FluentReveal(QWidget *widget)
    : QObject(widget)
    , d_ptr(new FluentRevealPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->widget = widget;
    if (widget) {
        // Following the pointer means hearing about it between the clicks,
        // which a widget only does when it has asked for moves without one.
        widget->setMouseTracking(true);
        widget->installEventFilter(this);
    }
}

FluentReveal::~FluentReveal(){QX_FINI_PRIVATE()}

QWidget *FluentReveal::widget() const
{
    return d_ptr->widget.data();
}

bool FluentReveal::isActive() const
{
    return d_ptr->active;
}

QPointF FluentReveal::pointer() const
{
    return d_ptr->pointer;
}

void FluentReveal::paint(QPainter *painter, const QRectF &rect, qreal radius, const QColor &color) const
{
    if (!d_ptr->active) {
        return;
    }
    // The reach is the surface's own longer side: far enough that a pointer in
    // a corner lights the whole of it, and no further - a wash that covered
    // more than the surface would be a flat tint with extra steps.
    const qreal reach = qMax(rect.width(), rect.height());
    QRadialGradient gradient(d_ptr->pointer, reach);
    QColor inner = color;
    inner.setAlphaF(QxControls::QxStateLayer::hoverOpacity);
    QColor outer = color;
    outer.setAlphaF(0.0);
    gradient.setColorAt(0.0, inner);
    gradient.setColorAt(1.0, outer);

    painter->save();
    QPainterPath clip;
    clip.addRoundedRect(rect, radius, radius);
    painter->setClipPath(clip);
    painter->setPen(Qt::NoPen);
    painter->setBrush(gradient);
    painter->drawRect(rect);
    painter->restore();
}

bool FluentReveal::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != d_ptr->widget) {
        return QObject::eventFilter(watched, event);
    }
    switch (event->type()) {
    case QEvent::Enter:
        // An enter carries no position of its own before Qt 6, and the cursor
        // is where the pointer is anyway.
        d_ptr->move(d_ptr->widget->mapFromGlobal(QCursor::pos()));
        break;
    case QEvent::Leave:
        d_ptr->clear();
        break;
    case QEvent::MouseMove:
        d_ptr->move(eventPos(static_cast<QMouseEvent *>(event)));
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}

QX_FLUENT_END_NAMESPACE
