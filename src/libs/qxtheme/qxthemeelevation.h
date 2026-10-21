/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEMEELEVATION_H
#define QXTHEMEELEVATION_H

#include "qxtheme_global.h"

#include <QtGui/QColor>

QX_THEME_BEGIN_NAMESPACE

/*!
 * The elevation vocabulary: how far above the page a surface sits, spelled as
 * the shadow that lift produces.
 *
 * QSS cannot draw a shadow, and a QGraphicsDropShadowEffect is one effect per
 * widget - too costly to hand out and awkward with QWidget::grab(). So a shadow
 * is painted, and what this header fixes is the answer to "how dark and how
 * far" before anyone asks it: five tiers, from a surface that sits on the page
 * to one that floats over everything. The painting and the caching of the
 * result belong to the controls base library; these are the values it reads.
 *
 * The tiers are named by how much they lift, the way the corner tiers are named
 * by how much they round. \a blurRadius and \a yOffset are in device
 * independent pixels; \a alpha is 0-255 and multiplies the theme's shadow
 * colour, so a theme can draw its shadows softer or harder without moving a
 * geometry value.
 */
struct QxShadow {
    qreal blurRadius; /*!< How far the shadow spreads. */
    qreal yOffset;    /*!< How far below the surface it sits. */
    int alpha;        /*!< 0-255, applied to the shadow colour. */
};

/*! A surface that sits on the page and casts nothing. */
inline constexpr QxShadow shadowNone{0.0, 0.0, 0};
/*! A hint of lift: the state a hover promises. blur 4, y 1. */
inline constexpr QxShadow shadowSm{4.0, 1.0, 45};
/*! A card above the page: the resting state of a raised element. blur 12, y 3. */
inline constexpr QxShadow shadowMd{12.0, 3.0, 55};
/*! A flyout: menus, tooltips and the popups a control opens. blur 24, y 6. */
inline constexpr QxShadow shadowLg{24.0, 6.0, 60};
/*! A dialog: the one tier that dims what is under it. blur 48, y 12. */
inline constexpr QxShadow shadowXl{48.0, 12.0, 64};

/*! The tiers in lifting order, none first - a table, not a chain of ifs. */
inline constexpr QxShadow shadows[] = {shadowNone, shadowSm, shadowMd, shadowLg, shadowXl};

/*!
 * The shadow \a tier asks for, saturating at \c shadowXl - a caller that walks
 * a level scale does not have to clamp it first. A negative tier reads as none.
 */
inline QxShadow shadowFor(int tier)
{
    if (tier <= 0) {
        return shadowNone;
    }
    const int last = int(sizeof(shadows) / sizeof(shadows[0])) - 1;
    return shadows[tier < last ? tier : last];
}

/*! The colour a shadow paints with, \a alpha over black - the one answer a
 *  theme that says nothing about shadows gets. */
inline QColor shadowColor(const QColor &base, int alpha)
{
    QColor c = base.isValid() ? base : QColor(Qt::black);
    c.setAlpha(alpha);
    return c;
}

// The tiers have to keep their order: a painter that blends between them and a
// caller that walks the table both read the order as meaning.
static_assert(shadowSm.blurRadius < shadowMd.blurRadius && shadowMd.blurRadius < shadowLg.blurRadius &&
                  shadowLg.blurRadius < shadowXl.blurRadius,
              "shadow blur must grow with the tier");
static_assert(shadowNone.alpha <= shadowSm.alpha && shadowSm.alpha <= shadowMd.alpha &&
                  shadowMd.alpha <= shadowLg.alpha && shadowLg.alpha <= shadowXl.alpha,
              "shadow alpha must grow with the tier");

QX_THEME_END_NAMESPACE

#endif   // QXTHEMEELEVATION_H
