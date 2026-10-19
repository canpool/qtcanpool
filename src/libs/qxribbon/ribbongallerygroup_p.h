/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbongallerygroup.h"

#include <QFont>
#include <QPalette>
#include <QRect>
#include <QSize>

class QAction;
class QPainter;

QX_RIBBON_BEGIN_NAMESPACE

/*
 * RibbonGalleryPaintSpec - everything painting a group's grid needs to know.
 *
 * The group and the gallery's popup both paint the same items, and they have to
 * agree on where every one of them is; one description of the grid is what makes
 * that true. It carries no widget and does no lookups, so the popup can paint a
 * group it does not own.
 */
struct RibbonGalleryPaintSpec {
    QList<QAction *> actions;
    QSize grid = QSize(64, 64);
    int spacing = 1;
    int columns = 1;
    RibbonGalleryGroup::GalleryGroupStyle style = RibbonGalleryGroup::IconWithText;
    QPalette palette;
    QFont font;
    int hoverIndex = -1;
    int selectedIndex = -1;
    int pressedIndex = -1;
    int firstRow = 0;      ///< the row painted at the top of the area
    int visibleRows = 0;   ///< how many rows to paint; 0 paints every row
};

/* Geometry shared by the group and the popup */
int galleryColumnCount(const QRect &area, const RibbonGalleryPaintSpec &spec);
int galleryRowCount(const RibbonGalleryPaintSpec &spec);
QRect galleryItemRect(const QRect &area, const RibbonGalleryPaintSpec &spec, int index);
int galleryItemAt(const QRect &area, const RibbonGalleryPaintSpec &spec, const QPoint &pos);

/* Painting shared by the group and the popup */
void galleryPaintGrid(QPainter *painter, const QRect &area, const RibbonGalleryPaintSpec &spec);

QX_RIBBON_END_NAMESPACE
