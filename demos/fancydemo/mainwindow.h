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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcanpool/fancywindow.h"

QCANPOOL_USE_NAMESPACE

class QMenu;
class QLabel;
class QToolBar;
class QStatusBar;
class QStackedWidget;

class ChartsMode;
class CustomMode;
class PaintMode;
class MenuMode;

QCANPOOL_BEGIN_NAMESPACE
class FancyTabWidget;
class FancyModeManager;
class FancyNavBar;
QCANPOOL_END_NAMESPACE

class MainWindow : public FancyWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow* instance();

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
    FancyModeManager *m_modeManager;
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
