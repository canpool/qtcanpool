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
