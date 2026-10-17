/**
 * Copyright (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribbonpage.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QAction>
#include <QActionGroup>
#include <QFile>
#include <QStatusBar>

QX_RIBBON_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{

/*! Reads one of the style sheets this example builds in (res/themes.qrc). */
QxThemeInfo bundledTheme(const QString &id, const QString &name, bool dark, const QString &fileName)
{
    QFile file(QStringLiteral(":/themes/") + fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QxThemeInfo();
    }

    QxThemeInfo info(id, name, dark);
    info.styleSheet = QString::fromUtf8(file.readAll());
    return info;
}

}   // namespace

/*
 * Two different things are switched here, and they are switched at two different
 * levels on purpose.
 *
 * The layout style belongs to the ribbon bar: it decides how the bar arranges its
 * own pages and groups, so it is a property of that widget.
 *
 * The colour theme belongs to the application. It is one style sheet for the whole
 * application plus one palette, and a ribbon is drawn with whatever reaches the
 * application - the library offers no theme of its own. So this example registers
 * the themes it wants to offer with the engine and then only ever names one of
 * them; what the engine does with that choice (palette, style sheet, remembering
 * it for the next start) is not this example's business.
 */
MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QIcon icon(":/logo");

    QxThemeManager *engine = QxThemeManager::instance();
    // Four families, each in its light and dark scheme (res/themes.qrc).
    engine->registerTheme(
        bundledTheme(QStringLiteral("soft-light"), tr("Soft Light"), false, QStringLiteral("soft_light.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("soft-dark"), tr("Soft Dark"), true, QStringLiteral("soft_dark.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("classic-light"), tr("Classic Light"), false, QStringLiteral("classic_light.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("classic-dark"), tr("Classic Dark"), true, QStringLiteral("classic_dark.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("flat-light"), tr("Flat Light"), false, QStringLiteral("flat_light.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("flat-dark"), tr("Flat Dark"), true, QStringLiteral("flat_dark.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("slate-light"), tr("Slate Light"), false, QStringLiteral("slate_light.css")));
    engine->registerTheme(
        bundledTheme(QStringLiteral("slate-dark"), tr("Slate Dark"), true, QStringLiteral("slate_dark.css")));
    connect(engine, &QxThemeManager::themeChanged, this, &MainWindow::onThemeChanged);

    RibbonBar *rb = ribbonBar();
    rb->addPage(tr("page1"));
    rb->addPage(tr("loooooooooooongpage2"));
    rb->addPage(tr("p3"));
    rb->addPage(tr("page4"));

    RibbonButtonGroup *rightBar = rb->cornerButtonGroup(Qt::BottomRightCorner);

    // --- layout style (Office / Wps / two-row) ---
    QActionGroup *styleGroup = new QActionGroup(this);
    QAction *actionStyle = new QAction(icon, tr("style"), this);
    RibbonMenu *styleMenu = new RibbonMenu(tr("style"), this);
    actionStyle->setMenu(styleMenu);
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("tab bar below")), RibbonBar::TabBarBelowStyle));
    styleGroup->addAction(addStyleAction(styleMenu->addAction(tr("tab bar inline")), RibbonBar::TabBarInlineStyle));
    styleGroup->addAction(
        addStyleAction(styleMenu->addAction(tr("tab bar below, 2 rows")), RibbonBar::TabBarBelowTwoRowStyle));
    styleGroup->addAction(
        addStyleAction(styleMenu->addAction(tr("tab bar inline, 2 rows")), RibbonBar::TabBarInlineTwoRowStyle));
    rightBar->addAction(actionStyle);
    styleGroup->actions().at(2)->trigger();

    // --- colour theme, registered above and picked by id ---
    QActionGroup *themeGroup = new QActionGroup(this);
    QAction *actionTheme = new QAction(icon, tr("theme"), this);
    RibbonMenu *themeMenu = new RibbonMenu(tr("theme"), this);
    actionTheme->setMenu(themeMenu);
    const QStringList ids = engine->themeIds();
    for (const QString &id : ids) {
        QAction *entry = themeMenu->addAction(engine->themeInfo(id).name);
        entry->setCheckable(true);
        entry->setChecked(engine->theme() == id);
        themeGroup->addAction(addThemeAction(entry, id));
    }
    rightBar->addAction(actionTheme);
    themeGroup->actions().at(0)->trigger();

    onThemeChanged(engine->theme());
    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

QAction *MainWindow::addStyleAction(QAction *action, int styleId)
{
    action->setCheckable(true);
    action->setData(QVariant(styleId));
    connect(action, &QAction::triggered, this, &MainWindow::onActionChangeStyleTriggered);
    return action;
}

void MainWindow::onActionChangeStyleTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int id = action->data().toInt();
        ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
    }
}

QAction *MainWindow::addThemeAction(QAction *action, const QString &themeId)
{
    action->setCheckable(true);
    action->setData(QVariant(themeId));
    connect(action, &QAction::triggered, this, &MainWindow::onActionChangeThemeTriggered);
    return action;
}

void MainWindow::onActionChangeThemeTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QxThemeManager::instance()->setTheme(action->data().toString());
    }
}

void MainWindow::onThemeChanged(const QString &id)
{
    const QxThemeInfo info = QxThemeManager::instance()->themeInfo(id);
    // What the engine made of the choice: a dark theme also means a dark palette,
    // which is what the widgets that no style sheet describes are drawn with.
    statusBar()->showMessage(tr("%1 (%2)").arg(info.name, info.dark ? tr("dark") : tr("light")));
}
