/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinobutton.h"

#include "cupertinotokens.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
// The capsule: thirty-two tall with sixteen either side of what it says, which
// is the same room the family's other single-line controls take and enough for
// the label never to touch the round end.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 16.0;

// The second level's fill: the blue thinned over the page. Enough of it that
// the capsule reads as a surface, little enough that the word in the blue is
// still what carries the action.
constexpr qreal kTintOpacity = 0.15;

// A disabled control keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;

// Which face of the semantic colours the control is drawn on.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class CupertinoButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoButton)

    QString text;
    CupertinoButton::Role role = CupertinoButton::Role::Filled;

    /*! What a press from the keyboard answers with. The pointer's press is the
     *  base's, and it answers the same signal. */
    void press()
    {
        Q_EMIT q_ptr->clicked();
    }
};

CupertinoButton::CupertinoButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
}

CupertinoButton::CupertinoButton(const QString &text, QWidget *parent)
    : CupertinoButton(parent)
{
    setText(text);
}

CupertinoButton::~CupertinoButton(){QX_FINI_PRIVATE()}

QString CupertinoButton::text() const
{
    return d_ptr->text;
}

CupertinoButton::Role CupertinoButton::role() const
{
    return d_ptr->role;
}

QSize CupertinoButton::sizeHint() const
{
    const QFontMetrics metrics(CupertinoType::font(CupertinoType::Ramp::Body));
    const qreal width = 2.0 * kPadding + metrics.horizontalAdvance(d_ptr->text);
    return QSize(qRound(width), int(kHeight));
}

QSize CupertinoButton::minimumSizeHint() const
{
    return QSize(int(kHeight), int(kHeight));
}

void CupertinoButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // What the button says is its accessible name for as long as the
    // application has not given it a better one.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
    Q_EMIT textChanged(text);
}

void CupertinoButton::setRole(Role role)
{
    if (d_ptr->role == role) {
        return;
    }
    d_ptr->role = role;
    update();
    Q_EMIT roleChanged(role);
}

qreal CupertinoButton::cornerRadius() const
{
    // The capsule: the roundest shape the family has, and the reason a row of
    // two buttons reads as two objects rather than as two blocks. The radius has
    // to be half the height and not the family's fully round step, which is a
    // radius no rectangle reaches: a painter clamps that one per axis, and on a
    // button this wide the ends become half an ellipse - a pointed end rather
    // than a capsule.
    return qMin(CupertinoShape::radius(CupertinoShape::Radius::Circular), height() / 2.0);
}

void CupertinoButton::paintSurface(QPainter *painter)
{
    const QRectF capsule = QRectF(rect());
    const qreal radius = cornerRadius();
    const QColor accent = CupertinoColor::color(CupertinoColor::Role::Accent, onDarkFace(this));

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    switch (d_ptr->role) {
    case Role::Filled:
        painter->setBrush(accent);
        painter->drawRoundedRect(capsule, radius, radius);
        break;
    case Role::Tinted: {
        QColor tint = accent;
        tint.setAlphaF(kTintOpacity);
        painter->setBrush(tint);
        painter->drawRoundedRect(capsule, radius, radius);
        break;
    }
    case Role::Plain:
        // Nothing to draw: the word is the whole control, and a fill under it
        // would be an action claiming more surface than it was given.
        break;
    }
    painter->restore();
}

void CupertinoButton::paintContent(QPainter *painter)
{
    const QColor accent = CupertinoColor::color(CupertinoColor::Role::Accent, onDarkFace(this));

    painter->save();
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    painter->setFont(CupertinoType::font(CupertinoType::Ramp::Body));
    // The label is the level: reversed out of the blue where the blue is the
    // surface, in the blue where there is no surface at all.
    painter->setPen(d_ptr->role == Role::Filled ? QColor(Qt::white) : accent);
    painter->drawText(QRectF(rect()), Qt::AlignCenter, d_ptr->text);
    painter->restore();
}

void CupertinoButton::keyPressEvent(QKeyEvent *event)
{
    if (!isEnabled()) {
        QxControls::QxControl::keyPressEvent(event);
        return;
    }
    // Enter answers on the press the way a button does; Space answers on the
    // release, so holding it down does not press the button over and over.
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

void CupertinoButton::keyReleaseEvent(QKeyEvent *event)
{
    if (isEnabled() && event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        d_ptr->press();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role CupertinoButton::accessibleRole() const
{
    return QAccessible::PushButton;
}

QX_CUPERTINO_END_NAMESPACE
