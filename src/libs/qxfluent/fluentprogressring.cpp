/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentprogressring.h"

#include "qxcontrols/qxcontrolaccessible.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's ring is drawn in a stroke four wide inside a circle thirty-two
// across, and its indeterminate arc is a quarter of the ring turning.
constexpr qreal kRingSize = 32.0;
constexpr qreal kStrokeWidth = 4.0;
constexpr qreal kArcFraction = 0.25;
constexpr int kTurnDuration = 1500;

// Qt measures arcs in sixteenths of a degree, from three o'clock, going
// counter-clockwise. Fluent's ring starts at twelve.
constexpr int kFullTurn = 360 * 16;
constexpr int kTop = 90 * 16;

/*! The accessible face of a ring: how far along is a number, and an assistive
 *  tool reads it the same way it reads a slider's. */
class FluentProgressRingAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit FluentProgressRingAccessible(FluentProgressRing *ring)
        : QxControls::QxControlAccessible(ring, QAccessible::ProgressBar)
    {
    }

    QVariant currentValue() const override
    {
        return ring()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        ring()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return 100;
    }
    QVariant minimumValue() const override
    {
        return 0;
    }
    QVariant minimumStepSize() const override
    {
        return 1;
    }

    void *interface_cast(QAccessible::InterfaceType type) override
    {
        if (type == QAccessible::ValueInterface) {
            return static_cast<QAccessibleValueInterface *>(this);
        }
        return QxControls::QxControlAccessible::interface_cast(type);
    }
private:
    FluentProgressRing *ring() const
    {
        return static_cast<FluentProgressRing *>(widget());
    }
};
}   // namespace

class FluentProgressRingPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentProgressRing)

    int value = 0;
    bool indeterminate = true;
    /*! Where the turning arc is, in degrees. */
    qreal turn = 0.0;
    QVariantAnimation turner;
};

FluentProgressRing::FluentProgressRing(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentProgressRingPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::NoFocus);
    d_ptr->turner.setLoopCount(-1);
    d_ptr->turner.setStartValue(qreal(0.0));
    d_ptr->turner.setEndValue(qreal(360.0));
    d_ptr->turner.setDuration(kTurnDuration);
    connect(&d_ptr->turner, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->turn = value.toReal();
        update();
    });
    updateTurning();
}

FluentProgressRing::~FluentProgressRing()
{
    QX_FINI_PRIVATE()
}

int FluentProgressRing::value() const
{
    return d_ptr->value;
}

bool FluentProgressRing::isIndeterminate() const
{
    return d_ptr->indeterminate;
}

void FluentProgressRing::setValue(int value)
{
    const int bounded = qBound(0, value, 100);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
}

void FluentProgressRing::setIndeterminate(bool on)
{
    if (d_ptr->indeterminate == on) {
        return;
    }
    d_ptr->indeterminate = on;
    updateTurning();
    update();
}

QSize FluentProgressRing::sizeHint() const
{
    return QSize(int(kRingSize), int(kRingSize));
}

QSize FluentProgressRing::minimumSizeHint() const
{
    return QSize(int(kRingSize) / 2, int(kRingSize) / 2);
}

void FluentProgressRing::paintSurface(QPainter *painter)
{
    const qreal side = qMin(width(), height());
    const qreal inset = kStrokeWidth / 2.0;
    const QRectF ring((width() - side) / 2.0 + inset, (height() - side) / 2.0 + inset, side - kStrokeWidth,
                      side - kStrokeWidth);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(QColor(), 0));

    // The whole ring, in the ink of the page thinned: what is left to do.
    QColor muted = textColor();
    muted.setAlphaF(0.12);
    painter->setPen(QPen(muted, kStrokeWidth));
    painter->drawEllipse(ring);

    // The part that is done, or the part that is turning.
    painter->setPen(QPen(accentColor(), kStrokeWidth, Qt::SolidLine, Qt::RoundCap));
    if (d_ptr->indeterminate) {
        const int start = kTop - int(d_ptr->turn * 16.0);
        painter->drawArc(ring, start, int(kArcFraction * kFullTurn));
    } else if (d_ptr->value > 0) {
        painter->drawArc(ring, kTop, int(-qreal(d_ptr->value) / 100.0 * kFullTurn));
    }
    painter->restore();
}

void FluentProgressRing::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::StyleChange) {
        updateTurning();
    }
}

void FluentProgressRing::updateTurning()
{
    // A turning ring is the one motion here that is not a transition, and it
    // is the one the reduce-motion switch is about: when the user asked for
    // less motion the ring stops and says what it still can - that it is
    // waiting.
    const bool wanted = d_ptr->indeterminate && isEnabled() && !QxTheme::isMotionReduced();
    if (wanted && d_ptr->turner.state() != QAbstractAnimation::Running) {
        d_ptr->turner.start();
    } else if (!wanted && d_ptr->turner.state() == QAbstractAnimation::Running) {
        d_ptr->turner.stop();
    }
    update();
}

QAccessible::Role FluentProgressRing::accessibleRole() const
{
    return QAccessible::ProgressBar;
}

QAccessibleInterface *FluentProgressRing::accessibleInterface()
{
    return new FluentProgressRingAccessible(this);
}

QX_FLUENT_END_NAMESPACE
