/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QIcon>

namespace QxRibbon {

/* color */
QX_RIBBON_EXPORT QIcon colorIcon(const QColor &color, const QSize &size);
QX_RIBBON_EXPORT QPixmap colorPixmap(const QColor &color, const QSize &size);

} // namespace QxRibbon
