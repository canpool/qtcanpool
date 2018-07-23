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
#ifndef FANCYNAVBAR_H
#define FANCYNAVBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class FancyNavBarPrivate;
class QSplitter;
class QCANPOOL_SHARED_EXPORT FancyNavBar : public QWidget
{
    Q_OBJECT
public:
    explicit FancyNavBar(QWidget *parent = 0);

    void add(const QString &title, QWidget *widget, QWidget *appendix = 0);
    void remove(QWidget *widget);
    void select(QWidget *widget);

    QWidget* panel() const;
    void setPanelColor(const QColor &color);
    void setSplitter(QSplitter *splitter);

    void setSideButtonVisable(bool visable);
    void setSideButtonEnable(bool enable);
    void setSideExpand(bool expand);

    void setHoverColor(const QColor &color);
    void setPressColor(const QColor &color);
    void setTextColor(const QColor &color);

signals:
    void sigSideExpand(bool expand);

public slots:

private:
    FancyNavBarPrivate *d;
};

#endif // FANCYNAVBAR_H
