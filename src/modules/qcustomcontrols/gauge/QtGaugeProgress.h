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
#ifndef QTGAUGEPROGRESS_H
#define QTGAUGEPROGRESS_H

#include <QWidget>

class QPropertyAnimation;

class QtGaugeProgress : public QWidget
{
    Q_OBJECT
public:
    enum PointerStyle {
        Circle,
        Indicator,
        IndicatorR,
        Triangle
    };

    explicit QtGaugeProgress(QWidget *parent = nullptr);

    void setStartAngle(int angle);
    void setEndAngle(int angle);

    void setBackColor(const QColor &color);
    void setProgressColor(const QColor &color);
    void setProgressBackColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setInnerDataColors(const QGradientStops &stopPoints);

    void setDecimals(int count);

    void setShowPointer(bool show);
    void setShowValue(bool show);

    void setPointerStyle(PointerStyle style);

signals:
    void valueChanged(int value);

public slots:
    void setRange(double min, double max);
    void setMinimum(double min);
    void setMaximum(double max);
    void setValue(double value);
    void setValue(int value);

protected:
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);

private:
    void drawBackground(QPainter &p);
    void drawProgress(QPainter &p);
    void drawCoverCircle(QPainter &p);
    void drawInnerCircle(QPainter &p);
    void drawPointerCircle(QPainter &p);
    void drawPointerIndicator(QPainter &p);
    void drawPointerIndicatorR(QPainter &p);
    void drawPointerTriangle(QPainter &p);
    void drawValue(QPainter &p);

    void setPressedValue(QPointF pressedPoint);

private slots:
    void updateRadius(const QVariant &radius);

private:
    double m_min, m_max, m_value;
    int m_decimals;

    int m_startAngle; // start rotation angle (/|)
    int m_endAngle;   // end rotation angle (|\)

    QColor m_backColor;
    QColor m_progressColor;
    QColor m_progressBackColor;
    QColor m_textColor;

    bool m_bShowPointer;
    bool m_bShowValue;
    PointerStyle m_pointerStyle;

    bool m_bMouseHover;
    bool m_bMousePress;

    int m_coverRadius;
    int m_innerRadius;
    QGradientStops m_innerGradientData;

    QPropertyAnimation *m_pAnimation;

};

#endif // QTGAUGEPROGRESS_H
