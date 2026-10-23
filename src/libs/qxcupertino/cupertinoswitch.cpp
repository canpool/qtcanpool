/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinoswitch.h"

#include "cupertinotokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
// The system's switch measures: a 51 x 31 track with a 27 knob sitting 2 in
// from the edge, so the knob has 20 to travel and the track is a bead's height
// taller than the knob. The slim track is the half of the likeness a shape can
// carry on its own.
constexpr qreal kTrackWidth = 51.0;
constexpr qreal kTrackHeight = 31.0;
constexpr qreal kThumbSize = 27.0;
constexpr qreal kThumbInset = 2.0;

// The off track is the family's grey thinned, not a colour of its own: one
// alpha over the page answers both faces - a pale track on the light one, a
// dark one on the dark one - which a single fixed value could not.
constexpr qreal kOffOpacity = 0.32;

// Where the track changes its mind: the knob crossing the middle. Half is the
// only point that reads the same going both ways, so the turn is one change
// rather than a blend that follows the knob across.
constexpr qreal kFillThreshold = 0.5;

// A disabled control keeps its drawing and loses its ink, as in the rest of
// the family.
constexpr qreal kDisabledOpacity = 0.4;

QRectF trackRect(const QWidget *widget)
{
    // The track is the control, so it sits in the middle of whatever room a
    // layout gave it: a switch stretched across a row still reads as a switch
    // rather than as a bar with a bead parked at one end.
    const qreal x = (widget->width() - kTrackWidth) / 2.0;
    const qreal y = (widget->height() - kTrackHeight) / 2.0;
    return QRectF(x, y, kTrackWidth, kTrackHeight);
}

// Which face of the semantic colours the control is drawn on. The system keeps
// a light and a dark cut of each of its colours, and which one applies is a
// question about the page, not about the theme's name.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class CupertinoSwitchPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoSwitch)

    bool on = false;
    /*! Where the knob is, 0 at the left end and 1 at the right. */
    qreal position = 0.0;
    QVariantAnimation animation;
};

CupertinoSwitch::CupertinoSwitch(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoSwitchPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // The travel is the only thing in here that moves, so it is also the only
    // thing that asks for a duration - and it asks the family's scale, which
    // answers in the shared tiers: the theme's motion scale reaches the travel
    // and the reduce switch silences it without this control checking either.
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });

    // A click is what the base makes of a press and its release; the keyboard
    // is handled here.
    connect(this, &QxControls::QxControl::clicked, this, &CupertinoSwitch::toggle);
}

CupertinoSwitch::~CupertinoSwitch()
{
    QX_FINI_PRIVATE()
}

bool CupertinoSwitch::isOn() const
{
    return d_ptr->on;
}

void CupertinoSwitch::setOn(bool on)
{
    if (d_ptr->on == on) {
        return;
    }
    d_ptr->on = on;

    const QxTheme::QxMotion motion =
        CupertinoMotion::forDuration(CupertinoMotion::Duration::Normal, CupertinoMotion::Easing::Standard);
    const qreal target = on ? 1.0 : 0.0;
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        // No duration is what reduce-motion answers, and it means jump: the
        // end state is what the user asked for, not the journey to it.
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

void CupertinoSwitch::toggle()
{
    setOn(!d_ptr->on);
}

QSize CupertinoSwitch::sizeHint() const
{
    return QSize(int(kTrackWidth), int(kTrackHeight));
}

QSize CupertinoSwitch::minimumSizeHint() const
{
    return sizeHint();
}

qreal CupertinoSwitch::cornerRadius() const
{
    return CupertinoShape::radius(CupertinoShape::Radius::Circular);
}

bool CupertinoSwitch::hitTest(const QPointF &pos) const
{
    // Only the capsule is the control: a switch given a wider cell than it
    // needs must not toggle from the empty room beside it.
    return trackRect(this).contains(pos);
}

QRectF CupertinoSwitch::chromeRect() const
{
    // The wash and the focus ring belong to the track - there is nothing else
    // in the widget for them to belong to.
    return trackRect(this);
}

void CupertinoSwitch::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal radius = kTrackHeight / 2.0;
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // The fill reads the knob's position and not the state, which is what makes
    // the turn happen at the middle: for the first half of the travel the
    // switch is on and still drawn as off.
    if (d_ptr->position >= kFillThreshold) {
        painter->setBrush(CupertinoColor::color(CupertinoColor::Role::Green, dark));
    } else {
        QColor off = CupertinoColor::color(CupertinoColor::Role::Gray, dark);
        off.setAlphaF(kOffOpacity);
        painter->setBrush(off);
    }
    painter->drawRoundedRect(track, radius, radius);

    const qreal travel = kTrackWidth - kThumbSize - 2.0 * kThumbInset;
    const QRectF thumb(track.x() + kThumbInset + d_ptr->position * travel, track.y() + kThumbInset, kThumbSize,
                       kThumbSize);
    // The knob sits over the track, so it is the one lift this control has: a
    // wide, faint shadow close under it, which is this family's raised level.
    QxControls::QxShadowPainter::paint(painter, thumb, kThumbSize / 2.0,
                                       CupertinoColor::color(CupertinoColor::Role::Gray, dark),
                                       CupertinoElevation::shadow(CupertinoElevation::Level::Raised));

    // The knob is the system's white on both faces: it reads as the highlight
    // on the track rather than as a surface of the page, and a theme that
    // darkened it would dim the one thing that makes the travel legible.
    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawEllipse(thumb);

    painter->restore();
}

void CupertinoSwitch::keyPressEvent(QKeyEvent *event)
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

void CupertinoSwitch::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        toggle();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role CupertinoSwitch::accessibleRole() const
{
    // Qt has no role for a switch in the versions this library builds on; the
    // closest true answer is a check box - a control that is on or off - and
    // the name is what says which.
    return QAccessible::CheckBox;
}

void CupertinoSwitch::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->on;
}

QX_CUPERTINO_END_NAMESPACE
