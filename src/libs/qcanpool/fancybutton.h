/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
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

#include <QToolButton>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyButtonPrivate;
class FancyButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FancyButton(QWidget *parent = nullptr);
    ~FancyButton();

    void select(bool selected);

    void setHoverColor(const QColor &color) { m_hoverColor = color;}
    QColor hoverColor() const { return m_hoverColor;}

    void setPressColor(const QColor &color) { m_pressColor = color;}
    QColor pressColor() const { return m_pressColor;}

    void setTextColor(const QColor &color);
    QColor textColor() const { return m_textColor; }

    void setColor(const QColor &color);

    void setNormalColor(const QColor &color);
    QColor normalColor() const { return m_normalColor; }

    void setHasMenu(bool has);
    void setHasBorder(bool has);
    void setRound(bool round);

private:
    void painterInfo(QColor &color);

    bool actionHasMenu();

signals:
    void menuTriggered(QMouseEvent *e);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

public:
    bool m_hasMenu = false;

private:
    bool m_bMouseHover;
    bool m_bMousePress;
    bool m_hasBorder;
    bool m_bRound;

    QColor m_hoverColor;
    QColor m_pressColor;
    QColor m_textColor;
    QColor m_normalColor;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBUTTON_H
