/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialrating.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrolaccessible.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtCore/QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// MUI's rating star is twenty-four across and the stars stand side by side
// with nothing between them: the star's own points are what separate it from
// its neighbour, so no gap is needed and none is drawn.
constexpr qreal kStarSize = 24.0;

// A star is five points on the outer circle and five on the inner one, and the
// inner radius is what decides whether it reads as a star or as a flower.
constexpr qreal kInnerRatio = 0.45;

// An empty star is a line in the ink Material keeps for what is not filled in;
// the stars a click would fill are the accent thinned, which is what makes the
// two readings of the row different without either one shouting.
constexpr qreal kEmptyOutlineOpacity = 0.26;
constexpr qreal kPendingOpacity = 0.35;
constexpr qreal kDisabledOpacity = 0.38;

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
class MaterialRatingAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit MaterialRatingAccessible(MaterialRating *rating)
        : QxControls::QxControlAccessible(rating, QAccessible::Slider)
    {
    }

    QVariant currentValue() const override
    {
        return rating()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        // A score that is only to be read is not set from the other side
        // either; the rating says so itself and this is what says it.
        if (!rating()->isReadOnly()) {
            rating()->setValue(value.toInt());
        }
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
    MaterialRating *rating() const
    {
        return static_cast<MaterialRating *>(widget());
    }
};
}   // namespace

class MaterialRatingPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialRating)

    int value = 0;
    int count = 5;
    bool readOnly = false;
    /*! What a click here would do: the star under the pointer, or -1 for none. */
    int pending = -1;

    QRectF starBox(int index) const
    {
        return QRectF(index * kStarSize, 0.0, kStarSize, kStarSize);
    }

    int starAt(const QPointF &pos) const
    {
        const int index = int(pos.x() / kStarSize);
        return (index >= 0 && index < count) ? index + 1 : -1;
    }
};

MaterialRating::MaterialRating(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialRatingPrivate)
{
    QX_SET_PRIVATE(d_ptr);
    // The pointer's position is part of what a rating says, so moves are wanted
    // whether or not a button is down.
    setMouseTracking(true);
}

MaterialRating::~MaterialRating()
{
    QX_FINI_PRIVATE();
}

int MaterialRating::value() const
{
    return d_ptr->value;
}

int MaterialRating::count() const
{
    return d_ptr->count;
}

bool MaterialRating::isReadOnly() const
{
    return d_ptr->readOnly;
}

void MaterialRating::setValue(int value)
{
    const int bounded = qBound(0, value, d_ptr->count);
    if (d_ptr->value == bounded) {
        return;
    }
    d_ptr->value = bounded;
    update();
    Q_EMIT valueChanged(bounded);
}

void MaterialRating::setCount(int count)
{
    const int bounded = qMax(1, count);
    if (d_ptr->count == bounded) {
        return;
    }
    d_ptr->count = bounded;
    // A score cannot be more stars than there are.
    const int previous = d_ptr->value;
    d_ptr->value = qMin(d_ptr->value, bounded);
    d_ptr->pending = -1;
    updateGeometry();
    update();
    if (d_ptr->value != previous) {
        Q_EMIT valueChanged(d_ptr->value);
    }
}

void MaterialRating::setReadOnly(bool readOnly)
{
    if (d_ptr->readOnly == readOnly) {
        return;
    }
    d_ptr->readOnly = readOnly;
    // A score to read is not a place in the way of the keyboard, and it does
    // not answer the pointer with what a click would do - there is no click.
    setFocusPolicy(readOnly ? Qt::NoFocus : Qt::StrongFocus);
    d_ptr->pending = -1;
    update();
}

QSize MaterialRating::sizeHint() const
{
    return QSize(int(d_ptr->count * kStarSize), int(kStarSize));
}

QSize MaterialRating::minimumSizeHint() const
{
    // Every star is needed to say how many there are, so there is no narrower
    // row.
    return sizeHint();
}

void MaterialRating::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    QColor filled = accentColor();
    QColor wouldFill = accentColor();
    wouldFill.setAlphaF(kPendingOpacity);
    QColor empty = textColor();
    empty.setAlphaF(kEmptyOutlineOpacity);

    const bool answersThePointer = d_ptr->pending >= 0;
    for (int i = 0; i < d_ptr->count; ++i) {
        const QPainterPath star = starPath(d_ptr->starBox(i));
        // What the pointer says replaces what is rated rather than adding to
        // it: a row that kept the four stars while the pointer was on the
        // second would be telling the reader that a click here would give four.
        const bool isOn = answersThePointer ? (i < d_ptr->pending) : (i < d_ptr->value);

        // A star that is on is a solid: the accent is its edge as well as its
        // inside, because there is nothing left for an outline to say.
        if (isOn) {
            const QColor &wash = answersThePointer ? wouldFill : filled;
            painter->setPen(QPen(wash, 1.0));
            painter->setBrush(wash);
        } else {
            painter->setPen(QPen(empty, 1.0));
            painter->setBrush(Qt::NoBrush);
        }
        painter->drawPath(star);
    }
    painter->restore();
}

void MaterialRating::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || d_ptr->readOnly || event->button() != Qt::LeftButton) {
        return;
    }
    const int star = d_ptr->starAt(eventPos(event));
    if (star > 0) {
        setValue(star);
    }
}

void MaterialRating::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const bool answers = isEnabled() && !d_ptr->readOnly;
    const int star = answers ? d_ptr->starAt(eventPos(event)) : -1;
    if (star == d_ptr->pending) {
        return;
    }
    d_ptr->pending = star;
    update();
}

void MaterialRating::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->pending == -1) {
        return;
    }
    d_ptr->pending = -1;
    update();
}

void MaterialRating::keyPressEvent(QKeyEvent *event)
{
    if (d_ptr->readOnly) {
        QxControls::QxControl::keyPressEvent(event);
        return;
    }
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Down:
        setValue(d_ptr->value - 1);
        event->accept();
        return;
    case Qt::Key_Right:
    case Qt::Key_Up:
        setValue(d_ptr->value + 1);
        event->accept();
        return;
    case Qt::Key_Home:
        setValue(0);
        event->accept();
        return;
    case Qt::Key_End:
        setValue(d_ptr->count);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role MaterialRating::accessibleRole() const
{
    return QAccessible::Slider;
}

QAccessibleInterface *MaterialRating::accessibleInterface()
{
    return new MaterialRatingAccessible(this);
}

QX_MATERIAL_END_NAMESPACE
