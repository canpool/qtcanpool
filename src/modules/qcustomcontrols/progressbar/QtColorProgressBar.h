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
#ifndef QTCOLORPROGRESSBAR_H
#define QTCOLORPROGRESSBAR_H

#include <QWidget>

class QtColorProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum TextPosition {
        TextInBar,
        TextOnBar,
        TextOutBar
    };

    enum BarStyle {
        StyleSolid,
        StyleGradual,
        StyleSegmented
    };

    // use for StyleSegmented
    struct BarStep {
        BarStep() : size(2), delta(0), space(5) {}
        int size;   // the size of step
        int delta;  // the x-delta of step
        int space;  // The space/gap between each step
    };

    explicit QtColorProgressBar(BarStyle style = StyleSolid, QWidget *parent = nullptr);

    void setTextForeColor(const QColor &color);
    void setTextBackColor(const QColor &color);

    void setBarForeColor(const QColor &color);
    void setBarForeAlternateColor(const QColor &color);
    void setBarBackColor(const QColor &color);

    void setBarForeDataColors(const QGradientStops &stopPoints);

    void setBarStep(int space, int delta, int size = 2);

    void setDecimals(int count);
    void setRadius(int radius);

    void setTextPosition(TextPosition position);
    void setBarStyle(BarStyle style);

public slots:
    void setRange(double min, double max);
    void setMinimum(double min);
    void setMaximum(double max);
    void setValue(double value);
    void setValue(int value);

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    void drawBar(QPainter &p, QRectF &barRect, QRectF &textRect);
    void drawBarBackground(QPainter &p, QRectF &barRect);
    void drawTextBackground(QPainter &p, QRectF &textRect);
    void drawData(QPainter &p, const QRectF &barRect);
    void drawBarStep(QPainter &p, const QRectF &barRect);
    void drawText(QPainter &p, QRectF &textRect);

private:
    double m_min, m_max, m_value;

    QColor m_barForeColor;
    QColor m_barForeAlternateColor;
    QColor m_barBackColor;
    QGradientStops m_gradientData;

    QColor m_textForeColor;
    QColor m_textBackColor;

    int m_decimals;
    int m_radius;

    TextPosition m_textPosition;
    BarStyle m_barStyle;
    BarStep m_barStep;
};

#endif // QTCOLORPROGRESSBAR_H
