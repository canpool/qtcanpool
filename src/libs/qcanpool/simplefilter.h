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
#ifndef SIMPLEFILTER_H
#define SIMPLEFILTER_H

#include "qcanpool_global.h"
#include <QWidget>

class QTreeWidget;
class QTableWidget;
class QTreeView;
class SimpleFilterPrivate;

class QCANPOOL_SHARED_EXPORT SimpleFilter : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleFilter(QTableWidget *table, QWidget *parent = 0);
    explicit SimpleFilter(QTreeWidget *tree, QWidget *parent = 0);
    explicit SimpleFilter(QTreeView *view, QWidget *parent = 0);

    void setTableWidget(QTableWidget *table);
    void setTreeWidget(QTreeWidget *tree);
    void setTreeView(QTreeView *view);

    void clear();

private:
    SimpleFilterPrivate *d;
};

#endif // SIMPLEFILTER_H
