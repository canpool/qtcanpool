/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentbutton.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's button measures: thirty-two tall, the content keeping twelve on
// each side, an icon of sixteen with eight between it and the words.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 12.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 8.0;
// A standard face is the page slightly inked under a hairline; the accents
// are fractions of the ink, as the state layer's are.
constexpr qreal kStandardFillOpacity = 0.06;
constexpr qreal kStandardStrokeOpacity = 0.10;
constexpr qreal kDisabledOpacity = 0.4;
}   // namespace

class FluentButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentButton)

    QString text;
    QIcon icon;
    FluentButton::Kind kind = FluentButton::Kind::Standard;
};

FluentButton::FluentButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A button is on the keyboard's way: it is one of the things the keyboard
    // is for.
    setFocusPolicy(Qt::StrongFocus);
}

FluentButton::~FluentButton(){QX_FINI_PRIVATE()}

QString FluentButton::text() const
{
    return d_ptr->text;
}

QIcon FluentButton::icon() const
{
    return d_ptr->icon;
}

FluentButton::Kind FluentButton::kind() const
{
    return d_ptr->kind;
}

void FluentButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void FluentButton::setIcon(const QIcon &icon)
{
    if (d_ptr->icon.cacheKey() == icon.cacheKey()) {
        return;
    }
    d_ptr->icon = icon;
    Q_EMIT iconChanged(icon);
    updateGeometry();
    update();
}

void FluentButton::setKind(Kind kind)
{
    if (d_ptr->kind == kind) {
        return;
    }
    d_ptr->kind = kind;
    update();
}

QSize FluentButton::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const qreal textWidth = d_ptr->text.isEmpty() ? 0.0 : metrics.horizontalAdvance(d_ptr->text);
    const qreal iconWidth = d_ptr->icon.isNull() ? 0.0 : kIconSize;
    qreal width = 2 * kPadding + iconWidth + (iconWidth && textWidth ? kIconGap : 0.0) + textWidth;
    if (!iconWidth && !textWidth) {
        // An empty face is still a face: it stands at the square the height
        // asks for even with nothing to say.
        width = kHeight;
    } else if (!textWidth) {
        // An icon alone keeps no padding either side: the face is that
        // square too.
        width = kHeight;
    }
    return QSize(qCeil(width), int(kHeight));
}

QSize FluentButton::minimumSizeHint() const
{
    return sizeHint();
}

qreal FluentButton::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

QColor FluentButton::washColor() const
{
    // On the accent the wash is the light it brings with it: the hover
    // lightens the fill, where a wash of ink would darken it.
    if (d_ptr->kind == Kind::Accent) {
        return QColor(Qt::white);
    }
    return QxControls::QxControl::washColor();
}

void FluentButton::paintSurface(QPainter *painter)
{
    const qreal radius = cornerRadius();
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    painter->save();
    painter->setPen(Qt::NoPen);
    switch (d_ptr->kind) {
    case Kind::Standard:
        // The standard face is the page slightly inked, with a hairline for
        // its edge; disabled, the fill drains to the page and the hairline
        // with it.
        painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                                            isEnabled() ? kStandardFillOpacity : 0.0));
        painter->drawRoundedRect(face, radius, radius);
        if (isEnabled()) {
            painter->setPen(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                                              kStandardStrokeOpacity));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
        }
        break;
    case Kind::Accent:
        // The accent face is the accent, or the page when disabled - a
        // disabled command does not keep pointing.
        painter->setBrush(isEnabled() ? accentColor()
                                      : QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                                          textColor(), kStandardFillOpacity));
        painter->drawRoundedRect(face, radius, radius);
        break;
    case Kind::Subtle:
        // The subtle face is no face at all until the pointer arrives; the
        // wash the base draws over it is the whole answer.
        break;
    }
    painter->restore();
}

void FluentButton::paintContent(QPainter *painter)
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const qreal textWidth = d_ptr->text.isEmpty() ? 0.0 : metrics.horizontalAdvance(d_ptr->text);
    const bool hasIcon = !d_ptr->icon.isNull();
    const qreal iconWidth = hasIcon ? kIconSize : 0.0;
    const qreal content = iconWidth + (hasIcon && textWidth ? kIconGap : 0.0) + textWidth;
    const qreal left = (qreal(width()) - content) / 2.0;

    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setPen(d_ptr->kind == Kind::Accent && isEnabled() ? QColor(Qt::white) : textColor());
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    if (hasIcon) {
        const QRectF iconRect(left, (qreal(height()) - kIconSize) / 2.0, kIconSize, kIconSize);
        d_ptr->icon.paint(painter, iconRect.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }
    if (textWidth) {
        const QRectF textRect(left + iconWidth + (hasIcon ? kIconGap : 0.0), 0.0, textWidth, qreal(height()));
        painter->drawText(textRect, Qt::AlignCenter | Qt::TextSingleLine, d_ptr->text);
    }
    painter->restore();
}

QAccessible::Role FluentButton::accessibleRole() const
{
    return QAccessible::Button;
}

QX_FLUENT_END_NAMESPACE
