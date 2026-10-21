/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentteachingtip.h"

#include "fluenttokens.h"

#include <QtGui/QCloseEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QWidget>
#include <QPointer>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A tip is a surface of three-twenty with twelve inside it, a tail of twelve
// that reaches from its edge to the target, and eight between the two.
constexpr qreal kPadding = 12.0;
constexpr qreal kTailSize = 12.0;
constexpr qreal kTailHalf = 10.0;
constexpr qreal kGap = 8.0;
constexpr qreal kWidth = 320.0;
constexpr qreal kCloseSize = 24.0;
constexpr qreal kIconGap = 12.0;
constexpr qreal kLineGap = 4.0;
constexpr qreal kStrokeWidth = 1.0;
constexpr qreal kStrokeOpacity = 0.10;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! The surface: the widget's rect less the strip the tail stands on, which is
 *  whichever edge faces the target. */
QRectF bodyRect(const QWidget *widget, FluentTeachingTip::Side side)
{
    const qreal width = widget->width();
    const qreal height = widget->height();
    switch (side) {
    case FluentTeachingTip::Side::Bottom:
        return QRectF(0.0, kTailSize, width, height - kTailSize);
    case FluentTeachingTip::Side::Top:
        return QRectF(0.0, 0.0, width, height - kTailSize);
    case FluentTeachingTip::Side::Right:
        return QRectF(kTailSize, 0.0, width - kTailSize, height);
    default:
        return QRectF(0.0, 0.0, width - kTailSize, height);
    }
}

/*! The tail, as a triangle: its apex reaches towards the target and its base
 *  sits on the surface's edge. */
QPainterPath tailPath(FluentTeachingTip::Side side, const QPointF &base, const QSizeF &widget)
{
    QPainterPath path;
    switch (side) {
    case FluentTeachingTip::Side::Bottom:
        path.moveTo(base.x(), 0.0);
        path.lineTo(base.x() - kTailHalf, kTailSize);
        path.lineTo(base.x() + kTailHalf, kTailSize);
        break;
    case FluentTeachingTip::Side::Top:
        path.moveTo(base.x(), widget.height());
        path.lineTo(base.x() - kTailHalf, widget.height() - kTailSize);
        path.lineTo(base.x() + kTailHalf, widget.height() - kTailSize);
        break;
    case FluentTeachingTip::Side::Right:
        path.moveTo(0.0, base.y());
        path.lineTo(kTailSize, base.y() - kTailHalf);
        path.lineTo(kTailSize, base.y() + kTailHalf);
        break;
    default:
        path.moveTo(widget.width(), base.y());
        path.lineTo(widget.width() - kTailSize, base.y() - kTailHalf);
        path.lineTo(widget.width() - kTailSize, base.y() + kTailHalf);
        break;
    }
    path.closeSubpath();
    return path;
}
}   // namespace

class FluentTeachingTipPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTeachingTip)

    QString title;
    QString message;
    QPointer<QWidget> target;
    FluentTeachingTip::Side preferred = FluentTeachingTip::Side::Auto;
    FluentTeachingTip::Side side = FluentTeachingTip::Side::Bottom;
    bool open = false;
    /*! Where the tail's base is, in the widget's own coordinates: the target's
     *  middle, pulled in far enough that the tail does not climb over the
     *  surface's corner. */
    QPointF tailBase;

    QSize bodySize() const;
    void place();
};

QSize FluentTeachingTipPrivate::bodySize() const
{
    const QFontMetrics title(FluentType::font(FluentType::Ramp::Body, true));
    const QFontMetrics message(FluentType::font(FluentType::Ramp::Caption));
    const int textWidth = int(kWidth - 2 * kPadding - kCloseSize - kIconGap);
    int height = int(2 * kPadding) + title.height();
    if (!this->message.isEmpty()) {
        const QRect box = message.boundingRect(QRect(0, 0, textWidth, 10000), Qt::TextWordWrap, this->message);
        height += int(kLineGap) + box.height();
    }
    return QSize(int(kWidth), height);
}

