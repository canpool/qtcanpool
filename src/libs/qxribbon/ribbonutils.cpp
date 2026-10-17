/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonutils.h"

#include <QImage>
#include <QPainter>

namespace QxRibbon
{

namespace
{

/*!
 * Loads \a path and rasterises it at \a size.
 *
 * QIcon does the loading, so an SVG asset is rendered at exactly the size it will
 * be drawn at instead of being upscaled from a raster, and a bitmap asset is
 * scaled the way the style sheet used to scale it when it named the file
 * directly. An empty \a size asks for the asset's own size.
 */
QPixmap maskedPixmap(const QString &path, const QSize &size)
{
    QIcon source(path);
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

}   // namespace

QIcon colorIcon(const QColor &color, const QSize &size)
{
    return QIcon(colorPixmap(color, size));
}

QPixmap colorPixmap(const QColor &color, const QSize &size)
{
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    QRect pr(1, 1, res.height() - 2, res.width() - 2);
    if (color.isValid()) {
        painter.fillRect(pr, color);
    } else {
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(pr);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(1, size.height()), QPoint(size.width() - 1, 1));
    }
    return res;
}

QPixmap tintedPixmap(const QString &path, const QColor &color, const QSize &size)
{
    const QPixmap mask = maskedPixmap(path, size);
    if (mask.isNull()) {
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

QIcon tintedIcon(const QString &path, const QColor &color, const QSize &size)
{
    QIcon res;
    // Rasterise the glyph once per device pixel ratio QIcon may ask for, so a
    // high dpi screen gets the asset's own sharpness instead of an upscaled copy.
    for (int factor : {1, 2, 3}) {
        const QPixmap pixmap = tintedPixmap(path, color, size.isEmpty() ? QSize() : size * factor);
        if (!pixmap.isNull()) {
            res.addPixmap(pixmap);
        }
    }
    return res;
}

}   // namespace QxRibbon
