/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCOMMANDFACE_P_H
#define FLUENTCOMMANDFACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtCanpool API. It exists purely as an
// implementation detail of the command surfaces - the command bar and the two
// buttons that stand alone - and it may change from release to release without
// notice, or disappear.
//

#include "qxfluent_global.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>

QX_FLUENT_BEGIN_NAMESPACE

// The measures of a Fluent command, shared by the places one is drawn: the
// command bar gives its row this height, and the two buttons that stand alone
// give their whole face the same - because a command in a bar and a command on
// its own are the same thing in two places, and a reader who sees both
// compares them.
namespace CommandFace
{

inline constexpr qreal kHeight = 32.0;
inline constexpr qreal kIconSize = 16.0;
inline constexpr qreal kIconGap = 8.0;
inline constexpr qreal kChevronWidth = 20.0;
inline constexpr qreal kChevronGlyph = 8.0;
inline constexpr qreal kChevronStroke = 1.5;

// The line between the two halves of a split command: the promise is that
// there are two things there, so the seam is drawn. Inset from the ends, and
// thin enough to be a seam rather than a border.
inline constexpr qreal kSeamInset = 6.0;
inline constexpr qreal kSeamOpacity = 0.16;

// A command in force is washed in the accent, and a command nobody can use is
// the same drawing thinned: neither moves the geometry.
inline constexpr qreal kCheckedOpacity = 0.12;
inline constexpr qreal kDisabledOpacity = 0.4;

/*! The chevron that says a menu is here: a point turned down, drawn rather
 *  than typed, because a glyph in a font is a glyph the theme cannot colour
 *  and an assistive tool would read. */
inline void paintChevron(QPainter *painter, const QRectF &rect, const QColor &color)
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

}   // namespace CommandFace

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCOMMANDFACE_P_H
