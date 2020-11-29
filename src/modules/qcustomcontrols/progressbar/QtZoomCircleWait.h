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
