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
#ifndef FANCYMODEBAR_P_H
#define FANCYMODEBAR_P_H

#include <QObject>
#include <QMap>

class QStackedWidget;
class QBoxLayout;
class QAction;
class FancyModeBar;
class FancyTab;
class FancyMode;

class FancyModeBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyModeBarPrivate(QObject *parent = nullptr);
    ~FancyModeBarPrivate();

    void selectMode(FancyMode *mode);
    void setEnabled(FancyMode *mode, bool enable);
    void setVisible(FancyMode *mode, bool visible);

public:
    QMap<FancyTab*, FancyMode*> m_modeMap;
//    QList<FancyTab*> m_modeButtons;
    QList<FancyMode*> m_modes;
    QList<FancyTab *> m_actionButtons;
    QMap<QAction*, FancyTab*> m_actionMap;
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

#endif // FANCYMODEBAR_P_H
