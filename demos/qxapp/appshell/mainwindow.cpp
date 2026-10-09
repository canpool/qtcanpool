/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "mainwindow.h"

#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

// QAction lives in QtGui from Qt 6 on and in QtWidgets before that; the
// module-less form is the one the rest of the project uses for it.
#include <QAction>
#include <QActionGroup>
#include <QtGui/QFont>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
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

    // Status -----------------------------------------------------------------
    RibbonGroup *status = page->addGroup(tr("Status"));

    QAction *busy = new QAction(tr("Busy"), this);
    busy->setCheckable(true);
    status->addSmallAction(busy);
    connect(busy, &QAction::toggled, this, [this](bool on) {
        setBusy(on);
        setStatusMessage(on ? tr("Working...") : tr("Ready"));
    });
}

void MainWindow::createPages()
{
    const QStyle *s = style();

    addPage(QStringLiteral("overview"), s->standardIcon(QStyle::SP_ComputerIcon), tr("Overview"), createOverviewPage());
    addPage(QStringLiteral("editor"), s->standardIcon(QStyle::SP_FileIcon), tr("Editor"), createEditorPage());
    addPage(QStringLiteral("log"), s->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Log"), createLogPage());

    connect(this, &QxAppShell::currentPageChanged, this, [this](int, const QString &id) {
        if (isBusy()) {
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