void FluentTeachingTipPrivate::place()
{
    Q_Q(FluentTeachingTip);
    if (!target) {
        return;
    }
    const QRect spot(target->mapToGlobal(QPoint(0, 0)), target->size());
    const QRect area = target->topLevelWidget()->frameGeometry();
    const QSize body = bodySize();

    // Auto is a promise, not a side: below if the room is there, above if it
    // is not, and what it decided is what side() answers afterwards.
    FluentTeachingTip::Side wanted = preferred;
    if (wanted == FluentTeachingTip::Side::Auto) {
        const int below = spot.bottom() + int(kGap + kTailSize) + body.height();
        const int above = spot.top() - int(kGap + kTailSize) - body.height();
        wanted = below <= area.bottom()
                     ? FluentTeachingTip::Side::Bottom
                     : (above >= area.top() ? FluentTeachingTip::Side::Top : FluentTeachingTip::Side::Bottom);
    }
    side = wanted;

    QSize size = body;
    QPoint pos;
    switch (side) {
    case FluentTeachingTip::Side::Bottom:
        size += QSize(0, int(kTailSize));
        pos = QPoint(spot.center().x() - size.width() / 2, spot.bottom() + int(kGap));
        break;
    case FluentTeachingTip::Side::Top:
        size += QSize(0, int(kTailSize));
        pos = QPoint(spot.center().x() - size.width() / 2, spot.top() - int(kGap) - size.height());
        break;
    case FluentTeachingTip::Side::Right:
        size += QSize(int(kTailSize), 0);
        pos = QPoint(spot.right() + int(kGap), spot.center().y() - size.height() / 2);
        break;
    default:
        size += QSize(int(kTailSize), 0);
        pos = QPoint(spot.left() - int(kGap) - size.width(), spot.center().y() - size.height() / 2);
        break;
    }

    // A tip that left the window would be a tip about nothing, so it stays in
    // as far as it can and no further.
    pos.setX(qBound(area.left(), pos.x(), qMax(area.left(), area.right() - size.width())));
    pos.setY(qBound(area.top(), pos.y(), qMax(area.top(), area.bottom() - size.height())));
    q->setGeometry(QRect(pos, size));

    const bool vertical = side == FluentTeachingTip::Side::Bottom || side == FluentTeachingTip::Side::Top;
    const qreal reach = q->cornerRadius() + kTailHalf + 2.0;
    if (vertical) {
        const qreal x = spot.center().x() - pos.x();
        tailBase = QPointF(qBound(reach, x, qreal(size.width()) - reach), 0.0);
    } else {
        const qreal y = spot.center().y() - pos.y();
        tailBase = QPointF(0.0, qBound(reach, y, qreal(size.height()) - reach));
    }
}

FluentTeachingTip::FluentTeachingTip(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentTeachingTipPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A tip is a popup: it is above the window it belongs to, and a click
    // anywhere else dismisses it - which is both what Fluent calls a light
    // dismiss and what Qt already does with this flag.
    setWindowFlags(Qt::Popup);
}

FluentTeachingTip::~FluentTeachingTip(){QX_FINI_PRIVATE()}

QString FluentTeachingTip::title() const
{
    return d_ptr->title;
}

QString FluentTeachingTip::message() const
{
    return d_ptr->message;
}

QWidget *FluentTeachingTip::target() const
{
    return d_ptr->target.data();
}

FluentTeachingTip::Side FluentTeachingTip::preferredSide() const
{
    return d_ptr->preferred;
}

FluentTeachingTip::Side FluentTeachingTip::side() const
{
    return d_ptr->side;
}

bool FluentTeachingTip::isOpen() const
{
    return d_ptr->open;
}

void FluentTeachingTip::setTitle(const QString &title)
{
    if (d_ptr->title == title) {
        return;
    }
    d_ptr->title = title;
    if (d_ptr->open) {
        d_ptr->place();
    }
    updateGeometry();
    update();
}

void FluentTeachingTip::setMessage(const QString &message)
{
    if (d_ptr->message == message) {
        return;
    }
    d_ptr->message = message;
    if (d_ptr->open) {
        d_ptr->place();
    }
    updateGeometry();
    update();
}

void FluentTeachingTip::setTarget(QWidget *target)
{
    if (d_ptr->target.data() == target) {
        return;
    }
    d_ptr->target = target;
    if (d_ptr->open) {
        d_ptr->place();
        update();
    }
}

