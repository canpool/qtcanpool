/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonybutton.h"

#include "harmonytokens.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_HARMONY_BEGIN_NAMESPACE

namespace
{
// The capsule: forty tall with twenty either side of what it says. A control
// here is a target before it is a label, which is why it is taller than the
// other family's button and why the word never comes near the round end.
constexpr qreal kHeight = 40.0;
constexpr qreal kPadding = 20.0;

// The second level's fill: the family's grey thinned into the page. The blue is
// kept for the word, so a row of two buttons has one action in it and not two.
constexpr qreal kNeutralFill = 0.12;

// A disabled control keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;

// Which face of the semantic colours the control is drawn on.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class HarmonyButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(HarmonyButton)

    QString text;
    HarmonyButton::Kind kind = HarmonyButton::Kind::Primary;

    /*! What a press from the keyboard answers with. The pointer's press is the
     *  base's, and it answers the same signal. */
    void press()
    {
        Q_EMIT q_ptr->clicked();
    }
};

HarmonyButton::HarmonyButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new HarmonyButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
}

HarmonyButton::HarmonyButton(const QString &text, QWidget *parent)
    : HarmonyButton(parent)
{
    setText(text);
}

HarmonyButton::~HarmonyButton(){QX_FINI_PRIVATE()}

QString HarmonyButton::text() const
{
    return d_ptr->text;
}

HarmonyButton::Kind HarmonyButton::kind() const
{
    return d_ptr->kind;
}

QSize HarmonyButton::sizeHint() const
{
    const QFontMetrics metrics(HarmonyType::font(HarmonyType::Ramp::Body));
    const qreal width = 2.0 * kPadding + metrics.horizontalAdvance(d_ptr->text);
    return QSize(qRound(width), int(kHeight));
}

QSize HarmonyButton::minimumSizeHint() const
{
    return QSize(int(kHeight), int(kHeight));
}

void HarmonyButton::setText(const QString &text)
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

void HarmonyButton::setKind(Kind kind)
{
    if (d_ptr->kind == kind) {
        return;
    }
    d_ptr->kind = kind;
    update();
    Q_EMIT kindChanged(kind);
}

qreal HarmonyButton::cornerRadius() const
{
    // A capsule is a rectangle whose corner is half its height, and that number
    // has to be given rather than implied: the family's fully round step is a
    // radius no rectangle reaches, and a painter clamps such a radius per axis -
    // on a button this wide the ends then become half an ellipse, which reads as
    // a pointed end rather than as a capsule.
    return qMin(HarmonyShape::radius(HarmonyShape::Radius::Circular), height() / 2.0);
}

void HarmonyButton::paintSurface(QPainter *painter)
{
    const QRectF capsule = QRectF(rect());
    const qreal radius = cornerRadius();
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    switch (d_ptr->kind) {
    case Kind::Primary:
        painter->setBrush(HarmonyColor::color(HarmonyColor::Role::Accent, dark));
        painter->drawRoundedRect(capsule, radius, radius);
        break;
    case Kind::Secondary: {
        QColor fill = HarmonyColor::color(HarmonyColor::Role::Gray, dark);
        fill.setAlphaF(kNeutralFill);
        painter->setBrush(fill);
        painter->drawRoundedRect(capsule, radius, radius);
        break;
    }
    case Kind::Text:
        // Nothing to draw: the word is the whole control.
        break;
    }
    painter->restore();
}

void HarmonyButton::paintContent(QPainter *painter)
{
    const QColor accent = HarmonyColor::color(HarmonyColor::Role::Accent, onDarkFace(this));

    painter->save();
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    painter->setFont(HarmonyType::font(HarmonyType::Ramp::Body));
    // The word is the level: reversed out of the blue where the blue is the
    // capsule, in the blue everywhere else.
    painter->setPen(d_ptr->kind == Kind::Primary ? QColor(Qt::white) : accent);
    painter->drawText(QRectF(rect()), Qt::AlignCenter, d_ptr->text);
    painter->restore();
}

void HarmonyButton::keyPressEvent(QKeyEvent *event)
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

void HarmonyButton::keyReleaseEvent(QKeyEvent *event)
{
    if (isEnabled() && event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        d_ptr->press();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role HarmonyButton::accessibleRole() const
{
    return QAccessible::PushButton;
}

QX_HARMONY_END_NAMESPACE
