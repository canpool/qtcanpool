/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXSTATELAYER_H
#define QXSTATELAYER_H

#include "qxcontrols_global.h"

#include <QtGui/QColor>
#include <QtCore/qglobal.h>

class QPainter;
class QRectF;

QX_CONTROLS_BEGIN_NAMESPACE

/*!
 * The state layer: the wash a control draws over itself to say where the
 * pointer is, where the focus sits, what is being pressed.
 *
 * Both design languages answer this the same way - one flat colour over the
 * control's own fill, the opacity carrying the state - so the answer lives
 * here rather than in each of them. What belongs to a family is *which*
 * colour it washes with and whether it agrees with the default opacities;
 * how a wash is composed and painted is mechanism, and mechanism is here.
 *
 * The opacities are fractions of the layer colour: 0.08 over white puts
 * eight percent of the layer on the page. They are defaults, not law - a
 * control passes what its family's tokens say.
 */
namespace QxStateLayer
{

/*! The resting wash barely there at all: the pointer has arrived. */
inline constexpr qreal hoverOpacity = 0.08;
/*! The keyboard is here. Stronger than hover: focus must survive next to one. */
inline constexpr qreal focusOpacity = 0.12;
/*! The control is being pushed. Stronger than hover: a press is a commitment. */
inline constexpr qreal pressedOpacity = 0.12;
/*! The content is being carried. The strongest wash a control wears. */
inline constexpr qreal draggedOpacity = 0.16;

/*!
 * \a opacity cut into [0, 1] - a token that overreaches clamps instead of
 * painting a state the design never drew.
 */
qreal QX_CONTROLS_EXPORT clampedOpacity(qreal opacity);

/*!
 * The colour \a layer at \a opacity over \a base - the answer compositing
 * would give for an opaque underlay, as a value. For reading, and for a test
 * that wants to predict a pixel; painting goes through paint().
 *
 * An invalid \a layer is no layer at all and answers \a base unchanged; an
 * invalid \a base is taken as transparent black.
 */
QColor QX_CONTROLS_EXPORT layered(const QColor &base, const QColor &layer, qreal opacity);

/*!
 * Draws the state wash onto \a painter over \a rect: the \a layer colour at
 * \a opacity, composited over whatever the painter already has. The painter's
 * state - hints, pen, brush - is the caller's; this only fills.
 *
 * An invalid \a layer or a clamped opacity of zero draws nothing, which is
 * the resting state and not an error.
 */
void QX_CONTROLS_EXPORT paint(QPainter *painter, const QRectF &rect, const QColor &layer, qreal opacity);

}   // namespace QxStateLayer

QX_CONTROLS_END_NAMESPACE

#endif   // QXSTATELAYER_H
