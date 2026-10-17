/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockstylesheet.h"
#include "qxdock/dockiconprovider.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QDir>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent, bool screenshotMode)
    : QMainWindow(parent)
    , m_screenshotMode(screenshotMode)
{
    QMenuBar *mb = menuBar();
    QMenu *viewMenu = mb->addMenu(tr("View"));

    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    // The dock look is the application's to install, and res/dockstyle.qrc is where
    // this demo carries it - the library no longer holds a copy and no longer
    // styles the DockWindow for us. The flag is set just above and is what picks
    // the sheet, so the two lines are one decision read in two places.
    qApp->setStyleSheet(dockStyleSheet(true));
    // Step 2 of the theme work: tint the dock's built-in SVG icons to the
    // application palette so they follow light/dark instead of staying the
    // hard-coded black the SVGs ship with. The library only provides the helper;
    // this demo opts in, the same way it installs the stylesheet. A palette
    // change re-tints and re-applies them (see eventFilter).
    applyDockIcons(qApp->palette(), this);
    qApp->installEventFilter(this);
    // set auto hide config before new DockWindow
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    // C5: also enable the auto-hide slide animation.
    DockManager::setAutoHideConfigFlag(DockManager::AutoHideAnimationEnabled, true);

    m_dockWindow = new DockWindow();

    // C3: react to a dock widget dropped back onto the window through the native
    // OS drag (e.g. re-dropping a detached dock widget from outside the app).
    connect(m_dockWindow, &DockWindow::dockWidgetDropped, this, [](const QString &name, const QPoint &pos) {
        qDebug() << "dock widget dropped:" << name << "at" << pos;
    });

    QTextEdit *te = nullptr;
    QWidget *cw = nullptr;

    DockWidget *w = nullptr;

    te = new QTextEdit(tr("AutoScrollArea"));
    w = new DockWidget(tr("dock1"));
    w->setIcon(QIcon(":/res/tab.svg"));
    w->setWidget(te);
    viewMenu->addAction(w->toggleViewAction());
    DockPanel *panel = m_dockWindow->addDockWidget(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(tr("dock2"));
    viewMenu->addAction(w->toggleViewAction());
    m_dockWindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    cw = new QWidget();
    w = new DockWidget(tr("looooooooooooooooongdock"));
    w->setWidget(cw, DockWidget::ForceScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    m_dockWindow->addDockWidget(Qx::RightDockWidgetArea, w, panel);

    w = new DockWidget(tr("dock4"));
    viewMenu->addAction(w->toggleViewAction());
    m_dockWindow->addDockWidget(Qx::CenterDockWidgetArea, w, panel);   // tabified with dock1

    w = new DockWidget(tr("dock5"));
    viewMenu->addAction(w->toggleViewAction());
    // tabified with dock1, because of dock1 panel is Left area panel
    m_dockWindow->addDockWidgetTab(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(tr("dock3"));
    viewMenu->addAction(w->toggleViewAction());
    panel = m_dockWindow->addDockWidget(Qx::BottomDockWidgetArea, w);

    cw = new QWidget();
    w = new DockWidget(tr("dock6"));
    w->setWidget(cw, DockWidget::ForceNoScrollArea);
    viewMenu->addAction(w->toggleViewAction());
    m_dockWindow->addDockWidgetTab(w, panel);   // tabified with dock3

    cw = new QWidget();
    w = new DockWidget(tr("auto1"));
    w->setWidget(cw);
    viewMenu->addAction(w->toggleViewAction());
    m_dockWindow->addAutoHideDockWidget(Qx::DockSideBarLeft, w);

    // edit menu
    QMenu *editMenu = mb->addMenu(tr("Edit"));
    editMenu->addAction(tr("saveState"), [this]() {
        saveToSettings();
    });
    editMenu->addAction(tr("restoreState"), [this]() {
        restoreFromSettings();
    });
    // C2: persist the docking layout to / restore it from an XML file.
    const QString dockStateFile = qApp->applicationDirPath() + QDir::separator() + QStringLiteral("dockstate.xml");
    editMenu->addAction(tr("saveStateToFile"), [this, dockStateFile]() {
        const bool ok = m_dockWindow->saveStateToFile(dockStateFile);
        qDebug() << "saveStateToFile" << dockStateFile << (ok ? "ok" : "FAILED");
    });
    editMenu->addAction(tr("restoreStateFromFile"), [this, dockStateFile]() {
        const bool ok = m_dockWindow->restoreStateFromFile(dockStateFile);
        qDebug() << "restoreStateFromFile" << dockStateFile << (ok ? "ok" : "FAILED");
    });

    setCentralWidget(m_dockWindow);

    QToolBar *tb = addToolBar(tr("tb1"));
    tb->setObjectName("tb1");
    QAction *a = tb->addAction("Lock Workspace");
    a->setCheckable(true);
    a->setChecked(false);
    connect(a, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            m_dockWindow->lockDockWidgetFeaturesGlobally();
        } else {
            m_dockWindow->lockDockWidgetFeaturesGlobally(DockWidget::NoDockWidgetFeatures);
        }
    });

    resize(800, 600);

    if (!m_screenshotMode) {
        restoreFromSettings();
    }
}

void MainWindow::saveToSettings()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    settings.setValue("mainWindow/Geometry", this->saveGeometry());
    settings.setValue("mainWindow/State", QMainWindow::saveState());
    settings.setValue("mainWindow/DockingState", m_dockWindow->saveState());
}

void MainWindow::restoreFromSettings()
{
    QSettings settings("Settings.ini", QSettings::IniFormat);
    this->restoreGeometry(settings.value("mainWindow/Geometry").toByteArray());
    QMainWindow::restoreState(settings.value("mainWindow/State").toByteArray());
    m_dockWindow->restoreState(settings.value("mainWindow/DockingState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_screenshotMode) {
        saveToSettings();
    }
    QMainWindow::closeEvent(event);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // The OS/theme palette changed (e.g. Windows light<->dark). Re-tint the dock
    // icons to the new palette and re-apply them to every existing dock button.
    if (watched == qApp && event->type() == QEvent::ApplicationPaletteChange) {
        applyDockIcons(qApp->palette(), this);
    }
    return QMainWindow::eventFilter(watched, event);
}
