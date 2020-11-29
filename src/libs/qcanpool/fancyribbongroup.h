/***************************************************************************
 **
 **  Copyright (C) 2020 MaMinJie <canpool@163.com>
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
#ifndef FANCYRIBBONGROUP_H
#define FANCYRIBBONGROUP_H

#include <QWidget>
#include <QIcon>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyRibbonGroupPrivate;
class QCANPOOL_SHARED_EXPORT FancyRibbonGroup : public QWidget
{
    Q_OBJECT
public:
    enum GroupSize {
        GroupLarge, GroupMedium, GroupSmall
    };

public:
    explicit FancyRibbonGroup(const QString &title, QWidget *parent = nullptr);
    ~FancyRibbonGroup();

    void setTitle(const QString &title);
    QString title() const;

    void setTitleFont(const QFont &font);
    QFont titleFont() const;

    void setOptionButtonVisible(bool visible = true);
    bool isOptionButtonVisible() const;

    QAction *addAction(const QIcon &icon, const QString &text, GroupSize size);
    void addAction(QAction *action, GroupSize size); // for action that has menu

    void addWidget(QWidget *widget);
    void removeWidget(QWidget *widget);

signals:
    void optionClicked();

private:
    FancyRibbonGroupPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYRIBBONGROUP_H
