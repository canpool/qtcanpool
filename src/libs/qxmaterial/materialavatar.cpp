/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialavatar.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The three sides, in the order of the enum. An avatar is square, so one
// measure answers for both of them.
constexpr int sides[] = {24, 40, 56};

// The stand of an avatar that stands on the quiet tone: the page with enough
// of its own ink in it to read as a surface rather than as a hole.
constexpr qreal kNeutralContainerOpacity = 0.12;
// An avatar that cannot be used is drawn faded, the way the family fades
// everything else that cannot be used.
constexpr qreal kDisabledOpacity = 0.38;
// The icon of an avatar takes six tenths of the face.
constexpr qreal kIconRatio = 0.6;

constexpr int side(MaterialAvatar::Size avatarSize)
{
    const int index = int(avatarSize);
    return (index >= 0 && index < int(sizeof(sides) / sizeof(sides[0]))) ? sides[index] : sides[1];
}

// The letter a name comes out as: the first character that is not a space, in
// capitals - a name is written the way a name is written.
QString letterOf(const QString &name)
{
    for (const QChar &ch : name) {
        if (!ch.isSpace()) {
            return QString(ch).toUpper();
        }
    }
    return QString();
}

// The type step a letter is written at: the face grows, the letter grows with
// it, and the three faces are the three steps the family already has.
MaterialType::Scale lettersScale(MaterialAvatar::Size avatarSize)
{
    switch (avatarSize) {
    case MaterialAvatar::Size::Small:
        return MaterialType::Scale::Label;
    case MaterialAvatar::Size::Medium:
        return MaterialType::Scale::Body;
    case MaterialAvatar::Size::Large:
        return MaterialType::Scale::Title;
    }
    return MaterialType::Scale::Body;
}

}   // namespace

class MaterialAvatarPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialAvatar)

    QString name;
    QIcon icon;
    QPixmap picture;
    MaterialAvatar::Size size = MaterialAvatar::Size::Medium;
    MaterialAvatar::Shape shape = MaterialAvatar::Shape::Circle;
    MaterialAvatar::Tone tone = MaterialAvatar::Tone::Primary;

    QString initials() const
    {
        return letterOf(name);
    }
};

MaterialAvatar::MaterialAvatar(QWidget *parent)
    : QxControl(parent)
    , d_ptr(new MaterialAvatarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A face is not a control: nothing to click, nothing to tab to.
    setFocusPolicy(Qt::NoFocus);
    setFixedSize(side(d_ptr->size), side(d_ptr->size));
}

MaterialAvatar::~MaterialAvatar(){QX_FINI_PRIVATE()}

QString MaterialAvatar::name() const
{
    return d_ptr->name;
}

QString MaterialAvatar::initials() const
{
    return d_ptr->initials();
}

QIcon MaterialAvatar::icon() const
{
    return d_ptr->icon;
}

QPixmap MaterialAvatar::picture() const
{
    return d_ptr->picture;
}

MaterialAvatar::Size MaterialAvatar::avatarSize() const
{
    return d_ptr->size;
}

MaterialAvatar::Shape MaterialAvatar::shape() const
{
    return d_ptr->shape;
}

MaterialAvatar::Tone MaterialAvatar::tone() const
{
    return d_ptr->tone;
}

QSize MaterialAvatar::sizeHint() const
{
    const int face = side(d_ptr->size);
    return QSize(face, face);
}

QSize MaterialAvatar::minimumSizeHint() const
{
    return sizeHint();
}

void MaterialAvatar::setName(const QString &name)
{
    if (d_ptr->name == name) {
        return;
    }
    d_ptr->name = name;
    // What a screen reader says is the name, not the one letter of it: the
    // letter is the picture a name is drawn as.
    setAccessibleName(name.isEmpty() ? d_ptr->initials() : name);
    update();
    Q_EMIT nameChanged(name);
}

void MaterialAvatar::setIcon(const QIcon &icon)
{
    d_ptr->icon = icon;
    update();
    Q_EMIT iconChanged(icon);
}

void MaterialAvatar::setPicture(const QPixmap &picture)
{
    d_ptr->picture = picture;
    update();
    Q_EMIT pictureChanged(picture);
}

void MaterialAvatar::setAvatarSize(Size avatarSize)
{
    if (d_ptr->size == avatarSize) {
        return;
    }
    d_ptr->size = avatarSize;
    // An avatar is square whatever else happens to it: the face is the size,
    // and a face stretched by a layout is not an avatar any more.
    setFixedSize(side(avatarSize), side(avatarSize));
    updateGeometry();
    update();
    Q_EMIT avatarSizeChanged(avatarSize);
}

void MaterialAvatar::setShape(Shape shape)
{
    if (d_ptr->shape == shape) {
        return;
    }
    d_ptr->shape = shape;
    update();
    Q_EMIT shapeChanged(shape);
}

void MaterialAvatar::setTone(Tone tone)
{
    if (d_ptr->tone == tone) {
        return;
    }
    d_ptr->tone = tone;
    update();
    Q_EMIT toneChanged(tone);
}

qreal MaterialAvatar::cornerRadius() const
{
    switch (d_ptr->shape) {
    case Shape::Circle:
        // The family's fully round radius, which the painter takes as "as
        // round as the rectangle allows".
        return MaterialShape::radius(MaterialShape::Radius::Circular);
    case Shape::Rounded:
        return MaterialShape::radius(MaterialShape::Radius::Small);
    case Shape::Square:
        break;
    }
    return 0.0;
}

QRectF MaterialAvatar::chromeRect() const
{
    // Nothing about an avatar is a control, so there is no part of it to wash
    // and none to draw a focus ring around.
    return QRectF();
}

void MaterialAvatar::paintSurface(QPainter *painter)
{
    const QColor stand = d_ptr->tone == Tone::Primary
                             ? accentColor()
                             : QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                                                 kNeutralContainerOpacity);
    QPainterPath face;
    const qreal radius = cornerRadius();
    face.addRoundedRect(QRectF(rect()), radius, radius);
    painter->fillPath(face, stand);
}

void MaterialAvatar::paintContent(QPainter *painter)
{
    const QRectF face(rect());
    QPainterPath clip;
    const qreal radius = cornerRadius();
    clip.addRoundedRect(face, radius, radius);

    painter->save();
    painter->setClipPath(clip);
    // A face that colours itself white on the primary cannot be faded twice,
    // so the fading is the whole of a disabled avatar's answer.
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

    if (!d_ptr->picture.isNull()) {
        painter->drawPixmap(face, d_ptr->picture, QRectF(d_ptr->picture.rect()));
    } else if (!d_ptr->icon.isNull()) {
        const qreal extent = qMin(face.width(), face.height()) * kIconRatio;
        const QRectF room(face.center().x() - extent / 2.0, face.center().y() - extent / 2.0, extent, extent);
        d_ptr->icon.paint(painter, room.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    } else if (!d_ptr->initials().isEmpty()) {
        painter->setFont(MaterialType::font(lettersScale(d_ptr->size), true));
        painter->setPen(d_ptr->tone == Tone::Primary ? QColor(Qt::white) : textColor());
        painter->drawText(face, Qt::AlignCenter | Qt::TextSingleLine, d_ptr->initials());
    }

    painter->restore();
}

QAccessible::Role MaterialAvatar::accessibleRole() const
{
    return QAccessible::Graphic;
}

void MaterialAvatar::fillAccessibleState(QAccessible::State *state) const
{
    // An avatar says who or what, and does nothing: there is nothing to
    // press and nothing to type into.
    state->readOnly = true;
}

QX_MATERIAL_END_NAMESPACE
