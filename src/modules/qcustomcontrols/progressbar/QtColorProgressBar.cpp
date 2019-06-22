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
#include "QtColorProgressBar.h"
#include <QPainter>

QtColorProgressBar::QtColorProgressBar(BarStyle style, QWidget *parent)
    : QWidget(parent),
      m_min(0), m_max(100), m_value(0),
      m_barForeColor(QColor(255, 107, 107)),
      m_barForeAlternateColor(QColor(255, 107, 10)),
      m_barBackColor(Qt::white),
      m_textForeColor(Qt::black),
      m_textBackColor(Qt::white),
      m_decimals(0),
      m_radius(10),
      m_textPosition(TextOutBar),
      m_barStyle(style)
{
}

void QtColorProgressBar::setTextForeColor(const QColor &color)
{
    if (color != m_textForeColor) {
        m_textForeColor = color;
        update();
    }
}

void QtColorProgressBar::setTextBackColor(const QColor &color)
{
    if (color != m_textBackColor) {
        m_textBackColor = color;
        update();
    }
}

void QtColorProgressBar::setBarForeColor(const QColor &color)
{
    if (color != m_barForeColor) {
        m_barForeColor = color;
        update();
    }
}

void QtColorProgressBar::setBarForeAlternateColor(const QColor &color)
{
    if (color != m_barForeAlternateColor) {
        m_barForeAlternateColor = color;
        update();
    }
}

void QtColorProgressBar::setBarBackColor(const QColor &color)
{
    if (color != m_barBackColor) {
        m_barBackColor = color;
        update();
    }
}

void QtColorProgressBar::setBarForeDataColors(const QGradientStops &stopPoints)
{
    if (stopPoints != m_gradientData) {
        m_gradientData = stopPoints;
        update();
    }
}

void QtColorProgressBar::setBarStep(int space, int delta, int size)
{
    if (m_barStyle == StyleSegmented) {
        if (space < 0 || size < 0) {
            return;
        }
        m_barStep.space = space;
        m_barStep.delta = delta;
        m_barStep.size = size;

        // When the size is too large, the bar should not be displayed in rounded corners
        if (size > 2) {
            m_radius = 0;
        }

        update();
    }
}

void QtColorProgressBar::setDecimals(int count)
{
    if (count != m_decimals) {
        m_decimals = count;
        update();
    }
}

void QtColorProgressBar::setRadius(int radius)
{
    if (radius != m_radius) {
        m_radius = radius;
        update();
    }
}

void QtColorProgressBar::setTextPosition(TextPosition position)
{
    if (position != m_textPosition) {
        m_textPosition = position;
        update();
    }
}

void QtColorProgressBar::setBarStyle(QtColorProgressBar::BarStyle style)
{
    if (style == m_barStyle) {
        m_barStyle = style;
        update();
    }
}

void QtColorProgressBar::setRange(double min, double max)
{
    m_min = min;
    m_max = max;

    if (m_max < m_min) {
        qSwap(m_max, m_min);
    }

    m_value = qBound(m_min, m_value, m_max);

    update();
}

void QtColorProgressBar::setMinimum(double min)
{
    setRange(min, m_max);
}

void QtColorProgressBar::setMaximum(double max)
{
    setRange(m_min, max);
}

void QtColorProgressBar::setValue(double value)
{
    if (!qFuzzyCompare(value, m_value)) {
        m_value = qBound(m_min, value, m_max);
        update();
    }
}

void QtColorProgressBar::setValue(int value)
{
    setValue(double(value));
}

void QtColorProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF barRect, textRect;

    drawBar(p, barRect, textRect);
}

void QtColorProgressBar::drawBar(QPainter &p, QRectF &barRect, QRectF &textRect)
{
    double scale = (m_value - m_min) / (m_max - m_min);
    double percent = scale * 100.0;
    QString strValue = QString("%1%").arg(QString::number(percent, 'f', m_decimals));
    QFontMetricsF fm = QFontMetricsF(p.font());
    QSizeF size = fm.size(Qt::TextSingleLine, strValue);

    double textWidth = size.width() * 1.2 + 5;
    double curPos = scale * width();

    bool needDrawTextBkFlag = false;
    if (m_textPosition == TextOutBar) {
        if (textWidth < 0.1 * width()) {
            barRect = QRectF(0, 0, 0.9 * width(), height());
            textRect = QRectF(0.9 * width() + 5, 0, 0.1 * width() - 5, height());
            needDrawTextBkFlag = true;
        } else {
            barRect = QRectF(0, 0, width(), height());
            textRect = QRectF(width() - textWidth, 0, textWidth, height()); // (like TextInBar)
        }
    } else if (m_textPosition == TextInBar) {
        barRect = QRectF(0, 0, width(), height());
        textRect = QRectF(curPos - textWidth, 0, textWidth, height());
    } else if (m_textPosition == TextOnBar) {
        barRect = QRectF(0, height() * 2 / 3, width(), height() / 3);
        if (textWidth < width() - curPos) {
            double offset = (curPos > 15 ? 15 : curPos); // 15 = 3 * radius
            textRect = QRectF(curPos - offset, height() / 12, textWidth, height() * 5 / 12);
        } else { // arrived at the tail of bar
            textRect = QRectF(width() - textWidth, height() / 12, textWidth, height() * 5 / 12);
        }
        if (m_value > 0) {
            needDrawTextBkFlag = true;
        }
    }
    drawBarBackground(p, barRect);
    drawData(p, barRect);
    if (needDrawTextBkFlag) {
        drawTextBackground(p, textRect);
    }
    drawText(p, textRect);
}

