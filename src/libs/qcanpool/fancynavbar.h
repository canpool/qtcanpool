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
#ifndef FANCYNAVBAR_H
#define FANCYNAVBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class QSplitter;

QCANPOOL_BEGIN_NAMESPACE

class FancyNavBarPrivate;
class QCANPOOL_SHARED_EXPORT FancyNavBar : public QWidget
{
    Q_OBJECT
public:
    explicit FancyNavBar(QWidget *parent = nullptr);

    void add(const QString &title, QWidget *widget, QWidget *appendix = nullptr);
    void remove(QWidget *widget);
    void select(QWidget *widget);

    QWidget* panel() const;
    void setPanelColor(const QColor &color);
    void setSplitter(QSplitter *splitter);

    void setSideButtonVisable(bool visable);
    void setSideButtonEnable(bool enable);
    void setSideExpand(bool expand);

    void setHoverColor(const QColor &color);
    void setPressColor(const QColor &color);
    void setTextColor(const QColor &color);

signals:
    void sigSideExpand(bool expand);

private:
    FancyNavBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYNAVBAR_H
