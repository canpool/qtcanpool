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
