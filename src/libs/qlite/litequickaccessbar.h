/***************************************************************************
 **
 **  Copyright (C) 2021-2022 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  Powered by QtitanRibbon
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
#ifndef LITEQUICKACCESSBAR_H
#define LITEQUICKACCESSBAR_H

#include <QToolBar>
#include <QMenu>
#include "qlite_global.h"

QLITE_BEGIN_NAMESPACE

class LiteQuickAccessBarPrivate;

/* LiteQuickAccessBar */
class QLITE_SHARED_EXPORT LiteQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit LiteQuickAccessBar(QWidget *parent = Q_NULLPTR);
    virtual ~LiteQuickAccessBar();

public:
    QAction *actionCustomizeButton() const;
    void setActionVisible(QAction *action, bool visible);
    bool isActionVisible(QAction *action) const;
    int visibleCount() const;

Q_SIGNALS:
    void showCustomizeMenu(QMenu *menu);

private Q_SLOTS:
    void customizeAction(QAction *action);
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();

protected:
    virtual bool event(QEvent *event);
    virtual void actionEvent(QActionEvent *event);

private:
    LiteQuickAccessBarPrivate *d;
};

QLITE_END_NAMESPACE

#endif // LITEQUICKACCESSBAR_H
