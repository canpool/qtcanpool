/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#ifndef WINDOWGEOMETRY_P_H
#define WINDOWGEOMETRY_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

// The resize border geometry, split out of the backend that used to hold it.
//
// It used to be two file-static helpers inside windowcontext_qt.cpp, taking a
// QWindow* only to read width(), height() and visibility(). Only the first two
// are geometry, and they are the whole of the calculation, so the arithmetic
// lives here as plain functions over a QSize - no window, no platform, nothing
// to skip when there is no window manager. The backend keeps a thin wrapper that
// adds the visibility guard and (on macOS) the deliberate "no resize cursor"
// answer, so its behaviour is unchanged.

#include "qxwindow_global.h"
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QtGui>

QX_WINDOW_BEGIN_NAMESPACE

/*!
    The strip along each edge of the window, in logical pixels, in which a left
    press starts a resize and the cursor advertises it. The native backends ask
    the window manager for this value; this is the one the portable backend uses
    instead, and the value the geometry below is written against.
*/
inline constexpr int kResizeBorderThickness = 8;

/*!
    The edges of a \a windowSize window that a press at \a pos resizes from, empty
    in the middle. A corner reports two edges at once, and a window narrower than
    two borders can report opposite edges at the same time.

    Note the asymmetry, which is the original behaviour and is kept on purpose:
    the left and top strips are \c x < thickness while the right and bottom ones
    are \c x >= width - thickness, so the far strips include their inner boundary
    column and the near ones do not.
*/
inline Qt::Edges resizeEdgesFor(const QSize &windowSize, const QPoint &pos)
{
    Qt::Edges edges = {};
    if (pos.x() < kResizeBorderThickness) {
        edges |= Qt::LeftEdge;
    }
    if (pos.x() >= (windowSize.width() - kResizeBorderThickness)) {
        edges |= Qt::RightEdge;
    }
    if (pos.y() < kResizeBorderThickness) {
        edges |= Qt::TopEdge;
    }
    if (pos.y() >= (windowSize.height() - kResizeBorderThickness)) {
        edges |= Qt::BottomEdge;
    }
    return edges;
}

/*!
    The cursor that advertises the edges \a pos would resize from: a diagonal one
    at a corner, a straight one along a single edge, the arrow in the middle.

    Kept in step with resizeEdgesFor() by construction - the two use the same
    four comparisons - so a point gets a non-arrow cursor exactly when it also
    reports at least one edge. The diagonal cursors are not symmetric: the
    top-left and bottom-right corners get \c SizeFDiagCursor and the other two get
    \c SizeBDiagCursor.
*/
inline Qt::CursorShape resizeCursorFor(const QSize &windowSize, const QPoint &pos)
{
    const int x = pos.x();
    const int y = pos.y();
    const int w = windowSize.width();
    const int h = windowSize.height();
    if (((x < kResizeBorderThickness) && (y < kResizeBorderThickness)) ||
        ((x >= (w - kResizeBorderThickness)) && (y >= (h - kResizeBorderThickness)))) {
        return Qt::SizeFDiagCursor;
    }
    if (((x >= (w - kResizeBorderThickness)) && (y < kResizeBorderThickness)) ||
        ((x < kResizeBorderThickness) && (y >= (h - kResizeBorderThickness)))) {
        return Qt::SizeBDiagCursor;
    }
    if ((x < kResizeBorderThickness) || (x >= (w - kResizeBorderThickness))) {
        return Qt::SizeHorCursor;
    }
    if ((y < kResizeBorderThickness) || (y >= (h - kResizeBorderThickness))) {
        return Qt::SizeVerCursor;
    }
    return Qt::ArrowCursor;
}

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWGEOMETRY_P_H
