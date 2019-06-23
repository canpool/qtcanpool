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
#ifndef LITEMODEBAR_H
#define LITEMODEBAR_H

#include "litetabbar.h"
#include "qlite_global.h"

class QStackedWidget;

namespace QLite {

class LiteModeBarPrivate;

class QLITE_SHARED_EXPORT LiteModeBar : public LiteTabBar
{
    Q_OBJECT
public:
    explicit LiteModeBar(QStackedWidget *modeStack, QWidget *parent = nullptr);
    ~LiteModeBar();

    int addMode(QWidget *widget, const QString &);
    int addMode(QWidget *widget, const QIcon& icon, const QString &label);

    int insertMode(int index, QWidget *widget, const QString &);
    int insertMode(int index, QWidget *widget, const QIcon& icon, const QString &label);

    void removeMode(int index);

    QWidget *currentMode() const;
    QWidget *Mode(int index) const;
    int indexOf(QWidget *widget) const;

    void setModeEnabled(QWidget *widget, bool enable);
    void setModeVisible(QWidget *widget, bool visible);

private:
    LiteModeBarPrivate *d;
};

} // namespace QLite

#endif // LITEMODEBAR_H
