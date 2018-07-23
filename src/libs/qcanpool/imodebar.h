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
#ifndef IMODEBAR_H
#define IMODEBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class QStackedWidget;
class IMode;
class QAction;
class IModeBarPrivate;

class QCANPOOL_SHARED_EXPORT IModeBar : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Direction)
public:
    enum Direction { Horizontal, Vertical };
    enum ActionPosition { AP_Front, AP_Middle, AP_Back };
    enum ButtonType { Mode, Action };

    explicit IModeBar(QStackedWidget *modeStack, Direction direction = Vertical, QWidget *parent = 0);
    ~IModeBar();

    void addMode(IMode *mode);
    void selectMode(IMode *mode);
    void setEnabled(IMode *mode, bool enable);
    void setVisible(IMode *mode, bool visible);

    void setButtonStyle(ButtonType type, Qt::ToolButtonStyle style);
    void setButtonFont(ButtonType type, QFont &font);
    void setIconSize(QSize size);
    void setButtonSpace(int space);
    void setActionStyle(QAction* action, Qt::ToolButtonStyle style);

    void addAction(QAction *action, ActionPosition position = AP_Back);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    IMode* currentMode() const;
    IMode* mode(int);

    QWidget* spacer() const;
    QWidget* line() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

signals:
    void currentChanged(int);

private:
    IModeBarPrivate *d;
    Direction m_direction;
};

#endif // IMODEBAR_H
