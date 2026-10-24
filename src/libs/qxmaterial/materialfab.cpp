/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialfab.h"

#include "materialripple.h"
#include "materialtokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtGui/QPainter>
#include <QtMath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's FAB is fifty-six across, and its icon is twenty-four of that.
constexpr qreal kSize = 56.0;
constexpr qreal kIconSize = 24.0;
// The step a control takes down when nobody can use it - the same .38 the rest
// of the family keeps.
constexpr qreal kDisabledOpacity = 0.38;
}   // namespace

class MaterialFABPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialFAB)

    QIcon icon;
    QString text;
    MaterialRipple *ripple = Q_NULLPTR;

    /*! The lift it is given: the highest surface on the screen but one, and it
     *  goes up while the pointer is on it. */
    MaterialElevation::Level level() const
    {
        return q_ptr->isActive() ? MaterialElevation::Level::Level4 : MaterialElevation::Level::Level3;
    }
    /*! The room it keeps for the shadow it casts - for the lift it reaches
     *  while the pointer is on it, so that going up does not clip it. */
    static int shadowMargin()
    {
        const QxTheme::QxShadow shadow = MaterialElevation::shadow(MaterialElevation::Level::Level4);
        return int(qCeil(shadow.blurRadius)) + 1 + int(qCeil(shadow.yOffset));
    }
};

MaterialFAB::MaterialFAB(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialFABPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    const int margin = MaterialFABPrivate::shadowMargin();
    setContentsMargins(margin, margin, margin, margin);
    // The ripple is what the surface does about being pressed, and this surface
    // is the accent's - so it answers in the colour that reads on the accent.
    d_ptr->ripple = new MaterialRipple(this);
    d_ptr->ripple->setColor(palette().color(QPalette::HighlightedText));
}

MaterialFAB::~MaterialFAB(){QX_FINI_PRIVATE()}

QIcon MaterialFAB::icon() const
{
    return d_ptr->icon;
}

QString MaterialFAB::text() const
{
    return d_ptr->text;
}

QSize MaterialFAB::sizeHint() const
{
    const int side = int(kSize) + 2 * MaterialFABPrivate::shadowMargin();
    return QSize(side, side);
}

QSize MaterialFAB::minimumSizeHint() const
{
    return sizeHint();
}

void MaterialFAB::setIcon(const QIcon &icon)
{
    d_ptr->icon = icon;
    update();
}

void MaterialFAB::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    // What it says is its accessible name for as long as the application has
    // not given it a better one.
    if (accessibleName().isEmpty() || accessibleName() == d_ptr->text) {
        setAccessibleName(text);
    }
    d_ptr->text = text;
    update();
}

qreal MaterialFAB::cornerRadius() const
{
    // A FAB is round: the radius is a number no rectangle reaches.
    return MaterialShape::radius(MaterialShape::Radius::Circular);
}

QRectF MaterialFAB::chromeRect() const
{
    // The wash and the ring belong to the disc, not to the room it keeps for
    // its shadow.
    return QRectF(contentsRect());
}

QColor MaterialFAB::washColor() const
{
    // The wash is what the pointer does to the surface underneath it, and this
    // surface is the accent's - so the wash is the colour that reads on the
    // accent, not the colour that reads on the page.
    return palette().color(QPalette::HighlightedText);
}

void MaterialFAB::paintSurface(QPainter *painter)
{
    // A floating action nobody can press steps its disc and its ripple down.
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    const QRectF surface = QRectF(contentsRect());
    const qreal radius = cornerRadius();
    const QxTheme::QxShadow shadow = MaterialElevation::shadow(d_ptr->level());

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
        QxControls::QxShadowPainter::paint(painter, surface, radius, textColor(), shadow);
    }
    // The accent's own colour: what a FAB stands for is what the screen is
    // for, and that is what the accent is for.
    painter->setPen(Qt::NoPen);
    painter->setBrush(accentColor());
    painter->drawRoundedRect(surface, radius, radius);
    // Pressed: it answers from where the pointer landed, and that travels.
    d_ptr->ripple->paint(painter, surface, radius);
    painter->restore();
}

void MaterialFAB::paintContent(QPainter *painter)
{
    // And its glyph with them, so the whole button keeps one voice.
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    if (d_ptr->icon.isNull()) {
        return;
    }
    const QRectF surface = QRectF(contentsRect());
    const QRectF area(surface.center().x() - kIconSize / 2.0, surface.center().y() - kIconSize / 2.0, kIconSize,
                      kIconSize);
    // The icon is drawn as it was given: a FAB's ink is the application's
    // choice, and recolouring it here would be guessing at what it meant.
    d_ptr->icon.paint(painter, area.toRect());
}

void MaterialFAB::keyPressEvent(QKeyEvent *event)
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

void MaterialFAB::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialFAB::accessibleRole() const
{
    return QAccessible::PushButton;
}

QX_MATERIAL_END_NAMESPACE
