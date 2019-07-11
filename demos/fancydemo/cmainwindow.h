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
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QTime>

class QMenu;
class QLabel;
class QToolBar;
class QStatusBar;
class QStackedWidget;

class ChartsMode;
class CustomMode;
class MenuMode;
class FancyTabWidget;
class ModeManager;

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

    static CMainWindow* instance();

private:
    void createWindow();
    void createToolBar();
    void createStatusBar();
    void createDockWidget();
    void createCentralWidget();
    void createModeBar();

    void createOthers();
    void deleteOthers();

    void createConnects();

    void readSettings();
    void writeSettings();


private:
    static CMainWindow *m_instance;
    ModeManager *m_modeManager;
    FancyTabWidget *m_modeStack;
    QStatusBar *m_statusBar;

    ChartsMode *m_pChartsMode;
    CustomMode *m_pCustomMode;
    MenuMode *m_pMenuMode;

};

#endif // CMAINWINDOW_H
