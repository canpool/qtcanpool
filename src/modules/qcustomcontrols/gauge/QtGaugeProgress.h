/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **  Reference: http://www.qtcn.org
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
