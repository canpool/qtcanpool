/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
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
#ifndef LITEWIDGET_H
#define LITEWIDGET_H

#include <QWidget>
#include "qlite_global.h"

namespace QLite {

class QLITE_SHARED_EXPORT LiteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LiteWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *e);
};

} // namespace QLite

#endif // LITEWIDGET_H
