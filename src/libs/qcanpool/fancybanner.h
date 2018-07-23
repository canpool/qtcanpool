/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
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
#ifndef FANCYBANNER_H
#define FANCYBANNER_H

#include <QWidget>
#include "qcanpool_global.h"

class FancyBannerPrivate;
class QCANPOOL_SHARED_EXPORT FancyBanner : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBanner(QWidget *parent = nullptr);

    void addPage(QPixmap pixmap);
    void addPages(QList<QPixmap> pixmaps);

    void startAutoPlay();
    void stopAutoPlay();
    void setDelayTime(int msec, bool start = false);

    void setIndicatorFrontColor(QColor color);
    void setIndicatorBackColor(QColor color);

signals:
    void currentClicked(int index);

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    FancyBannerPrivate *d;
};

#endif // FANCYBANNER_H
