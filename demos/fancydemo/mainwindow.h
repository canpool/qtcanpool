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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcanpool/fancywindow.h"

class QMenu;
class QLabel;
class QToolBar;
class QStatusBar;
class QStackedWidget;

class ChartsMode;
class CustomMode;
class PaintMode;
class MenuMode;
class FancyTabWidget;
class ModeManager;
class FancyNavBar;

class MainWindow : public FancyWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static MainWindow* instance();

    void checkLicense();

private:
    void createWindow();
    void createQuickAccessBar();
    void createMenuBar();
    void createAdditionalControls();
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
	
public slots:
    void updateTheme();
	
private slots:
    void slotHelp();
    void slotNew();
    void slotSave();
    void slotSkin();
    void slotResizable(bool resizable);
    void slotChangeSkin(QString skinName);
    void slotSetStyle(QAction *action);

private:
    static MainWindow *m_instance;
    ModeManager *m_modeManager;
    FancyTabWidget *m_modeStack;
    QStatusBar *m_statusBar;
    FancyNavBar *m_pNavBar;

    ChartsMode *m_pChartsMode;
    CustomMode *m_pCustomMode;
    PaintMode *m_pPaintMode;
    MenuMode *m_pMenuMode;
    QList<QAction *> m_styleActions;
    QAction *m_styleAction;
};

#endif // MAINWINDOW_H
