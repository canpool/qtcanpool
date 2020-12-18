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
#ifndef FANCYNAVBUTTON_H
#define FANCYNAVBUTTON_H

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
#include <QIcon>
#include <QColor>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyNavButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FancyNavButton(QWidget *parent = nullptr);

    enum ButtonType { Button, Action};

    void select(bool selected);

    void setType(ButtonType type) { m_type = type;}
    ButtonType type() const { return m_type;}

    void setHoverColor(const QColor &color) { m_hoverColor = color;}
    QColor hoverColor() const { return m_hoverColor;}

    void setPressColor(const QColor &color) { m_pressColor = color;}
    QColor pressColor() const { return m_pressColor;}

    void setTextColor(const QColor &color);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);
    void painterInfo(QColor &color);

private:
    bool m_isMouseOver;
    bool m_isMousePress;
    ButtonType m_type;

    QColor m_hoverColor;
    QColor m_pressColor;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYNAVBUTTON_H
