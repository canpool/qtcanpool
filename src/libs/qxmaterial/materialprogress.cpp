/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialprogress.h"

#include "materialtokens.h"
#include "qxcontrols/qxcontrolaccessible.h"

#include "qxtheme/qxthememotion.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's linear track is four tall with fully round ends, and its endless
// block is about a third of the track wide. The unfilled track reads as a
// surface one step from the page, which is the muted ink the slider's track
// is made of.
constexpr qreal kTrackHeight = 4.0;
constexpr qreal kBlockFraction = 0.4;
constexpr qreal kMutedOpacity = 0.24;

/*! The accessible face of a track: how far along is a number, and an assistive
 *  tool reads it the same way it reads a slider's. */
class MaterialProgressAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit MaterialProgressAccessible(MaterialProgress *progress)
        : QxControls::QxControlAccessible(progress, QAccessible::ProgressBar)
    {
    }

    QVariant currentValue() const override
    {
        return progress()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        progress()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return progress()->maximum();
    }
    QVariant minimumValue() const override
    {
        return progress()->minimum();
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
    MaterialProgress *progress() const
    {
        return static_cast<MaterialProgress *>(widget());
    }
};
}   // namespace

class MaterialProgressPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialProgress)

    int minimum = 0;
    int maximum = 100;
    int value = 0;
    bool indeterminate = false;
    /*! Where the sliding block is, as a fraction of its one pass. */
    qreal slide = 0.0;
    QVariantAnimation slider;
};

MaterialProgress::MaterialProgress(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialProgressPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A track is not something the keyboard stops at and not something the
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

MaterialProgress::~MaterialProgress()
{
    QX_FINI_PRIVATE()
}

int MaterialProgress::minimum() const
{
    return d_ptr->minimum;
}

int MaterialProgress::maximum() const
{
    return d_ptr->maximum;
}

int MaterialProgress::value() const
{
    return d_ptr->value;
}

bool MaterialProgress::isIndeterminate() const
{
    return d_ptr->indeterminate;
}

void MaterialProgress::setMinimum(int minimum)
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

void MaterialProgress::setMaximum(int maximum)
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

void MaterialProgress::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
    Q_EMIT valueChanged(d_ptr->value);
}

void MaterialProgress::setIndeterminate(bool on)
{
    if (d_ptr->indeterminate == on) {
        return;
    }
    d_ptr->indeterminate = on;
    updateSliding();
    update();
}

QSize MaterialProgress::sizeHint() const
{
    return QSize(120, int(kTrackHeight));
}

QSize MaterialProgress::minimumSizeHint() const
{
    return QSize(40, int(kTrackHeight));
}

void MaterialProgress::paintSurface(QPainter *painter)
{
    // The whole track: a surface one step from the page, the muted ink the
    // slider's track is made of.
    QColor muted = textColor();
    muted.setAlphaF(kMutedOpacity);

    const QRectF track(0.0, (height() - kTrackHeight) / 2.0, width(), kTrackHeight);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(muted);
    painter->drawRoundedRect(track, kTrackHeight / 2.0, kTrackHeight / 2.0);
    painter->restore();
}

void MaterialProgress::paintContent(QPainter *painter)
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

void MaterialProgress::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange || event->type() == QEvent::StyleChange) {
        updateSliding();
    }
}

QColor MaterialProgress::washColor() const
{
    // An indicator is not interactive, so the states the base tracks have
    // nothing to tint: the track is the answer, and a wash over it would dim
    // the very thing the track is there to show.
    return QColor();
}

QAccessible::Role MaterialProgress::accessibleRole() const
{
    return QAccessible::ProgressBar;
}

QAccessibleInterface *MaterialProgress::accessibleInterface()
{
    return new MaterialProgressAccessible(this);
}

void MaterialProgress::updateSliding()
{
    // The slide is the one motion here that is not a transition, and it is
    // the one the reduce-motion switch is about: asked for less motion, the
    // block holds still at the left and still says it is waiting. The
    // duration rides on the family's motion tier, so a theme that retimes
    // its motion retimes the pass.
    const int duration = MaterialMotion::forDuration(MaterialMotion::Duration::Long).duration;
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

QX_MATERIAL_END_NAMESPACE
