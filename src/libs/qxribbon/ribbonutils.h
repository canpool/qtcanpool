/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QColor>
#include <QIcon>
#include <QPixmap>

namespace QxRibbon
{

/* color */
QX_RIBBON_EXPORT QIcon colorIcon(const QColor &color, const QSize &size);
QX_RIBBON_EXPORT QPixmap colorPixmap(const QColor &color, const QSize &size);

/* mask */
QX_RIBBON_EXPORT QPixmap tintedPixmap(const QString &path, const QColor &color, const QSize &size = QSize());
QX_RIBBON_EXPORT QIcon tintedIcon(const QString &path, const QColor &color, const QSize &size = QSize());

}   // namespace QxRibbon
