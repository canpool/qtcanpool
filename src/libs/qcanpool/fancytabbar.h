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
#ifndef FANCYTABBAR_H
#define FANCYTABBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class FancyTabBarPrivate;

class QCANPOOL_SHARED_EXPORT FancyTabBar : public QWidget
{
    Q_OBJECT
public:
    enum Direction {Horizontal, Vertical};
    enum ActionPosition {Front, Middle, Back };
    enum TabType { Mode, Action };
    enum TabStyle {IconOnly, TextOnly, TextBesideIcon, TextUnderIcon};

    explicit FancyTabBar(QWidget *parent = 0);
    ~FancyTabBar();

    void setDirection(Direction direction);

    bool validIndex(int index) const;

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void setTabVisible(int index, bool visible);
    bool isTabVisible(int index) const;

    void insertTab(int index, const QIcon &icon, const QString &label, bool hasMenu);
    void removeTab(int index);

    void setCurrentIndex(int index);
    int currentIndex() const;

    void setTabToolTip(int index, QString toolTip);
    QString tabToolTip(int index) const;

    void setTabStyle(TabType type, TabStyle style);
    void setTabFont(TabType type, QFont &font);
    void setTabSpace(TabType type, int space);
    void setTabIconSize(QSize size);

    int addAction(QAction *action, ActionPosition position = Back);
    void removeAction(QAction *action);
    void setActionStyle(QAction *action, TabStyle style);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

    void setBackgroundColor(const QColor &color);

    void setHeadSpace(int space);

signals:
    void currentChanged(int index);
    void menuTriggered(int index, QPoint pos);

public slots:
    void hideMenu(int index);

private:
    FancyTabBarPrivate *d;
};

#endif // FANCYTABBAR_H
