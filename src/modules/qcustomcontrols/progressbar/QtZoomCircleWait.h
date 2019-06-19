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
#ifndef QTZOOMCIRCLEWAIT_H
#define QTZOOMCIRCLEWAIT_H

#include <QWidget>

class QtZoomCircleWait : public QWidget
{
    Q_OBJECT
public:
    explicit QtZoomCircleWait(QWidget *parent = nullptr);

    void setColor(QColor color);
    void setZoomOut(bool zoomOut);
    void setZoomDelta(int delta);
    void setMinRadius(int value);
    void setRadius(int value);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void drawZoomCricleWait(QPainter &p);

private:
    void fixRaidus(int max);

private:
    QColor m_color;
    bool m_zoomOut;
    int m_zoomDelta;
    int m_radius;
    int m_minRadius;

    QTimer *m_timer;
};

#endif // QTZOOMCIRCLEWAIT_H
