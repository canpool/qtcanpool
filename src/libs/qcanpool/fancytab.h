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
#ifndef FANCYTAB_H
#define FANCYTAB_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "fancybutton.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyTabPrivate;
class FancyTab : public FancyButton
{
    Q_OBJECT
public:
    enum TabType { Mode, Action };

    explicit FancyTab(QWidget *parent = nullptr);
    FancyTab(const QIcon &icon, QWidget *parent = nullptr);
    virtual ~FancyTab();

    virtual void select(bool selected);
    bool isSelected();

    void setType(TabType type);
    TabType type() const;

    void setSelectedTextColor(const QColor &color);

    void setColor(const QColor &color);

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);

private:
    void update();

private:
    FancyTabPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTAB_H
