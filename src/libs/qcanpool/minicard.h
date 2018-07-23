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
/*************************************************************
  * MiniCard
  *
  *     \author: canpool@163.com
  *     \version: 0.0.1
  *     \date: 2017-10-22
  *
  *     \brief: A portable display control widget
  *
  *     -------------------------------------
  *     |   title               info        |
  *     |------------------------------------
  *     |                                   |
  *     |   value                           |
  *     |                                   |
  *     |   desc                note        |
  *     -------------------------------------
  *
  *************************************************************
  */
#ifndef MINICARD_H
#define MINICARD_H

#include <QWidget>
#include "qcanpool_global.h"

class MiniCardPrivate;

class QCANPOOL_SHARED_EXPORT MiniCard : public QWidget
{
    Q_OBJECT
public:
    explicit MiniCard(QWidget *parent = 0);

    void setTitle(const QString &text);
    void setInfo(const QString &text);
    void setValue(const QString &text);
    void setDesc(const QString &text);
    void setNote(const QString &text);

    void setToolTip(const QString &tips);   // for info

    void setTitleColor(const QColor &color);
    void setInfoColor(const QColor &color);
    void setInfoBackgroundColor(const QColor &color);
    void setSpacerColor(const QColor &color);
    void setValueColor(const QColor &color);
    void setDescColor(const QColor &color);
    void setNoteColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

signals:
    void sigNoteClicked(QString notes);
    void sigInfoClicked(QString info);

private:
    MiniCardPrivate *d;

};

#endif // MINICARD_H
