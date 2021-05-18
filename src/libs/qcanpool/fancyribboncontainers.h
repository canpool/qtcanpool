/***************************************************************************
 **
 **  Copyright (C) 2021 MaMinJie <canpool@163.com>
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
#ifndef FANCYRIBBONCONTAINERS_H
#define FANCYRIBBONCONTAINERS_H

#include "qcanpool_global.h"
#include <QWidget>

#include "fancyribbongroup.h"

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT FancyRibbonContainer : public QWidget
{
    Q_OBJECT
public:
    explicit FancyRibbonContainer(QWidget *parent = nullptr);
    virtual ~FancyRibbonContainer();
};

/* FancyRibbonGridContainer */
class FancyRibbonGridContainerPrivate;
class QCANPOOL_SHARED_EXPORT FancyRibbonGridContainer : public FancyRibbonContainer
{
    Q_OBJECT
public:
    explicit FancyRibbonGridContainer(QWidget *parent = nullptr);
    virtual ~FancyRibbonGridContainer();

public:
    void addWidget(QWidget *widget);

private:
    FancyRibbonGridContainerPrivate *d;
};

/* FancyRibbonActionContainer */
class FancyRibbonActionContainerPrivate;
class QCANPOOL_SHARED_EXPORT FancyRibbonActionContainer : public FancyRibbonContainer
{
    Q_OBJECT
public:
    explicit FancyRibbonActionContainer(QWidget *parent = nullptr);
    virtual ~FancyRibbonActionContainer();

public:
    QAction *addAction(const QIcon &icon, const QString &text, FancyRibbonGroup::GroupSize size);
    void addAction(QAction *action, FancyRibbonGroup::GroupSize size); // for action that has menu

private:
    FancyRibbonActionContainerPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYRIBBONCONTAINERS_H
