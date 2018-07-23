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
#ifndef SCREENHELPER_H
#define SCREENHELPER_H

#include <QApplication>
#include <QRect>

/* on screen display */
class ScreenHelper
{
public:
    ScreenHelper();

    typedef struct
    {
        int no;
        QRect rect;
    }ScreenInfo;

    int currentScreen(const int x);
    QRect screenRect(const int current);
private:
    QList<ScreenInfo> m_screenInfoList;
};

#endif // SCREENHELPER_H
