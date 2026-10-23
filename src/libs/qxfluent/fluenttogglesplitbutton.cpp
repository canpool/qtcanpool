/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttogglesplitbutton.h"

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

// The line between the two halves: the promise is that there are two things
// here, so the seam is drawn.
constexpr qreal kSeamInset = CommandFace::kSeamInset;
constexpr qreal kSeamOpacity = CommandFace::kSeamOpacity;

// A command in force is washed in the accent.
constexpr qreal kCheckedOpacity = CommandFace::kCheckedOpacity;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentToggleSplitButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentToggleSplitButton)

    QString text;
    QIcon icon;
    bool checked = false;
    QMenu *menu = Q_NULLPTR;
    /*! The half the press began on: a press that is answered by the half it
     *  started on and by nothing else. */
    FluentToggleSplitButton::Half pressedHalf = FluentToggleSplitButton::Half::None;
};

FluentToggleSplitButton::FluentToggleSplitButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentToggleSplitButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->menu = new QMenu(this);
    // The base's click means "pressed and released on this control", and what
    // that is depends on the half - which the press has already recorded.
    connect(this, &QxControl::clicked, this, [this]() {
        switch (d_ptr->pressedHalf) {
        case Half::Chevron:
            openMenu();
            break;
        case Half::Body:
            toggle();
            break;
        case Half::None:
            break;
        }
    });
}

FluentToggleSplitButton::~FluentToggleSplitButton(){QX_FINI_PRIVATE()}

QString FluentToggleSplitButton::text() const
{
    return d_ptr->text;
}

QIcon FluentToggleSplitButton::icon() const
{
    return d_ptr->icon;
}

bool FluentToggleSplitButton::isChecked() const
{
    return d_ptr->checked;
}

QMenu *FluentToggleSplitButton::menu() const
{
    return d_ptr->menu;
}

void FluentToggleSplitButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // The label is the accessible name for as long as the application has not
    // given the control a better one.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
    Q_EMIT textChanged(text);
}

void FluentToggleSplitButton::setIcon(const QIcon &icon)
{
    d_ptr->icon = icon;
    updateGeometry();
    update();
    Q_EMIT iconChanged(icon);
}

void FluentToggleSplitButton::setChecked(bool checked)
{
    if (d_ptr->checked == checked) {
        return;
    }
    d_ptr->checked = checked;
    update();
    Q_EMIT toggled(checked);
}

void FluentToggleSplitButton::toggle()
{
    setChecked(!d_ptr->checked);
}

void FluentToggleSplitButton::showMenu()
{
    openMenu();
}

QSize FluentToggleSplitButton::sizeHint() const
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

QSize FluentToggleSplitButton::minimumSizeHint() const
{
    // The body can lose its text to an elision, but neither half can lose its
    // own shape: the chevron and the insets are the least this button can be.
    qreal width = kPadding + CommandFace::kChevronWidth + kPadding;
    if (!d_ptr->icon.isNull()) {
        width += CommandFace::kIconSize + CommandFace::kIconGap;
    }
    return QSize(qCeil(width), int(CommandFace::kHeight));
}

QRectF FluentToggleSplitButton::chevronRect() const
{
    return QRectF(width() - kPadding - CommandFace::kChevronWidth, 0.0, CommandFace::kChevronWidth, height());
}

QRectF FluentToggleSplitButton::contentRect() const
{
    const qreal left = kPadding + (d_ptr->icon.isNull() ? 0.0 : CommandFace::kIconSize + CommandFace::kIconGap);
    const qreal right = chevronRect().left();
    return QRectF(left, 0.0, qMax(qreal(0.0), right - left), height());
}

QRectF FluentToggleSplitButton::iconRect() const
{
    return QRectF(kPadding, (height() - CommandFace::kIconSize) / 2.0, CommandFace::kIconSize, CommandFace::kIconSize);
}

