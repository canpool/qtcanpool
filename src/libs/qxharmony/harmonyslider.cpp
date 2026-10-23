/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonyslider.h"

#include "harmonytokens.h"

#include "qxcontrols/qxcontrolaccessible.h"
#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>

QX_HARMONY_BEGIN_NAMESPACE

namespace
{
// The family's slider measures, as this library reads them: a rail six tall -
// a line with some body to it rather than a groove - and a handle of
// twenty-six that rides over it. The rail sits low in the widget because the
// room above it is where the bubble goes.
constexpr qreal kTrackHeight = 6.0;
constexpr qreal kHandleSize = 26.0;

// The bubble: how wide it is, how far above the handle it floats, and how much
// padding its number gets. The height is the caption's line plus that padding.
constexpr qreal kBubbleWidth = 40.0;
constexpr qreal kBubbleGap = 2.0;
constexpr qreal kBubblePadding = 3.0;

// The part of the rail the handle has not reached is the family's grey thinned
// over the page, the same one the switch draws its off track with.
constexpr qreal kMutedOpacity = 0.28;

// A disabled control keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

// The rail runs between the places the middle of the handle can reach, so the
// handle never hangs off either end of the control.
qreal trackWidth(const QWidget *widget)
{
    return qMax(qreal(0.0), widget->width() - kHandleSize);
}

QRectF trackRect(const QWidget *widget)
{
    return QRectF(kHandleSize / 2.0, widget->height() - 14.0 - kTrackHeight / 2.0, trackWidth(widget), kTrackHeight);
}

QRectF handleRect(const QWidget *widget, qreal fraction)
{
    const QRectF track = trackRect(widget);
    const qreal center = track.left() + fraction * track.width();
    return QRectF(center - kHandleSize / 2.0, track.center().y() - kHandleSize / 2.0, kHandleSize, kHandleSize);
}

/*! The bubble's place: centred over the handle, kept inside the widget at both
 *  ends - a bubble half off the control would be a bubble cut in half. */
QRectF bubbleRect(const QWidget *widget, const QRectF &handle)
{
    const qreal height = QFontMetricsF(HarmonyType::font(HarmonyType::Ramp::Caption)).height() + 2.0 * kBubblePadding;
    const qreal top = handle.top() - kBubbleGap - height;
    const qreal left = qBound(qreal(0.0), handle.center().x() - kBubbleWidth / 2.0, widget->width() - kBubbleWidth);
    return QRectF(left, top, kBubbleWidth, height);
}

// Which face of the semantic colours the control is drawn on.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

/*!
 * The accessible face of a slider: a role and a state are not enough for a
 * control whose answer is a number, because an assistive tool wants to set that
 * number back. This is the face the base asks for and hands out.
 */
class HarmonySliderAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit HarmonySliderAccessible(HarmonySlider *slider)
        : QxControls::QxControlAccessible(slider, QAccessible::Slider)
    {
    }

    QVariant currentValue() const override
    {
        return slider()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        slider()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return slider()->maximum();
    }
    QVariant minimumValue() const override
    {
        return slider()->minimum();
    }
    QVariant minimumStepSize() const override
    {
        return slider()->singleStep();
    }

    void *interface_cast(QAccessible::InterfaceType type) override
    {
        if (type == QAccessible::ValueInterface) {
            return static_cast<QAccessibleValueInterface *>(this);
        }
        return QxControls::QxControlAccessible::interface_cast(type);
    }
private:
    HarmonySlider *slider() const
    {
        return static_cast<HarmonySlider *>(widget());
    }
};

class HarmonySliderPrivate
{
public:
    QX_DECLARE_PUBLIC(HarmonySlider)

    int value = 0;
    int minimum = 0;
    int maximum = 100;
    int singleStep = 1;
    int pageStep = 10;
    bool dragging = false;
    /*! Where the handle is drawn, as a fraction of the rail. */
    qreal position = 0.0;
    /*! How solid the bubble is: one while the handle is held, falling to zero
     *  once it is let go. Zero is what keeps the room above the rail empty. */
    qreal bubble = 0.0;
    QVariantAnimation animation;
    QVariantAnimation bubbleFade;

