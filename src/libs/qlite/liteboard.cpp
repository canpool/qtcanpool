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
#include "liteboard.h"

#include <QMouseEvent>

using namespace QLite;

namespace QLite
{
    class LiteBoardPrivate
    {
    public:
        LiteBoardPrivate();

        QPoint m_movePoint;
        bool m_bPressed;
    };
}

LiteBoardPrivate::LiteBoardPrivate()
    : m_bPressed(false)
{
}

LiteBoard::LiteBoard(QWidget *parent)
    : QDialog(parent), d(new LiteBoardPrivate())
{
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::Dialog
                   | Qt::WindowSystemMenuHint
                   | Qt::WindowMinimizeButtonHint
                   );
}

LiteBoard::~LiteBoard()
{
    delete d;
}

void LiteBoard::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->m_bPressed = true;
    }
    d->m_movePoint = event->globalPos() - pos();
}

void LiteBoard::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (event->globalY() == 0) {
        this->move(this->frameGeometry().x(), 100);
    }
    d->m_bPressed = false;
}

void LiteBoard::mouseMoveEvent(QMouseEvent *event)
{
    if (d->m_bPressed) {
        QPoint movePos = event->globalPos();
        move(movePos - d->m_movePoint);
    }
}
