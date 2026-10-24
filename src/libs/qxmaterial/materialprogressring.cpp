/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialprogressring.h"

#include "qxcontrols/qxcontrolaccessible.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's ring is forty-eight across with a four-wide stroke.
constexpr qreal kRingSize = 48.0;
constexpr qreal kStrokeWidth = 4.0;

// The track - the part the arc has not reached - is the same ink, thinned.
constexpr qreal kTrackOpacity = 0.24;

// One turn of the sweep, and the arc's length within it: the shortest it gets,
// and how much longer than that it grows to.
constexpr int kSweepDuration = 1500;
constexpr qreal kShortestArc = 40.0;
constexpr qreal kLongestArc = 300.0;
// The step a control takes down when nobody can use it - the same .38 the rest
// of the family keeps.
constexpr qreal kDisabledOpacity = 0.38;

// Qt measures arcs in sixteenths of a degree, from three o'clock, going
// counter-clockwise. Material's ring starts at twelve.
constexpr int kFullTurn = 360 * 16;
constexpr int kTop = 90 * 16;

/*! The accessible face of a ring: how far along is a number, and an assistive
 *  tool reads it the same way it reads a slider's. */
class MaterialProgressRingAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit MaterialProgressRingAccessible(MaterialProgressRing *ring)
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
    MaterialProgressRing *ring() const
    {
        return static_cast<MaterialProgressRing *>(widget());
    }
};
}   // namespace

class MaterialProgressRingPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialProgressRing)

    int value = 0;
    bool indeterminate = true;
    /*! Where the sweep is, in degrees: its start, and the length it has grown
     *  to. */
    qreal turn = 0.0;
    QVariantAnimation sweeper;

    /*! The arc a sweep at \a turn draws: starting where it has got to, and
     *  long in proportion to how far through the turn it is - so the ring
     *  grows and shrinks as it goes round, which is one movement rather than
     *  a length going round on its own. */
    void arc(int *start, int *span) const
    {
        const qreal phase = turn / 360.0;
        *start = int(kTop - turn * 16.0);
        const qreal grow = 0.5 - 0.5 * qCos(2.0 * M_PI * phase);
        *span = -int(qRound((kShortestArc + (kLongestArc - kShortestArc) * grow) * 16.0));
    }
};

MaterialProgressRing::MaterialProgressRing(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialProgressRingPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::NoFocus);
    d_ptr->sweeper.setLoopCount(-1);
    d_ptr->sweeper.setStartValue(qreal(0.0));
    d_ptr->sweeper.setEndValue(qreal(360.0));
    d_ptr->sweeper.setDuration(kSweepDuration);
    QObject::connect(&d_ptr->sweeper, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->turn = value.toReal();
        update();
    });
    updateSweeping();
}

MaterialProgressRing::~MaterialProgressRing()
{
    QX_FINI_PRIVATE()
}

int MaterialProgressRing::value() const
{
    return d_ptr->value;
}

bool MaterialProgressRing::isIndeterminate() const
{
    return d_ptr->indeterminate;
}

QSize MaterialProgressRing::sizeHint() const
{
    return QSize(int(kRingSize), int(kRingSize));
}

QSize MaterialProgressRing::minimumSizeHint() const
{
    return QSize(int(kRingSize), int(kRingSize));
}

void MaterialProgressRing::setValue(int value)
{
    const int bounded = qBound(0, value, 100);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
    Q_EMIT valueChanged(bounded);
}

void MaterialProgressRing::setIndeterminate(bool on)
{
    if (d_ptr->indeterminate == on) {
        return;
    }
    d_ptr->indeterminate = on;
    updateSweeping();
    update();
}

void MaterialProgressRing::paintSurface(QPainter *painter)
{
    // A ring nobody can use steps its track and its arc down together.
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    const qreal inset = kStrokeWidth / 2.0;
    const QRectF area = QRectF(rect()).adjusted(inset, inset, -inset, -inset);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::NoBrush);

    if (d_ptr->indeterminate) {
        // No track: an arc that only says something is happening has nothing to
        // be measured against.
        int start = 0;
        int span = 0;
        d_ptr->arc(&start, &span);
        painter->setPen(QPen(accentColor(), kStrokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter->drawArc(area, start, span);
    } else {
        QColor track = accentColor();
        track.setAlphaF(kTrackOpacity);
        painter->setPen(QPen(track, kStrokeWidth, Qt::SolidLine, Qt::RoundCap));
        painter->drawArc(area, kTop, -kFullTurn);
        if (d_ptr->value > 0) {
            painter->setPen(QPen(accentColor(), kStrokeWidth, Qt::SolidLine, Qt::RoundCap));
            painter->drawArc(area, kTop, -int(qRound(d_ptr->value / 100.0 * kFullTurn)));
        }
    }
    painter->restore();
}

void MaterialProgressRing::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::StyleChange) {
        updateSweeping();
    }
}

void MaterialProgressRing::updateSweeping()
{
    // A sweep is the one motion here that is not a transition, and it is the
    // one the reduce-motion switch is about: when the user asked for less
    // motion the ring stops and says what it still can - that it is waiting.
    const bool wanted = d_ptr->indeterminate && isEnabled() && !QxTheme::isMotionReduced();
    if (wanted && d_ptr->sweeper.state() != QAbstractAnimation::Running) {
        d_ptr->sweeper.start();
    } else if (!wanted && d_ptr->sweeper.state() == QAbstractAnimation::Running) {
        d_ptr->sweeper.stop();
    }
}

QAccessible::Role MaterialProgressRing::accessibleRole() const
{
    return QAccessible::ProgressBar;
}

QAccessibleInterface *MaterialProgressRing::accessibleInterface()
{
    return new MaterialProgressRingAccessible(this);
}

QX_MATERIAL_END_NAMESPACE
