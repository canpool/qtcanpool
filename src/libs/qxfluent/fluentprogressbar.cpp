/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentprogressbar.h"

#include "fluenttokens.h"
#include "qxcontrols/qxcontrolaccessible.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's bar is a track four tall with fully round ends, and its endless
// block is about a third of the track wide.
constexpr qreal kTrackHeight = 4.0;
constexpr qreal kBlockFraction = 0.4;
constexpr qreal kTrackOpacity = 0.12;

/*! The accessible face of a bar: how far along is a number, and an assistive
 *  tool reads it the same way it reads a slider's. */
class FluentProgressBarAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit FluentProgressBarAccessible(FluentProgressBar *bar)
        : QxControls::QxControlAccessible(bar, QAccessible::ProgressBar)
    {
    }

    QVariant currentValue() const override
    {
        return bar()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        bar()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return bar()->maximum();
    }
    QVariant minimumValue() const override
    {
        return bar()->minimum();
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
    FluentProgressBar *bar() const
    {
        return static_cast<FluentProgressBar *>(widget());
    }
};
}   // namespace

class FluentProgressBarPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentProgressBar)

    int minimum = 0;
    int maximum = 100;
    int value = 0;
    bool indeterminate = false;
    /*! Where the sliding block is, as a fraction of its one pass. */
    qreal slide = 0.0;
    QVariantAnimation slider;
};

FluentProgressBar::FluentProgressBar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentProgressBarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A bar is not something the keyboard stops at and not something the
    // pointer works: it reports, and that is all it is.
    setFocusPolicy(Qt::NoFocus);
    d_ptr->slider.setLoopCount(-1);
    d_ptr->slider.setStartValue(qreal(0.0));
    d_ptr->slider.setEndValue(qreal(1.0));
    connect(&d_ptr->slider, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->slide = value.toReal();
        update();
    });
    updateSliding();
}

FluentProgressBar::~FluentProgressBar()
{
    QX_FINI_PRIVATE()
}

int FluentProgressBar::minimum() const
{
    return d_ptr->minimum;
}

int FluentProgressBar::maximum() const
{
    return d_ptr->maximum;
}

int FluentProgressBar::value() const
{
    return d_ptr->value;
}

bool FluentProgressBar::isIndeterminate() const
{
    return d_ptr->indeterminate;
}

void FluentProgressBar::setMinimum(int minimum)
{
    if (d_ptr->minimum == minimum) {
        return;
    }
    d_ptr->minimum = minimum;
    if (d_ptr->maximum < minimum) {
        d_ptr->maximum = minimum;
    }
    if (d_ptr->value < minimum) {
        setValue(minimum);
    }
    update();
}

void FluentProgressBar::setMaximum(int maximum)
{
    if (d_ptr->maximum == maximum) {
        return;
    }
    d_ptr->maximum = maximum;
    if (maximum < d_ptr->minimum) {
        d_ptr->minimum = maximum;
    }
    if (d_ptr->value > maximum) {
        setValue(maximum);
    }
    update();
}

void FluentProgressBar::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
    Q_EMIT valueChanged(d_ptr->value);
}

void FluentProgressBar::setIndeterminate(bool on)
{
    if (d_ptr->indeterminate == on) {
        return;
    }
    d_ptr->indeterminate = on;
    updateSliding();
    update();
}

QSize FluentProgressBar::sizeHint() const
{
    return QSize(120, int(kTrackHeight));
}

QSize FluentProgressBar::minimumSizeHint() const
{
    return QSize(40, int(kTrackHeight));
}

void FluentProgressBar::paintSurface(QPainter *painter)
{
    // The whole track, in the ink of the page thinned: what is left to do.
    QColor muted = textColor();
    muted.setAlphaF(kTrackOpacity);

    const QRectF track(0.0, (height() - kTrackHeight) / 2.0, width(), kTrackHeight);
    QPainterPath path;
    // Half the track's own height, not the round-any-corner number: past half
    // the short side a corner is drawn as an oval arc, and the track would be
    // a lens rather than a bar with round ends.
    path.addRoundedRect(track, kTrackHeight / 2.0, kTrackHeight / 2.0);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillPath(path, muted);
    painter->restore();
}

void FluentProgressBar::paintContent(QPainter *painter)
{
    const QRectF track(0.0, (height() - kTrackHeight) / 2.0, width(), kTrackHeight);
    const qreal radius = kTrackHeight / 2.0;
    QPainterPath path;
    if (d_ptr->indeterminate) {
        // The block that will not end: one pass across the track, looping.
        // While the slide is stopped - reduce motion, disabled - the block
        // holds at the track's left edge: still there, still waiting.
        const qreal block = track.width() * kBlockFraction;
        const bool running = d_ptr->slider.state() == QAbstractAnimation::Running;
        const qreal passed = running ? d_ptr->slide * (track.width() + block) - block : 0.0;
        path.addRoundedRect(QRectF(track.left() + passed, track.top(), block, kTrackHeight), radius, radius);
    } else {
        const int span = d_ptr->maximum - d_ptr->minimum;
        const qreal ratio = span > 0 ? qreal(d_ptr->value - d_ptr->minimum) / qreal(span) : 1.0;
        const qreal fill = track.width() * ratio;
        if (fill <= 0.0) {
            return;
        }
        path.addRoundedRect(QRectF(track.left(), track.top(), fill, kTrackHeight), radius, radius);
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillPath(path, accentColor());
    painter->restore();
}

void FluentProgressBar::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::StyleChange) {
        updateSliding();
    }
}

QColor FluentProgressBar::washColor() const
{
    // An indicator is not interactive, so the states the base tracks have
    // nothing to tint: the track is the answer, and a wash over it would dim
    // the very thing the bar is there to show.
    return QColor();
}

QAccessible::Role FluentProgressBar::accessibleRole() const
{
    return QAccessible::ProgressBar;
}

QAccessibleInterface *FluentProgressBar::accessibleInterface()
{
    return new FluentProgressBarAccessible(this);
}

void FluentProgressBar::updateSliding()
{
    // The slide is the one motion here that is not a transition, and it is
    // the one the reduce-motion switch is about: asked for less motion, the
    // block holds still at the left and still says it is waiting. The
    // duration rides on the family's motion tier, so a theme that retimes
    // its motion retimes the pass.
    const int duration = FluentMotion::forDuration(FluentMotion::Duration::Slower).duration;
    d_ptr->slider.setDuration(duration);
    const bool wanted = d_ptr->indeterminate && isEnabled() && duration > 0 && !QxTheme::isMotionReduced();
    if (wanted && d_ptr->slider.state() != QAbstractAnimation::Running) {
        d_ptr->slider.start();
    } else if (!wanted && d_ptr->slider.state() == QAbstractAnimation::Running) {
        d_ptr->slider.stop();
        d_ptr->slide = 0.0;
    }
    update();
}

QX_FLUENT_END_NAMESPACE
