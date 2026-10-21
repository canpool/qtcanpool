/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialchip.h"

#include "materialtokens.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's chip is thirty-two tall, with sixteen either side of what it
// says - smaller than a button, which is what makes a row of them a set
// rather than a toolbar.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 16.0;

// The mark a filter chip carries while its filter is in force, and the room it
// takes away from the text.
constexpr qreal kMarkSize = 18.0;
constexpr qreal kMarkGap = 8.0;

constexpr qreal kOutlineWidth = 1.0;
constexpr qreal kOutlineOpacity = 0.30;
// The fill of a chip whose filter is in force: a tint, because the chip is
// still on the page and not a button of its own.
constexpr qreal kCheckedFill = 0.16;
constexpr qreal kDisabledOpacity = 0.38;
}   // namespace

class MaterialChipPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialChip)

    MaterialChip::Kind kind = MaterialChip::Kind::Assist;
    QString text;
    bool checked = false;

    bool holdsState() const
    {
        return kind == MaterialChip::Kind::Filter;
    }
    /*! The room the mark takes, which the text starts after. */
    qreal markWidth() const
    {
        return (holdsState() && checked) ? kMarkSize + kMarkGap : 0.0;
    }
    /*! The answer to a press that came from the keyboard. */
    void press()
    {
        if (holdsState()) {
            q_ptr->toggle();
        } else {
            Q_EMIT q_ptr->clicked();
        }
    }
};

MaterialChip::MaterialChip(Kind kind, QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialChipPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->kind = kind;
    setFocusPolicy(Qt::StrongFocus);
    // A press with the pointer is the base's to answer; what a chip adds to it
    // is the state a filter chip holds.
    QObject::connect(this, &QxControls::QxControl::clicked, this, [this]() {
        if (d_ptr->holdsState()) {
            toggle();
        }
    });
}

MaterialChip::MaterialChip(const QString &text, QWidget *parent)
    : MaterialChip(Kind::Assist, parent)
{
    setText(text);
}

MaterialChip::~MaterialChip(){QX_FINI_PRIVATE()}

MaterialChip::Kind MaterialChip::kind() const
{
    return d_ptr->kind;
}

QString MaterialChip::text() const
{
    return d_ptr->text;
}

bool MaterialChip::isChecked() const
{
    return d_ptr->checked;
}

QSize MaterialChip::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const qreal width = 2.0 * kPadding + d_ptr->markWidth() + metrics.horizontalAdvance(d_ptr->text);
    return QSize(qRound(width), int(kHeight));
}

QSize MaterialChip::minimumSizeHint() const
{
    const int width = qMax(int(kHeight), int(2.0 * kPadding + d_ptr->markWidth()));
    return QSize(width, int(kHeight));
}

void MaterialChip::setKind(Kind kind)
{
    if (d_ptr->kind == kind) {
        return;
    }
    d_ptr->kind = kind;
    // A chip that holds nothing cannot be left holding something, so the state
    // goes with the kind that held it - and says so going.
    const bool dropped = d_ptr->checked && !d_ptr->holdsState();
    if (dropped) {
        d_ptr->checked = false;
    }
    updateGeometry();
    update();
    if (dropped) {
        Q_EMIT toggled(false);
    }
}

void MaterialChip::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // What the chip says is its accessible name for as long as the application
    // has not given it a better one.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
}

void MaterialChip::setChecked(bool checked)
{
    if (!d_ptr->holdsState() || d_ptr->checked == checked) {
        return;
    }
    d_ptr->checked = checked;
    updateGeometry();
    update();
    Q_EMIT toggled(checked);
}

void MaterialChip::toggle()
{
    setChecked(!d_ptr->checked);
}

qreal MaterialChip::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Medium);
}

void MaterialChip::paintSurface(QPainter *painter)
{
    const QRectF area = QRectF(rect());
    const qreal radius = cornerRadius();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    if (d_ptr->checked) {
        QColor fill = accentColor();
        fill.setAlphaF(kCheckedFill);
        painter->setPen(Qt::NoPen);
        painter->setBrush(fill);
        painter->drawRoundedRect(area, radius, radius);
    } else {
        // Unselected is a chip seen through an outline, which is what makes it
        // findable on a surface of the same colour.
        QColor outline = textColor();
        outline.setAlphaF(kOutlineOpacity);
        painter->setPen(QPen(outline, kOutlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(area.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    }
    painter->restore();
}

void MaterialChip::paintContent(QPainter *painter)
{
    const QRectF area = QRectF(rect());
    qreal left = area.left() + kPadding;

    if (d_ptr->holdsState() && d_ptr->checked) {
        // The mark is what makes "which of these are on" readable without
        // pressing each one to find out.
        const qreal y = area.center().y();
        QPainterPath tick;
        tick.moveTo(left, y);
        tick.lineTo(left + 5.0, y + 5.0);
        tick.lineTo(left + kMarkSize - 4.0, y - 5.0);
        painter->setPen(QPen(textColor(), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(tick);
        left += kMarkSize + kMarkGap;
    }

    painter->setPen(textColor());
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    painter->drawText(QRectF(left, area.top(), area.right() - kPadding - left, area.height()),
                      Qt::AlignVCenter | Qt::AlignLeft, d_ptr->text);
}

void MaterialChip::keyPressEvent(QKeyEvent *event)
{
    // Enter answers on the press the way a button does; Space answers on the
    // release, so holding it down does not press the chip over and over.
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        d_ptr->press();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialChip::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        d_ptr->press();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialChip::accessibleRole() const
{
    return d_ptr->holdsState() ? QAccessible::CheckBox : QAccessible::PushButton;
}

void MaterialChip::fillAccessibleState(QAccessible::State *state) const
{
    if (!d_ptr->holdsState()) {
        return;
    }
    state->checkable = true;
    state->checked = d_ptr->checked;
}

QX_MATERIAL_END_NAMESPACE
