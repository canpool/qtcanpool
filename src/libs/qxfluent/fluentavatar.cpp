/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentavatar.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QStringList>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The three sides, in the order of the enum. A face is round, so one measure
// answers for both.
constexpr int sides[] = {24, 40, 56};

// The stand: the page with enough of its own ink in it to read as a surface
// rather than as a hole.
constexpr qreal kStandOpacity = 0.12;
// A face that cannot be used is faded, the way everything else is.
constexpr qreal kDisabledOpacity = 0.38;

constexpr int side(FluentAvatar::Size avatarSize)
{
    const int index = int(avatarSize);
    return (index >= 0 && index < int(sizeof(sides) / sizeof(sides[0]))) ? sides[index] : sides[1];
}

// The type step the initials are written at: the face grows, the letters grow
// with it, and the three faces are three steps the family already has.
FluentType::Ramp lettersRamp(FluentAvatar::Size avatarSize)
{
    switch (avatarSize) {
    case FluentAvatar::Size::Small:
        return FluentType::Ramp::Caption;
    case FluentAvatar::Size::Medium:
        return FluentType::Ramp::Body;
    case FluentAvatar::Size::Large:
        return FluentType::Ramp::Subtitle;
    }
    return FluentType::Ramp::Body;
}
}   // namespace

class FluentAvatarPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentAvatar)

    QString name;
    QPixmap picture;
    FluentAvatar::Size size = FluentAvatar::Size::Medium;

    /*! The name shortened to what fits in a face: the first letters of its
     *  first two words, in capitals. One word gives one letter - inventing a
     *  second would be putting a letter in a person's mouth. */
    QString initials() const
    {
        QString letters;
        const QStringList words = name.split(u' ', Qt::SkipEmptyParts);
        for (const QString &word : words) {
            if (letters.count() == 2) {
                break;
            }
            for (const QChar &ch : word) {
                if (!ch.isSpace()) {
                    letters.append(QString(ch).toUpper());
                    break;
                }
            }
        }
        return letters;
    }
};

FluentAvatar::FluentAvatar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentAvatarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A face is a face and not a control: nothing to press, nothing to focus.
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFixedSize(side(d_ptr->size), side(d_ptr->size));
}

FluentAvatar::~FluentAvatar() = default;

QString FluentAvatar::name() const
{
    return d_ptr->name;
}

QString FluentAvatar::initials() const
{
    return d_ptr->initials();
}

QPixmap FluentAvatar::picture() const
{
    return d_ptr->picture;
}

FluentAvatar::Size FluentAvatar::avatarSize() const
{
    return d_ptr->size;
}

void FluentAvatar::setName(const QString &name)
{
    if (d_ptr->name == name) {
        return;
    }
    d_ptr->name = name;
    setAccessibleName(name);
    update();
    Q_EMIT nameChanged();
}

void FluentAvatar::setPicture(const QPixmap &picture)
{
    if (d_ptr->picture.cacheKey() == picture.cacheKey() && d_ptr->picture.isNull() == picture.isNull()) {
        return;
    }
    d_ptr->picture = picture;
    update();
    Q_EMIT pictureChanged();
}

void FluentAvatar::setAvatarSize(Size avatarSize)
{
    if (d_ptr->size == avatarSize) {
        return;
    }
    d_ptr->size = avatarSize;
    setFixedSize(side(avatarSize), side(avatarSize));
    update();
    Q_EMIT avatarSizeChanged();
}

QSize FluentAvatar::sizeHint() const
{
    return QSize(side(d_ptr->size), side(d_ptr->size));
}

QSize FluentAvatar::minimumSizeHint() const
{
    return sizeHint();
}

QRectF FluentAvatar::chromeRect() const
{
    // Nothing about a face is a control, so there is no part of it to wash and
    // none to draw a focus ring around.
    return QRectF();
}

void FluentAvatar::paintSurface(QPainter *painter)
{
    const QColor stand =
        QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kStandOpacity);
    QPainterPath face;
    const qreal round = width() / 2.0;
    face.addRoundedRect(QRectF(rect()), round, round);
    painter->fillPath(face, stand);
}

void FluentAvatar::paintContent(QPainter *painter)
{
    const QRectF face(rect());
    QPainterPath clip;
    const qreal round = width() / 2.0;
    clip.addRoundedRect(face, round, round);

    painter->save();
    painter->setClipPath(clip);
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

    if (!d_ptr->picture.isNull()) {
        painter->drawPixmap(face, d_ptr->picture, QRectF(d_ptr->picture.rect()));
    } else if (!d_ptr->initials().isEmpty()) {
        painter->setFont(FluentType::font(lettersRamp(d_ptr->size)));
        painter->setPen(textColor());
        painter->drawText(face, Qt::AlignCenter | Qt::TextSingleLine, d_ptr->initials());
    }

    painter->restore();
}

QAccessible::Role FluentAvatar::accessibleRole() const
{
    return QAccessible::Graphic;
}

void FluentAvatar::fillAccessibleState(QAccessible::State *state) const
{
    // A face says who, and does nothing: there is nothing to press and nothing
    // to type into.
    state->readOnly = true;
}

QX_FLUENT_END_NAMESPACE
