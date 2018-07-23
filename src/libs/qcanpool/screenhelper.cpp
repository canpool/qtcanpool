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
#include "screenhelper.h"

#include <QDesktopWidget>
#include <QDebug>

ScreenHelper::ScreenHelper()
{
    QDesktopWidget *desktop = QApplication::desktop();
    int screenCnt = desktop->screenCount();
    int primScreen = desktop->primaryScreen();

    m_screenInfoList.clear();
    for(int i = 0; i < screenCnt; i++)
    {
        ScreenInfo info;
        info.no = primScreen + i;
// 		info.rect = desktop->screenGeometry(info.no);
        info.rect = desktop->availableGeometry(info.no);
        m_screenInfoList.append(info);

//       qDebug()<<"screen no: "<<m_screenInfoList.at(i).no;
//       qDebug()<<"x:"<<m_screenInfoList.at(i).rect.x()<<" y:"<<m_screenInfoList.at(i).rect.y()
//             <<" width:"<<m_screenInfoList.at(i).rect.width()
//            <<" height:"<<m_screenInfoList.at(i).rect.height();
    }

}

int ScreenHelper::currentScreen(const int x)
{
    int width;
    for(int i = 0; i < m_screenInfoList.count(); i++)
    {
        width = m_screenInfoList.at(i).rect.x() + m_screenInfoList.at(i).rect.width();
        if(x > width){
            continue;
        }else{
            return m_screenInfoList.at(i).no;
        }
    }
    return m_screenInfoList.at(0).no;
}

QRect ScreenHelper::screenRect(const int current)
{
    if(current >= m_screenInfoList.count()){
        return m_screenInfoList.at(0).rect;
    }else{
        return m_screenInfoList.at(current).rect;
    }
}