FluentToggleSplitButton::Half FluentToggleSplitButton::halfAt(const QPointF &pos) const
{
    if (chevronRect().contains(pos)) {
        return Half::Chevron;
    }
    if (rect().contains(pos.toPoint())) {
        return Half::Body;
    }
    return Half::None;
}

qreal FluentToggleSplitButton::cornerRadius() const
{
    // A command's turn, the same as the commands in a bar: four.
    return FluentShape::radius(FluentShape::Radius::Medium);
}

void FluentToggleSplitButton::paintSurface(QPainter *painter)
{
    if (!d_ptr->checked || !isEnabled()) {
        return;
    }
    // The command is in force: the accent, washed over the whole face - both
    // halves, because the button is one thing standing in one state.
    QColor wash = accentColor();
    wash.setAlphaF(kCheckedOpacity);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    const qreal radius = cornerRadius();
    painter->setBrush(wash);
    painter->drawRoundedRect(QRectF(rect()), radius, radius);
    painter->restore();
}

void FluentToggleSplitButton::paintContent(QPainter *painter)
{
    QColor ink = textColor();
    if (!isEnabled()) {
        ink.setAlphaF(CommandFace::kDisabledOpacity);
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // The seam: the two halves are two things, and a face that says so is one
    // the reader does not have to learn.
    QColor seam = ink;
    seam.setAlphaF(kSeamOpacity);
    painter->setPen(QPen(seam, 1.0));
    const QRectF chevron = chevronRect();
    painter->drawLine(QPointF(chevron.left(), kSeamInset), QPointF(chevron.left(), height() - kSeamInset));

    if (!d_ptr->icon.isNull()) {
        d_ptr->icon.paint(painter, iconRect().toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }

    if (!d_ptr->text.isEmpty()) {
        const QRectF room = contentRect();
        const QFont font = FluentType::font(FluentType::Ramp::Body);
        painter->setPen(ink);
        painter->setFont(font);
        const QFontMetricsF metrics(font);
        const QString shown = metrics.elidedText(d_ptr->text, Qt::ElideRight, room.width());
        painter->drawText(room, Qt::AlignLeft | Qt::AlignVCenter, shown);
    }

    CommandFace::paintChevron(painter, chevron, ink);
    painter->restore();
}

void FluentToggleSplitButton::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        // The keys of a button toggle what the button stands for.
        toggle();
        event->accept();
        return;
    case Qt::Key_Down:
    case Qt::Key_F4:
        // The keys of a menu open it - there is no keyboard half to aim at,
        // so the two have to be told apart by the key instead.
        openMenu();
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void FluentToggleSplitButton::mousePressEvent(QMouseEvent *event)
{
    d_ptr->pressedHalf = halfAt(eventPos(event));
    QxControls::QxControl::mousePressEvent(event);
}

void FluentToggleSplitButton::mouseReleaseEvent(QMouseEvent *event)
{
    // A press that wanders into the other half is not that half's: the press
    // chose, and the release confirms rather than decides.
    if (d_ptr->pressedHalf != Half::None && halfAt(eventPos(event)) != d_ptr->pressedHalf) {
        d_ptr->pressedHalf = Half::None;
    }
    QxControls::QxControl::mouseReleaseEvent(event);
    d_ptr->pressedHalf = Half::None;
}

void FluentToggleSplitButton::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->checked;
}

QAccessible::Role FluentToggleSplitButton::accessibleRole() const
{
    // A check box, because the question it answers is "is this in force" -
    // the state a check box carries and a button does not.
    return QAccessible::CheckBox;
}

void FluentToggleSplitButton::openMenu()
{
    if (!isEnabled()) {
        return;
    }
    Q_EMIT aboutToShowMenu();
    if (d_ptr->menu->isEmpty()) {
        return;
    }
    d_ptr->menu->popup(mapToGlobal(QPoint(0, height())));
}

QX_FLUENT_END_NAMESPACE