    qreal fractionOf(int value) const
    {
        const int span = maximum - minimum;
        return span > 0 ? qreal(value - minimum) / qreal(span) : 0.0;
    }
};

HarmonySlider::HarmonySlider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new HarmonySliderPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });
    // Letting go of the handle fades the bubble rather than removing it: what
    // was being read a moment ago should not vanish from under the eye.
    connect(&d_ptr->bubbleFade, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->bubble = value.toReal();
        update();
    });
}

HarmonySlider::~HarmonySlider()
{
    QX_FINI_PRIVATE()
}

int HarmonySlider::value() const
{
    return d_ptr->value;
}

int HarmonySlider::minimum() const
{
    return d_ptr->minimum;
}

int HarmonySlider::maximum() const
{
    return d_ptr->maximum;
}

int HarmonySlider::singleStep() const
{
    return d_ptr->singleStep;
}

int HarmonySlider::pageStep() const
{
    return d_ptr->pageStep;
}

void HarmonySlider::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;

    // A drag answers at once - a handle that lags the pointer is a broken
    // slider - but a step travels, because there the movement is what tells the
    // user it moved.
    const QxTheme::QxMotion motion =
        HarmonyMotion::forDuration(HarmonyMotion::Duration::Quick, HarmonyMotion::Easing::Standard);
    const qreal target = d_ptr->fractionOf(bounded);
    d_ptr->animation.stop();
    if (d_ptr->dragging || motion.duration <= 0) {
        d_ptr->position = target;
        update();
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->position);
        d_ptr->animation.setEndValue(target);
        d_ptr->animation.start();
    }
    Q_EMIT valueChanged(bounded);
}

void HarmonySlider::setMinimum(int minimum)
{
    setRange(minimum, qMax(minimum, d_ptr->maximum));
}

void HarmonySlider::setMaximum(int maximum)
{
    setRange(qMin(d_ptr->minimum, maximum), maximum);
}

void HarmonySlider::setRange(int minimum, int maximum)
{
    const int low = qMin(minimum, maximum);
    const int high = qMax(minimum, maximum);
    if (d_ptr->minimum == low && d_ptr->maximum == high) {
        return;
    }
    d_ptr->minimum = low;
    d_ptr->maximum = high;
    setValue(qBound(low, d_ptr->value, high));
    update();
}

void HarmonySlider::setSingleStep(int step)
{
    d_ptr->singleStep = qMax(1, step);
}

void HarmonySlider::setPageStep(int step)
{
    d_ptr->pageStep = qMax(1, step);
}

QSize HarmonySlider::sizeHint() const
{
    return QSize(160, int(kHandleSize + 24));
}

QSize HarmonySlider::minimumSizeHint() const
{
    return QSize(int(kHandleSize * 2), int(kHandleSize + 8));
}

qreal HarmonySlider::cornerRadius() const
{
    return HarmonyShape::radius(HarmonyShape::Radius::Circular);
}

QRectF HarmonySlider::chromeRect() const
{
    // The wash and the ring belong to the handle: that is the part a pointer
    // takes hold of, and the rail is not something to press.
    return handleRect(this, d_ptr->position);
}

void HarmonySlider::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal railRadius = kTrackHeight / 2.0;
    const bool dark = onDarkFace(this);
    const QRectF handle = handleRect(this, d_ptr->position);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // The rail in two parts: the family's grey where the handle has not been,
    // and its blue behind the handle, so the value is readable from the fill
    // alone.
    QColor muted = HarmonyColor::color(HarmonyColor::Role::Gray, dark);
    muted.setAlphaF(kMutedOpacity);
    painter->setBrush(muted);
    painter->drawRoundedRect(track, railRadius, railRadius);

    const qreal reached = handle.center().x() - track.left();
    if (reached > 0.0) {
        painter->setBrush(HarmonyColor::color(HarmonyColor::Role::Accent, dark));
        painter->drawRoundedRect(QRectF(track.left(), track.top(), reached, track.height()), railRadius, railRadius);
    }

    // The handle is the one lift here: a tight, dark shadow close under it,
    // which is this family's raised level.
    QxControls::QxShadowPainter::paint(painter, handle, kHandleSize / 2.0,
                                       HarmonyColor::color(HarmonyColor::Role::Gray, dark),
                                       HarmonyElevation::shadow(HarmonyElevation::Level::Raised));

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawEllipse(handle);

    painter->restore();
}

