/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentdropdownbutton.h"

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
// Fluent's command: thirty-two tall, with an icon of sixteen at the leading
// edge and a chevron of twenty at the other - the same measure the command bar
// gives a drop-down command, because the two are the same thing in two places.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 12.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 8.0;
constexpr qreal kChevronWidth = 20.0;
constexpr qreal kChevronGlyph = 8.0;
constexpr qreal kChevronStroke = 1.5;

// A control nobody can use is the same drawing thinned.
constexpr qreal kDisabledOpacity = 0.4;

/*! The chevron that says a menu is here: a point turned down, drawn rather
 *  than typed, because a glyph in a font is a glyph the theme cannot colour
 *  and an assistive tool would read. */
void paintChevron(QPainter *painter, const QRectF &rect, const QColor &color)
{
    const qreal x = rect.center().x() - kChevronGlyph / 2.0;
    const qreal y = rect.center().y() - kChevronGlyph / 4.0;
    QPainterPath chevron;
    chevron.moveTo(x, y);
    chevron.lineTo(x + kChevronGlyph / 2.0, y + kChevronGlyph / 2.0);
    chevron.lineTo(x + kChevronGlyph, y);
    painter->setPen(QPen(color, kChevronStroke, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(chevron);
}
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
    qreal width = kPadding + kChevronWidth + kPadding;
    if (!d_ptr->icon.isNull()) {
        width += kIconSize + kIconGap;
    }
    if (!d_ptr->text.isEmpty()) {
        const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Body));
        width += metrics.horizontalAdvance(d_ptr->text);
    }
    return QSize(qCeil(width), int(kHeight));
}

QSize FluentDropDownButton::minimumSizeHint() const
{
    // The button can lose its text to an elision, but not its promise: the
    // chevron and the insets around it are the least it can be.
    qreal width = kPadding + kChevronWidth + kPadding;
    if (!d_ptr->icon.isNull()) {
        width += kIconSize + kIconGap;
    }
    return QSize(qCeil(width), int(kHeight));
}

QRectF FluentDropDownButton::chevronRect() const
{
    return QRectF(width() - kPadding - kChevronWidth, 0.0, kChevronWidth, height());
}

QRectF FluentDropDownButton::contentRect() const
{
    const qreal left = kPadding + (d_ptr->icon.isNull() ? 0.0 : kIconSize + kIconGap);
    const qreal right = chevronRect().left();
    return QRectF(left, 0.0, qMax(qreal(0.0), right - left), height());
}

QRectF FluentDropDownButton::iconRect() const
{
    return QRectF(kPadding, (height() - kIconSize) / 2.0, kIconSize, kIconSize);
}

void FluentDropDownButton::paintContent(QPainter *painter)
{
    QColor ink = textColor();
    if (!isEnabled()) {
        ink.setAlphaF(kDisabledOpacity);
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

    paintChevron(painter, chevronRect(), ink);
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
