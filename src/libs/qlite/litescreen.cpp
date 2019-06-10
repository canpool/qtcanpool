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
#include "litescreen.h"

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QApplication>
#include <QDesktopWidget>
#else
#include <QGuiApplication>
#include <QScreen>
#endif

using namespace QLite;

LiteScreen::LiteScreen()
{
    m_screenRects.clear();

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDesktopWidget *desktop = QApplication::desktop();
    int screenCnt = desktop->screenCount();

    for (int i = 0; i < screenCnt; i++) {
        QRect rect = desktop->availableGeometry(i);
        m_screenRects.append(rect);
    }
#else
    QList<QScreen *> screenList = QGuiApplication::screens();
    foreach (QScreen *screen, screenList) {
        m_screenRects.append(screen->availableGeometry());
    }
#endif
}

int LiteScreen::currentScreen(const int x)
{
    int width;

    for (int i = 0; i < m_screenRects.count(); i++) {
        width = m_screenRects.at(i).x() + m_screenRects.at(i).width();

        if (x > width) {
            continue;
        }
        return i;
    }

    return 0;
}

QRect LiteScreen::screenRect(const int current)
{
    if (current >= m_screenRects.count()) {
        return m_screenRects.at(0);
    } else {
        return m_screenRects.at(current);
    }
}

QRect LiteScreen::normalRect()
{
    QRect geom;
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    geom = QApplication::desktop()->availableGeometry();
#else
    geom = QGuiApplication::primaryScreen()->availableGeometry();
#endif
    return QRect(geom.x() + 100, geom.y() + 100,
                 2 * geom.width() / 3, 2 * geom.height() / 3);
}
