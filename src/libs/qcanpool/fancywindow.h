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
#ifndef FANCYWINDOW_H
#define FANCYWINDOW_H

#include <QMainWindow>
#include "qcanpool_global.h"

class FancyBar;
class FancyWindowPrivate;
class QCANPOOL_SHARED_EXPORT FancyWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FancyWindow(QWidget *parent = 0);
    ~FancyWindow();

    FancyBar* fancyBar() const;

signals:
    void resizable(bool resizable);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    FancyWindowPrivate *d;
};

#endif // FANCYWINDOW_H
