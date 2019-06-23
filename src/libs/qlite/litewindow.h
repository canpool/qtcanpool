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
#ifndef LITEWINDOW_H
#define LITEWINDOW_H

#include <QMainWindow>
#include "qlite_global.h"

namespace QLite {

class LiteBar;
class LiteWindowPrivate;

class QLITE_SHARED_EXPORT LiteWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LiteWindow(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~LiteWindow();

    LiteBar *liteBar() const;

    QMenuBar *menuBar() const;
    void setMenuBar(QMenuBar *menuBar);

    QStatusBar *statusBar() const;
    void setStatusBar(QStatusBar *statusbar);

    void setFixedSize(const QSize &);
    void setFixedSize(int w, int h);
    void setFixedWidth(int w);
    void setFixedHeight(int h);

    void setWindowFlags(Qt::WindowFlags type);

private:
    LiteWindowPrivate *d;
};

} // namespace QLite

#endif // LITEWINDOW_H
