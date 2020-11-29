/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
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
#ifndef FANCYQUICKACCESSBAR_H
#define FANCYQUICKACCESSBAR_H

#include <QToolBar>
#include <QMenu>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyQuickAccessBarPrivate;
class QCANPOOL_SHARED_EXPORT FancyQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    explicit FancyQuickAccessBar(QWidget *parent = nullptr);

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
    FancyQuickAccessBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYQUICKACCESSBAR_H
