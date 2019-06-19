/*
 * QtRoundProgressBar - a circular progress bar Qt widget.
 *
 * Sintegrial Technologies (c) 2015-now
 *
 * The software is freeware and is distributed "as is" with the complete source codes.
 * Anybody is free to use it in any software projects, either commercial or non-commercial.
 * Please do not remove this copyright message and remain the name of the author unchanged.
 *
 * It is very appreciated if you produce some feedback to us case you are going to use
 * the software.
 *
 * Please send your questions, suggestions, and information about found issues to the
 *
 * sintegrial@gmail.com
 *
 */
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

#include "QtRoundProgressBar.h"

#include <QtGui/QPainter>

QtRoundProgressBar::QtRoundProgressBar(QWidget *parent) :
    QWidget(parent),
    m_min(0), m_max(100),
    m_value(25),
    m_nullPosition(PositionTop),
    m_barStyle(StyleDonut),
    m_outlinePenWidth(1),
    m_dataPenWidth(1),
    m_rebuildBrush(false),
    m_format("%p%"),
    m_decimals(1),
    m_updateFlags(UF_PERCENT)
{
}

void QtRoundProgressBar::setRange(double min, double max)
{
    m_min = min;
    m_max = max;

    if (m_max < m_min) {
        qSwap(m_max, m_min);
    }

    m_value = qBound(m_min, m_value, m_max);

    if (!m_gradientData.isEmpty()) {
        m_rebuildBrush = true;
    }

    update();
}

void QtRoundProgressBar::setMinimum(double min)
{
    setRange(min, m_max);
}

void QtRoundProgressBar::setMaximum(double max)
{
    setRange(m_min, max);
}

void QtRoundProgressBar::setValue(double value)
{
    if (!qFuzzyCompare(m_value, value)) {
        m_value = qBound(m_min, value, m_max);
        update();
    }
}

void QtRoundProgressBar::setValue(int value)
{
    setValue(double(value));
}

void QtRoundProgressBar::setNullPosition(double position)
{
    if (!qFuzzyCompare(position, m_nullPosition)) {
        m_nullPosition = position;

        if (!m_gradientData.isEmpty()) {
            m_rebuildBrush = true;
        }

        update();
    }
}

void QtRoundProgressBar::setBarStyle(QtRoundProgressBar::BarStyle style)
{
    if (style != m_barStyle) {
        m_barStyle = style;
        update();
    }
}

void QtRoundProgressBar::setOutlinePenWidth(double penWidth)
{
    if (!qFuzzyCompare(penWidth, m_outlinePenWidth)) {
        m_outlinePenWidth = penWidth;
        update();
    }
}

void QtRoundProgressBar::setDataPenWidth(double penWidth)
{
    if (!qFuzzyCompare(penWidth, m_dataPenWidth)) {
        m_dataPenWidth = penWidth;
        update();
    }
}

void QtRoundProgressBar::setDataColors(const QGradientStops &stopPoints)
{
    if (stopPoints != m_gradientData) {
        m_gradientData = stopPoints;
        m_rebuildBrush = true;

        update();
    }
}

void QtRoundProgressBar::setFormat(const QString &format)
{
    if (format != m_format){
        m_format = format;
        valueFormatChanged();
    }
}

void QtRoundProgressBar::resetFormat()
{
    m_format = QString();

    valueFormatChanged();
}

void QtRoundProgressBar::setDecimals(int count)
{
    if (count >= 0 && count != m_decimals) {
        m_decimals = count;

        valueFormatChanged();
    }
}

void QtRoundProgressBar::paintEvent(QPaintEvent* /*event*/)
{
    double outerRadius = qMin(width(), height());
    QRectF baseRect(1, 1, outerRadius-2, outerRadius-2);

    QImage buffer(static_cast<int>(outerRadius), static_cast<int>(outerRadius), QImage::Format_ARGB32_Premultiplied);

    QPainter p(&buffer);
    p.setRenderHint(QPainter::Antialiasing);

    // data brush
    rebuildDataBrushIfNeeded();

    // background
    drawBackground(p, buffer.rect());

    // base circle
    drawBase(p, baseRect);

    // data circle
    double arcStep = 360.0 / (m_max - m_min) * m_value;
    drawValue(p, baseRect, m_value, arcStep);

    // center circle
    double innerRadius(0);
    QRectF innerRect;
    calculateInnerRect(baseRect, outerRadius, innerRect, innerRadius);
    drawInnerBackground(p, innerRect);

    // text
    drawText(p, innerRect, innerRadius, m_value);

    // finally draw the bar
    p.end();

    QPainter painter(this);
    painter.fillRect(baseRect, palette().background());
    painter.drawImage(0, 0, buffer);
}

void QtRoundProgressBar::drawBackground(QPainter &p, const QRectF &baseRect)
{
    p.fillRect(baseRect, palette().background());
}

