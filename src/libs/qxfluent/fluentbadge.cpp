/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentbadge.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetricsF>
#include <QtGui/QPainter>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A dot is six across; a number wears a pill sixteen tall, and its width is
// the number's with room to breathe on both sides.
constexpr qreal kDotSize = 6.0;
constexpr qreal kPillHeight = 16.0;
constexpr qreal kPillPadding = 6.0;
}   // namespace

class FluentBadgePrivate
{
public:
    QX_DECLARE_PUBLIC(FluentBadge)

    int count = 0;
    int maximumCount = 99;

    QString label() const
    {
        if (count <= 0) {
            return QString();
        }
        if (count > maximumCount) {
            return QString::number(maximumCount) + QLatin1Char('+');
        }
        return QString::number(count);
    }
};

FluentBadge::FluentBadge(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentBadgePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A badge is not something the keyboard stops at: it says something about
    // the control it sits beside.
    setFocusPolicy(Qt::NoFocus);
    // And it is the size its count asks for, whatever it is put in: a row that
    // has room to spare would otherwise stretch it, and a circle stretched one
    // way only is an ellipse - which is what the demo's badge had become.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

FluentBadge::~FluentBadge()
{
    QX_FINI_PRIVATE()
}

int FluentBadge::count() const
{
    return d_ptr->count;
}

int FluentBadge::maximumCount() const
{
    return d_ptr->maximumCount;
}

void FluentBadge::setCount(int count)
{
    const int bounded = qMax(0, count);
    if (d_ptr->count == bounded) {
        return;
    }
    d_ptr->count = bounded;
    setAccessibleName(d_ptr->label().isEmpty() ? QStringLiteral("new") : d_ptr->label());
    updateGeometry();
    update();
}

void FluentBadge::setMaximumCount(int maximum)
{
    if (d_ptr->maximumCount == maximum) {
        return;
    }
    d_ptr->maximumCount = qMax(1, maximum);
    updateGeometry();
    update();
}

QSize FluentBadge::sizeHint() const
{
    if (d_ptr->count <= 0) {
        return QSize(int(kDotSize), int(kDotSize));
    }
    // One figure is a circle - a number read at a glance is a dot with a
    // figure in it, not a lozenge - and more than one is a pill whose width is
    // the figure's with room on both sides. Fractional metrics, rounded up, for
    // the same reason the rest of the family measures that way: half a pixel
    // short is a drawn digit over the edge.
    const QString label = d_ptr->label();
    const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Caption));
    const int needed = int(qCeil(metrics.horizontalAdvance(label))) + int(2 * kPillPadding);
    const int width = label.size() <= 1 ? int(kPillHeight) : qMax(int(kPillHeight), needed);
    return QSize(width, int(kPillHeight));
}

QSize FluentBadge::minimumSizeHint() const
{
    return sizeHint();
}

qreal FluentBadge::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Circular);
}

void FluentBadge::paintSurface(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(accentColor());
    const qreal radius = cornerRadius();
    const QRectF badge = d_ptr->count <= 0 ? QRectF(0.0, 0.0, kDotSize, kDotSize) : QRectF(rect());
    painter->drawRoundedRect(badge, radius, radius);
    painter->restore();
}

void FluentBadge::paintContent(QPainter *painter)
{
    const QString label = d_ptr->label();
    if (label.isEmpty()) {
        return;
    }
    painter->save();
    painter->setFont(FluentType::font(FluentType::Ramp::Caption));
    painter->setPen(palette().color(QPalette::HighlightedText));
    painter->drawText(rect(), Qt::AlignCenter, label);
    painter->restore();
}

QAccessible::Role FluentBadge::accessibleRole() const
{
    return QAccessible::StaticText;
}

QX_FLUENT_END_NAMESPACE
