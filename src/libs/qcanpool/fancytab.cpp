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

QCANPOOL_USE_NAMESPACE

FancyTab::FancyTab(QWidget *parent)
    : FancyButton(parent)
{
    init();
}

FancyTab::FancyTab(const QIcon &icon, QWidget *parent)
    : FancyButton(parent)
{
    setIcon(icon);
    init();
}


void FancyTab::init()
{
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(24, 24));
    setAutoRaise(true);
    m_bMouseHover = false;
    m_bMousePress = false;
    m_selectedTextColor = QColor(255, 255, 255);
    m_type = Mode;

    setPressColor(QColor(0, 0, 0, 100));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setStyleSheet("QToolButton{background-color: transparent;color: white;}");
}

FancyTab::~FancyTab()
{
}

void FancyTab::select(bool selected)
{
    m_bMousePress = selected;
    if (!selected) {
        m_bMouseHover = false;
    }
    update();
}

void FancyTab::setSelectedTextColor(const QColor &color)
{
    m_selectedTextColor = color;
}

void FancyTab::setColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void FancyTab::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (!isEnabled()) {
        return;
    }

    m_bMouseHover = true;
}

void FancyTab::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bMouseHover = false;
}

void FancyTab::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if (m_type == Action) {
            m_bMousePress = true; // add for action
            update();
        } else {
            if (m_hasMenu) {
                m_bMousePress = true; // add for action
                update();
                emit menuTriggered(event);
            } else {
                emit clicked();
            }
        }
    }
}

void FancyTab::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_type == Action) {
            m_bMousePress = false; // add for action
            update();

            if (this->rect().contains(event->pos())) { // don't emit when move out
                emit clicked();
            }
        }
    }
}

void FancyTab::paintEvent(QPaintEvent *event)
{
    if (m_bMousePress) {
        painterInfo(pressColor());
    } else if (m_bMouseHover) {
        painterInfo(hoverColor());
    }

    if (m_hasMenu) {
        if (m_bMousePress || m_bMouseHover) {
            painterArrow(m_selectedTextColor);
        } else {
            painterArrow(textColor());
        }
    }

    QToolButton::paintEvent(event);
}

void FancyTab::update()
{
    QToolButton::update();
    if (m_bMousePress || m_bMouseHover) {
        setColor(m_selectedTextColor);
    } else {
        setColor(textColor());
    }
}

void FancyTab::painterInfo(const QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    painter.setBrush(color);
    painter.drawRect(rect());
}

void FancyTab::painterArrow(const QColor &color)
{
    QPainter painter(this);
    painter.setBrush(QBrush(color));
    QPolygonF polygonf;
    QPointF pt;
    QPoint pos = rect().center();
    pos.setX(pos.x() + rect().width() / 2);
    pt = QPointF(pos.x() - 6, pos.y());
    polygonf.append(pt);
    pt = QPointF(pos.x() - 10, pos.y() + 4);
    polygonf.append(pt);
    pt = QPointF(pos.x() - 10, pos.y() - 4);
    polygonf.append(pt);
    painter.drawPolygon(polygonf);
}
