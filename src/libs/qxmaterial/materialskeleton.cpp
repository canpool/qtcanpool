/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialskeleton.h"

#include "materialtokens.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A line of text and a face are not the same size, and a skeleton is the size
// of the thing it stands in for.
constexpr QSize kLineSize(120, 16);
constexpr QSize kCircleSize(40, 40);

// The tint breathes between these two: both faint, because a skeleton is not
// content and must not be read as if it were.
constexpr qreal kFaintest = 0.08;
constexpr qreal kStrongest = 0.24;

// One breath. Like a turning ring this is not a transition - nothing is moving
// from one state to another - so it reads the reduce-motion switch itself.
constexpr int kBreathDuration = 1500;
}   // namespace

class MaterialSkeletonPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSkeleton)

    MaterialSkeleton::Shape shape = MaterialSkeleton::Shape::Rectangle;
    /*! How far through a breath it is, 0 to 1; half way when it is not
     *  breathing. */
    qreal breath = 0.5;
    QVariantAnimation breather;

    qreal tint() const
    {
        return kFaintest + (kStrongest - kFaintest) * (0.5 - 0.5 * qCos(2.0 * M_PI * breath));
    }
};

MaterialSkeleton::MaterialSkeleton(Shape shape, QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialSkeletonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // What is not there yet is neither a stop on the way through the keyboard
    // nor something the pointer takes hold of.
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    d_ptr->breather.setLoopCount(-1);
    d_ptr->breather.setStartValue(qreal(0.0));
    d_ptr->breather.setEndValue(qreal(1.0));
    d_ptr->breather.setDuration(kBreathDuration);
    QObject::connect(&d_ptr->breather, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->breath = value.toReal();
        update();
    });
    setShape(shape);
    updateBreathing();
}

MaterialSkeleton::~MaterialSkeleton(){QX_FINI_PRIVATE()}

MaterialSkeleton::Shape MaterialSkeleton::shape() const
{
    return d_ptr->shape;
}

QSize MaterialSkeleton::sizeHint() const
{
    return d_ptr->shape == Shape::Circle ? kCircleSize : kLineSize;
}

QSize MaterialSkeleton::minimumSizeHint() const
{
    return QSize(8, 8);
}

void MaterialSkeleton::setShape(Shape shape)
{
    if (d_ptr->shape == shape) {
        return;
    }
    d_ptr->shape = shape;
    updateGeometry();
    update();
}

qreal MaterialSkeleton::cornerRadius() const
{
    return d_ptr->shape == Shape::Circle ? MaterialShape::radius(MaterialShape::Radius::Circular)
                                         : MaterialShape::radius(MaterialShape::Radius::Small);
}

QColor MaterialSkeleton::washColor() const
{
    // There is nothing here to answer the pointer with.
    return QColor();
}

bool MaterialSkeleton::hitTest(const QPointF &pos) const
{
    Q_UNUSED(pos)
    return false;
}

void MaterialSkeleton::paintSurface(QPainter *painter)
{
    const QRectF area = QRectF(rect());
    const qreal radius = cornerRadius();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    QColor tint = textColor();
    tint.setAlphaF(d_ptr->tint());
    painter->setPen(Qt::NoPen);
    painter->setBrush(tint);
    painter->drawRoundedRect(area, radius, radius);
    painter->restore();
}

void MaterialSkeleton::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::StyleChange) {
        updateBreathing();
    }
}

void MaterialSkeleton::updateBreathing()
{
    // Breathing is not a transition, and it is the kind of movement the
    // reduce-motion switch is about: a shape that keeps changing is a shape the
    // user cannot stop reading. So it stops, and holds the middle of its breath
    // - which is a shape, and says nothing is here yet.
    const bool wanted = isEnabled() && !QxTheme::isMotionReduced();
    if (wanted && d_ptr->breather.state() != QAbstractAnimation::Running) {
        d_ptr->breather.start();
    } else if (!wanted && d_ptr->breather.state() == QAbstractAnimation::Running) {
        d_ptr->breather.stop();
    }
}

QAccessible::Role MaterialSkeleton::accessibleRole() const
{
    // It stands where a line of text will be, and it says nothing of its own:
    // what is loading is the application's to name.
    return QAccessible::StaticText;
}

QX_MATERIAL_END_NAMESPACE
