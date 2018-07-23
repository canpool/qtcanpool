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
#ifndef FANCYBUTTON_H
#define FANCYBUTTON_H

#include <QToolButton>

class FancyButtonPrivate;
class FancyButton : public QToolButton
{
    Q_OBJECT
public:
    explicit FancyButton(QWidget *parent = 0);
    ~FancyButton();

    void select(bool selected);

    void setHoverColor(const QColor &color) { m_hoverColor = color;}
    QColor hoverColor() const { return m_hoverColor;}

    void setPressColor(const QColor &color) { m_pressColor = color;}
    QColor pressColor() const { return m_pressColor;}

    void setTextColor(const QColor &color);

    void setColor(const QColor &color);

    void setNormalColor(const QColor &color);

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

#endif // FANCYBUTTON_H
