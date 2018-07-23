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
#include "fancynavbutton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QAction>

FancyNavButton::FancyNavButton(QWidget *parent)
    : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextOnly);

    setAutoRaise(false);
    setArrowType(Qt::NoArrow);
    setFocusPolicy(Qt::NoFocus);

    setIconSize(QSize(16,16));

    m_isMouseOver = false;
    m_isMousePress = false;
    m_type = Button;

    m_hoverColor = QColor(255, 255, 255, 50);
    m_pressColor = QColor(0, 0, 0, 100);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

//    QPalette palette;
//    palette.setColor(QPalette::WindowText,Qt::white);
//    this->setPalette(palette);
    setStyleSheet("QToolButton{background-color: transparent;border:none;color: white;}");
}

void FancyNavButton::select(bool selected)
{
    m_isMousePress = selected;
    update();
}

void FancyNavButton::setTextColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;border:none;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void FancyNavButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!isEnabled())
        return;
    m_isMouseOver = true;
}

void FancyNavButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isMouseOver = false;
}

void FancyNavButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        if(m_type == Action){
            m_isMousePress = true;
            update();
        }else{
            if(this->isCheckable()){
                if(this->isChecked()){
                    m_isMousePress = false;
                }else{
                    m_isMousePress = true;
                }
                this->setChecked(m_isMousePress);
                update();
            }else{
                m_isMousePress = true;
                emit clicked();
            }
        }
    }
    // added on 2018-4-30 17:17:32, cancel the sink
//    QToolButton::mousePressEvent(event);
}

void FancyNavButton::mouseReleaseEvent(QMouseEvent *event)
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

void FancyNavButton::paintEvent(QPaintEvent *event)
{
    if(m_isMousePress){
        painterInfo(m_pressColor);
    }else if(m_isMouseOver){
        painterInfo(m_hoverColor);
    }

    QToolButton::paintEvent(event);
}

void FancyNavButton::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);

    painter.setBrush(color);
    painter.drawRect(rect());
}
