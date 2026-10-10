/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "mainwindow.h"

#include "qxapp/qxworkspacemanager.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

// QAction lives in QtGui from Qt 6 on and in QtWidgets before that; the
// module-less form is the one the rest of the project uses for it.
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QtCore/QTimer>
#include <QtGui/QFont>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QX_RIBBON_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QxAppShell(parent)
{
    setWindowTitle(tr("QtCanpool AppShell"));
    resize(1100, 720);

    // Installing the engine first restores the theme the user picked last time
    // and polishes the widgets once, before the window is even shown.
    QxThemeManager::instance();

    createRibbon();
    createPages();
    createDocks();

    setStatusMessage(tr("Ready"));
}

void MainWindow::createRibbon()
{
    const QStyle *s = style();

    RibbonPage *page = ribbonBar()->addPage(tr("&Home"));

    // Pages ------------------------------------------------------------------
    RibbonGroup *pages = page->addGroup(tr("Pages"));

    QAction *action = new QAction(s->standardIcon(QStyle::SP_ComputerIcon), tr("Overview"), this);
    pages->addLargeAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        setCurrentPage(QStringLiteral("overview"));
    });

    action = new QAction(s->standardIcon(QStyle::SP_FileIcon), tr("Editor"), this);
    pages->addLargeAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        setCurrentPage(QStringLiteral("editor"));
    });

    action = new QAction(s->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Log"), this);
    pages->addLargeAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        setCurrentPage(QStringLiteral("log"));
    });

    // Appearance -------------------------------------------------------------
    RibbonGroup *appearance = page->addGroup(tr("Appearance"));

    QActionGroup *themes = new QActionGroup(this);
    themes->setExclusive(true);
    const Theme builtIn[] = {LightOffice2013, LightClassic, LightFancy, DarkWps, DarkOfficePlus};
    for (Theme theme : builtIn) {
        QAction *entry = new QAction(themeName(theme), this);
        entry->setCheckable(true);
        entry->setChecked(QxThemeManager::instance()->theme() == theme);
        entry->setData(int(theme));
        themes->addAction(entry);
        appearance->addSmallAction(entry);
    }
    connect(themes, &QActionGroup::triggered, this, [](QAction *entry) {
        QxThemeManager::instance()->setTheme(Theme(entry->data().toInt()));
    });

    QAction *followSystem = new QAction(tr("Follow system"), this);
    followSystem->setCheckable(true);
    followSystem->setChecked(QxThemeManager::instance()->followSystemTheme());
    appearance->addSmallAction(followSystem);
    connect(followSystem, &QAction::toggled, this, [](bool on) {
        QxThemeManager::instance()->setFollowSystemTheme(on);
    });

    // Layout -----------------------------------------------------------------
    RibbonGroup *layout = page->addGroup(tr("Layout"));

    action = new QAction(s->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), this);
    layout->addMediumAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        saveLayout();
        setStatusMessage(tr("Layout saved"));
    });

    action = new QAction(s->standardIcon(QStyle::SP_BrowserReload), tr("Restore"), this);
    layout->addMediumAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        restoreLayout();
        setStatusMessage(tr("Layout restored"));
    });

    // Workspaces ------------------------------------------------------------
    // The group next to "Layout" is the point of the comparison: Layout keeps
    // the one arrangement the shell always had, while this one keeps as many
    // named arrangements as the user cares to store.
    RibbonGroup *workspaces = page->addGroup(tr("Workspaces"));

    QAction *saveAs = new QAction(s->standardIcon(QStyle::SP_DialogSaveButton), tr("Save as..."), this);
    workspaces->addMediumAction(saveAs);
    connect(saveAs, &QAction::triggered, this, &MainWindow::saveWorkspaceAs);

    m_workspaceMenu = new QMenu(tr("Apply"), this);
    workspaces->addSmallMenu(m_workspaceMenu);

    QAction *forget = new QAction(tr("Forget..."), this);
    workspaces->addMediumAction(forget);
    connect(forget, &QAction::triggered, this, &MainWindow::forgetWorkspace);

    // The menu is rebuilt from the manager rather than appended to, which is
    // what keeps it in step with a workspace being renamed somewhere else.
    refreshWorkspaceMenu();
    connect(workspaceManager(), &QxWorkspaceManager::workspaceSaved, this, &MainWindow::refreshWorkspaceMenu);
    connect(workspaceManager(), &QxWorkspaceManager::workspaceRemoved, this, &MainWindow::refreshWorkspaceMenu);

    // Status -----------------------------------------------------------------
    RibbonGroup *status = page->addGroup(tr("Status"));

    QAction *busy = new QAction(tr("Busy"), this);
    busy->setCheckable(true);
    status->addSmallAction(busy);
    connect(busy, &QAction::toggled, this, [this](bool on) {
        setBusy(on);
        setStatusMessage(on ? tr("Working...") : tr("Ready"));
    });

    QAction *progress = new QAction(tr("Progress"), this);
    status->addMediumAction(progress);
    connect(progress, &QAction::triggered, this, &MainWindow::runProgress);

    m_progressTimer = new QTimer(this);
    connect(m_progressTimer, &QTimer::timeout, this, &MainWindow::stepProgress);
}

void MainWindow::refreshWorkspaceMenu()
{
    m_workspaceMenu->clear();

    const QStringList names = workspaceNames();
    for (const QString &name : names) {
        QAction *entry = m_workspaceMenu->addAction(name);
        connect(entry, &QAction::triggered, this, [this, name]() {
            if (applyWorkspace(name)) {
                setStatusMessage(tr("Workspace: %1").arg(name));
            } else {
                // The usual reason is a dock that the arrangement mentions and
                // this build no longer has.
                showToast(tr("The workspace \"%1\" could not be applied").arg(name), QxToast::Warning);
            }
        });
    }

    if (names.isEmpty()) {
        QAction *empty = m_workspaceMenu->addAction(tr("(none saved yet)"));
        empty->setEnabled(false);
    }
}

