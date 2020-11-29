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

class MiniCardPrivate;
class MiniCard : public QWidget
{
    Q_OBJECT
public:
    explicit MiniCard(QWidget *parent = nullptr);

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
