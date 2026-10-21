/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentslider.h"

#include "fluenttokens.h"

#include "qxcontrols/qxcontrolaccessible.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's slider measures: a rail four tall, a thumb sixteen across that
// rides it, and the marks underneath at every interval.
constexpr qreal kRailHeight = 4.0;
constexpr qreal kThumbSize = 16.0;
constexpr qreal kTickHeight = 4.0;
constexpr qreal kTickGap = 2.0;

// The rail the thumb has not reached, and the marks under it, are the ink of
// the page thinned - the same relationship a stroke has, because that is what
// they are drawn as.
constexpr qreal kMutedOpacity = 0.12;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

qreal railWidth(const QWidget *widget)
{
    return qMax(qreal(0.0), widget->width() - kThumbSize);
}

QRectF railRect(const QWidget *widget)
{
    const qreal y = (widget->height() - kRailHeight) / 2.0;
    return QRectF(kThumbSize / 2.0, y, railWidth(widget), kRailHeight);
}

QRectF thumbRect(const QWidget *widget, qreal fraction)
{
    const QRectF rail = railRect(widget);
    const qreal center = rail.left() + fraction * rail.width();
    return QRectF(center - kThumbSize / 2.0, rail.center().y() - kThumbSize / 2.0, kThumbSize, kThumbSize);
}
}   // namespace

/*!
 * The accessible face of a slider: a role and a state are not enough for a
 * control whose answer is a number, because an assistive tool wants to set
 * that number back. This is the face the base asks for and hands out.
 */
class FluentSliderAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit FluentSliderAccessible(FluentSlider *slider)
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
    FluentSlider *slider() const
    {
        return static_cast<FluentSlider *>(widget());
    }
};

class FluentSliderPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentSlider)

    int value = 0;
    int minimum = 0;
    int maximum = 100;
    int singleStep = 1;
    int pageStep = 10;
    int tickInterval = 0;
    bool dragging = false;
    /*! Where the thumb is drawn, as a fraction of the rail. */
    qreal position = 0.0;
    QVariantAnimation animation;

    qreal fractionOf(int value) const
    {
        const int span = maximum - minimum;
        return span > 0 ? qreal(value - minimum) / qreal(span) : 0.0;
    }
};

FluentSlider::FluentSlider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentSliderPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });
}

FluentSlider::~FluentSlider()
{
    QX_FINI_PRIVATE()
}

int FluentSlider::value() const
{
    return d_ptr->value;
}

int FluentSlider::minimum() const
{
    return d_ptr->minimum;
}

int FluentSlider::maximum() const
{
    return d_ptr->maximum;
}

int FluentSlider::singleStep() const
{
    return d_ptr->singleStep;
}

int FluentSlider::pageStep() const
{
    return d_ptr->pageStep;
}

int FluentSlider::tickInterval() const
{
    return d_ptr->tickInterval;
}

void FluentSlider::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;

    // A drag answers at once - a thumb that lags the pointer is a broken
    // slider - but a step, from the keyboard or from the application,
    // travels: there the movement is what tells the user it moved.
    const QxTheme::QxMotion motion = FluentMotion::forDuration(FluentMotion::Duration::Fast);
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

void FluentSlider::setMinimum(int minimum)
{
    setRange(minimum, qMax(minimum, d_ptr->maximum));
}

void FluentSlider::setMaximum(int maximum)
{
    setRange(qMin(d_ptr->minimum, maximum), maximum);
}

void FluentSlider::setRange(int minimum, int maximum)
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

void FluentSlider::setSingleStep(int step)
{
    d_ptr->singleStep = qMax(1, step);
}

void FluentSlider::setPageStep(int step)
{
    d_ptr->pageStep = qMax(1, step);
}

void FluentSlider::setTickInterval(int interval)
{
    if (d_ptr->tickInterval == interval) {
        return;
    }
    d_ptr->tickInterval = interval;
    update();
}

QSize FluentSlider::sizeHint() const
{
    // Wide enough to be a slider, tall enough for the rail, the thumb and the
    // marks under it.
    return QSize(120, int(kThumbSize + kTickGap + kTickHeight + 4));
}

QSize FluentSlider::minimumSizeHint() const
{
    return QSize(int(kThumbSize * 2), int(kThumbSize));
}

qreal FluentSlider::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Circular);
}

QRectF FluentSlider::chromeRect() const
{
    // The wash and the ring belong to the thumb: that is the part a pointer
    // takes hold of.
    return thumbRect(this, d_ptr->position);
}

void FluentSlider::paintSurface(QPainter *painter)
{
    const QRectF rail = railRect(this);
    const qreal radius = kRailHeight / 2.0;
    const QRectF thumb = thumbRect(this, d_ptr->position);

    painter->save();
    painter->setPen(Qt::NoPen);

    // The rail: the part the thumb has reached is the accent, the part it has
    // not is the page thinned.
    QColor muted = textColor();
    muted.setAlphaF(kMutedOpacity);
    painter->setBrush(muted);
    painter->drawRoundedRect(rail, radius, radius);

    const qreal reached = thumb.center().x() - rail.left();
    if (reached > 0.0) {
        painter->setBrush(accentColor());
        painter->drawRoundedRect(QRectF(rail.left(), rail.top(), reached, rail.height()), radius, radius);
    }

    // The marks: where the rail is divided, which is the one thing Fluent's
    // slider says that Material's does not.
    if (d_ptr->tickInterval > 0) {
        const int span = d_ptr->maximum - d_ptr->minimum;
        painter->setPen(QPen(muted, 1.0));
        for (int tick = d_ptr->minimum; tick <= d_ptr->maximum && span > 0; tick += d_ptr->tickInterval) {
            const qreal x = rail.left() + d_ptr->fractionOf(tick) * rail.width();
            painter->drawLine(QPointF(x, rail.bottom() + kTickGap), QPointF(x, rail.bottom() + kTickGap + kTickHeight));
        }
    }

    // The thumb: the accent, ringed in the page so it reads on the rail it
    // covers.
    painter->setPen(QPen(palette().color(QPalette::Window), 1.0));
    painter->setBrush(accentColor());
    painter->drawEllipse(thumb);
    painter->restore();
}

void FluentSlider::paintContent(QPainter *painter)
{
    Q_UNUSED(painter)
}

void FluentSlider::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    // A press anywhere on the rail moves the thumb there: that is what a
    // slider does, and waiting for a drag would be a slider that lies about
    // where the pointer is.
    d_ptr->dragging = true;
    const qreal rail = railWidth(this);
    const qreal fraction = rail > 0.0 ? (eventPos(event).x() - kThumbSize / 2.0) / rail : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
}

void FluentSlider::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    if (!d_ptr->dragging) {
        return;
    }
    const qreal rail = railWidth(this);
    const qreal fraction = rail > 0.0 ? (eventPos(event).x() - kThumbSize / 2.0) / rail : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
}

void FluentSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    d_ptr->dragging = false;
}

void FluentSlider::keyPressEvent(QKeyEvent *event)
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

void FluentSlider::wheelEvent(QWheelEvent *event)
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

QAccessible::Role FluentSlider::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *FluentSlider::accessibleInterface()
{
    return new FluentSliderAccessible(this);
}

QX_FLUENT_END_NAMESPACE
