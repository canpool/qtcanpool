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
#ifndef QTWAITPROGRESSBAR_H
#define QTWAITPROGRESSBAR_H

#include <QWidget>

class QtWaitProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum WaitType { Line, Pie, Donut, Circle, Taichi, Petal, Bearing, Bubble, Infinite };

    QtWaitProgressBar(WaitType type = Line, QWidget *parent = nullptr);

    void setForeColor(QColor color);
    void setBackColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void drawLineWait(QPainter &p);
    void drawCircleWait(QPainter &p);
    void drawDonutWait(QPainter &p);
    void drawPieWait(QPainter &p);
    void drawTaiChiWait(QPainter &p);
    void drawPetalWait(QPainter &p);
    void drawBearingWait(QPainter &p);
    void drawBubbleWait(QPainter &p);
    void drawInfiniteWait(QPainter &p);

private:
    QColor m_foreColor;
    QColor m_backColor;
    bool m_clockwiseRotate;
    int m_rotateDelta;
    int m_rotateAngle;

    QTimer *m_timer;

    WaitType m_type;
};

#endif // QTWAITPROGRESSBAR_H
