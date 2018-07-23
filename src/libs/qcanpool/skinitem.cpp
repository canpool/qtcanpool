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
#include "skinitem.h"
#include <QMouseEvent>
#include <QPainter>

#ifndef SHADOW_WIDTH
#define SHADOW_WIDTH    5
#endif
#define BORDER_COLOR    QColor(255, 102, 0)

class SkinItemPrivate
{
public:
    SkinItemPrivate();

    QString m_pixmapName;
    QPixmap m_fitPixmap;

    bool m_bMousePress;
    bool m_bMouseEnter;

    bool m_isSelected;
};

SkinItemPrivate::SkinItemPrivate()
{
    m_bMousePress = false;
    m_bMouseEnter = false;
    m_isSelected = false;
}

SkinItem::SkinItem(QWidget *parent)
    : QWidget(parent),d(new SkinItemPrivate())
{

}

SkinItem::~SkinItem()
{
    delete d;
}

void SkinItem::setPixmapName(const QString &name)
{
    if(d->m_pixmapName == name) return;
    d->m_pixmapName = name;
    QPixmap pixmap(name);
    d->m_fitPixmap = pixmap.scaled(300, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QString SkinItem::pixmapName() const
{
    return d->m_pixmapName;
}

void SkinItem::select(bool selected)
{
    d->m_isSelected  = selected;
    update();
}

void SkinItem::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        d->m_bMousePress = true;
    }
}

void SkinItem::mouseReleaseEvent(QMouseEvent *event)
{
    if(d->m_bMousePress && (this->rect()).contains(event->pos()))
    {
        emit clicked();
    }
    d->m_bMousePress = false;
}

void SkinItem::enterEvent(QEvent *)
{
    d->m_bMouseEnter = true;
    update();
}

void SkinItem::leaveEvent(QEvent *)
{
    d->m_bMouseEnter = false;
    update();
}

void SkinItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawPixmap(QRect(SHADOW_WIDTH,SHADOW_WIDTH,
                             this->width()-2*SHADOW_WIDTH,
                             this->height()-2*SHADOW_WIDTH),
                       d->m_fitPixmap);
    painter.restore();
    painter.save();
    if(d->m_bMouseEnter || d->m_isSelected)
    {
        QPen pen(BORDER_COLOR);
        pen.setWidth(5);
        painter.setPen(pen);
        painter.drawRoundRect(0,0,this->width()-1, this->height()-1, 5, 5);

        if(d->m_isSelected)
        {
            QString state = QString(":/skin/state_ok");
            QPixmap pixmap(state);
            painter.drawPixmap(QRect(this->width() - pixmap.width() - 10,
                                     this->height() - pixmap.height() - 10,
                                     pixmap.width(),
                                     pixmap.height()),
                               pixmap);
        }
    }
    painter.restore();
}


