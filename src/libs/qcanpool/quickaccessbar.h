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
#ifndef QUICKACCESSBAR_H
#define QUICKACCESSBAR_H

#include "qcanpool_global.h"
#include <QToolBar>
#include <QMenu>

class QuickAccessBarPrivate;
class QCANPOOL_SHARED_EXPORT QuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit QuickAccessBar(QWidget *parent = 0);

public:
    QAction* actionCustomizeButton() const;
    void setActionVisible(QAction* action, bool visible);
    bool isActionVisible(QAction* action) const;
    int visibleCount() const;

    void setHoverColor(const QColor &color);
    void setPressColor(const QColor &color);
    void setTextColor(const QColor &color);

protected:
    virtual void actionEvent(QActionEvent*);

private:
    QuickAccessBarPrivate *d;
};

#endif // QUICKACCESSBAR_H
