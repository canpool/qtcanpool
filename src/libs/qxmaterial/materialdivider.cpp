/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialdivider.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// One hairline, and the indent a row of content in the family keeps from the
// edge it lives against: the same sixteen a list item keeps.
constexpr qreal kIndent = 16.0;
constexpr qreal kLineOpacity = 0.12;
}   // namespace

class MaterialDividerPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialDivider)

    MaterialDivider::Variant variant = MaterialDivider::Variant::Full;
};

MaterialDivider::MaterialDivider(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialDividerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A line is not something the keyboard stops at and not something the
    // pointer works: it separates, and that is all it is.
    setFocusPolicy(Qt::NoFocus);
}

MaterialDivider::~MaterialDivider(){QX_FINI_PRIVATE()}

MaterialDivider::Variant MaterialDivider::variant() const
{
    return d_ptr->variant;
}

void MaterialDivider::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    d_ptr->variant = variant;
    update();
}

QSize MaterialDivider::sizeHint() const
{
    // The width is the layout's to give; the line asks for its hairline and
    // nothing more.
    return QSize(0, 1);
}

QSize MaterialDivider::minimumSizeHint() const
{
    return sizeHint();
}

void MaterialDivider::paintSurface(QPainter *painter)
{
    const qreal from = d_ptr->variant == Variant::Full ? 0.0 : kIndent;
    const qreal to = d_ptr->variant == Variant::Middle ? kIndent : 0.0;
    const QRectF line(from, 0.0, qMax<qreal>(0.0, qreal(width()) - from - to), 1.0);
    painter->fillRect(line,
                      QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kLineOpacity));
}

QAccessible::Role MaterialDivider::accessibleRole() const
{
    return QAccessible::Separator;
}

QX_MATERIAL_END_NAMESPACE
