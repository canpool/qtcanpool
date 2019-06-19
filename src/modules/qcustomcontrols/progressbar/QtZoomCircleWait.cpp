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
#include "QtZoomCircleWait.h"
#include <QPainter>
#include <QTimer>

QtZoomCircleWait::QtZoomCircleWait(QWidget *parent) : QWidget(parent),
    m_color(Qt::black),
    m_zoomOut(false),
    m_zoomDelta(5),
    m_radius(0),
    m_minRadius(0)
{
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(100);
}

void QtZoomCircleWait::setColor(QColor color)
{
    if (color != m_color) {
        m_color = color;
        update();
    }
}

void QtZoomCircleWait::setZoomOut(bool zoomOut)
{
    if (zoomOut != m_zoomOut) {
        m_zoomOut = zoomOut;
        update();
    }
}

void QtZoomCircleWait::setZoomDelta(int delta)
{
    if (delta != m_zoomDelta) {
        m_zoomDelta = delta;
        update();
    }
}

void QtZoomCircleWait::setMinRadius(int value)
{
    if (value > 0 && value != m_minRadius) {
        m_minRadius = value;
        update();
    }
}

void QtZoomCircleWait::setRadius(int value)
{
    if (value != m_radius && value > m_minRadius) {
        m_radius = value;
        update();
    }
}

void QtZoomCircleWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    m_radius += m_zoomDelta;
    fixRaidus(50);
    drawZoomCricleWait(p);
}

void QtZoomCircleWait::drawZoomCricleWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_color));
    p.drawEllipse(-m_radius, -m_radius, 2*m_radius, 2*m_radius);
    p.restore();
}

void QtZoomCircleWait::fixRaidus(int max)
{
    if (m_radius < m_minRadius) {
        m_radius = m_minRadius;
        m_zoomDelta = qAbs(m_zoomDelta);
    } else if (m_radius > max) {
        if (m_zoomOut)  {
            m_radius = max;
            m_zoomDelta = -qAbs(m_zoomDelta);
        } else {
            m_radius = m_minRadius;
            m_zoomDelta = qAbs(m_zoomDelta);
        }
    }
}
