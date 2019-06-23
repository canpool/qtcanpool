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
#ifndef LITEBOARD_H
#define LITEBOARD_H

#include <QDialog>
#include "qlite_global.h"

namespace QLite {

class LiteBoardPrivate;

class QLITE_SHARED_EXPORT LiteBoard : public QDialog
{
    Q_OBJECT
public:
    explicit LiteBoard(QWidget *parent = nullptr);
    ~LiteBoard();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    LiteBoardPrivate *d;
};

} // namespace QLite

#endif // LITEBOARD_H
