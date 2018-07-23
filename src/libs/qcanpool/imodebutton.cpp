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
#include "imodebutton.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QAction>

/********************************************************************************
 * class :
 *      IModeButton
 *
 *******************************************************************************/
IModeButton::IModeButton(const QIcon &icon, QWidget *parent)
    :QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    setIcon(icon);
    setIconSize(QSize(22,22));

    setAutoRaise(true);

    m_isMouseOver = false;
    m_isMousePress = false;
    m_type = Mode;

    m_hoverColor = QColor(255, 255, 255, 50);
    m_pressColor = QColor(0, 0, 0, 100);
    m_textColor = QColor(255,255,255);
    m_selectedTextColor = QColor(255,255,255);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

//    QPalette palette;
//    palette.setColor(QPalette::WindowText,Qt::white);
//    this->setPalette(palette);
    setStyleSheet("QToolButton{background-color: transparent;color: white;}");
}

IModeButton::~IModeButton()
{

}


void IModeButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!isEnabled())
        return;
    m_isMouseOver = true;
}

void IModeButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isMouseOver = false;
}

void IModeButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
//        emit clicked();
        if(m_type == Action){
            m_isMousePress = true; // add for action
            update();
        }else{
            emit clicked();
        }
    }
}

void IModeButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(m_type == Action){
            m_isMousePress = false; // add for action
            update();
            if(this->rect().contains(event->pos())){ // don't emit when move out
                emit clicked();
            }
        }
    }
}


void IModeButton::select(bool selected)
{
    this->m_isMousePress = selected;
    update();
}

void IModeButton::setTextColor(const QColor &color)
{
    m_textColor = color;
}

void IModeButton::setSelectedTextColor(const QColor &color)
{
    m_selectedTextColor = color;
}

void IModeButton::setColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void IModeButton::paintEvent(QPaintEvent *event)
{
    if(m_isMousePress){
        painterInfo(m_pressColor);
        setColor(m_selectedTextColor);
    }else if(m_isMouseOver){
        painterInfo(m_hoverColor);
        setColor(m_selectedTextColor);
    }else{
        setColor(m_textColor);
    }

    QToolButton::paintEvent(event);
}

//void IModeButton::painterInfo(int top_color, int middle_color, int bottom_color)
//{
//    QPainter painter(this);
//    QPen pen(Qt::NoBrush, 1);
//    painter.setPen(pen);

//    QLinearGradient linear(rect().topLeft(), rect().bottomLeft());
//    linear.setColorAt(0, QColor(230, 230, 230, top_color));
//    linear.setColorAt(0.5, QColor(230, 230, 230, middle_color));
//    linear.setColorAt(1, QColor(230, 230, 230, bottom_color));
//    painter.setBrush(linear);
//    painter.drawRect(rect());
//}

void IModeButton::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);

    painter.setBrush(color);
    painter.drawRect(rect());
}
