/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentswitch.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's switch measures: a 40x20 track with a 12 knob sitting 4 in from
// either end, so the knob has 20 to travel. The gap is the space between the
// track and the sentence beside it.
constexpr qreal kTrackWidth = 40.0;
constexpr qreal kTrackHeight = 20.0;
constexpr qreal kThumbSize = 12.0;
constexpr qreal kThumbInset = 4.0;
constexpr qreal kTextGap = 8.0;
constexpr qreal kStrokeWidth = 1.0;

// A stroke is not a colour of its own in Fluent: it is the ink the page is
// written in, thinned. The alpha is what carries the tier, the same way the
// state layer carries a state.
constexpr qreal kStrokeOpacity = 0.10;

// A disabled control is the same drawing thinned - the geometry does not
// move, only the ink does, which is how Fluent draws one.
constexpr qreal kDisabledOpacity = 0.4;

QRectF trackRect(const QWidget *widget)
{
    // The widget may be taller than its track - a layout is free to give it
    // more room - so the track sits in the middle of what it was given.
    const qreal y = (widget->height() - kTrackHeight) / 2.0;
    return QRectF(0.0, y, kTrackWidth, kTrackHeight);
}
}   // namespace

class FluentSwitchPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentSwitch)

    bool on = false;
    /*! Where the knob is, 0 at the left end and 1 at the right. */
    qreal position = 0.0;
    QString text;
    QVariantAnimation animation;
};

FluentSwitch::FluentSwitch(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentSwitchPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // The knob's travel is the only thing in here that moves, and it moves on
    // a Fluent duration - which is a motion tier, so the reduce switch and the
    // theme's scale reach it without the control asking about either.
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });

    // A click is what the base makes of a press and its release; the keyboard
    // is handled here.
    connect(this, &QxControls::QxControl::clicked, this, &FluentSwitch::toggle);
}

FluentSwitch::~FluentSwitch()
{
    QX_FINI_PRIVATE()
}

bool FluentSwitch::isOn() const
{
    return d_ptr->on;
}

void FluentSwitch::setOn(bool on)
{
    if (d_ptr->on == on) {
        return;
    }
    d_ptr->on = on;

    const QxTheme::QxMotion motion = FluentMotion::forDuration(FluentMotion::Duration::Fast);
    const qreal target = on ? 1.0 : 0.0;
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        // No duration is the answer reduce-motion gives, and it means jump:
        // the end state is what the user asked for, not the journey.
        d_ptr->position = target;
        update();
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->position);
        d_ptr->animation.setEndValue(target);
        d_ptr->animation.start();
    }
    Q_EMIT toggled(on);
}

void FluentSwitch::toggle()
{
    setOn(!d_ptr->on);
}

QString FluentSwitch::text() const
{
    return d_ptr->text;
}

void FluentSwitch::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // The sentence is the accessible name for as long as the application has
    // not given the control a better one - which it does by simply setting it.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
}

QSize FluentSwitch::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : int(kTextGap + metrics.horizontalAdvance(d_ptr->text));
    return QSize(int(kTrackWidth) + textWidth, int(kTrackHeight));
}

QSize FluentSwitch::minimumSizeHint() const
{
    return QSize(int(kTrackWidth), int(kTrackHeight));
}

qreal FluentSwitch::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Circular);
}

QRectF FluentSwitch::chromeRect() const
{
    // The wash and the focus ring belong to the track, not to the row: a hover
    // that tinted the sentence would be dimming the label.
    return trackRect(this);
}

void FluentSwitch::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal radius = kTrackHeight / 2.0;

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    if (d_ptr->on) {
        // On is the accent: the whole track fills, and no stroke is drawn,
        // because an emphasized surface needs no edge to be found.
        painter->setBrush(accentColor());
        painter->drawRoundedRect(track, radius, radius);
    } else {
        // Off is the page, seen through a stroke.
        painter->setBrush(palette().color(QPalette::Window));
        painter->drawRoundedRect(track, radius, radius);
        QColor stroke = textColor();
        stroke.setAlphaF(kStrokeOpacity);
        painter->setPen(QPen(stroke, kStrokeWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(track, radius, radius);
    }

    const qreal travel = kTrackWidth - kThumbSize - 2.0 * kThumbInset;
    const QRectF thumb(kThumbInset + d_ptr->position * travel, track.y() + kThumbInset, kThumbSize, kThumbSize);
    // The knob is the ink of the page while the switch is off and the ink of
    // the accent while it is on: it changes colour as it crosses, which is
    // what makes the travel legible at a glance.
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->on ? palette().color(QPalette::HighlightedText) : textColor());
    painter->drawEllipse(thumb);

    painter->restore();
}

void FluentSwitch::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    const QRectF label(kTrackWidth + kTextGap, 0.0, width() - kTrackWidth - kTextGap, height());
    painter->save();
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    painter->setPen(textColor());
    painter->drawText(label, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->text);
    painter->restore();
}

void FluentSwitch::keyPressEvent(QKeyEvent *event)
{
    // Enter answers on the press the way a button does; Space answers on the
    // release, so holding it down does not flip the switch over and over.
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        toggle();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void FluentSwitch::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        toggle();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role FluentSwitch::accessibleRole() const
{
    // Qt has no role for a switch in the versions this library builds on; the
    // closest true answer is a check box - a control that is on or off - and
    // the name is what says which.
    return QAccessible::CheckBox;
}

void FluentSwitch::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->on;
}

QX_FLUENT_END_NAMESPACE
