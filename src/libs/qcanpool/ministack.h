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
/*******************************************************************************************
  * MiniStack
  *
  *     \author: canpool@163.com
  *     \version: 0.0.1
  *     \date: 2017-10-22
  *
  *     \brief: A mini stack control widget
  *
  *     ---------------------------------------------------------------------------------
  *     |   title    |      appendix        |       tabbar         |     toolbar   |^ * |
  *     |--------------------------------------------------------------------------------
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                   stack                                       |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     |                                                                               |
  *     ---------------------------------------------------------------------------------
  *
  *******************************************************************************************
  */
#ifndef MINISTACK_H
#define MINISTACK_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class MiniStackPrivate;
class QCANPOOL_SHARED_EXPORT MiniStack : public QWidget
{
    Q_OBJECT
public:
    explicit MiniStack(QWidget *parent = nullptr);

    void addTab(const QString &text, QWidget *widget, QWidget *appendix = nullptr);

    void addAction(QAction *action);

    void setIconSize(const QSize &size);

    void setBodySpacing(int spacing);
    void setBodyMargins(int left, int top, int right, int bottom);

    void setTitleSpacing(int spacing);
    void setTitleMargins(int left, int top, int right, int bottom);

    void setTitle(const QString &text);

    void setTitleColor(const QColor &color);
    void setSpacerColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

    void setTabBackgroundColor(const QColor &color);
    void setTabTextColor(const QColor &color);
    void setTabHoverBackgroundColor(const QColor &color);
    void setTabHoverTextColor(const QColor &color);
    void setTabSelectedBackgroundColor(const QColor &color);
    void setTabSelectedTextColor(const QColor &color);

signals:
    void sigSetupClicked();

private:
    MiniStackPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // MINISTACK_H
