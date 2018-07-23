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
#include "fancybutton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QAction>
#include "qcanpool.h"

FancyButton::FancyButton(QWidget *parent)
    : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonIconOnly);

//    setIconSize(QSize(18,18));

    setAutoRaise(true);

    m_bMouseHover = false;
    m_bMousePress = false;
    m_hasBorder = true;
    m_bRound = false;

    m_hoverColor = QColor(255,255,255,50);
    m_pressColor = QColor(0, 0, 0, 50);
    m_textColor = QColor(255,255,255);
    m_normalColor = QColor();

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setColor(Qt::white);
}

FancyButton::~FancyButton()
{

}

void FancyButton::select(bool selected)
{
    m_bMousePress = selected;
    update();
}

void FancyButton::setTextColor(const QColor &color)
{
    m_textColor = color;
}

void FancyButton::setColor(const QColor &color)
{
    if(m_hasBorder){
        setStyleSheet(QString("QToolButton::menu-indicator { image: None; }"
                              "QToolButton{background-color: transparent;color: #%1;}")
                      .arg(QCanpool::colorToArgb(color)));
    }else{
        setStyleSheet(QString("QToolButton::menu-indicator { image: None; }"
                              "QToolButton{background-color: transparent;color: #%1;border:none;}")
                      .arg(QCanpool::colorToArgb(color)));
    }
}

void FancyButton::setNormalColor(const QColor &color)
{
    m_normalColor = color;
}

void FancyButton::setHasMenu(bool has)
{
    m_hasMenu = has;
}

void FancyButton::setHasBorder(bool has)
{
    m_hasBorder = has;
    update();
}

void FancyButton::setRound(bool round)
{
    m_bRound = round;
}

void FancyButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!isEnabled())
        return;
    m_bMouseHover = true;
}

void FancyButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = false;
}

void FancyButton::mousePressEvent(QMouseEvent *event)
{
    if(!isEnabled()) return;

    if(event->button() == Qt::LeftButton)
    {
        m_bMousePress = true;
        if(m_hasMenu || actionHasMenu()){
            emit menuTriggered(event);
        }
        update();
    }
}

void FancyButton::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        m_bMousePress = false;
        update();
        // don't emit when move out
        if(this->rect().contains(event->pos())){
            emit clicked();
        }
    }
}

void FancyButton::paintEvent(QPaintEvent *event)
{
    if(m_bMousePress){
        painterInfo(m_pressColor);
    }else if(m_bMouseHover){
        painterInfo(m_hoverColor);
    }else{
        setColor(m_textColor);
        if(m_normalColor.isValid()){
            painterInfo(m_normalColor);
        }
    }

    QToolButton::paintEvent(event);
}

void FancyButton::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setBrush(color);
    if(m_bRound){
        int r = qMin(width(),height())/2;
        painter.drawEllipse(QPointF(width()/2, height()/2), r, r);
    }else{
        painter.drawRect(rect());
    }
}

bool FancyButton::actionHasMenu()
{
    QAction *action = this->defaultAction();
    if(action == 0) return false;
    QMenu *menu = action->menu();
    if(menu == 0) return false;
    return true;
}


