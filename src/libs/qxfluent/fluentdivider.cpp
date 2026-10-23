/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentdivider.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QPainter>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// One hairline, and the indent an inset one keeps: the same indent a row of
// content in the family keeps from the edge it lives against.
constexpr qreal kIndent = 24.0;
constexpr qreal kLineOpacity = 0.08;
}   // namespace

class FluentDividerPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentDivider)

    FluentDivider::Kind kind = FluentDivider::Kind::Full;
};

FluentDivider::FluentDivider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentDividerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A line is not something the keyboard stops at and not something the
    // pointer works: it separates, and that is all it is.
    setFocusPolicy(Qt::NoFocus);
}

FluentDivider::~FluentDivider(){QX_FINI_PRIVATE()}

FluentDivider::Kind FluentDivider::kind() const
{
    return d_ptr->kind;
}

void FluentDivider::setKind(Kind kind)
{
    if (d_ptr->kind == kind) {
        return;
    }
    d_ptr->kind = kind;
    update();
}

QSize FluentDivider::sizeHint() const
{
    // The width is the layout's to give; the line asks for its hairline and
    // nothing more.
    return QSize(0, 1);
}

QSize FluentDivider::minimumSizeHint() const
{
    return sizeHint();
}

void FluentDivider::paintSurface(QPainter *painter)
{
    const qreal from = d_ptr->kind == Kind::Inset ? kIndent : 0.0;
    const QRectF line(from, 0.0, qreal(width()) - from, 1.0);
    painter->fillRect(line,
                      QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kLineOpacity));
}

QAccessible::Role FluentDivider::accessibleRole() const
{
    return QAccessible::Separator;
}

QX_FLUENT_END_NAMESPACE
