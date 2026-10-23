/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialbutton.h"

#include "materialripple.h"
#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's button measures: thirty-six tall, the content keeping sixteen
// on each side, an icon of twenty with eight between it and the words.
constexpr qreal kHeight = 36.0;
constexpr qreal kPadding = 16.0;
constexpr qreal kIconSize = 20.0;
constexpr qreal kIconGap = 8.0;
// The outline is the ink at the strength Material gives an edge; a disabled
// face is the ink thinned to what a command nobody can reach wears.
constexpr qreal kOutlineOpacity = 0.12;
constexpr qreal kDisabledFillOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.38;
}   // namespace

class MaterialButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialButton)

    QString text;
    QIcon icon;
    MaterialButton::Variant variant = MaterialButton::Variant::Filled;
    MaterialRipple *ripple = Q_NULLPTR;

    /*! The ink the ripple answers with: on the accent's own face it is the
     *  colour that reads on the accent, on the page's face it is the ink. */
    QColor rippleInk() const;
};

QColor MaterialButtonPrivate::rippleInk() const
{
    return variant == MaterialButton::Variant::Filled ? q_ptr->palette().color(QPalette::HighlightedText)
                                                      : q_ptr->textColor();
}

MaterialButton::MaterialButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A button is on the keyboard's way: it is one of the things the keyboard
    // is for.
    setFocusPolicy(Qt::StrongFocus);
    // The ripple is what the surface does about being pressed, and it is the
    // same answer the chip and the floating action button give.
    d_ptr->ripple = new MaterialRipple(this);
    d_ptr->ripple->setColor(d_ptr->rippleInk());
}

MaterialButton::~MaterialButton(){QX_FINI_PRIVATE()}

QString MaterialButton::text() const
{
    return d_ptr->text;
}

QIcon MaterialButton::icon() const
{
    return d_ptr->icon;
}

MaterialButton::Variant MaterialButton::variant() const
{
    return d_ptr->variant;
}

void MaterialButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void MaterialButton::setIcon(const QIcon &icon)
{
    if (d_ptr->icon.cacheKey() == icon.cacheKey()) {
        return;
    }
    d_ptr->icon = icon;
    Q_EMIT iconChanged(icon);
    updateGeometry();
    update();
}

void MaterialButton::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    d_ptr->variant = variant;
    d_ptr->ripple->setColor(d_ptr->rippleInk());
    update();
}

QSize MaterialButton::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
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

QSize MaterialButton::minimumSizeHint() const
{
    return sizeHint();
}

qreal MaterialButton::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Small);
}

QColor MaterialButton::washColor() const
{
    // On the accent's own face the wash is the light it brings with it: the
    // hover lightens the fill, where a wash of ink would darken it.
    if (d_ptr->variant == Variant::Filled) {
        return palette().color(QPalette::HighlightedText);
    }
    return QxControls::QxControl::washColor();
}

void MaterialButton::paintSurface(QPainter *painter)
{
    const qreal radius = cornerRadius();
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    switch (d_ptr->variant) {
    case Variant::Filled:
        // The accent's own colour, or the page when disabled - a disabled
        // command does not keep pointing.
        painter->setBrush(isEnabled() ? accentColor()
                                      : QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                                          textColor(), kDisabledFillOpacity));
        painter->drawRoundedRect(face, radius, radius);
        // Pressed: it answers from where the pointer landed, and that travels.
        if (isEnabled()) {
            d_ptr->ripple->paint(painter, face, radius);
        }
        break;
    case Variant::Outlined:
        // The page's own colour under an outline; a disabled command drops
        // the outline with the pointing it would do.
        if (isEnabled()) {
            painter->setPen(
                QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kOutlineOpacity));
            painter->setBrush(Qt::NoBrush);
            painter->drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
            d_ptr->ripple->paint(painter, face, radius);
        }
        break;
    case Variant::Text:
        // The text variant is no surface at all; the wash the base draws
        // over it is the whole answer, and there is no ripple on it.
        break;
    }
    painter->restore();
}

void MaterialButton::paintContent(QPainter *painter)
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const qreal textWidth = d_ptr->text.isEmpty() ? 0.0 : metrics.horizontalAdvance(d_ptr->text);
    const bool hasIcon = !d_ptr->icon.isNull();
    const qreal iconWidth = hasIcon ? kIconSize : 0.0;
    const qreal content = iconWidth + (hasIcon && textWidth ? kIconGap : 0.0) + textWidth;
    const qreal left = (qreal(width()) - content) / 2.0;

    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    if (d_ptr->variant == Variant::Filled && isEnabled()) {
        painter->setPen(palette().color(QPalette::HighlightedText));
    } else if (d_ptr->variant == Variant::Text && isEnabled()) {
        // The text variant has no surface to carry the pointing, so the
        // words point: they are the accent's own colour.
        painter->setPen(accentColor());
    } else {
        painter->setPen(textColor());
    }
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

void MaterialButton::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialButton::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialButton::accessibleRole() const
{
    return QAccessible::Button;
}

QX_MATERIAL_END_NAMESPACE
