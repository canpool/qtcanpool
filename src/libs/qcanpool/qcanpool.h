/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef QCANPOOL_H
#define QCANPOOL_H

#include <QObject>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

// color
QCANPOOL_SHARED_EXPORT QColor argbToColor(const QString &argb);
QCANPOOL_SHARED_EXPORT QString colorToArgb(const QColor &color); /// ffaabbcc

// file
QCANPOOL_SHARED_EXPORT void showInExplorer(QString fileName);

// settings
QCANPOOL_SHARED_EXPORT void setSysSettings(const QString &organization, const QString &application);

QCANPOOL_END_NAMESPACE

#endif // QCANPOOL_H
