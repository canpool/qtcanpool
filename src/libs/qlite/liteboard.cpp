/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "liteboard.h"

#include <QMouseEvent>

QLITE_BEGIN_NAMESPACE

class LiteBoardPrivate
{
public:
    LiteBoardPrivate();

    QPoint m_movePoint;
    bool m_bPressed;
};

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

QLITE_END_NAMESPACE
