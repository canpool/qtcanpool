/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockautohidecontainer.h"
#include "qxapp/qxthemebundle.h"
#include "qxapp/qxthemeinstaller.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QActionGroup>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTextEdit>
#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStringList>

QX_DOCK_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent, bool screenshotMode, const QString &themeId)
    : QMainWindow(parent)
    , m_screenshotMode(screenshotMode)
{
    QMenuBar *mb = menuBar();
    QMenu *viewMenu = mb->addMenu(tr("View"));

    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    // The dock look is the application's to install, and res/dockstyle.qrc is where
    // this demo carries it - the library no longer holds a copy and no longer
    // styles the DockWindow for us. The flag is set just above and is what picks
    // the sheet's focus fragment, so the two lines are one decision read in two
    // places. A theme decides the palette and the style sheet the whole
    // application wears, and the dock's sheet is a second application-level one:
    // that is a single string, so installing the theme's takes the dock's with it.
    createTheme(themeId);
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
    m_autoHideDockWidget = w;

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

void MainWindow::createTheme(const QString &themeId)
{
    QxThemeManager *engine = QxThemeManager::instance();
    // The bundle is registered here rather than in main(), so the window is the
    // one place that decides what this demo offers.
    QxApp::registerBundledThemes(engine);
    // Following the desktop is worth showing in a dock demo above all: the
    // engine swaps between these two ids on the system's own light/dark, and
    // hands the color scheme back to the platform while it does, which is what
    // keeps the window frame agreeing with the dock.
    engine->setSystemThemePair(QStringLiteral("soft-light"), QStringLiteral("soft-dark"));
    // The engine has already applied whatever it restored. A named theme
    // replaces it; without one, a demo that has neither a restored theme nor a
    // command-line one would sit on the palette Qt came up with - the system's,
    // not one of ours.
    if (!themeId.isEmpty()) {
        engine->setTheme(themeId);
    } else if (!engine->hasTheme(engine->theme())) {
        engine->setTheme(QStringLiteral("soft-light"));
    }

    // The one place the theme is installed from: its sheet, the dock's sheet
    // after it and the dock's icons, redone on a theme switch or a platform
    // palette change. Built before the docks exist on purpose - the icon set it
    // installs is what they take their glyphs from as they are built.
    m_theme = new QxApp::ThemeInstaller(engine, this, this);

    createThemeMenu();
}

void MainWindow::applyTheme(const QString &themeId)
{
    // One call, because the window installs the theme in one place: the
    // ThemeInstaller m_theme owns listens to themeChanged and redoes the sheet,
    // the dock's sheet and the glyphs. Adding anything here would be a second
    // path to the same result, which is the mistake the dock's own styling
    // history is a record of.
    QxThemeManager::instance()->setTheme(themeId);
}

void MainWindow::createThemeMenu()
{
    QxThemeManager *engine = QxThemeManager::instance();
    QMenu *menu = menuBar()->addMenu(tr("Theme"));

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    // One entry per registration: the ids are the engine's, the names are what
    // was registered under them, so a family added to res/themes.qrc appears
    // here without this file being told about it.
    const QStringList ids = engine->themeIds();
    for (const QString &id : ids) {
        QAction *entry = menu->addAction(engine->themeInfo(id).name);
        entry->setCheckable(true);
        entry->setChecked(engine->theme() == id);
        entry->setData(id);
        group->addAction(entry);
    }

    QAction *followSystem = menu->addAction(tr("Follow system"));
    followSystem->setCheckable(true);
    followSystem->setChecked(engine->followSystemTheme());
    menu->addSeparator();
    menu->addAction(followSystem);

    connect(group, &QActionGroup::triggered, this, [](QAction *entry) {
        QxThemeManager::instance()->setTheme(entry->data().toString());
    });
    connect(engine, &QxThemeManager::themeChanged, this, [group](const QString &id) {
        // Following the system swaps the theme without the menu being clicked,
        // so the check marks are read back from the engine rather than left
        // where the last click put them.
        const QList<QAction *> entries = group->actions();
        for (QAction *entry : entries) {
            entry->setChecked(entry->data().toString() == id);
        }
    });
    connect(followSystem, &QAction::toggled, this, [engine](bool on) {
        engine->setFollowSystemTheme(on);
    });
}

void MainWindow::expandAutoHidePanel()
{
    if (!m_autoHideDockWidget) {
        return;
    }
    DockAutoHideContainer *container = m_autoHideDockWidget->autoHideContainer();
    if (container) {
        container->toggleCollapseState();
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
