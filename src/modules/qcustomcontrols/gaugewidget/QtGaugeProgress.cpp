/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **  Reference: http://www.qtcn.org
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
#include "QtGaugeProgress.h"

#include <QPainter>
#include <QtMath>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QVariant>
#include <QtDebug>

QtGaugeProgress::QtGaugeProgress(QWidget *parent)
    : QWidget(parent),
      m_min(0), m_max(100), m_value(40),
      m_decimals(0),
      m_startAngle(0),
      m_endAngle(0),
      m_backColor(QColor(30, 30, 30)),
      m_progressColor(QColor(214, 77, 84)),
      m_progressBackColor(QColor(50, 50, 50)),
      m_textColor(QColor(255, 255, 255)),
      m_bShowPointer(true),
      m_bShowValue(false),
      m_pointerStyle(Circle),
      m_bMouseHover(false),
      m_bMousePress(false),
      m_coverRadius(85),
      m_innerRadius(81),
      m_innerGradientData{QGradientStop(0, QColor(64, 64, 64))},
      m_pAnimation(nullptr)
{
    m_pAnimation = new QPropertyAnimation(this, "");
    m_pAnimation->setStartValue(0);
    m_pAnimation->setEndValue(7);
    m_pAnimation->setDuration(200);
    connect(m_pAnimation, SIGNAL(valueChanged(QVariant)), this, SLOT(updateRadius(QVariant)));
}

void QtGaugeProgress::setStartAngle(int angle)
{
    if (angle >= 180 || angle < 0 || m_startAngle == angle) {
        return;
    }
    m_startAngle = angle;
    update();
}

void QtGaugeProgress::setEndAngle(int angle)
{
    if (angle >= 180 || angle < 0 || m_endAngle == angle) {
        return;
    }
    m_endAngle = angle;
    update();
}

void QtGaugeProgress::setBackColor(const QColor &color)
{
    if (color != m_backColor) {
        m_backColor = color;
        update();
    }
}

void QtGaugeProgress::setProgressColor(const QColor &color)
{
    if (color != m_progressColor) {
        m_progressColor = color;
        update();
    }
}

void QtGaugeProgress::setProgressBackColor(const QColor &color)
{
    if (color != m_progressBackColor) {
        m_progressBackColor = color;
        update();
    }
}

void QtGaugeProgress::setTextColor(const QColor &color)
{
    if (color != m_textColor) {
        m_textColor = color;
        update();
    }
}

void QtGaugeProgress::setInnerDataColors(const QGradientStops &stopPoints)
{
    if (stopPoints.isEmpty() || stopPoints == m_innerGradientData) {
        m_innerGradientData = stopPoints;
        update();
    }
}

void QtGaugeProgress::setDecimals(int count)
{
    if (count != m_decimals) {
        m_decimals = count;
        update();
    }
}

void QtGaugeProgress::setShowPointer(bool show)
{
    if (show != m_bShowPointer) {
        m_bShowPointer = show;
        update();
    }
}

void QtGaugeProgress::setShowValue(bool show)
{
    if (show != m_bShowValue) {
        m_bShowValue = show;
        update();
    }
}

void QtGaugeProgress::setPointerStyle(PointerStyle style)
{
    if (style != m_pointerStyle) {
        m_pointerStyle = style;
        update();
    }
}

void QtGaugeProgress::setRange(double min, double max)
{
    m_min = min;
    m_max = max;

    if (m_max < m_min) {
        qSwap(m_max, m_min);
    }

    m_value = qBound(m_min, m_value, m_max);

    update();
}

void QtGaugeProgress::setMinimum(double min)
{
    setRange(min, m_max);
}

void QtGaugeProgress::setMaximum(double max)
{
    setRange(m_min, max);
}

void QtGaugeProgress::setValue(double value)
{
    if (!qFuzzyCompare(value, m_value)) {
        m_value = qBound(m_min, value, m_max);
        update();
        emit valueChanged(static_cast<int>(value));
    }
}

void QtGaugeProgress::setValue(int value)
{
    setValue(double(value));
}

void QtGaugeProgress::enterEvent(QEvent *e)
{
    Q_UNUSED(e);
    m_bMouseHover = true;
    m_pAnimation->start();
}

void QtGaugeProgress::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    m_bMouseHover = false;
    m_pAnimation->start();
}

void QtGaugeProgress::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_bMousePress = true;
        setPressedValue(e->pos());
    }
}

void QtGaugeProgress::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        m_bMousePress = false;
    }
}

void QtGaugeProgress::mouseMoveEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    if (m_bMousePress) {
        setPressedValue(e->pos());
    }
}

void QtGaugeProgress::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/200.0, side/200.0);

    drawBackground(p);
    drawProgress(p);
    drawCoverCircle(p);
    drawInnerCircle(p);

    if (m_bShowPointer) {
        if (m_pointerStyle == Circle) {
            drawPointerCircle(p);
        } else if (m_pointerStyle == Indicator) {
            drawPointerIndicator(p);
        } else if (m_pointerStyle == IndicatorR) {
            drawPointerIndicatorR(p);
        } else if (m_pointerStyle == Triangle) {
            drawPointerTriangle(p);
        }
    }

    if (m_bShowValue) {
        drawValue(p);
    }
}

void QtGaugeProgress::drawBackground(QPainter &p)
{
    int radius = 99;
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_backColor);
    p.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    p.restore();
}


