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
#ifndef FANCYBUTTON_H
#define FANCYBUTTON_H

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

#include <QToolButton>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyButtonPrivate;
class FancyButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FancyButton(QWidget *parent = nullptr);
    virtual ~FancyButton();

    virtual void select(bool selected);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setColor(const QColor &color);

    void setNormalColor(const QColor &color);
    QColor normalColor() const;

    void setHasMenu(bool has);
    bool hasMenu();

    void setHasBorder(bool has);
    void setRound(bool round);

signals:
    void menuTriggered(QMouseEvent *e);
    void doubleClicked();

protected:
    virtual void enterEvent(QEvent *event);
    virtual void leaveEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    virtual void paintEvent(QPaintEvent *event);

private:
    FancyButtonPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBUTTON_H
