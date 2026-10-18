/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemeicon.h"

#include <QAction>
#include <QCoreApplication>
#include <QEvent>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QPointer>

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

/*! What has to be remembered to paint one action's icon again. */
struct IconBinding {
    QPointer<QAction> action;
    QString filePath;
    QPalette::ColorRole role;
    QSize size;
};

class QxThemeIconsPrivate
{
public:
    QX_DECLARE_PUBLIC(QxThemeIcons)

    QList<IconBinding> bindings;
};

QxThemeIcons::QxThemeIcons(QObject *parent)
    : QObject(parent)
{
    QX_INIT_PRIVATE(QxThemeIcons)

    // The application object is the one place a palette change is announced
    // regardless of which widget tree is on screen, which is why the filter goes
    // there rather than on the actions: an action is not an event target at all.
    if (QCoreApplication::instance()) {
        QCoreApplication::instance()->installEventFilter(this);
    }
}

QxThemeIcons::~QxThemeIcons()
{
    if (QCoreApplication::instance()) {
        QCoreApplication::instance()->removeEventFilter(this);
    }
    QX_FINI_PRIVATE()
}

void QxThemeIcons::add(QAction *action, const QString &filePath, QPalette::ColorRole role, const QSize &size)
{
    Q_D(QxThemeIcons);
    if (!action) {
        return;
    }
    remove(action);

    IconBinding binding;
    binding.action = action;
    binding.filePath = filePath;
    binding.role = role;
    binding.size = size;
    d->bindings.append(binding);

    action->setIcon(themedIcon(filePath, role, size));
}

bool QxThemeIcons::remove(QAction *action)
{
    Q_D(QxThemeIcons);
    for (int i = 0; i < d->bindings.size(); ++i) {
        if (d->bindings.at(i).action == action) {
            d->bindings.removeAt(i);
            return true;
        }
    }
    return false;
}

int QxThemeIcons::count() const
{
    Q_D(const QxThemeIcons);
    int live = 0;
    for (const IconBinding &binding : d->bindings) {
        if (!binding.action.isNull()) {
            ++live;
        }
    }
    return live;
}

void QxThemeIcons::clear()
{
    Q_D(QxThemeIcons);
    d->bindings.clear();
}

void QxThemeIcons::refresh()
{
    Q_D(QxThemeIcons);
    // Walking backwards so that dropping a binding for an action that was
    // destroyed does not skip the one behind it.
    for (int i = d->bindings.size() - 1; i >= 0; --i) {
        const IconBinding binding = d->bindings.at(i);
        if (binding.action.isNull()) {
            d->bindings.removeAt(i);
            continue;
        }
        binding.action->setIcon(themedIcon(binding.filePath, binding.role, binding.size));
    }
}

bool QxThemeIcons::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == QCoreApplication::instance() && event->type() == QEvent::ApplicationPaletteChange) {
        refresh();
    }
    return QObject::eventFilter(watched, event);
}

QX_THEME_END_NAMESPACE
