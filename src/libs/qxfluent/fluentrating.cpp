/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentrating.h"

#include "qxcontrols/qxcontrolaccessible.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's rating star is twenty across with four between the stars.
constexpr qreal kStarSize = 20.0;
constexpr qreal kStarGap = 4.0;

// A star is five points on the outer circle and five on the inner one, and
// the inner radius is what decides whether it reads as a star or as a flower.
constexpr qreal kInnerRatio = 0.45;

// A rating nobody can use is drawn faded, the way every other control is. The
// stars were the one part of a disabled row that still looked ready to answer.
constexpr qreal kDisabledOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! One star, drawn in the square \a box: the outline of a five-pointed star,
 *  which is ten vertices alternating between two circles. */
QPainterPath starPath(const QRectF &box)
{
    const QPointF center = box.center();
    const qreal outer = qMin(box.width(), box.height()) / 2.0;
    const qreal inner = outer * kInnerRatio;

    QPainterPath path;
    for (int i = 0; i < 10; ++i) {
        // Twelve o'clock first, then clockwise: the star stands on a point.
        const qreal angle = -M_PI / 2.0 + i * M_PI / 5.0;
        const qreal radius = (i % 2 == 0) ? outer : inner;
        const QPointF point(center.x() + radius * qCos(angle), center.y() + radius * qSin(angle));
        if (i == 0) {
            path.moveTo(point);
        } else {
            path.lineTo(point);
        }
    }
    path.closeSubpath();
    return path;
}

/*! The accessible face of a rating: it is a value in a range, and the range is
 *  the number of stars there are. */
class FluentRatingAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit FluentRatingAccessible(FluentRating *rating)
        : QxControls::QxControlAccessible(rating, QAccessible::Slider)
    {
    }

    QVariant currentValue() const override
    {
        return rating()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        rating()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return rating()->count();
    }
    QVariant minimumValue() const override
    {
        return 0;
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
    FluentRating *rating() const
    {
        return static_cast<FluentRating *>(widget());
    }
};
}   // namespace

class FluentRatingPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentRating)

    int value = 0;
    int count = 5;
    /*! What a click here would do: the star under the pointer, or -1 for none. */
    int hovered = -1;

    QRectF starBox(int index) const
    {
        const qreal x = index * (kStarSize + kStarGap);
        return QRectF(x, 0.0, kStarSize, kStarSize);
    }

    int starAt(const QPointF &pos) const
    {
        const qreal step = kStarSize + kStarGap;
        const int index = int(pos.x() / step);
        return (index >= 0 && index < count) ? index + 1 : -1;
    }
};

FluentRating::FluentRating(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentRatingPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setAttribute(Qt::WA_Hover, true);
    // The pointer's position is part of what a rating says, so moves are
    // wanted whether or not a button is down.
    setMouseTracking(true);
}

FluentRating::~FluentRating()
{
    QX_FINI_PRIVATE()
}

int FluentRating::value() const
{
    return d_ptr->value;
}

int FluentRating::count() const
{
    return d_ptr->count;
}

void FluentRating::setValue(int value)
{
    const int bounded = qBound(0, value, d_ptr->count);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
    Q_EMIT valueChanged(bounded);
}

void FluentRating::setCount(int count)
{
    const int bounded = qMax(1, count);
    if (d_ptr->count == bounded) {
        return;
    }
    d_ptr->count = bounded;
    setValue(qMin(d_ptr->value, bounded));
    updateGeometry();
    update();
}

QSize FluentRating::sizeHint() const
{
    const qreal width = d_ptr->count * kStarSize + (d_ptr->count - 1) * kStarGap;
    return QSize(int(width), int(kStarSize));
}

QSize FluentRating::minimumSizeHint() const
{
    return sizeHint();
}

void FluentRating::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

    // The row reads as two rows at once: what is rated, and - while the
    // pointer is over it - what a click here would rate it. The second is
    // drawn in the ink of the page thinned, so it cannot be mistaken for the
    // first.
    QColor pending = textColor();
    pending.setAlphaF(0.35);

    for (int i = 0; i < d_ptr->count; ++i) {
        const QRectF box = d_ptr->starBox(i);
        const QPainterPath star = starPath(box);
        const bool filled = i < d_ptr->value;
        const bool would = d_ptr->hovered > 0 && i < d_ptr->hovered;

        if (filled || would) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(filled ? accentColor() : pending);
            painter->drawPath(star);
        }
        painter->setPen(QPen(textColor(), 1.0));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(star);
    }
    painter->restore();
}

void FluentRating::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    const int star = d_ptr->starAt(eventPos(event));
    if (star > 0) {
        setValue(star);
    }
}

void FluentRating::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const int star = isEnabled() ? d_ptr->starAt(eventPos(event)) : -1;
    if (star == d_ptr->hovered) {
        return;
    }
    d_ptr->hovered = star;
    update();
}

void FluentRating::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->hovered == -1) {
        return;
    }
    d_ptr->hovered = -1;
    update();
}

void FluentRating::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_Down) {
        setValue(d_ptr->value - 1);
        event->accept();
        return;
    }
    if (key == Qt::Key_Right || key == Qt::Key_Up) {
        setValue(d_ptr->value + 1);
        event->accept();
        return;
    }
    if (key == Qt::Key_Home) {
        setValue(0);
        event->accept();
        return;
    }
    if (key == Qt::Key_End) {
        setValue(d_ptr->count);
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentRating::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *FluentRating::accessibleInterface()
{
    return new FluentRatingAccessible(this);
}

QX_FLUENT_END_NAMESPACE
