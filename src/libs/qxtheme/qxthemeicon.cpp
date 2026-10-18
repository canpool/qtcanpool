/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemeicon.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QImage>
#include <QtGui/QPainter>

QX_THEME_BEGIN_NAMESPACE

namespace
{

/*!
 * Loads \a filePath and rasterises it at \a size.
 *
 * QIcon does the loading, so a vector asset is rendered at the size it will be
 * drawn at instead of being upscaled from one raster, and a bitmap asset is
 * scaled the way a style sheet naming the file directly would have scaled it.
 * An empty \a size asks for the asset's own size.
 */
QPixmap rasterise(const QString &filePath, const QSize &size)
{
    const QIcon source(filePath);
    if (source.isNull()) {
        return QPixmap();
    }

    QSize target = size;
    if (target.isEmpty()) {
        target = source.actualSize(QSize(1024, 1024));
    }
    if (target.isEmpty()) {
        return QPixmap();
    }
    return source.pixmap(target);
}

/*!
 * The colour \a role names in the palette in force, or an invalid colour while
 * there is no application - there is no palette without one, and the caller is
 * told so rather than being handed a colour nobody installed.
 */
QColor roleColor(QPalette::ColorRole role)
{
    if (!QGuiApplication::instance()) {
        return QColor();
    }
    return QGuiApplication::palette().color(role);
}

}   // namespace

QPixmap tintedPixmap(const QString &filePath, const QColor &color, const QSize &size)
{
    const QPixmap mask = rasterise(filePath, size);
    if (mask.isNull() || !color.isValid()) {
        return mask;
    }

    // SourceIn keeps the glyph's alpha and replaces what is under it, so a mask
    // drawn in any single colour comes out in \a color at exactly the same
    // coverage - the edges keep their antialiasing.
    QImage image = mask.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(image.rect(), color);
    painter.end();
    return QPixmap::fromImage(image);
}

QIcon tintedIcon(const QString &filePath, const QColor &color, const QSize &size)
{
    QIcon icon;
    // One rasterisation per device pixel ratio a screen is likely to ask for, so
    // a high dpi screen gets the asset's own sharpness instead of an upscaled
    // copy of the 1x raster.
    for (int factor : {1, 2, 3}) {
        const QPixmap pixmap = tintedPixmap(filePath, color, size.isEmpty() ? QSize() : size * factor);
        if (!pixmap.isNull()) {
            icon.addPixmap(pixmap);
        }
    }
    return icon;
}

QPixmap themedPixmap(const QString &filePath, QPalette::ColorRole role, const QSize &size)
{
    return tintedPixmap(filePath, roleColor(role), size);
}

QIcon themedIcon(const QString &filePath, QPalette::ColorRole role, const QSize &size)
{
    return tintedIcon(filePath, roleColor(role), size);
}

QX_THEME_END_NAMESPACE
