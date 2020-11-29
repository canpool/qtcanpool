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
#ifndef LITETABWIDGET_H
#define LITETABWIDGET_H

#include "litewidget.h"
#include "qlite_global.h"

QLITE_BEGIN_NAMESPACE

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

QLITE_END_NAMESPACE

#endif // LITETABWIDGET_H