void QtColorProgressBar::drawBarBackground(QPainter &p, QRectF &barRect)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_barBackColor));
    p.drawRoundedRect(barRect, m_radius, m_radius);
    p.restore();
}

void QtColorProgressBar::drawTextBackground(QPainter &p, QRectF &textRect)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_textBackColor));
    if (m_textPosition == TextOnBar) {
        p.drawRoundedRect(textRect, 3, 3);
        // draw flag-dot in the bar
        double curPos = (m_value - m_min) / (m_max - m_min) * width();
        double length = qMin(width(), height());
        double radius = (length < 10 ? length : 10) / 2;
        p.drawEllipse(QPointF(curPos - 2 * radius, height() * 5 / 6), radius, radius);
    } else {
        p.drawRoundedRect(textRect, m_radius, m_radius);
    }
    p.restore();
}

/******************************************************************************
 *
 * min                                          value        max
 *  |--------------------------------------------|------------|
 *  =================== rect ====================|
 *  ---------------------------------------------|------------|
 *  |                                            |<--- dx --->|
 *
******************************************************************************/
void QtColorProgressBar::drawData(QPainter &p, const QRectF &barRect)
{
    double dx = (1 - (m_value - m_min) / (m_max - m_min)) * barRect.width();
    QRectF rect(barRect.adjusted(0, 0, -dx, 0));

    // draw the value of bar
    p.save();
    p.setPen(Qt::NoPen);
    if (m_barStyle == StyleGradual) { // gradient style
        QLinearGradient linear(rect.x(), rect.y(), rect.width(), rect.height());
        for (int i = 0; i < m_gradientData.count(); ++i) {
            linear.setColorAt(m_gradientData.at(i).first, m_gradientData.at(i).second);
        }
        p.setBrush(linear);
        p.setPen(Qt::transparent);
        p.drawRoundedRect(rect, m_radius, m_radius);
    } else { // for segmented and solid style
        p.setBrush(QBrush(m_barForeColor));
        p.drawRoundedRect(rect, m_radius, m_radius);
    }
    p.restore();

    drawBarStep(p, rect);
}

/******************************************************************************
 *                        space                            size
 *                    -->|    |<--                      x1 x2
 *  ---------------------|----|------------------   -----|-|-----
 *                 /|   /    /                          /=/
 *                / |  /    /                          /=/
 *  -------------|--|----------------------------   --|-|--------
 *            -->|  |<--                             x4 x3
 *              delta
 ******************************************************************************/
void QtColorProgressBar::drawBarStep(QPainter &p, const QRectF &barRect)
{
    if (m_barStyle != StyleSegmented) {
        return;
    }

    p.save();
    if (m_barStep.size < 3) {
        p.setPen(QPen(QBrush(m_barForeAlternateColor), m_barStep.size));
        for (int i = 0; i < barRect.right(); i += m_barStep.size + m_barStep.space) {
            p.drawLine(QPointF(i, barRect.top()), QPointF(i + m_barStep.delta, barRect.bottom()));
        }
    } else {
        p.setPen(Qt::NoPen);
        p.setBrush(m_barForeAlternateColor);
        for (int i = 0; i < barRect.right(); i += m_barStep.size + m_barStep.space) {
            int x1 = i;
            int x2 = i + m_barStep.size;
            int x3 = i + m_barStep.size + m_barStep.delta;
            int x4 = i + m_barStep.delta;
            // fixed coordinates
            int maxWidth = static_cast<int>(barRect.right());
            x2 = qBound(0, x2, maxWidth);
            x3 = qBound(0, x3, maxWidth);
            x4 = qBound(0, x4, maxWidth);

            QVector<QPointF> vec = { QPointF(x1, barRect.top()), QPointF(x2, barRect.top()),
                                     QPointF(x3, barRect.bottom()), QPointF(x4, barRect.bottom()) };
            p.drawPolygon(vec);
       }
    }
    p.restore();
}

void QtColorProgressBar::drawText(QPainter &p, QRectF &textRect)
{
    double percent = (m_value - m_min) / (m_max - m_min) * 100.0;
    QString strValue = QString("%1%").arg(QString::number(percent, 'f', m_decimals));

    /* draw text value */
    p.save();
    p.setPen(m_textForeColor);
    p.drawText(textRect, Qt::AlignCenter, strValue);
    p.restore();
}
