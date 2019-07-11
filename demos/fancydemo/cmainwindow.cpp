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
#include "cmainwindow.h"

#include "cwidgets.h"
#include "chartsmode.h"
#include "custommode.h"
#include "menumode.h"

#include "qcanpool/fancytabwidget.h"
#include "qcanpool/fancytabbar.h"
#include "qcanpool/modemanager.h"
#include "qcanpool/screenhelper.h"

#define COMPANY_NAME    QString::fromUtf8("QT FRAMEWORK")

CMainWindow *CMainWindow::m_instance = nullptr;

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_instance = this;

    m_modeStack = new FancyTabWidget(this);
    m_modeManager = new ModeManager(m_modeStack);
    m_statusBar = m_modeStack->statusBar();

    createWindow();

    setAcceptDrops(true);

    setWindowTitle(tr("Customized Window"));
    setWindowIcon(QIcon(":/main/logo"));
    setMinimumSize(QSize(600,400));
    setMouseTracking(true);

    QRect geom = ScreenHelper::normalRect();
    resize(geom.width(), geom.height());
    raise();
    activateWindow();
}

CMainWindow::~CMainWindow()
{
    delete m_modeManager;
    deleteOthers();
}

CMainWindow *CMainWindow::instance()
{
    return m_instance;
}

void CMainWindow::createWindow()
{
    createToolBar();
    createStatusBar();
    createDockWidget();
    createCentralWidget();
    createModeBar();

    createOthers();
}

void CMainWindow::createToolBar()
{
    setContextMenuPolicy(Qt::NoContextMenu);
}


void CMainWindow::createDockWidget()
{
}

void CMainWindow::createStatusBar()
{

}

void CMainWindow::createCentralWidget()
{
    setCentralWidget(m_modeStack);
}

void CMainWindow::createModeBar()
{
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,0,0,20));

    FancyTabBar *modeBar = m_modeStack->tabBar();
    modeBar->setPalette(palette);
    modeBar->setAutoFillBackground(true);

    m_pChartsMode = new ChartsMode();
    m_pCustomMode = new CustomMode();
    m_pMenuMode = new MenuMode();

    m_modeManager->objectAdded(m_pCustomMode);
    m_modeManager->objectAdded(m_pChartsMode);
    m_modeManager->objectAdded(m_pMenuMode);

    m_modeManager->setCurrentMode(m_pCustomMode);

//    m_modeManager->setEnabled(m_pChartsMode, false);

    // mode action
    QAction *action = new QAction(QIcon(":/tools/start"), tr("Start"),this);
    action->setShortcut(tr("Ctrl+R"));
    action->setToolTip(tr("Start <i>Ctrl+R</i>"));
    action->setEnabled(false);
    modeBar->addAction(action);

    action = new QAction(QIcon(":/tools/suspend"), tr("Suspend"),this);
    modeBar->addAction(action);
    action->setEnabled(false);
    action = new QAction(QIcon(":/tools/stop"), tr("Stop"),this);
    modeBar->addAction(action);
    action->setEnabled(false);

    modeBar->setTabIconSize(QSize(24,24));
    modeBar->setTextColor(QColor(10,10,10));
    modeBar->setSelectedTextColor(QColor(255,255,255));
    modeBar->setHoverColor(QColor(180,180,180));
}

void CMainWindow::createOthers()
{

}

void CMainWindow::deleteOthers()
{

}

void CMainWindow::createConnects()
{

}

void CMainWindow::readSettings()
{
    QSettings settings("Canpool", "qtproject");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    move(pos);
    resize(size);
}

void CMainWindow::writeSettings()
{
    QSettings settings("Canpool", "qtproject");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}
