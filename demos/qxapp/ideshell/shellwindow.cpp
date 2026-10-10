/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "shellwindow.h"

#include "qxapp/qxpluginmanagerdialog.h"

#include "qxplugin/qxpluginspec.h"

#include "qxdock/dockwindow.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

// QAction lives in QtGui from Qt 6 on and in QtWidgets before that; the
// module-less form is the one the rest of the project uses for it.
#include <QAction>
#include <QCoreApplication>
#include <QDir>
#include <QMenu>
#include <QtWidgets/QStyle>

QX_RIBBON_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
/*! The state names the composition report uses; QxPluginSpec only has the enum. */
QString stateName(QxPluginState state)
{
    switch (state) {
    case QxPluginState::Invalid:
        return QStringLiteral("invalid");
    case QxPluginState::Read:
        return QStringLiteral("read");
    case QxPluginState::Resolved:
        return QStringLiteral("resolved");
    case QxPluginState::Loaded:
        return QStringLiteral("loaded");
    case QxPluginState::Initialized:
        return QStringLiteral("initialized");
    case QxPluginState::Stopped:
        return QStringLiteral("stopped");
    case QxPluginState::Disabled:
        return QStringLiteral("disabled");
    }
    return QStringLiteral("unknown");
}
}   // namespace

ShellWindow::ShellWindow(QWidget *parent)
    : QxAppShell(parent)
    , m_manager(new QxPluginManager(this))
{
    setWindowTitle(tr("QtCanpool IDE Shell"));
    resize(1100, 720);

    // Installing the engine first restores the theme the user picked last time.
    QxThemeManager::instance();

    createRibbon();
    setStatusMessage(tr("Ready"));
}

void ShellWindow::createRibbon()
{
    const QStyle *s = style();
    RibbonPage *page = ribbonBar()->addPage(tr("&Home"));

    // Application ------------------------------------------------------------
    RibbonGroup *application = page->addGroup(tr("Application"));

    QAction *manage = new QAction(s->standardIcon(QStyle::SP_FileDialogListView), tr("Plugins..."), this);
    application->addMediumAction(manage);
    connect(manage, &QAction::triggered, this, &ShellWindow::openPluginManager);

    // Layout -----------------------------------------------------------------
    RibbonGroup *layout = page->addGroup(tr("Layout"));

    QAction *action = new QAction(s->standardIcon(QStyle::SP_DialogSaveButton), tr("Save"), this);
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

    // Pages ------------------------------------------------------------------
    // The entries are the page ids the modules registered, so this menu is empty
    // until a module puts a page in - which is the point: the shell has no names
    // of its own to offer.
    RibbonGroup *pages = page->addGroup(tr("Pages"));

    m_pageMenu = new QMenu(tr("Go to"), this);
    pages->addSmallMenu(m_pageMenu);
    refreshPageMenu();
}

void ShellWindow::refreshPageMenu()
{
    m_pageMenu->clear();

    for (int i = 0; i < pageCount(); ++i) {
        const QString id = pageId(i);
        QAction *entry = m_pageMenu->addAction(id);
        connect(entry, &QAction::triggered, this, [this, id]() {
            setCurrentPage(id);
        });
    }

    if (pageCount() == 0) {
        QAction *empty = m_pageMenu->addAction(tr("(no module added a page)"));
        empty->setEnabled(false);
    }
}

void ShellWindow::start()
{
    // The switches are read before loadPlugins() and nowhere else. They are a
    // deviation from the metadata rather than an absolute state, the plugin
    // manager dialog is the only thing that writes them, and the change takes
    // effect the next time the application starts - so this really is the whole
    // of the host's part in "which modules run".
    m_manager->setDisabledPlugins(QxPluginManagerDialog::disabledPlugins(settings()));
    m_manager->setEnabledPlugins(QxPluginManagerDialog::enabledPlugins(settings()));
    m_manager->setContext(pluginContext());
    m_manager->setPluginPaths({pluginDirectory()});
    m_manager->loadPlugins();

    refreshPageMenu();
    announceFailures();
}

void ShellWindow::announceFailures()
{
    if (m_manager->hasError()) {
        showToast(tr("Some plugins did not start - see Plugins"), QxToast::Warning);
    }
}

void ShellWindow::openPluginManager()
{
    // A new dialog each time is the cheap end of the trade: it re-reads the
    // manager, which is what makes it show a plugin that failed since the last
    // look, and it costs one widget that is gone as soon as it closes.
    QxPluginManagerDialog dialog(m_manager, settings(), this);
    if (dialog.exec() == QDialog::Accepted) {
        // The switches never load or unload anything - only start() does that -
        // so the caller has to be told when what it is looking at no longer
        // matches what it asked for.
        showToast(tr("Plugin changes take effect the next time the application starts"), QxToast::Information);
    }
}

QString ShellWindow::pluginDirectory()
{
    // RELATIVE_PLUGIN_PATH is the one name the build writes down for a layout
    // that differs on every platform: bin/ beside lib/qtproject/plugins here, an
    // app bundle's MacOS/ and PlugIns/ on macOS. It is a compile definition on
    // every target for exactly this reason - guessing the layout at run time
    // would be guessing wrong somewhere.
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1Char('/') +
                           QString::fromLatin1(RELATIVE_PLUGIN_PATH));
}

QxPluginManager *ShellWindow::pluginManager() const
{
    return m_manager;
}

QString ShellWindow::composition() const
{
    QStringList lines;

    lines.append(QStringLiteral("plugin dir: %1").arg(pluginDirectory()));
    for (QxPluginSpec *spec : m_manager->allSpecs()) {
        QString line = QStringLiteral("%1 %2 %3")
                           .arg(spec->id(), -12)
                           .arg(stateName(spec->state()), -12)
                           .arg(spec->version().toString());
        if (!spec->error().isEmpty()) {
            line += QStringLiteral(" - ") + spec->error();
        }
        lines.append(line);
    }

    // The discovery lines above come in the order the directory handed the files
    // over; this is the order the manager decided to initialize them in, which is
    // the one a declared dependency is supposed to change.
    QStringList order;
    for (QxPluginSpec *spec : m_manager->specs()) {
        order.append(spec->id());
    }
    lines.append(QStringLiteral("load order: %1")
                     .arg(order.isEmpty() ? QStringLiteral("(none)") : order.join(QStringLiteral(" -> "))));

    QStringList pages;
    for (int i = 0; i < pageCount(); ++i) {
        pages.append(pageId(i));
    }
    lines.append(
        QStringLiteral("pages: %1").arg(pages.isEmpty() ? QStringLiteral("(none)") : pages.join(QLatin1Char(','))));

    QStringList docks = dockWindow()->dockWidgetsMap().keys();
    lines.append(
        QStringLiteral("docks: %1").arg(docks.isEmpty() ? QStringLiteral("(none)") : docks.join(QLatin1Char(','))));

    // What the modules offered each other, read back off the pool the shell
    // handed over. The host lists names it got from the objects themselves -
    // there is no module name in this file to compare them against, which is the
    // point of reading the pool rather than writing the list down.
    QStringList published;
    for (QObject *object : pluginPool()->objects()) {
        const QString name = object->objectName();
        published.append(name.isEmpty() ? QStringLiteral("<unnamed>") : name);
    }
    lines.append(QStringLiteral("pool: %1")
                     .arg(published.isEmpty() ? QStringLiteral("(empty)") : published.join(QLatin1Char(','))));

    lines.append(QStringLiteral("status: %1").arg(statusMessage()));

    return lines.join(QLatin1Char('\n'));
}
