/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "icons.h"

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>

namespace
{

// The pane draws its icons at sixteen pixels, and a screen may hold two of
// those in one: the glyph is rendered at double and told that it is one, so a
// high-density screen gets the strokes' edges rather than a magnified copy.
constexpr int kPixels = 32;
constexpr qreal kDensity = 2.0;

QString pathOf(const QString &name)
{
    return QStringLiteral(":/icon/res/icons/%1.svg").arg(name);
}

}   // namespace

QIcon Icons::named(const QString &name, const QColor &ink)
{
    // The ink is part of what is asked for, not a detail of the drawing: the
    // same name on the other theme is another picture, and a cache keyed by
    // the name alone would hand a dark page the colour of the light one.
    static QHash<QString, QIcon> drawn;
    const QString key = name + QLatin1Char('#') + ink.name(QColor::HexArgb);
    const auto found = drawn.constFind(key);
    if (found != drawn.constEnd()) {
        return found.value();
    }

    QImage image(kPixels, kPixels, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    {
        QPainter painter(&image);
        // The file is asked to draw itself rather than read as a picture: svg
        // has no size of its own that matters here, and what is wanted is the
        // glyph filling the square at whatever density it was asked for.
        QIcon(pathOf(name)).paint(&painter, QRect(0, 0, kPixels, kPixels), Qt::AlignCenter);
        // What the glyph was drawn in is thrown away and only its shape kept:
        // filling where it was drawn with the ink is what makes one file serve
        // both a light page and a dark one.
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(0, 0, kPixels, kPixels, ink);
    }

    QPixmap pixmap = QPixmap::fromImage(image);
    pixmap.setDevicePixelRatio(kDensity);
    const QIcon icon(pixmap);
    drawn.insert(key, icon);
    return icon;
}
