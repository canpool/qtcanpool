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

class MiniStackPrivate;
class QCANPOOL_SHARED_EXPORT MiniStack : public QWidget
{
    Q_OBJECT
public:
    explicit MiniStack(QWidget *parent = 0);

    void addTab(const QString &text, QWidget *widget, QWidget *appendix = 0);

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

#endif // MINISTACK_H
