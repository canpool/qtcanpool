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
#ifndef MINICENTRALAREA_H
#define MINICENTRALAREA_H

#include <QScrollArea>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT MiniCentralArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit MiniCentralArea(QWidget *parent = nullptr);
};

QCANPOOL_END_NAMESPACE

#endif // MINICENTRALAREA_H
