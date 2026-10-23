/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinoslider.h"

#include "cupertinotokens.h"

#include "qxcontrols/qxcontrolaccessible.h"
#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
// The family's slider measures: a rail four tall - a line rather than a groove -
// and a handle of twenty-eight that rides over it, so what is taken hold of is
// seven times what it runs on. Dragged, the handle grows by four, which is the
// only thing about this control that moves without a value changing.
constexpr qreal kTrackHeight = 4.0;
constexpr qreal kHandleSize = 28.0;
constexpr qreal kHandleGrown = 32.0;

// The part of the rail the handle has not reached is the family's grey thinned
// over the page, the same one the switch draws its off track with.
constexpr qreal kMutedOpacity = 0.32;

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
    return QRectF(kHandleSize / 2.0, (widget->height() - kTrackHeight) / 2.0, trackWidth(widget), kTrackHeight);
}

QRectF handleRect(const QWidget *widget, qreal fraction, qreal size)
{
    const QRectF track = trackRect(widget);
    const qreal center = track.left() + fraction * track.width();
    return QRectF(center - size / 2.0, track.center().y() - size / 2.0, size, size);
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
class CupertinoSliderAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit CupertinoSliderAccessible(CupertinoSlider *slider)
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
    CupertinoSlider *slider() const
    {
        return static_cast<CupertinoSlider *>(widget());
    }
};

class CupertinoSliderPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoSlider)

    int value = 0;
    int minimum = 0;
    int maximum = 100;
    int singleStep = 1;
    int pageStep = 10;
    bool dragging = false;
    /*! Where the handle is drawn, as a fraction of the rail. */
    qreal position = 0.0;
    QVariantAnimation animation;

    qreal fractionOf(int value) const
    {
        const int span = maximum - minimum;
        return span > 0 ? qreal(value - minimum) / qreal(span) : 0.0;
    }
};

CupertinoSlider::CupertinoSlider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoSliderPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });
}

CupertinoSlider::~CupertinoSlider()
{
    QX_FINI_PRIVATE()
}

int CupertinoSlider::value() const
{
    return d_ptr->value;
}

int CupertinoSlider::minimum() const
{
    return d_ptr->minimum;
}

int CupertinoSlider::maximum() const
{
    return d_ptr->maximum;
}

int CupertinoSlider::singleStep() const
{
    return d_ptr->singleStep;
}

int CupertinoSlider::pageStep() const
{
    return d_ptr->pageStep;
}

void CupertinoSlider::setValue(int value)
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
        CupertinoMotion::forDuration(CupertinoMotion::Duration::Quick, CupertinoMotion::Easing::Standard);
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

void CupertinoSlider::setMinimum(int minimum)
{
    setRange(minimum, qMax(minimum, d_ptr->maximum));
}

void CupertinoSlider::setMaximum(int maximum)
{
    setRange(qMin(d_ptr->minimum, maximum), maximum);
}

void CupertinoSlider::setRange(int minimum, int maximum)
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

void CupertinoSlider::setSingleStep(int step)
{
    d_ptr->singleStep = qMax(1, step);
}

void CupertinoSlider::setPageStep(int step)
{
    d_ptr->pageStep = qMax(1, step);
}

QSize CupertinoSlider::sizeHint() const
{
    return QSize(160, int(kHandleSize + 8));
}

QSize CupertinoSlider::minimumSizeHint() const
{
    return QSize(int(kHandleSize * 2), int(kHandleSize));
}

qreal CupertinoSlider::cornerRadius() const
{
    return CupertinoShape::radius(CupertinoShape::Radius::Circular);
}

QRectF CupertinoSlider::chromeRect() const
{
    // The wash and the ring belong to the handle: that is the part a pointer
    // takes hold of, and the rail is not something to press.
    return handleRect(this, d_ptr->position, d_ptr->dragging ? kHandleGrown : kHandleSize);
}

void CupertinoSlider::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal railRadius = kTrackHeight / 2.0;
    const bool dark = onDarkFace(this);
    const qreal handleSize = d_ptr->dragging ? kHandleGrown : kHandleSize;
    const QRectF handle = handleRect(this, d_ptr->position, handleSize);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // The rail in two parts: the family's grey where the handle has not been,
    // and its blue behind the handle, so the value is readable from the fill
    // alone.
    QColor muted = CupertinoColor::color(CupertinoColor::Role::Gray, dark);
    muted.setAlphaF(kMutedOpacity);
    painter->setBrush(muted);
    painter->drawRoundedRect(track, railRadius, railRadius);

    const qreal reached = handle.center().x() - track.left();
    if (reached > 0.0) {
        painter->setBrush(CupertinoColor::color(CupertinoColor::Role::Accent, dark));
        painter->drawRoundedRect(QRectF(track.left(), track.top(), reached, track.height()), railRadius, railRadius);
    }

    // The handle is the one lift here, and the same raised shadow the switch
    // gives its knob: the family draws a lift as light, so the handle reads as
    // lying on the rail rather than being cut out of it.
    QxControls::QxShadowPainter::paint(painter, handle, handleSize / 2.0,
                                       CupertinoColor::color(CupertinoColor::Role::Gray, dark),
                                       CupertinoElevation::shadow(CupertinoElevation::Level::Raised));

    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::white);
    painter->drawEllipse(handle);

    painter->restore();
}

void CupertinoSlider::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    // A press anywhere on the rail moves the handle there: waiting for a drag
    // would be a slider that lies about where the pointer is.
    d_ptr->dragging = true;
    const qreal width = trackWidth(this);
    const qreal fraction = width > 0.0 ? (eventPos(event).x() - kHandleSize / 2.0) / width : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
}

void CupertinoSlider::mouseMoveEvent(QMouseEvent *event)
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

void CupertinoSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    if (d_ptr->dragging) {
        // The handle shrinks back to the size it rests at, which is the only
        // thing the release changes.
        d_ptr->dragging = false;
        update();
    }
}

void CupertinoSlider::keyPressEvent(QKeyEvent *event)
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

void CupertinoSlider::wheelEvent(QWheelEvent *event)
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

QAccessible::Role CupertinoSlider::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *CupertinoSlider::accessibleInterface()
{
    return new CupertinoSliderAccessible(this);
}

QX_CUPERTINO_END_NAMESPACE
