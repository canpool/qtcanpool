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
#ifndef SKINITEM_H
#define SKINITEM_H

#include <QWidget>
#include "qcanpool_global.h"

class SkinItemPrivate;
class QCANPOOL_SHARED_EXPORT SkinItem : public QWidget
{
    Q_OBJECT
public:
    explicit SkinItem(QWidget *parent = 0);
    ~SkinItem();

    void setPixmapName(const QString &name);
    QString pixmapName()const;

    void select(bool selected);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    SkinItemPrivate *d;
};

#endif // SKINITEM_H