void QtRoundProgressBar::drawBase(QPainter &p, const QRectF &baseRect)
{
    switch (m_barStyle)
    {
    case StyleDonut:
        p.setPen(QPen(palette().shadow().color(), m_outlinePenWidth));
        p.setBrush(palette().base());
        p.drawEllipse(baseRect);
        break;

    case StylePie:
        p.setPen(QPen(palette().base().color(), m_outlinePenWidth));
        p.setBrush(palette().base());
        p.drawEllipse(baseRect);
        break;

    case StyleLine:
        p.setPen(QPen(palette().base().color(), m_outlinePenWidth));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(baseRect.adjusted(m_outlinePenWidth/2, m_outlinePenWidth/2, -m_outlinePenWidth/2, -m_outlinePenWidth/2));
        break;
    }

}

void QtRoundProgressBar::drawValue(QPainter &p, const QRectF &baseRect, double value, double arcLength)
{
    // nothing to draw
    if (qFuzzyCompare(value, m_min)) {
        return;
    }

    // for Line style
    if (m_barStyle == StyleLine) {
        p.setPen(QPen(palette().highlight().color(), m_dataPenWidth));
        p.setBrush(Qt::NoBrush);
        p.drawArc(baseRect.adjusted(m_outlinePenWidth/2, m_outlinePenWidth/2, -m_outlinePenWidth/2, -m_outlinePenWidth/2),
                  static_cast<int>(m_nullPosition * 16),
                  static_cast<int>(-arcLength * 16));
        return;
    }

    // for Pie and Donut styles
    QPainterPath dataPath;
    dataPath.setFillRule(Qt::WindingFill);

    // pie segment outer
    dataPath.moveTo(baseRect.center());
    dataPath.arcTo(baseRect, m_nullPosition, -arcLength);
    dataPath.lineTo(baseRect.center());

    p.setBrush(palette().highlight());
    p.setPen(QPen(palette().shadow().color(), m_dataPenWidth));
    p.drawPath(dataPath);
}

void QtRoundProgressBar::calculateInnerRect(const QRectF &/*baseRect*/, double outerRadius, QRectF &innerRect, double &innerRadius)
{
    // for Line style
    if (m_barStyle == StyleLine) {
        innerRadius = outerRadius - m_outlinePenWidth;
    } else {  // for Pie and Donut styles
        innerRadius = outerRadius * 0.75;
    }

    double delta = (outerRadius - innerRadius) / 2;
    innerRect = QRectF(delta, delta, innerRadius, innerRadius);
}

void QtRoundProgressBar::drawInnerBackground(QPainter &p, const QRectF &innerRect)
{
    if (m_barStyle == StyleDonut) {
        p.setBrush(palette().alternateBase());
        p.drawEllipse(innerRect);
    }
}

void QtRoundProgressBar::drawText(QPainter &p, const QRectF &innerRect, double innerRadius, double value)
{
    if (m_format.isEmpty()) {
        return;
    }

    // !!! to revise
    QFont f(font());
    f.setPixelSize(static_cast<int>(innerRadius * qMax(0.05, (0.35 - m_decimals * 0.08))));
    p.setFont(f);

    QRectF textRect(innerRect);
    p.setPen(palette().text().color());
    p.drawText(textRect, Qt::AlignCenter, valueToText(value));
}

QString QtRoundProgressBar::valueToText(double value) const
{
    QString textToDraw(m_format);

    if (m_updateFlags & UF_VALUE)
        textToDraw.replace("%v", QString::number(value, 'f', m_decimals));

    if (m_updateFlags & UF_PERCENT) {
        double procent = (value - m_min) / (m_max - m_min) * 100.0;
        textToDraw.replace("%p", QString::number(procent, 'f', m_decimals));
    }

    if (m_updateFlags & UF_MAX)
        textToDraw.replace("%m", QString::number(m_max - m_min + 1, 'f', m_decimals));

    return textToDraw;
}

void QtRoundProgressBar::valueFormatChanged()
{
    m_updateFlags = 0;

    if (m_format.contains("%v"))
        m_updateFlags |= UF_VALUE;

    if (m_format.contains("%p"))
        m_updateFlags |= UF_PERCENT;

    if (m_format.contains("%m"))
        m_updateFlags |= UF_MAX;

    update();
}

void QtRoundProgressBar::rebuildDataBrushIfNeeded()
{
    if (m_rebuildBrush) {
        m_rebuildBrush = false;

        QConicalGradient dataBrush;
        dataBrush.setCenter(0.5, 0.5);
        dataBrush.setCoordinateMode(QGradient::StretchToDeviceMode);

        // invert colors
        for (int i = 0; i < m_gradientData.count(); i++) {
            dataBrush.setColorAt(1.0 - m_gradientData.at(i).first, m_gradientData.at(i).second);
        }

        // angle
        dataBrush.setAngle(m_nullPosition);

        QPalette p(palette());
        p.setBrush(QPalette::Highlight, dataBrush);
        setPalette(p);
    }
}



