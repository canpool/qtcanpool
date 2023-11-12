/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "mainwindow.h"


#include "plugindialog.h"

#include <app/app_version.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/hostosinfo.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/stringutils.h>

#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
#include <QActionGroup>

using namespace ExtensionSystem;
using namespace Utils;

namespace Core {
namespace Internal {


MainWindow::MainWindow()
    : QMainWindow()
{
    setWindowTitle(Constants::IDE_DISPLAY_NAME);
    QCoreApplication::setApplicationName(QLatin1String(Constants::IDE_CASED_ID));
    QCoreApplication::setApplicationVersion(QLatin1String(Constants::IDE_VERSION_LONG));
    QCoreApplication::setOrganizationName(QLatin1String(Constants::IDE_SETTINGSVARIANT_STR));

    QMenuBar *mb = new QMenuBar();
    if (!HostOsInfo::isMacHost()) // System menu bar on Mac
        setMenuBar(mb);
    QMenu *menu = mb->addMenu(tr("&Help"));
    QAction *tmpaction = nullptr;

    // About Plugins Action
    tmpaction = new QAction(tr("About &Plugins..."), this);
    tmpaction->setMenuRole(QAction::ApplicationSpecificRole);
    tmpaction->setEnabled(true);
    connect(tmpaction, &QAction::triggered, this, &MainWindow::aboutPlugins);
    menu->addAction(tmpaction);
}


MainWindow::~MainWindow()
{
}

void MainWindow::init()
{

}

void MainWindow::extensionsInitialized()
{
    readSettings();

    QTimer::singleShot(0, this, &MainWindow::restoreWindowState);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // work around QTBUG-43344
    static bool alreadyClosed = false;
    if (alreadyClosed) {
        event->accept();
        return;
    }

    saveSettings();

    saveWindowSettings();

    event->accept();
    alreadyClosed = true;
}

void MainWindow::raiseWindow()
{
    setWindowState(windowState() & ~Qt::WindowMinimized);

    raise();

    activateWindow();
}

void MainWindow::exit()
{
    // this function is most likely called from a user action
    // that is from an event handler of an object
    // since on close we are going to delete everything
    // so to prevent the deleting of that object we
    // just append it
    QTimer::singleShot(0, this,  &QWidget::close);
}

void MainWindow::updateFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)

    // Prevent changing the context object just because the menu or a menu item is activated
    if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
        return;
}

void MainWindow::aboutToShutdown()
{
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);
    hide();
}

static const char settingsGroup[] = "MainWindow";
static const char windowGeometryKey[] = "WindowGeometry";
static const char windowStateKey[] = "WindowState";

void MainWindow::readSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));
    // todo something
    settings->endGroup();
}

void MainWindow::saveSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));
    // todo something
    settings->endGroup();
}

void MainWindow::saveWindowSettings()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    // On OS X applications usually do not restore their full screen state.
    // To be able to restore the correct non-full screen geometry, we have to put
    // the window out of full screen before saving the geometry.
    // Works around QTBUG-45241
    if (Utils::HostOsInfo::isMacHost() && isFullScreen())
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    settings->setValue(QLatin1String(windowGeometryKey), saveGeometry());
    settings->setValue(QLatin1String(windowStateKey), saveState());

    settings->endGroup();
}

void MainWindow::aboutPlugins()
{
    PluginDialog dialog(this);
    dialog.exec();
}

void MainWindow::restoreWindowState()
{
    QSettings *settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));
    if (!restoreGeometry(settings->value(QLatin1String(windowGeometryKey)).toByteArray()))
        resize(1260, 700); // size without window decoration
    restoreState(settings->value(QLatin1String(windowStateKey)).toByteArray());
    settings->endGroup();
    show();
}

} // namespace Internal
} // namespace Core
