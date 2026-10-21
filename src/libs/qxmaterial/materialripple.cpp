/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialripple.h"

#include "materialtokens.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QWidget>
#include <QEvent>
#include <QPointer>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A ripple is a faint thing: it is the surface answering, not a second surface.
constexpr qreal kPeakOpacity = 0.24;
// How much of the ripple's life it takes to come up to its full strength; the
// rest is it going away.
constexpr qreal kRiseFraction = 0.2;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class MaterialRipplePrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialRipple)

    QPointer<QWidget> widget;
    QColor color;
    QPointF origin;
    qreal progress = 0.0;
    bool spreading = false;
    QVariantAnimation animation;

    QColor ink() const;
    void start(const QPointF &pos);
    void stop();
};

QColor MaterialRipplePrivate::ink() const
{
    if (color.isValid()) {
        return color;
    }
    // No colour given means the widget's own: a ripple is the surface's colour
    // coming through, not a colour the ripple brought with it.
    return widget ? widget->palette().color(QPalette::WindowText) : QColor();
}

void MaterialRipplePrivate::start(const QPointF &pos)
{
    if (!widget) {
        return;
    }
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Medium);
    if (motion.duration <= 0) {
        // Nothing moves, so there is nothing to draw: a ripple is the movement.
        return;
    }
    origin = pos;
    progress = 0.0;
    spreading = true;
    animation.stop();
    animation.setDuration(motion.duration);
    animation.setEasingCurve(motion.curve);
    animation.setStartValue(qreal(0.0));
    animation.setEndValue(qreal(1.0));
    animation.start();
    widget->update();
}

void MaterialRipplePrivate::stop()
{
    spreading = false;
    progress = 0.0;
    if (widget) {
        widget->update();
    }
}

MaterialRipple::MaterialRipple(QWidget *widget)
    : QObject(widget)
    , d_ptr(new MaterialRipplePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->widget = widget;
    if (widget) {
        widget->installEventFilter(this);
        QObject::connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
            d_ptr->progress = value.toReal();
            if (d_ptr->widget) {
                d_ptr->widget->update();
            }
        });
        QObject::connect(&d_ptr->animation, &QVariantAnimation::finished, this, [this]() {
            d_ptr->stop();
        });
    }
}

MaterialRipple::~MaterialRipple(){QX_FINI_PRIVATE()}

QWidget *MaterialRipple::widget() const
{
    return d_ptr->widget;
}

QColor MaterialRipple::color() const
{
    return d_ptr->color;
}

void MaterialRipple::setColor(const QColor &color)
{
    d_ptr->color = color;
}

bool MaterialRipple::isSpreading() const
{
    return d_ptr->spreading;
}

QPointF MaterialRipple::origin() const
{
    return d_ptr->origin;
}

qreal MaterialRipple::progress() const
{
    return d_ptr->progress;
}

void MaterialRipple::paint(QPainter *painter, const QRectF &rect, qreal radius) const
{
    if (!d_ptr->spreading || d_ptr->progress <= 0.0 || !d_ptr->widget) {
        return;
    }
    // It rises to its strength quickly and then goes away: a ripple that
    // stayed would be a second surface rather than an answer.
    const qreal rising = qMin(qreal(1.0), d_ptr->progress / kRiseFraction);
    const qreal opacity = kPeakOpacity * rising * (1.0 - d_ptr->progress);
    if (opacity <= 0.0) {
        return;
    }
    // It reaches the furthest corner from where it started: a ripple that
    // stopped short would be a circle inside the control rather than the
    // control answering.
    qreal reach = 0.0;
    for (const QPointF &corner : {rect.topLeft(), rect.topRight(), rect.bottomLeft(), rect.bottomRight()}) {
        reach = qMax(reach, QLineF(d_ptr->origin, corner).length());
    }
    const qreal grown = reach * d_ptr->progress;
    if (grown <= 0.0) {
        return;
    }

    QColor ink = d_ptr->ink();
    if (!ink.isValid()) {
        return;
    }
    ink.setAlphaF(ink.alphaF() * opacity);

    painter->save();
    QPainterPath clip;
    clip.addRoundedRect(rect, radius, radius);
    painter->setClipPath(clip);
    painter->setPen(Qt::NoPen);
    painter->setBrush(ink);
    painter->drawEllipse(QRectF(d_ptr->origin.x() - grown, d_ptr->origin.y() - grown, grown * 2.0, grown * 2.0));
    painter->restore();
}

bool MaterialRipple::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->widget && event->type() == QEvent::MouseButtonPress) {
        const auto *mouse = static_cast<const QMouseEvent *>(event);
        if (mouse->button() == Qt::LeftButton) {
            d_ptr->start(eventPos(mouse));
        }
    }
    // The press is the widget's to answer as well: a ripple is what the surface
    // does about it, not what it does instead.
    return QObject::eventFilter(watched, event);
}

QX_MATERIAL_END_NAMESPACE
