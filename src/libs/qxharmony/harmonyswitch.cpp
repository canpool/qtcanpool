/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonyswitch.h"

#include "harmonytokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_HARMONY_BEGIN_NAMESPACE

namespace
{
// The family's switch measures, as this library reads them: a 46 x 26 track
// with a 22 knob sitting 2 in from either end, so the knob has 20 to travel and
// the track is two taller than it at each end. The held knob grows to 28, which
// is what the spring has room for without leaving the capsule.
constexpr qreal kTrackWidth = 46.0;
constexpr qreal kTrackHeight = 26.0;
constexpr qreal kThumbSize = 22.0;
constexpr qreal kThumbHeld = 28.0;
constexpr qreal kThumbInset = 2.0;

// The off track is the family's grey thinned over the page: one alpha answers
// both faces - a pale track on the light one, a dark one on the dark one -
// which a single fixed value could not.
constexpr qreal kOffOpacity = 0.28;

// A disabled control keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;

QRectF trackRect(const QWidget *widget)
{
    // The track is the control, so it sits in the middle of whatever room a
    // layout gave it rather than being pinned to the left.
    const qreal x = (widget->width() - kTrackWidth) / 2.0;
    const qreal y = (widget->height() - kTrackHeight) / 2.0;
    return QRectF(x, y, kTrackWidth, kTrackHeight);
}

/*! The knob, at \a position along its travel. Held, it grows into a rounded
 *  rectangle around the place it holds and is kept inside the track; let go, it
 *  is a circle wherever the spring has taken it - which is how the spring shows
 *  at all: the last stretch of its travel is outside the room the knob rests
 *  in. */
QRectF knobRect(const QxControls::QxControl *control, qreal position)
{
    const QRectF track = trackRect(control);
    const qreal top = track.y() + kThumbInset;
    const qreal restLeft = track.x() + kThumbInset + position * (kTrackWidth - kThumbSize - 2.0 * kThumbInset);
    if (control->state() != QxControls::QxControl::State::Pressed) {
        return QRectF(restLeft, top, kThumbSize, kThumbSize);
    }
    const qreal left = qBound(track.x() + kThumbInset, restLeft - (kThumbHeld - kThumbSize) / 2.0,
                              track.x() + kTrackWidth - kThumbInset - kThumbHeld);
    return QRectF(left, top, kThumbHeld, kThumbSize);
}

// Which face of the semantic colours the control is drawn on. The system keeps
// a light and a dark cut of each of its colours, and which one applies is a
// question about the page, not about the theme's name.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class HarmonySwitchPrivate
{
public:
    QX_DECLARE_PUBLIC(HarmonySwitch)

    bool on = false;
    /*! Where the knob is, 0 at the left end and 1 at the right; the spring
     *  takes it a little past either end before it settles. */
    qreal position = 0.0;
    QVariantAnimation animation;
};

HarmonySwitch::HarmonySwitch(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new HarmonySwitchPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // The travel asks the family's scale for its curve, which is the springy
    // one: it is what makes the knob arrive with a little more than it needs.
    // The scale answers in the shared tiers, so the theme's motion scale
    // reaches the travel and the reduce switch silences it without this control
    // checking either.
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });

    // A click is what the base makes of a press and its release; the keyboard
    // is handled here.
    connect(this, &QxControls::QxControl::clicked, this, &HarmonySwitch::toggle);
}

HarmonySwitch::~HarmonySwitch()
{
    QX_FINI_PRIVATE()
}

bool HarmonySwitch::isOn() const
{
    return d_ptr->on;
}

void HarmonySwitch::setOn(bool on)
{
    if (d_ptr->on == on) {
        return;
    }
    d_ptr->on = on;

    const QxTheme::QxMotion motion =
        HarmonyMotion::forDuration(HarmonyMotion::Duration::Normal, HarmonyMotion::Easing::Springy);
    const qreal target = on ? 1.0 : 0.0;
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        // No duration is what reduce-motion answers, and it means jump: the end
        // state is what the user asked for, not the journey to it - and a
        // journey is where a spring would show.
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

void HarmonySwitch::toggle()
{
    setOn(!d_ptr->on);
}

QSize HarmonySwitch::sizeHint() const
{
    return QSize(int(kTrackWidth), int(kTrackHeight));
}

QSize HarmonySwitch::minimumSizeHint() const
{
    return sizeHint();
}

qreal HarmonySwitch::cornerRadius() const
{
    return HarmonyShape::radius(HarmonyShape::Radius::Circular);
}

bool HarmonySwitch::hitTest(const QPointF &pos) const
{
    // Only the capsule is the control: a switch given a wider cell than it
    // needs must not toggle from the empty room beside it.
    return trackRect(this).contains(pos);
}

QRectF HarmonySwitch::chromeRect() const
{
    // The wash and the focus ring belong to the track - there is nothing else
    // in the widget for them to belong to.
    return trackRect(this);
}

void HarmonySwitch::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal radius = kTrackHeight / 2.0;
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // On is the brand's blue, off is the family's grey thinned; the turn
    // happens with the state and not with the knob, because here the colour is
    // not the thing that carries the travel - the spring is.
    if (d_ptr->on) {
        painter->setBrush(HarmonyColor::color(HarmonyColor::Role::Accent, dark));
    } else {
        QColor off = HarmonyColor::color(HarmonyColor::Role::Gray, dark);
        off.setAlphaF(kOffOpacity);
        painter->setBrush(off);
    }
    painter->drawRoundedRect(track, radius, radius);

    const QRectF knob = knobRect(this, d_ptr->position);
    // The knob sits over the track, so it is the one lift this control has: a
    // tight, dark shadow close under it, which is this family's raised level.
    QxControls::QxShadowPainter::paint(painter, knob, kThumbSize / 2.0,
                                       HarmonyColor::color(HarmonyColor::Role::Gray, dark),
                                       HarmonyElevation::shadow(HarmonyElevation::Level::Raised));

    // The knob is the system's white on both faces: it reads as the lit side of
    // the track rather than as a surface of the page, and a theme that darkened
    // it would dim the one thing that makes the travel legible. Held, it is a
    // rounded rectangle of the family's medium corner - one state, not a shape
    // for every distance travelled.
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    if (state() == QxControl::State::Pressed) {
        const qreal corner = HarmonyShape::radius(HarmonyShape::Radius::Medium);
        painter->drawRoundedRect(knob, corner, corner);
    } else {
        painter->drawEllipse(knob);
    }

    painter->restore();
}

void HarmonySwitch::keyPressEvent(QKeyEvent *event)
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

void HarmonySwitch::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        toggle();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role HarmonySwitch::accessibleRole() const
{
    // Qt has no role for a switch in the versions this library builds on; the
    // closest true answer is a check box - a control that is on or off - and
    // the name is what says which.
    return QAccessible::CheckBox;
}

void HarmonySwitch::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->on;
}

QX_HARMONY_END_NAMESPACE