void QtGaugeProgress::drawProgress(QPainter &p)
{
    int radius = 95;
    p.save();
    p.setPen(Qt::NoPen);
    QRectF rect(-radius, -radius, radius * 2, radius * 2);

    int angleAll = 360 - m_startAngle - m_endAngle; // --> "/|\"
    int angleCurrent = static_cast<int>(angleAll * ((m_value - m_min) / (m_max - m_min)));
    int angleOther = angleAll - angleCurrent;

    p.setBrush(m_progressColor);
    p.drawPie(rect, (270 - m_startAngle - angleCurrent) * 16, angleCurrent * 16); // counter-clockwise

    p.setBrush(m_progressBackColor);
    p.drawPie(rect, (270 - m_startAngle - angleCurrent - angleOther) * 16, angleOther * 16);
    p.restore();
}

void QtGaugeProgress::drawCoverCircle(QPainter &p)
{
    int radius = m_coverRadius;
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_backColor);
    p.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    p.restore();
}

void QtGaugeProgress::drawInnerCircle(QPainter &p)
{
    int radius = m_innerRadius;
    p.save();
    p.setPen(Qt::NoPen);
    QLinearGradient linear(0, -radius, 0, radius);
    for (int i = 0; i < m_innerGradientData.count(); ++i) {
        linear.setColorAt(m_innerGradientData.at(i).first, m_innerGradientData.at(i).second);
    }
    p.setBrush(linear);
    p.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    p.restore();
}

void QtGaugeProgress::drawPointerCircle(QPainter &p)
{
    int radius = 15;
    int offset = m_innerRadius - 60;

    p.save();
    p.setPen(Qt::NoPen);
    p.rotate(m_startAngle);
    double degRotate = (360.0 - m_startAngle - m_endAngle) * (m_value - m_min) / (m_max - m_min);
    p.rotate(degRotate);
    p.setBrush(m_progressColor);
    p.drawEllipse(-radius, radius + offset, radius * 2, radius * 2);
    p.restore();
}

void QtGaugeProgress::drawPointerIndicator(QPainter &p)
{
    int radius = m_innerRadius - 15;
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_progressColor);

    QPolygon pts;
    pts.setPoints(3, -8, 0, 8, 0, 0, radius);
    p.rotate(m_startAngle);
    double degRotate = (360.0 - m_startAngle - m_endAngle) / (m_max - m_min) * (m_value - m_min);
    p.rotate(degRotate);
    p.drawConvexPolygon(pts);

    radius = radius / 4;
    p.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    p.restore();
}

void QtGaugeProgress::drawPointerIndicatorR(QPainter &p)
{
    int radius = m_innerRadius - 15;

    p.save();
    QPen pen;
    pen.setWidth(1);
    pen.setColor(m_progressColor);
    p.setPen(pen);
    p.setBrush(m_progressColor);
    QPolygon pts;
    pts.setPoints(3, -8, 0, 8, 0, 0, radius);
    p.rotate(m_startAngle);
    double degRotate = (360.0 - m_startAngle - m_endAngle) / (m_max - m_min) * (m_value - m_min);
    p.rotate(degRotate);
    p.drawConvexPolygon(pts);

    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(4);
    p.setPen(pen);
    p.drawLine(0, 0, 0, radius);

    radius = radius / 4;
    p.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    p.restore();
}

void QtGaugeProgress::drawPointerTriangle(QPainter &p)
{
    int radius = 20;
    int offset = m_innerRadius - 25;

    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(m_progressColor);
    QPolygon pts;
    pts.setPoints(3, -radius / 2, offset, radius / 2, offset, 0, radius + offset);
    p.rotate(m_startAngle);
    double degRotate = (360.0 - m_startAngle - m_endAngle) * (m_value - m_min) / (m_max - m_min);
    p.rotate(degRotate);
    p.drawConvexPolygon(pts);
    p.restore();
}

void QtGaugeProgress::drawValue(QPainter &p)
{
    int radius = 100;
    p.save();
    p.setPen(m_textColor);
    QFont font;
    font.setPixelSize(m_bShowPointer ? radius - 50 : radius - 15);
    font.setBold(true);
    p.setFont(font);
    QRectF textRect(-radius, -radius, radius * 2, radius * 2);
    QString strValue = QString("%1").arg(m_value, 0, 'f', m_decimals);
    p.drawText(textRect, Qt::AlignCenter, strValue);
    p.restore();
}

void QtGaugeProgress::setPressedValue(QPointF pressedPoint)
{
    double x = pressedPoint.x() - width() / 2;
    double y = height() / 2 - pressedPoint.y();
    double curValue = 0;
    double curAngle = 0;
    double rate = 0;

    // calculate the current angle [-180, 0] [0, 180]
    if (!qFuzzyCompare(x, 0)) {
        curAngle = qRadiansToDegrees(qAtan2(y, x));
    } else {
        curAngle = y < 0 ? -90 : 90;
    }

    // (-90, 90)
    if (curAngle > -90 && curAngle < 90) {
        double endAngle = 90 - (180 - m_endAngle);
        if (curAngle < endAngle) {
            return;
        }
        rate = 1 - (curAngle - endAngle) / (360 - m_startAngle - m_endAngle);
    } else { // [90, 180], [-180, -90]
        curAngle = curAngle > 0 ? curAngle : (360 + curAngle);
        double startAngle = 270 - m_startAngle;
        if (curAngle > startAngle) {
            return;
        }
        rate = (startAngle - curAngle) / (360 - m_startAngle - m_endAngle);
    }
    curValue = rate * (m_max - m_min) + m_min;
    setValue(curValue);
}

void QtGaugeProgress::updateRadius(const QVariant &radius)
{
    Q_UNUSED(radius);
    if (m_bMouseHover) {
        if (m_coverRadius <= 78) { // 85 - 7
            return;
        }
        m_coverRadius--;
        m_innerRadius--;
    } else {
        if (m_coverRadius >= 85) {
            return;
        }
        m_coverRadius++;
        m_innerRadius++;
    }
    update();
}
