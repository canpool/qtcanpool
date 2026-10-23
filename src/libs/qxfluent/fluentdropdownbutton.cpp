/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentdropdownbutton.h"

#include "fluentcommandface_p.h"
#include "fluenttokens.h"

#include <QtCore/QtMath>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QMenu>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The inset a command keeps from the edges of its own face when it stands
// alone: roomier than a command in a bar, which holds many.
constexpr qreal kPadding = 12.0;
}   // namespace

class FluentDropDownButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentDropDownButton)

    QString text;
    QIcon icon;
    QMenu *menu = Q_NULLPTR;
};

FluentDropDownButton::FluentDropDownButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentDropDownButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->menu = new QMenu(this);
    // The whole face means the menu, so the base's click - which already means
    // "pressed and released on this control" - is the right event to answer.
    connect(this, &QxControl::clicked, this, [this]() {
        openMenu();
    });
}

FluentDropDownButton::~FluentDropDownButton(){QX_FINI_PRIVATE()}

QString FluentDropDownButton::text() const
{
    return d_ptr->text;
}

QIcon FluentDropDownButton::icon() const
{
    return d_ptr->icon;
}

QMenu *FluentDropDownButton::menu() const
{
    return d_ptr->menu;
}

void FluentDropDownButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // The label is the accessible name for as long as the application has not
    // given the control a better one - which it does by simply setting it.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
    Q_EMIT textChanged(text);
}

void FluentDropDownButton::setIcon(const QIcon &icon)
{
    d_ptr->icon = icon;
    updateGeometry();
    update();
    Q_EMIT iconChanged(icon);
}

void FluentDropDownButton::showMenu()
{
    openMenu();
}

QSize FluentDropDownButton::sizeHint() const
{
    qreal width = kPadding + CommandFace::kChevronWidth + kPadding;
    if (!d_ptr->icon.isNull()) {
        width += CommandFace::kIconSize + CommandFace::kIconGap;
    }
    if (!d_ptr->text.isEmpty()) {
        const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Body));
        width += metrics.horizontalAdvance(d_ptr->text);
    }
    return QSize(qCeil(width), int(CommandFace::kHeight));
}

QSize FluentDropDownButton::minimumSizeHint() const
{
    // The button can lose its text to an elision, but not its promise: the
    // chevron and the insets around it are the least it can be.
    qreal width = kPadding + CommandFace::kChevronWidth + kPadding;
    if (!d_ptr->icon.isNull()) {
        width += CommandFace::kIconSize + CommandFace::kIconGap;
    }
    return QSize(qCeil(width), int(CommandFace::kHeight));
}

QRectF FluentDropDownButton::chevronRect() const
{
    return QRectF(width() - kPadding - CommandFace::kChevronWidth, 0.0, CommandFace::kChevronWidth, height());
}

QRectF FluentDropDownButton::contentRect() const
{
    const qreal left = kPadding + (d_ptr->icon.isNull() ? 0.0 : CommandFace::kIconSize + CommandFace::kIconGap);
    const qreal right = chevronRect().left();
    return QRectF(left, 0.0, qMax(qreal(0.0), right - left), height());
}

QRectF FluentDropDownButton::iconRect() const
{
    return QRectF(kPadding, (height() - CommandFace::kIconSize) / 2.0, CommandFace::kIconSize, CommandFace::kIconSize);
}

qreal FluentDropDownButton::cornerRadius() const
{
    // A command's turn, the same as the commands in a bar: four.
    return FluentShape::radius(FluentShape::Radius::Medium);
}

void FluentDropDownButton::paintContent(QPainter *painter)
{
    QColor ink = textColor();
    if (!isEnabled()) {
        ink.setAlphaF(CommandFace::kDisabledOpacity);
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!d_ptr->icon.isNull()) {
        d_ptr->icon.paint(painter, iconRect().toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }

    if (!d_ptr->text.isEmpty()) {
        const QRectF room = contentRect();
        const QFont font = FluentType::font(FluentType::Ramp::Body);
        painter->setPen(ink);
        painter->setFont(font);
        // A command that has been given less room than it wants says less of
        // itself rather than a truncated syllable.
        const QFontMetricsF metrics(font);
        const QString shown = metrics.elidedText(d_ptr->text, Qt::ElideRight, room.width());
        painter->drawText(room, Qt::AlignLeft | Qt::AlignVCenter, shown);
    }

    CommandFace::paintChevron(painter, chevronRect(), ink);
    painter->restore();
}

void FluentDropDownButton::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Down:
    case Qt::Key_F4:
        // Every key that opens a menu on the platforms this runs on: Space and
        // Enter because it is a button, Down and F4 because it is a menu.
        openMenu();
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentDropDownButton::accessibleRole() const
{
    return QAccessible::Button;
}

void FluentDropDownButton::openMenu()
{
    if (!isEnabled()) {
        return;
    }
    Q_EMIT aboutToShowMenu();
    if (d_ptr->menu->isEmpty()) {
        // A menu with nothing in it is not a menu: opening one would be a
        // promise the button cannot keep.
        return;
    }
    d_ptr->menu->popup(mapToGlobal(QPoint(0, height())));
}

QX_FLUENT_END_NAMESPACE
