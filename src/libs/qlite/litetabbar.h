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
#ifndef LITETABBAR_H
#define LITETABBAR_H

#include "litewidget.h"
#include "qlite_global.h"

QLITE_BEGIN_NAMESPACE

class LiteTabBarPrivate;

class QLITE_SHARED_EXPORT LiteTabBar : public LiteWidget
{
    Q_OBJECT
public:
    enum Direction { Horizontal, Vertical };
    enum ActionPosition { Front, Middle, Back };

    explicit LiteTabBar(QWidget *parent = nullptr);
    virtual ~LiteTabBar();

    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon&icon, const QString &text);

    void removeTab(int index);

    int currentIndex() const;
    int count() const;

    void addAction(QAction *action, ActionPosition position = Middle);

    void setTabEnabled(int index, bool enable);
    void setTabVisible(int index, bool visible);

    void setTabButtonStyle(Qt::ToolButtonStyle style);
    void setActionStyle(QAction *action, Qt::ToolButtonStyle style);
    void setTabSpacing(int spacing);

    void setIconSize(QSize size);

    void setDirection(Direction direction);

signals:
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    LiteTabBarPrivate *d;
};

QLITE_END_NAMESPACE

#endif // LITETABBAR_H