void MainWindow::saveWorkspaceAs()
{
    bool accepted = false;
    const QString name =
        QInputDialog::getText(this, tr("Save workspace"), tr("Name:"), QLineEdit::Normal, QString(), &accepted);
    if (!accepted) {
        return;
    }
    if (saveWorkspace(name)) {
        setStatusMessage(tr("Workspace: %1").arg(name.trimmed()));
    } else {
        // The only way this fails is an empty name, which is the one thing the
        // dialog cannot prevent.
        showToast(tr("A workspace needs a name"), QxToast::Warning);
    }
}

void MainWindow::forgetWorkspace()
{
    const QStringList names = workspaceNames();
    if (names.isEmpty()) {
        showToast(tr("There is no workspace to forget"), QxToast::Information);
        return;
    }

    bool accepted = false;
    const QString name =
        QInputDialog::getItem(this, tr("Forget workspace"), tr("Workspace:"), names, 0, false, &accepted);
    if (!accepted) {
        return;
    }
    workspaceManager()->removeWorkspace(name);
    setStatusMessage(tr("Forgot the workspace \"%1\"").arg(name));
}

void MainWindow::runProgress()
{
    // A job of known length: the indicator takes over from the marquee and
    // reports how far it has got, while the message line says what it is.
    m_progressStep = 0;
    setProgressRange(0, 100);
    setProgress(0);
    setStatusMessage(tr("Working... 0%"));
    m_progressTimer->start(40);
}

void MainWindow::stepProgress()
{
    m_progressStep += 2;
    setProgress(m_progressStep);
    setStatusMessage(tr("Working... %1%").arg(m_progressStep));

    if (m_progressStep >= 100) {
        m_progressTimer->stop();
        clearProgress();
        setStatusMessage(tr("Done"));
    }
}

void MainWindow::createPages()
{
    const QStyle *s = style();

    addPage(QStringLiteral("overview"), s->standardIcon(QStyle::SP_ComputerIcon), tr("Overview"), createOverviewPage());
    addPage(QStringLiteral("editor"), s->standardIcon(QStyle::SP_FileIcon), tr("Editor"), createEditorPage());
    addPage(QStringLiteral("log"), s->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Log"), createLogPage());

    connect(this, &QxAppShell::currentPageChanged, this, [this](int, const QString &id) {
        // While the indicator says something about work in progress, the page
        // is not the most interesting thing on screen.
        if (isBusy() || isProgressVisible()) {
            return;
        }
        setStatusMessage(tr("Page: %1").arg(id));
    });
}

void MainWindow::createDocks()
{
    addDock(Qx::LeftDockWidgetArea, QStringLiteral("explorer"), tr("Explorer"), createExplorerDock());
    addDock(Qx::RightDockWidgetArea, QStringLiteral("properties"), tr("Properties"), createPropertiesDock());
    addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), tr("Output"), createOutputDock());
}

QWidget *MainWindow::createOverviewPage()
{
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);

    QLabel *title = new QLabel(tr("QtCanpool AppShell"), page);
    QFont titleFont = title->font();
    titleFont.setPointSizeF(titleFont.pointSizeF() + 6);
    titleFont.setBold(true);
    title->setFont(titleFont);

    QLabel *text = new QLabel(tr("The rail on the left switches pages, the docks around them can be moved, "
                                 "floated and closed, and the whole layout is stored in the configuration "
                                 "file, so the next launch looks exactly like this one."),
                              page);
    text->setWordWrap(true);

    layout->addWidget(title);
    layout->addWidget(text);
    layout->addStretch(1);
    return page;
}

QWidget *MainWindow::createEditorPage()
{
    QPlainTextEdit *editor = new QPlainTextEdit;
    editor->setPlainText(tr("A page is a plain widget. The shell keeps it in a stack and brings it to the "
                            "front when its rail entry is selected."));
    return editor;
}

QWidget *MainWindow::createLogPage()
{
    QPlainTextEdit *log = new QPlainTextEdit;
    log->setReadOnly(true);
    log->setPlainText(QStringLiteral("13:59:12  shell      ready\n"
                                     "13:59:12  theme      dark-officeplus\n"
                                     "13:59:13  layout     restored\n"));
    return log;
}

QWidget *MainWindow::createExplorerDock()
{
    QTreeWidget *tree = new QTreeWidget;
    tree->setHeaderHidden(true);

    QTreeWidgetItem *root = new QTreeWidgetItem(tree, QStringList(tr("qtcanpool")));
    new QTreeWidgetItem(root, QStringList(tr("src/libs")));
    new QTreeWidgetItem(root, QStringList(tr("demos")));
    new QTreeWidgetItem(root, QStringList(tr("tests")));
    root->setExpanded(true);

    return tree;
}

QWidget *MainWindow::createPropertiesDock()
{
    QWidget *page = new QWidget;
    QFormLayout *layout = new QFormLayout(page);
    layout->addRow(tr("Name"), new QLabel(tr("AppShellDemo")));
    layout->addRow(tr("Version"), new QLabel(QStringLiteral("3.0")));
    layout->addRow(tr("Theme"), new QLabel(themeName(QxThemeManager::instance()->theme())));
    return page;
}

QWidget *MainWindow::createOutputDock()
{
    QPlainTextEdit *output = new QPlainTextEdit;
    output->setReadOnly(true);
    output->setPlainText(tr("Build finished with 0 warnings and 0 errors."));
    return output;
}
