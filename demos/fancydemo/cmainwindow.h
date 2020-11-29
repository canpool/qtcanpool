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
#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include "qcanpool/qcanpool_global.h"

QCANPOOL_USE_NAMESPACE

class QMenu;
class QLabel;
class QToolBar;
class QStatusBar;
class QStackedWidget;

class ChartsMode;
class CustomMode;
class MenuMode;

QCANPOOL_BEGIN_NAMESPACE
class FancyModeManager;
class FancyTabWidget;
QCANPOOL_END_NAMESPACE

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
    FancyModeManager *m_modeManager;
    FancyTabWidget *m_modeStack;
    QStatusBar *m_statusBar;

    ChartsMode *m_pChartsMode;
    CustomMode *m_pCustomMode;
    MenuMode *m_pMenuMode;

};

#endif // CMAINWINDOW_H
