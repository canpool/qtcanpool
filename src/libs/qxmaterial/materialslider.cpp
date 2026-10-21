/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialslider.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrolaccessible.h"
#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's slider measures: a track twelve tall and a handle of twenty that
// rides over it - the handle is bigger than what it rides, which is what makes
// it the thing the user takes hold of.
constexpr qreal kTrackHeight = 12.0;
constexpr qreal kHandleSize = 20.0;

// The track the handle has not reached is the surface the page is made of,
// one step away from it.
constexpr qreal kMutedOpacity = 0.24;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

qreal trackWidth(const QWidget *widget)
{
    return qMax(qreal(0.0), widget->width() - kHandleSize);
}

QRectF trackRect(const QWidget *widget)
{
    return QRectF(kHandleSize / 2.0, (widget->height() - kTrackHeight) / 2.0, trackWidth(widget), kTrackHeight);
}

QRectF handleRect(const QWidget *widget, qreal fraction)
{
    const QRectF track = trackRect(widget);
    const qreal center = track.left() + fraction * track.width();
    return QRectF(center - kHandleSize / 2.0, track.center().y() - kHandleSize / 2.0, kHandleSize, kHandleSize);
}
}   // namespace

/*!
 * The accessible face of a slider: a role and a state are not enough for a
 * control whose answer is a number, because an assistive tool wants to set
 * that number back. This is the face the base asks for and hands out.
 */
class MaterialSliderAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit MaterialSliderAccessible(MaterialSlider *slider)
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
    MaterialSlider *slider() const
    {
        return static_cast<MaterialSlider *>(widget());
    }
};

class MaterialSliderPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSlider)

    int value = 0;
    int minimum = 0;
    int maximum = 100;
    int singleStep = 1;
    int pageStep = 10;
    bool dragging = false;
    /*! Where the handle is drawn, as a fraction of the track. */
    qreal position = 0.0;
    QVariantAnimation animation;

    qreal fractionOf(int value) const
    {
        const int span = maximum - minimum;
        return span > 0 ? qreal(value - minimum) / qreal(span) : 0.0;
    }
};

MaterialSlider::MaterialSlider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialSliderPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });
}

MaterialSlider::~MaterialSlider()
{
    QX_FINI_PRIVATE()
}

int MaterialSlider::value() const
{
    return d_ptr->value;
}

int MaterialSlider::minimum() const
{
    return d_ptr->minimum;
}

int MaterialSlider::maximum() const
{
    return d_ptr->maximum;
}

int MaterialSlider::singleStep() const
{
    return d_ptr->singleStep;
}

int MaterialSlider::pageStep() const
{
    return d_ptr->pageStep;
}

void MaterialSlider::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;

    // A drag answers at once - a handle that lags the pointer is a broken
    // slider - but a step travels, because there the movement is what tells
    // the user it moved.
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Short);
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

void MaterialSlider::setMinimum(int minimum)
{
    setRange(minimum, qMax(minimum, d_ptr->maximum));
}

void MaterialSlider::setMaximum(int maximum)
{
    setRange(qMin(d_ptr->minimum, maximum), maximum);
}

void MaterialSlider::setRange(int minimum, int maximum)
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

void MaterialSlider::setSingleStep(int step)
{
    d_ptr->singleStep = qMax(1, step);
}

void MaterialSlider::setPageStep(int step)
{
    d_ptr->pageStep = qMax(1, step);
}

QSize MaterialSlider::sizeHint() const
{
    return QSize(120, int(kHandleSize + 8));
}

QSize MaterialSlider::minimumSizeHint() const
{
    return QSize(int(kHandleSize * 2), int(kHandleSize));
}

qreal MaterialSlider::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Circular);
}

QRectF MaterialSlider::chromeRect() const
{
    // The wash and the ring belong to the handle: that is the part a pointer
    // takes hold of.
    return handleRect(this, d_ptr->position);
}

void MaterialSlider::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal radius = kTrackHeight / 2.0;
    const QRectF handle = handleRect(this, d_ptr->position);

    painter->save();
    painter->setPen(Qt::NoPen);

    // The track: the part the handle has reached is the primary colour, the
    // part it has not is a surface one step from the page.
    QColor muted = textColor();
    muted.setAlphaF(kMutedOpacity);
    painter->setBrush(muted);
    painter->drawRoundedRect(track, radius, radius);

    const qreal reached = handle.center().x() - track.left();
    if (reached > 0.0) {
        painter->setBrush(accentColor());
        painter->drawRoundedRect(QRectF(track.left(), track.top(), reached, track.height()), radius, radius);
    }

    // The handle lifts off the track it covers, which is the one lift a slider
    // in Material has.
    QxControls::QxShadowPainter::paint(painter, handle, kHandleSize / 2.0, textColor(),
                                       MaterialElevation::shadow(MaterialElevation::Level::Level1));
    painter->setBrush(accentColor());
    painter->drawEllipse(handle);
    painter->restore();
}

void MaterialSlider::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    // A press anywhere on the track moves the handle there: waiting for a drag
    // would be a slider that lies about where the pointer is.
    d_ptr->dragging = true;
    const qreal width = trackWidth(this);
    const qreal fraction = width > 0.0 ? (eventPos(event).x() - kHandleSize / 2.0) / width : 0.0;
    const int span = d_ptr->maximum - d_ptr->minimum;
    setValue(d_ptr->minimum + qRound(qBound(qreal(0.0), fraction, qreal(1.0)) * span));
}

void MaterialSlider::mouseMoveEvent(QMouseEvent *event)
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

void MaterialSlider::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    d_ptr->dragging = false;
}

void MaterialSlider::keyPressEvent(QKeyEvent *event)
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

void MaterialSlider::wheelEvent(QWheelEvent *event)
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

QAccessible::Role MaterialSlider::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *MaterialSlider::accessibleInterface()
{
    return new MaterialSliderAccessible(this);
}

QX_MATERIAL_END_NAMESPACE
