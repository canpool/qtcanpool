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
#include "QtWaitProgressBar.h"
#include <QPainter>
#include <QTimer>
#include <QtMath>

QtWaitProgressBar::QtWaitProgressBar(WaitType type, QWidget *parent)
    : QWidget(parent),
      m_foreColor(Qt::black),
      m_backColor(Qt::lightGray),
      m_clockwiseRotate(true),
      m_rotateDelta(36),
      m_rotateAngle(0),
      m_type(type)
{
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(100);
}

void QtWaitProgressBar::setForeColor(QColor color)
{
    if (color != m_foreColor) {
        m_foreColor = color;
        update();
    }
}

void QtWaitProgressBar::setBackColor(QColor color)
{
    if (color != m_backColor) {
        m_backColor = color;
        update();
    }
}

void QtWaitProgressBar::setClockwiseRotate(bool clockwise)
{
    if (clockwise != m_clockwiseRotate) {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void QtWaitProgressBar::setRotateDelta(int delta)
{
    if (delta != m_rotateDelta) {
        m_rotateDelta = delta;
        update();
    }
}

void QtWaitProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    switch (m_type) {
        case Pie: drawPieWait(p); break;
        case Line: drawLineWait(p); break;
        case Circle: drawCircleWait(p); break;
        case Taichi: drawTaiChiWait(p); break;
        case Donut: drawDonutWait(p); break;
        case Petal: drawPetalWait(p); break;
        case Bearing: drawBearingWait(p); break;
        case Bubble: drawBubbleWait(p); break;
        case Infinite: drawInfiniteWait(p); break;
    }
    m_rotateAngle += m_clockwiseRotate ? m_rotateDelta : -m_rotateDelta;
    m_rotateAngle %= 360;
}

void QtWaitProgressBar::drawLineWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_foreColor;
    for (int i = 0; i <= 10; ++i) {
        color.setAlphaF(1.0*i/10);
        p.setBrush(QBrush(color));
        p.drawRoundedRect(50*2/5, -3, 50*3/5, 6, 2, 2);
        p.rotate(36);
    }
    p.restore();
}

void QtWaitProgressBar::drawCircleWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_foreColor;
    for (int i = 0; i <= 10; ++i) {
        color.setAlphaF(1.0*i/10);
        p.setBrush(QBrush(color));
        p.drawEllipse(30, -10, 20, 20);
        p.rotate(36);
    }
    p.restore();
}

void QtWaitProgressBar::drawDonutWait(QPainter &p)
{
    int outRadius = 50;
    int inRadius = 30;
    QString valueStr = QString("%1%").arg(QString::number(qAbs(1.0*m_rotateAngle/360*100), 'f', 0));
    QRectF outRect(-outRadius, -outRadius, 2*outRadius, 2*outRadius);
    QRectF inRect(-inRadius, -inRadius, 2*inRadius, 2*inRadius);

    p.save();
    p.setPen(Qt::NoPen);

    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(outRect);
    p.setBrush(QBrush(m_foreColor));
    p.drawPie(outRect, 0, 60*16);

    p.setBrush(palette().window().color());
    p.drawEllipse(inRect);

    p.rotate(-m_rotateAngle);
    QFont f = p.font();
    f.setBold(true);
    p.setFont(f);
    p.setPen(m_foreColor);
    p.drawText(inRect, Qt::AlignCenter, valueStr);

    p.restore();
}

void QtWaitProgressBar::drawPieWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(-50, -50, 100, 100);
    p.setBrush(QBrush(m_foreColor));
    p.drawPie(-50, -50, 100, 100, 0*16, 60*16);
    p.restore();
}

void QtWaitProgressBar::drawTaiChiWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_foreColor));
    p.drawPie(-50, -50, 100, 100, 90*16, 180*16);
    p.setBrush(QBrush(m_backColor));
    p.drawPie(-50, -50, 100, 100, 270*16, 180*16);
    p.drawEllipse(-25, -50, 50, 50);
    p.setBrush(QBrush(m_foreColor));
    p.drawEllipse(-25, 0, 50, 50);
    p.drawEllipse(-50/8, -50*5/8, 50/4, 50/4);
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(-50/8, 50*3/8, 50/4, 50/4);
    p.restore();
}

void QtWaitProgressBar::drawPetalWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_foreColor;
    for (int i = 0; i <= 10; ++i) {
        color.setAlphaF(1.0*i/10);
        p.setBrush(QBrush(color));
        p.drawEllipse(20, -6, 30, 12);
        p.rotate(36);
    }
    p.restore();
}

void QtWaitProgressBar::drawBearingWait(QPainter &p)
{
#define ENCLOSING_RECT(r) QRect(-(r), -(r), 2*(r), 2*(r))

    p.save();
    p.setPen(Qt::NoPen);
    QColor foreColor = m_foreColor;
    QColor backColor = m_backColor;
    for (int i = 0; i <= 12; ++i) {
        foreColor.setAlphaF(1.0*i/12);
        p.setBrush(QBrush(foreColor));
        p.drawPie(ENCLOSING_RECT(50), 4*16, 28*16);
        p.setBrush(palette().window().color());
        p.drawPie(ENCLOSING_RECT(40), 4*16, 28*16);
        backColor.setAlphaF(1.0*i/12);
        p.setBrush(QBrush(backColor));
        p.drawPie(ENCLOSING_RECT(37), 4*16, 28*16);
        p.rotate(30);
    }
    p.setBrush(palette().window().color());
    p.drawEllipse(ENCLOSING_RECT(20));
    p.restore();
}

void QtWaitProgressBar::drawBubbleWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_foreColor;
    for (int i = 0; i <= 10; ++i) {
        color.setAlphaF(1.0*i/10);
        p.setBrush(QBrush(color));
        p.drawEllipse(50-2*i, -i, 2*i, 2*i);
        p.rotate(36);
    }
    p.restore();
}

void QtWaitProgressBar::drawInfiniteWait(QPainter &p)
{
    p.save();

    p.rotate(45);

    // x * |cos(a)| = 50
    // side = x / sqrt(2) = 50 / sqrt(2) / |cos(a)|
    double mapAngle = m_rotateAngle % 45; // a mapTo [0, 45]
    double cosA = qCos(qDegreesToRadians(mapAngle));
    cosA = qAbs(cosA);
    double side = 50 / M_SQRT2;
    if (!qFuzzyCompare(cosA, 0)) {
        side /= cosA;
    }

    p.setBrush(Qt::NoBrush);
    QLinearGradient linear(QPointF(0, -side/2), QPointF(0, side/2));
    linear.setColorAt(0, m_foreColor);
    linear.setColorAt(1, m_backColor);
    p.setPen(QPen(QBrush(linear), 5));
    p.drawLine(QPointF(0, -side/2), QPointF(0, side/2));
    linear.setStart(QPointF(-side/2, 0));
    linear.setFinalStop(QPointF(side/2, 0));
    linear.setColorAt(0, m_backColor);
    linear.setColorAt(1, m_foreColor);
    p.setPen(QPen(QBrush(linear), 5));
    p.drawLine(QPointF(-side/2, 0), QPointF(side/2, 0));

    p.setPen(QPen(QBrush(m_foreColor), 5));
    p.setBrush(QBrush(m_foreColor));
    p.drawPie(QRectF(0, -side, side, side), 270*16, 270*16);

    p.setPen(QPen(QBrush(m_backColor), 5));
    p.setBrush(QBrush(m_backColor));
    p.drawPie(QRectF(-side, 0, side, side), 90*16, 270*16);

    p.restore();
}
