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
#ifndef IMODEBAR_P_H
#define IMODEBAR_P_H

#include <QObject>
#include <QMap>

class QStackedWidget;
class QBoxLayout;
class QAction;
class IModeBar;
class IModeButton;
class IMode;

class IModeBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit IModeBarPrivate(QObject *parent = 0);
    ~IModeBarPrivate();

    void selectMode(IMode *mode);
    void setEnabled(IMode *mode, bool enable);
    void setVisible(IMode *mode, bool visible);

public:
    QMap<IModeButton*, IMode*> m_modeMap;
//    QList<IModeButton*> m_modeButtons;
    QList<IMode*> m_modes;
    QList<IModeButton *> m_actionButtons;
    QMap<QAction*, IModeButton*> m_actionMap;
    QStackedWidget *m_modeStack;
    QBoxLayout *m_modeLayout;
    QBoxLayout *m_frontActionLayout;
    QBoxLayout *m_middleActionLayout;
    QBoxLayout *m_backActionLayout;

    QWidget *m_spacer;
    QWidget *m_line;

    Qt::ToolButtonStyle m_modeStyle;
    Qt::ToolButtonStyle m_actionStyle;

private slots:
    void switchMode();

};

#endif // IMODEBAR_P_H
