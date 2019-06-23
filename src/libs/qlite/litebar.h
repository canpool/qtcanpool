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
#ifndef LITEBAR_H
#define LITEBAR_H

#include <QObject>
#include "qlite_global.h"

class QMouseEvent;
class QLabel;
class QToolButton;
class QToolBar;
class QAction;

namespace QLite {

class LiteBarPrivate;

class QLITE_SHARED_EXPORT LiteBar : public QObject
{
    Q_OBJECT
public:
    explicit LiteBar(QWidget *parent);
    ~LiteBar();

    QWidget *mainWidget() const;
    QLabel *titleLabel() const;
    QToolButton *logoButton() const;
    QWidget *sysToolBar() const;

    void addAction(QAction *action);

    void setCloseIcon(QIcon &icon);
    void setMaximizeIcon(QIcon &icon);
    void setMinimizeIcon(QIcon &icon);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    void updateWidgetFlags();

signals:
    void sigWidgetResizable(bool resizable);
    void sigWidgetMovable(bool movable);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private:
    LiteBarPrivate *d;
};

} // namespace QLite

#endif // LITEBAR_H
