/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinoactivityindicator.h"

#include "cupertinotokens.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtGui/QShowEvent>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
/*! Eight spokes, as the system draws them: one step of the turn is a spoke,
 *  so a full turn takes eight of them. */
constexpr int kSpokes = 8;

/*! The side the control asks for: the small indicator of the system, which is
 *  the one a row or a page uses in place of a sentence. */
constexpr qreal kSide = 20.0;

/*! How wide a spoke is drawn, relative to the side, and how far from the
 *  centre it starts. Both are proportions of the side, so the control keeps
 *  its look when a layout stretches it. */
constexpr qreal kSpokeWidthRatio = 1.0 / 12.0;
constexpr qreal kSpokeInnerRatio = 0.42;

/*! The faintest spoke of the eight. The fade is what says which way the wheel
 *  is going, so it has to be visible all the way round - a spoke that faded
 *  to nothing would make the wheel look like it had a gap. */
constexpr qreal kFaintest = 0.18;

bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class CupertinoActivityIndicatorPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoActivityIndicator)

    bool animating = true;
    /*! Where the wheel is, in degrees. */
    qreal angle = 0.0;
    QVariantAnimation turn;
};

CupertinoActivityIndicator::CupertinoActivityIndicator(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoActivityIndicatorPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // A waiting mark is not a thing to click and not a thing to tab to.
    setFocusPolicy(Qt::NoFocus);

    // The turn is a loop of the whole circle. It is the one movement in the
    // family that does not accelerate: something that keeps going round has no
    // reason to speed up and slow down eight times a turn, so the curve is
    // linear rather than one of the family's - and its length is the family's
    // step times the number of spokes, which is what makes the timing the
    // theme's to change and reduce motion's to silence.
    connect(&d_ptr->turn, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->angle = value.toReal();
        update();
    });
}

CupertinoActivityIndicator::~CupertinoActivityIndicator()
{
    QX_FINI_PRIVATE()
}

bool CupertinoActivityIndicator::isAnimating() const
{
    return d_ptr->animating;
}

void CupertinoActivityIndicator::setAnimating(bool animating)
{
    if (d_ptr->animating == animating) {
        return;
    }
    d_ptr->animating = animating;
    restart();
    Q_EMIT animatingChanged(animating);
}

QSize CupertinoActivityIndicator::sizeHint() const
{
    return QSize(int(kSide), int(kSide));
}

QSize CupertinoActivityIndicator::minimumSizeHint() const
{
    return sizeHint();
}

bool CupertinoActivityIndicator::hitTest(const QPointF &pos) const
{
    // Nothing here is clickable: the mark says the page is busy, and a click on
    // it would have nowhere to go.
    Q_UNUSED(pos)
    return false;
}

void CupertinoActivityIndicator::paintContent(QPainter *painter)
{
    const qreal side = qMin(width(), height());
    const qreal outer = side / 2.0;
    const qreal thickness = qMax(1.5, side * kSpokeWidthRatio);
    const qreal inner = outer * kSpokeInnerRatio;
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->translate(width() / 2.0, height() / 2.0);

    // The whole wheel turns; the fade belongs to the spokes and turns with it,
    // which is what makes the eye read a direction.
    painter->rotate(d_ptr->angle);
    for (int i = 0; i < kSpokes; ++i) {
        painter->save();
        painter->rotate(i * (360.0 / kSpokes));

        QColor ink = CupertinoColor::color(CupertinoColor::Role::Gray, dark);
        // The first spoke is the brightest and every one after it fades a
        // little more, so the wheel has a head and a tail.
        ink.setAlphaF(kFaintest + (1.0 - kFaintest) * (1.0 - qreal(i) / (kSpokes - 1)));
        painter->setPen(QPen(ink, thickness, Qt::SolidLine, Qt::RoundCap));
        // Spoke i points up before the rotation, so the wheel's head sits at
        // twelve o'clock when the angle is zero.
        painter->drawLine(QPointF(0, -outer), QPointF(0, -inner));

        painter->restore();
    }

    painter->restore();
}

void CupertinoActivityIndicator::showEvent(QShowEvent *event)
{
    QxControls::QxControl::showEvent(event);
    // A turn that was running while the widget was hidden has been stopped;
    // showing it again is what starts one.
    restart();
}

void CupertinoActivityIndicator::hideEvent(QHideEvent *event)
{
    QxControls::QxControl::hideEvent(event);
    // Nothing is waiting for a caller that cannot see the mark.
    d_ptr->turn.stop();
}

QAccessible::Role CupertinoActivityIndicator::accessibleRole() const
{
    // The nearest true answer: this says a wait is in progress and carries no
    // number, which is what a progress bar with no value is.
    return QAccessible::ProgressBar;
}

void CupertinoActivityIndicator::restart()
{
    const QxTheme::QxMotion motion =
        CupertinoMotion::forDuration(CupertinoMotion::Duration::Quick, CupertinoMotion::Easing::Standard);

    d_ptr->turn.stop();
    if (!d_ptr->animating) {
        // Stopped is a wheel at rest, not an empty widget: the spokes are still
        // there, saying the same thing without moving.
        d_ptr->angle = 0.0;
        update();
        return;
    }
    if (!isVisible()) {
        return;
    }
    if (motion.duration <= 0) {
        // Reduce motion answers no duration, and a wheel that does not turn
        // still has to be drawn - so this is a still frame, not a timer.
        d_ptr->angle = 0.0;
        update();
        return;
    }

    d_ptr->turn.setDuration(motion.duration * kSpokes);
    d_ptr->turn.setEasingCurve(QEasingCurve::Linear);
    d_ptr->turn.setStartValue(0.0);
    d_ptr->turn.setEndValue(360.0);
    d_ptr->turn.setLoopCount(-1);
    d_ptr->turn.start();
}

QX_CUPERTINO_END_NAMESPACE
