/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialbadge.h"

#include "materialtokens.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's badge measures: a dot of six, and a number in a pill sixteen tall
// with four either side of the digits.
constexpr qreal kDotSize = 6.0;
constexpr qreal kBadgeHeight = 16.0;
constexpr qreal kPadding = 4.0;

// The colour is the family's own: a theme has a role for a highlight and none
// for "this needs attention", which is what a badge says.
const QColor kAttention(0xB3, 0x26, 0x1E);
}   // namespace

class MaterialBadgePrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialBadge)

    int count = 0;
    int maximumCount = 99;

    QString label() const
    {
        if (count <= 0) {
            return QString();
        }
        if (count > maximumCount) {
            // Past the maximum the badge stops counting and starts saying so:
            // "127" is not more useful than "99+" and it is wider.
            return QString::number(maximumCount) + QStringLiteral("+");
        }
        return QString::number(count);
    }

    /*! What an assistive tool reads is what the badge says, and the dot has
     *  nothing to say but that there is something. Both of the things that
     *  change what it says have to say so. */
    void announce(MaterialBadge *badge)
    {
        const QString text = label();
        badge->setAccessibleName(text.isEmpty() ? QStringLiteral("There is something") : text);
    }
};

MaterialBadge::MaterialBadge(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialBadgePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A badge is not a stop on the way through the keyboard and not a thing
    // the pointer takes hold of: it is a mark on something else.
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

MaterialBadge::~MaterialBadge()
{
    QX_FINI_PRIVATE()
}

int MaterialBadge::count() const
{
    return d_ptr->count;
}

int MaterialBadge::maximumCount() const
{
    return d_ptr->maximumCount;
}

void MaterialBadge::setCount(int count)
{
    const int bounded = qMax(0, count);
    if (d_ptr->count == bounded) {
        return;
    }
    d_ptr->count = bounded;
    d_ptr->announce(this);
    updateGeometry();
    update();
}

void MaterialBadge::setMaximumCount(int maximum)
{
    if (d_ptr->maximumCount == maximum) {
        return;
    }
    d_ptr->maximumCount = maximum;
    d_ptr->announce(this);
    updateGeometry();
    update();
}

QSize MaterialBadge::sizeHint() const
{
    if (d_ptr->count <= 0) {
        return QSize(int(kDotSize), int(kDotSize));
    }
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Label));
    const int width = metrics.horizontalAdvance(d_ptr->label()) + int(2 * kPadding);
    return QSize(qMax(width, int(kBadgeHeight)), int(kBadgeHeight));
}

QSize MaterialBadge::minimumSizeHint() const
{
    return sizeHint();
}

qreal MaterialBadge::cornerRadius() const
{
    // Sixteen tall and a radius of eight: the pill Material's badge is.
    return kBadgeHeight / 2.0;
}

void MaterialBadge::paintSurface(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(kAttention);
    if (d_ptr->count <= 0) {
        painter->drawEllipse(QRectF(0.0, 0.0, kDotSize, kDotSize));
    } else {
        const qreal radius = cornerRadius();
        painter->drawRoundedRect(QRectF(rect()), radius, radius);
    }
    painter->restore();
}

void MaterialBadge::paintContent(QPainter *painter)
{
    if (d_ptr->count <= 0) {
        return;
    }
    painter->save();
    painter->setFont(MaterialType::font(MaterialType::Scale::Label, true));
    // On error: the one colour that reads on the badge's own.
    painter->setPen(Qt::white);
    painter->drawText(QRectF(rect()), Qt::AlignCenter, d_ptr->label());
    painter->restore();
}

QAccessible::Role MaterialBadge::accessibleRole() const
{
    return QAccessible::StaticText;
}

QX_MATERIAL_END_NAMESPACE