void FluentTeachingTip::setPreferredSide(Side side)
{
    if (d_ptr->preferred == side) {
        return;
    }
    d_ptr->preferred = side;
    if (d_ptr->open) {
        d_ptr->place();
        update();
    }
}

void FluentTeachingTip::setOpen(bool open)
{
    if (d_ptr->open == open) {
        return;
    }
    // A tip with nothing to point at is not a tip: it is a sentence floating
    // over the window, and it says nothing about anything on screen.
    if (open && !d_ptr->target) {
        return;
    }
    d_ptr->open = open;
    if (open) {
        d_ptr->place();
        show();
        raise();
    } else {
        hide();
        Q_EMIT closed();
    }
}

QSize FluentTeachingTip::sizeHint() const
{
    // The surface, without the strip the tail stands on - that one is the
    // tip's own business, decided by where it was opened.
    return d_ptr->bodySize();
}

qreal FluentTeachingTip::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::XLarge);
}

QRectF FluentTeachingTip::chromeRect() const
{
    // The tail is not part of the surface the wash and the ring belong to; it
    // is a pointer, not a control.
    return bodyRect(this, d_ptr->side);
}

void FluentTeachingTip::paintSurface(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QColor fill = palette().color(QPalette::Window);
    QColor stroke = textColor();
    stroke.setAlphaF(kStrokeOpacity);

    // The tail is drawn first and the surface over it, so the base of the
    // triangle disappears under the edge it stands on.
    const QPainterPath tail = tailPath(d_ptr->side, d_ptr->tailBase, QSizeF(width(), height()));
    painter->setPen(QPen(stroke, kStrokeWidth));
    painter->setBrush(fill);
    painter->drawPath(tail);

    const qreal radius = cornerRadius();
    const QRectF body = bodyRect(this, d_ptr->side);
    painter->drawRoundedRect(body, radius, radius);
    painter->restore();
}

void FluentTeachingTip::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QRectF body = bodyRect(this, d_ptr->side);
    const qreal right = body.right() - kPadding - kCloseSize - kIconGap;
    const qreal left = body.left() + kPadding;

    painter->setPen(textColor());
    painter->setFont(FluentType::font(FluentType::Ramp::Body, true));
    const QFontMetrics title(painter->font());
    painter->drawText(QRectF(left, body.top() + kPadding, right - left, title.height()),
                      Qt::AlignLeft | Qt::AlignVCenter, d_ptr->title);

    if (!d_ptr->message.isEmpty()) {
        painter->setFont(FluentType::font(FluentType::Ramp::Caption));
        const QRectF box(left, body.top() + kPadding + title.height() + kLineGap,
                         right - kPadding + kCloseSize + kIconGap - left, body.height());
        painter->drawText(box, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, d_ptr->message);
    }

    // The way out: a cross in the corner, and the only part of a tip that
    // does something.
    const QRectF close(body.right() - kPadding - kCloseSize, body.top() + kPadding, kCloseSize, kCloseSize);
    painter->setPen(QPen(textColor(), 1.5, Qt::SolidLine, Qt::RoundCap));
    const qreal inset = 8.0;
    painter->drawLine(close.topLeft() + QPointF(inset, inset), close.bottomRight() - QPointF(inset, inset));
    painter->drawLine(close.topRight() + QPointF(-inset, inset), close.bottomLeft() + QPointF(inset, -inset));

    painter->restore();
}

void FluentTeachingTip::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    const QRectF body = bodyRect(this, d_ptr->side);
    const QRectF close(body.right() - kPadding - kCloseSize, body.top() + kPadding, kCloseSize, kCloseSize);
    if (close.contains(eventPos(event))) {
        setOpen(false);
    }
}

void FluentTeachingTip::closeEvent(QCloseEvent *event)
{
    // Dismissed by Qt - a click elsewhere, an Escape - which is a dismissal
    // all the same, and has to say so: the application opened this thing.
    if (d_ptr->open) {
        d_ptr->open = false;
        Q_EMIT closed();
    }
    QxControls::QxControl::closeEvent(event);
}

QAccessible::Role FluentTeachingTip::accessibleRole() const
{
    // A balloon is the role Qt has for a flyout that explains something on
    // screen, which is what a teaching tip is.
    return QAccessible::HelpBalloon;
}

QX_FLUENT_END_NAMESPACE
