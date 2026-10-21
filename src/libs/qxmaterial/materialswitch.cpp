/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialswitch.h"

#include "materialtokens.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's switch measures: a 52x32 track with a 16 handle that keeps 8
// between itself and either end, so it has 20 to cross.
constexpr qreal kTrackWidth = 52.0;
constexpr qreal kTrackHeight = 32.0;
constexpr qreal kThumbSize = 16.0;
constexpr qreal kThumbInset = 8.0;
constexpr qreal kTextGap = 12.0;
constexpr qreal kOutlineWidth = 2.0;

// An unselected track is a surface seen through an outline, and a disabled
// control is the same drawing thinned - the geometry does not move.
constexpr qreal kOutlineOpacity = 0.30;
constexpr qreal kDisabledOpacity = 0.38;

QRectF trackRect(const QWidget *widget)
{
    // The widget may be taller than its track - a layout is free to give it
    // more room - so the track sits in the middle of what it was given.
    return QRectF(0.0, (widget->height() - kTrackHeight) / 2.0, kTrackWidth, kTrackHeight);
}
}   // namespace

class MaterialSwitchPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSwitch)

    bool on = false;
    /*! Where the handle is, 0 at the left end and 1 at the right. */
    qreal position = 0.0;
    QString text;
    QVariantAnimation animation;
};

MaterialSwitch::MaterialSwitch(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialSwitchPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->position = value.toReal();
        update();
    });

    // A click is what the base makes of a press and its release; the keyboard
    // is handled here.
    connect(this, &QxControls::QxControl::clicked, this, &MaterialSwitch::toggle);
}

MaterialSwitch::~MaterialSwitch()
{
    QX_FINI_PRIVATE()
}

bool MaterialSwitch::isOn() const
{
    return d_ptr->on;
}

void MaterialSwitch::setOn(bool on)
{
    if (d_ptr->on == on) {
        return;
    }
    d_ptr->on = on;

    // The handle crosses on a Material duration, which is a tier: the theme's
    // scale and the reduce-motion switch reach it without the switch asking
    // about either.
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Short);
    const qreal target = on ? 1.0 : 0.0;
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        d_ptr->position = target;
        update();
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->position);
        d_ptr->animation.setEndValue(target);
        d_ptr->animation.start();
    }
    Q_EMIT toggled(on);
}

void MaterialSwitch::toggle()
{
    setOn(!d_ptr->on);
}

QString MaterialSwitch::text() const
{
    return d_ptr->text;
}

void MaterialSwitch::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // The sentence is the accessible name for as long as the application has
    // not given the control a better one - which it does by simply setting it.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    updateGeometry();
    update();
}

QSize MaterialSwitch::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : int(kTextGap + metrics.horizontalAdvance(d_ptr->text));
    return QSize(int(kTrackWidth) + textWidth, int(kTrackHeight));
}

QSize MaterialSwitch::minimumSizeHint() const
{
    return QSize(int(kTrackWidth), int(kTrackHeight));
}

qreal MaterialSwitch::cornerRadius() const
{
    // A Material switch's track is a pill: the radius is half its height.
    return kTrackHeight / 2.0;
}

QRectF MaterialSwitch::chromeRect() const
{
    // The wash and the ring belong to the track, not to the row: a hover that
    // tinted the sentence would be dimming the label.
    return trackRect(this);
}

void MaterialSwitch::paintSurface(QPainter *painter)
{
    const QRectF track = trackRect(this);
    const qreal radius = kTrackHeight / 2.0;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    painter->setPen(Qt::NoPen);
    if (d_ptr->on) {
        painter->setBrush(accentColor());
        painter->drawRoundedRect(track, radius, radius);
    } else {
        // Off is a surface seen through an outline - and the outline is what
        // makes an unselected switch findable on a surface of the same colour.
        painter->setBrush(palette().color(QPalette::AlternateBase));
        painter->drawRoundedRect(track, radius, radius);
        QColor outline = textColor();
        outline.setAlphaF(kOutlineOpacity);
        painter->setPen(QPen(outline, kOutlineWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(track, radius, radius);
    }

    const qreal travel = kTrackWidth - kThumbSize - 2.0 * kThumbInset;
    const QRectF thumb(kThumbInset + d_ptr->position * travel, track.y() + kThumbInset, kThumbSize, kThumbSize);
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->on ? palette().color(QPalette::HighlightedText) : textColor());
    painter->drawEllipse(thumb);

    if (d_ptr->on) {
        // The mark is the difference Material makes: the state can be read
        // from the handle alone, without comparing it to the far end.
        QPainterPath tick;
        tick.moveTo(thumb.left() + 4.0, thumb.center().y());
        tick.lineTo(thumb.center().x() - 1.0, thumb.bottom() - 5.0);
        tick.lineTo(thumb.right() - 4.0, thumb.top() + 5.0);
        painter->setPen(QPen(accentColor(), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(tick);
    }
    painter->restore();
}

void MaterialSwitch::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    const QRectF label(kTrackWidth + kTextGap, 0.0, width() - kTrackWidth - kTextGap, height());
    painter->save();
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    painter->setPen(textColor());
    painter->drawText(label, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->text);
    painter->restore();
}

void MaterialSwitch::keyPressEvent(QKeyEvent *event)
{
    // Enter answers on the press the way a button does; Space answers on the
    // release, so holding it down does not flip the switch over and over.
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        toggle();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialSwitch::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        toggle();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialSwitch::accessibleRole() const
{
    return QAccessible::CheckBox;
}

void MaterialSwitch::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->on;
}

QX_MATERIAL_END_NAMESPACE
