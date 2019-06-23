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
#ifndef LITETABWIDGET_H
#define LITETABWIDGET_H

#include "litewidget.h"
#include "qlite_global.h"

namespace QLite {

class LiteTabBar;
class LiteTabWidgetPrivate;

class QLITE_SHARED_EXPORT LiteTabWidget : public LiteWidget
{
    Q_OBJECT

    Q_PROPERTY(TabPosition tabPosition READ tabPosition WRITE setTabPosition)
public:
    explicit LiteTabWidget(QWidget *parent = nullptr);
    ~LiteTabWidget();

    int addTab(QWidget *widget, const QString &);
    int addTab(QWidget *widget, const QIcon& icon, const QString &label);

    int insertTab(int index, QWidget *widget, const QString &);
    int insertTab(int index, QWidget *widget, const QIcon& icon, const QString &label);

    void removeTab(int index);

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

    void setTabEnabled(QWidget *widget, bool enable);
    void setTabVisible(QWidget *widget, bool visible);

    LiteTabBar *tabBar(void) const;

    enum TabPosition { North, South, West, East };
    Q_ENUM(TabPosition)
    TabPosition tabPosition() const;
    void setTabPosition(TabPosition);

    void setTabButtonStyle(Qt::ToolButtonStyle style);
    void setTabSpacing(int spacing);

signals:

public slots:

private:
    LiteTabWidgetPrivate *d;
};

} // namespace QLite

#endif // LITETABWIDGET_H
