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
#ifndef LITEMODEBAR_H
#define LITEMODEBAR_H

#include "litetabbar.h"
#include "qlite_global.h"

class QStackedWidget;

QLITE_BEGIN_NAMESPACE

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

QLITE_END_NAMESPACE

#endif // LITEMODEBAR_H
