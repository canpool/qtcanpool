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
#include "fancytab.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QAction>

static QColor s_hoverColor = QColor(255, 255, 255, 50);
static QColor s_pressColor = QColor(0, 0, 0, 100);
static QColor s_textColor = QColor(255, 255, 255);
static QColor s_selectedTextColor = QColor(255,255,255);

FancyTab::FancyTab(QWidget *parent)
    : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    setIconSize(QSize(24,24));

    setAutoRaise(true);

    m_bMouseHover = false;
    m_bMousePress = false;
    m_type = Mode;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

//    QPalette palette;
//    palette.setColor(QPalette::WindowText,Qt::white);
//    this->setPalette(palette);
    setStyleSheet("QToolButton{background-color: transparent;color: white;}");
}

FancyTab::~FancyTab()
{

}

void FancyTab::select(bool selected)
{
    m_bMousePress = selected;
    update();
}

void FancyTab::setHoverColor(const QColor &color)
{
    s_hoverColor = color;
}

QColor FancyTab::hoverColor()
{
    return s_hoverColor;
}

void FancyTab::setPressColor(const QColor &color)
{
    s_pressColor = color;
}

QColor FancyTab::pressColor()
{
    return s_pressColor;
}

void FancyTab::setTextColor(const QColor &color)
{
    s_textColor = color;
}

void FancyTab::setSelectedTextColor(const QColor &color)
{
    s_selectedTextColor = color;
}

void FancyTab::setColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void FancyTab::setHasMenu(bool has)
{
    m_hasMenu = has;
}

void FancyTab::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!isEnabled())
        return;
    m_bMouseHover = true;
}

void FancyTab::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = false;
}

void FancyTab::mousePressEvent(QMouseEvent *event)
{
    if(!isEnabled())
        return;
    if(event->button() == Qt::LeftButton)
    {
        if(m_type == Action){
            m_bMousePress = true; // add for action
            update();
        }
        else
        {
            if(m_hasMenu){
                m_bMousePress = true; // add for action
                update();
                emit menuTriggered(event);
            }else{
                emit clicked();
            }
        }
    }
}

void FancyTab::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(m_type == Action){
            m_bMousePress = false; // add for action
            update();
            if(this->rect().contains(event->pos())){ // don't emit when move out
                emit clicked();
            }
        }
    }
}

void FancyTab::paintEvent(QPaintEvent *event)
{
    if(m_bMousePress){
        painterInfo(s_pressColor);
        setColor(s_selectedTextColor);
    }else if(m_bMouseHover){
        painterInfo(s_hoverColor);
        setColor(s_selectedTextColor);
    }else{
        setColor(s_textColor);
    }
    if(m_hasMenu){
        if(m_bMousePress || m_bMouseHover){
            painterArrow(s_selectedTextColor);
        }else{
            painterArrow(s_textColor);
        }
    }

    QToolButton::paintEvent(event);
}

void FancyTab::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);

    painter.setBrush(color);
    painter.drawRect(rect());
}

void FancyTab::painterArrow(QColor &color)
{
    QPainter painter(this);
    painter.setBrush(QBrush(color));
    QPolygonF polygonf;
    QPointF pt;

    QPoint pos = rect().center();
    pos.setX(pos.x() + rect().width()/2);

    pt = QPointF(pos.x()-6,pos.y());
    polygonf.append(pt);
    pt = QPointF(pos.x()-10,pos.y()+4);
    polygonf.append(pt);
    pt = QPointF(pos.x()-10,pos.y()-4);
    polygonf.append(pt);

    painter.drawPolygon(polygonf);
}
