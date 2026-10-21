/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentinfobar.h"

#include "fluenttokens.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A bar is twelve in from every edge of what it says, and its icon is a
// twenty-four disc on the left.
constexpr qreal kPadding = 12.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kIconGap = 12.0;
constexpr qreal kCloseSize = 24.0;
constexpr qreal kLineGap = 2.0;

// The severities, as colours: a theme has a role for a highlight and none for
// "warning", so a bar carries these itself.
const QColor kSuccess(0x10, 0x7C, 0x10);
const QColor kWarning(0xF7, 0x63, 0x0C);
const QColor kError(0xC4, 0x2B, 0x1C);

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

QColor severityColor(FluentInfoBar::Severity severity, const QColor &accent)
{
    switch (severity) {
    case FluentInfoBar::Severity::Success:
        return kSuccess;
    case FluentInfoBar::Severity::Warning:
        return kWarning;
    case FluentInfoBar::Severity::Error:
        return kError;
    default:
        return accent;
    }
}

/*! The mark inside the disc: a letter for the severities that read as one, a
 *  tick for the one that does not. */
QString severityMark(FluentInfoBar::Severity severity)
{
    switch (severity) {
    case FluentInfoBar::Severity::Success:
        return QString();
    case FluentInfoBar::Severity::Warning:
    case FluentInfoBar::Severity::Error:
        return QStringLiteral("!");
    default:
        return QStringLiteral("i");
    }
}
}   // namespace

class FluentInfoBarPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentInfoBar)

    QString title;
    QString message;
    FluentInfoBar::Severity severity = FluentInfoBar::Severity::Informational;
    bool open = true;

    QRectF closeRect(const QWidget *widget) const
    {
        return QRectF(widget->width() - kPadding - kCloseSize, (widget->height() - kCloseSize) / 2.0, kCloseSize,
                      kCloseSize);
    }
};

FluentInfoBar::FluentInfoBar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentInfoBarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::NoFocus);
}

FluentInfoBar::~FluentInfoBar(){QX_FINI_PRIVATE()}

QString FluentInfoBar::title() const
{
    return d_ptr->title;
}

QString FluentInfoBar::message() const
{
    return d_ptr->message;
}

FluentInfoBar::Severity FluentInfoBar::severity() const
{
    return d_ptr->severity;
}

bool FluentInfoBar::isOpen() const
{
    return d_ptr->open;
}

void FluentInfoBar::setTitle(const QString &title)
{
    if (d_ptr->title == title) {
        return;
    }
    d_ptr->title = title;
    updateGeometry();
    update();
}

void FluentInfoBar::setMessage(const QString &message)
{
    if (d_ptr->message == message) {
        return;
    }
    d_ptr->message = message;
    updateGeometry();
    update();
}

void FluentInfoBar::setSeverity(Severity severity)
{
    if (d_ptr->severity == severity) {
        return;
    }
    d_ptr->severity = severity;
    update();
}

void FluentInfoBar::setOpen(bool open)
{
    if (d_ptr->open == open) {
        return;
    }
    d_ptr->open = open;
    // A closed bar is not a bar drawn differently: it is gone, and the layout
    // it was in gets its room back.
    setVisible(open);
    if (!open) {
        Q_EMIT closed();
    }
}

QSize FluentInfoBar::sizeHint() const
{
    const QFontMetrics body(FluentType::font(FluentType::Ramp::Body, true));
    const QFontMetrics caption(FluentType::font(FluentType::Ramp::Caption));
    const int left = int(kPadding + kIconSize + kIconGap);
    const int right = int(kPadding + kCloseSize + kIconGap);
    const int text = qMax(body.horizontalAdvance(d_ptr->title), caption.horizontalAdvance(d_ptr->message));

    int height = int(kIconSize) + int(2 * kPadding);
    if (!d_ptr->title.isEmpty() && !d_ptr->message.isEmpty()) {
        height += int(body.height() + kLineGap);
    }
    return QSize(left + text + right, height);
}

qreal FluentInfoBar::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

void FluentInfoBar::paintSurface(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette().color(QPalette::Window));
    const qreal radius = cornerRadius();
    painter->drawRoundedRect(QRectF(rect()), radius, radius);
    painter->restore();
}

void FluentInfoBar::paintContent(QPainter *painter)
{
    const QColor color = severityColor(d_ptr->severity, accentColor());

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // The disc: the severity, in its own colour, holding its own mark.
    const QRectF disc(kPadding, (height() - kIconSize) / 2.0, kIconSize, kIconSize);
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawEllipse(disc);

    const QString mark = severityMark(d_ptr->severity);
    if (mark.isEmpty()) {
        // Success is the one severity that reads as a shape rather than as a
        // letter: a tick.
        QPainterPath tick;
        tick.moveTo(disc.left() + 6.0, disc.center().y());
        tick.lineTo(disc.center().x() - 1.0, disc.bottom() - 7.0);
        tick.lineTo(disc.right() - 6.0, disc.top() + 7.0);
        painter->setPen(QPen(Qt::white, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(tick);
    } else {
        painter->setPen(Qt::white);
        QFont markFont = FluentType::font(FluentType::Ramp::Body, true);
        painter->setFont(markFont);
        painter->drawText(disc, Qt::AlignCenter, mark);
    }

    // What it says: a title in the heavier cut, and the message under it.
    const qreal left = kPadding + kIconSize + kIconGap;
    const qreal right = width() - kPadding - kCloseSize - kIconGap;
    const bool twoLines = !d_ptr->title.isEmpty() && !d_ptr->message.isEmpty();
    painter->setPen(textColor());
    painter->setFont(FluentType::font(FluentType::Ramp::Body, true));
    const qreal titleHeight = twoLines ? QFontMetrics(painter->font()).height() : height();
    painter->drawText(QRectF(left, kPadding, right - left, titleHeight), Qt::AlignLeft | Qt::AlignVCenter,
                      d_ptr->title);
    if (twoLines) {
        painter->setFont(FluentType::font(FluentType::Ramp::Caption));
        painter->drawText(QRectF(left, kPadding + titleHeight, right - left, height() - kPadding - titleHeight),
                          Qt::AlignLeft | Qt::AlignVCenter, d_ptr->message);
    }

    // The way out.
    const QRectF close = d_ptr->closeRect(this);
    painter->setPen(QPen(textColor(), 1.5, Qt::SolidLine, Qt::RoundCap));
    const qreal inset = 8.0;
    painter->drawLine(close.topLeft() + QPointF(inset, inset), close.bottomRight() - QPointF(inset, inset));
    painter->drawLine(close.topRight() + QPointF(-inset, inset), close.bottomLeft() + QPointF(inset, -inset));
    painter->restore();
}

void FluentInfoBar::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    // The cross is the only part of a bar that does something, and it is small
    // enough that a miss should not count as a hit.
    if (d_ptr->closeRect(this).contains(eventPos(event))) {
        setOpen(false);
    }
}

QAccessible::Role FluentInfoBar::accessibleRole() const
{
    return QAccessible::AlertMessage;
}

QX_FLUENT_END_NAMESPACE