void HarmonySlider::paintContent(QPainter *painter)
{
    if (d_ptr->bubble <= 0.0) {
        return;
    }
    const QRectF bubble = bubbleRect(this, handleRect(this, d_ptr->position));
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setOpacity(d_ptr->bubble);
    // The bubble is the family's blue with white in it, so it reads as the
    // filled rail stepping out of the control for a moment rather than as a
    // tooltip from somewhere else.
    painter->setPen(Qt::NoPen);
    painter->setBrush(HarmonyColor::color(HarmonyColor::Role::Accent, dark));
    const qreal corner = HarmonyShape::radius(HarmonyShape::Radius::Medium);
    painter->drawRoundedRect(bubble, corner, corner);

    painter->setFont(HarmonyType::font(HarmonyType::Ramp::Caption));
    painter->setPen(Qt::white);
    painter->drawText(bubble, Qt::AlignCenter, QString::number(d_ptr->value));
    painter->restore();
}

void HarmonySlider::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    // A press anywhere on the rail moves the handle there: waiting for a drag
    // would be a slider that lies about where the pointer is.
    d_ptr->dragging = true;
    d_ptr->bubbleFade.stop();
    d_ptr->bubble = 1.0;
    const qreal width = trackWidth(this);
    const qreal fraction = width > 0.0 ? (eventPos(event).x() - kHandleSize / 2.0) / width : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
    update();
}

void HarmonySlider::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    if (!d_ptr->dragging) {
        return;
    }
    const qreal width = trackWidth(this);
    const qreal fraction = width > 0.0 ? (eventPos(event).x() - kHandleSize / 2.0) / width : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
}

void HarmonySlider::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    if (!d_ptr->dragging) {
        return;
    }
    d_ptr->dragging = false;
    // The bubble goes the way it came, over the family's quick duration - and
    // at once when motion is reduced, which answers a zero duration here the
    // same way it does everywhere else.
    const QxTheme::QxMotion motion = HarmonyMotion::forDuration(HarmonyMotion::Duration::Quick);
    if (motion.duration <= 0) {
        d_ptr->bubble = 0.0;
        update();
        return;
    }
    d_ptr->bubbleFade.stop();
    d_ptr->bubbleFade.setDuration(motion.duration);
    d_ptr->bubbleFade.setEasingCurve(motion.curve);
    d_ptr->bubbleFade.setStartValue(d_ptr->bubble);
    d_ptr->bubbleFade.setEndValue(0.0);
    d_ptr->bubbleFade.start();
}

void HarmonySlider::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    int step = 0;
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Down:
        step = -d_ptr->singleStep;
        break;
    case Qt::Key_Right:
    case Qt::Key_Up:
        step = d_ptr->singleStep;
        break;
    case Qt::Key_PageUp:
        step = d_ptr->pageStep;
        break;
    case Qt::Key_PageDown:
        step = -d_ptr->pageStep;
        break;
    case Qt::Key_Home:
        setValue(d_ptr->minimum);
        event->accept();
        return;
    case Qt::Key_End:
        setValue(d_ptr->maximum);
        event->accept();
        return;
    default:
        break;
    }
    if (step != 0) {
        setValue(d_ptr->value + step);
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void HarmonySlider::wheelEvent(QWheelEvent *event)
{
    // One notch is one step, the way every other slider reads the wheel.
    const int delta = event->angleDelta().y();
    if (delta != 0) {
        setValue(d_ptr->value + (delta > 0 ? d_ptr->singleStep : -d_ptr->singleStep));
        event->accept();
        return;
    }
    QxControls::QxControl::wheelEvent(event);
}

QAccessible::Role HarmonySlider::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *HarmonySlider::accessibleInterface()
{
    return new HarmonySliderAccessible(this);
}

QX_HARMONY_END_NAMESPACE
