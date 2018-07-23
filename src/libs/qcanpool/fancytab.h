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
#ifndef FANCYTAB_H
#define FANCYTAB_H

#include <QToolButton>
#include <QColor>

class FancyTab : public QToolButton
{
    Q_OBJECT
public:
    enum TabType { Mode, Action };
    explicit FancyTab(QWidget *parent = 0);
    ~FancyTab();

    void select(bool selected);

    void setType(TabType type) { m_type = type; }
    TabType type() const { return m_type; }

    static void setHoverColor(const QColor &color);
    static QColor hoverColor();

    static void setPressColor(const QColor &color);
    static QColor pressColor();

    static void setTextColor(const QColor &color);
    static void setSelectedTextColor(const QColor &color);

    void setColor(const QColor &color);

    void setHasMenu(bool has);

private:
    void painterInfo(QColor &color);
    void painterArrow(QColor &color);

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

    TabType m_type;
};

#endif // FANCYTAB_H
