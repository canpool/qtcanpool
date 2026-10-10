/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXAPPSHELL_H
#define QXAPPSHELL_H

#include "qxapp_global.h"
#include "qxplugin/qxplugincontext.h"
#include "qxtoast.h"
#include "ribbonappwindow.h"

#include "qxcore/qxsettings.h"
#include "qxdock/qxdock_global.h"

#include <QtCore/QString>
#include <QtGui/QIcon>

QT_BEGIN_NAMESPACE
class QCloseEvent;
class QLabel;
class QProgressBar;
class QStackedWidget;
QT_END_NAMESPACE

QX_DOCK_BEGIN_NAMESPACE
class DockWidget;
class DockWindow;
QX_DOCK_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxAppShellPrivate;
class QxNavigationBar;
class QxToastManager;
class QxWorkspaceManager;

/*!
 * The window skeleton of a ribbon application.
 *
 * It is a RibbonAppWindow - so it already carries the ribbon, the window agent
 * and a state bar - with the three parts every application otherwise rebuilds
 * by hand:
 *
 * - a navigation rail (QxNavigationBar) on the left edge,
 * - a page stack in the middle, one page per rail entry,
 * - a qxdock DockWindow whose docks surround the pages.
 *
 * On top of that it plugs the shell into the rest of the framework: the status
 * bar gets a message line and a busy indicator, and saveLayout()/restoreLayout()
 * keep the geometry, the dock layout and the selected page in a QxSettings
 * object.
 *
 * Typical use:
 * @code
 * QxAppShell shell;
 * shell.addPage(QStringLiteral("home"), homeIcon, tr("Home"), new HomePage);
 * shell.addPage(QStringLiteral("log"), logIcon, tr("Log"), new LogPage);
 * shell.addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), tr("Output"), new OutputView);
 * shell.setStatusMessage(tr("Ready"));
 * shell.restoreLayout();   // after every page and dock exists
 * shell.show();
 * @endcode
 *
 * Pages are pages of a QStackedWidget, not dock widgets: they are the fixed
 * workspace in the middle, while the docks around them can be moved, floated
 * and closed as usual.
 */
class QX_APP_EXPORT QxAppShell : public RibbonAppWindow
{
    Q_OBJECT
public:
    explicit QxAppShell(QWidget *parent = Q_NULLPTR);
    ~QxAppShell() override;

    /*! The navigation rail; owned by the shell. */
    QxNavigationBar *navigationBar() const;
    /*! The dock area; owned by the shell. */
    QX_DOCK_PREPEND_NAMESPACE(DockWindow) * dockWindow() const;

    // Pages -----------------------------------------------------------------
    /*!
     * Adds a page at the end of the workspace and an entry with the same icon
     * and title to the rail. The id addresses the page in the configuration
     * file, so it has to be stable and unique; the shell takes ownership of
     * \a page while it is installed.
     *
     * The first page added becomes the current one.
     */
    void addPage(const QString &id, const QIcon &icon, const QString &title, QWidget *page);
    /*! Inserts a page at the given index of the workspace and of the rail. */
    void insertPage(int index, const QString &id, const QIcon &icon, const QString &title, QWidget *page);
    /*!
     * Removes the page with \a id and its rail entry. The page widget is only
     * taken out of the shell - it stays alive and the caller owns it again.
     */
    void removePage(const QString &id);

    int pageCount() const;
    /*! Index of the page with \a id, or -1 when there is no such page. */
    int indexOfPage(const QString &id) const;
    /*! Id of the page at the given index, or an empty string. */
    QString pageId(int index) const;
    /*! The page widget with \a id, or null. */
    QWidget *page(const QString &id) const;

    int currentPageIndex() const;
    QString currentPageId() const;
    QWidget *currentPage() const;

    // Docks -----------------------------------------------------------------
    /*!
     * Creates a dock widget named \a id, titles it \a title, fills it with
     * \a widget and drops it in \a area. The id is stored as the object name,
     * which is what dock() and the persisted layout use.
     *
     * Returns null when \a id is empty or already in use.
     */
    QX_DOCK_PREPEND_NAMESPACE(DockWidget) *
        addDock(Qx::DockWidgetArea area, const QString &id, const QString &title, QWidget *widget);
    /*! The dock widget created with addDock() under \a id, or null. */
    QX_DOCK_PREPEND_NAMESPACE(DockWidget) * dock(const QString &id) const;

    // Plugin context --------------------------------------------------------
    /*!
     * Returns the QxPluginContext adapter. Plugins reach the host only through
     * this interface and never the QxAppShell class, so a K14 rename of this
     * class changes nothing in any plugin.
     *
     * The adapter is created on first use and owned by the shell; hand the
     * returned pointer to QxPluginManager::setContext().
     */
    QxPlugin::QxPluginContext *pluginContext() const;

    // Status bar ------------------------------------------------------------
    /*! Text of the permanent message line at the left of the status bar. */
    void setStatusMessage(const QString &message);
    QString statusMessage() const;
    /*!
     * Shows or hides the busy indicator in the status bar. It is a bar without
     * a percentage: the shell only reports "working", the caller knows how far
     * the work is.
     *
     * The indicator is one widget with two modes, and the last call wins:
     * setting a progress takes it over, and going busy takes it back.
     */
    void setBusy(bool busy);
    bool isBusy() const;

    /*!
     * Range the progress runs in, 0 to 100 by default. The range is remembered
     * and applied the next time a progress is shown, so changing it while the
     * busy indicator is up does not turn that indicator into a bar at zero.
     */
    void setProgressRange(int minimum, int maximum);
    int progressMinimum() const;
    int progressMaximum() const;
    /*!
     * Shows how far the work has got, which also shows the indicator - setting a
     * progress means wanting it seen. \a value is clamped to the range.
     *
     * @code
     * shell.setStatusMessage(tr("Copying..."));
     * shell.setProgressRange(0, files.count());
     * for (...) { copy(); shell.setProgress(++done); }
     * shell.clearProgress();
     * @endcode
     */
    void setProgress(int value);
    int progress() const;
    /*! Hides the indicator and puts the progress back at the minimum. */
    void clearProgress();
    /*!
     * Whether the indicator is up, in either of its two modes. The question is
     * about the indicator, not about the window: a hidden window leaves the
     * answer where the last call put it.
     */
    bool isProgressVisible() const;

    // Notifications ---------------------------------------------------------
    /*!
     * The stack of toasts that shows up over the shell, created on first use
     * and owned by the shell.
     *
     * A shell is a plain window, so it never asks for the object itself before
     * someone has something to say; once it exists, every showToast() lands in
     * it - see QxToastManager for the stacking, the timeout and the placement.
     */
    QxToastManager *toastManager() const;
    /*!
     * Puts a toast over the shell and returns it, so that a caller who needs to
     * can hold on to it or dismiss it early.
     *
     * @code
     * shell.showToast(tr("The document was saved"), QxToast::Success);
     * @endcode
     */
    QxToast *showToast(const QString &text, QxToast::Level level = QxToast::Information,
                       int timeoutMs = QxToast::DefaultTimeout);

    // Workspaces ------------------------------------------------------------
    /*!
     * The named layouts of this shell, created on first use and owned by it.
     *
     * A shell persists one layout - the one saveLayout() writes - and that is
     * enough for "start where I left off". The manager adds the cases that
     * need a name: keeping two arrangements apart and asking for one back.
     * See QxWorkspaceManager for what a workspace holds and, more to the
     * point, what it does not.
     */
    QxWorkspaceManager *workspaceManager() const;
    /*! Names of the stored workspaces, in the order they were saved. */
    QStringList workspaceNames() const;
    /*!
     * Stores the current arrangement under \a name - see
     * QxWorkspaceManager::saveWorkspace(). Returns false for an empty name.
     */
    bool saveWorkspace(const QString &name);
    /*!
     * Brings back the arrangement stored under \a name - see
     * QxWorkspaceManager::applyWorkspace(). Returns false when there is no
     * such workspace.
     *
     * @code
     * if (!shell.applyWorkspace(QStringLiteral("Writing"))) {
     *     shell.showToast(tr("That layout is gone"), QxToast::Warning);
     * }
     * @endcode
     */
    bool applyWorkspace(const QString &name);

    // Persistence -----------------------------------------------------------
    /*!
     * The settings object backing persistence; created on first use from the
     * application name and owned by the shell.
     */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;
    /*!
     * Replaces the settings object; ownership is transferred to the shell.
     *
     * Unlike the theme manager this does not apply anything: a layout can only
     * be restored once the pages and docks it refers to exist, so call
     * restoreLayout() yourself, after the shell has been built.
     */
    void setSettings(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings);

    /*! Stores the geometry, the dock layout and the current page. */
    void saveLayout();
    /*!
     * Restores whatever saveLayout() stored. Missing or stale values are
     * skipped, so calling it on a fresh installation is harmless.
     */
    void restoreLayout();

    /*! Whether closeEvent() calls saveLayout(); true by default. */
    bool autoSaveLayout() const;
    void setAutoSaveLayout(bool on);

public Q_SLOTS:
    /*! Brings the page at the given index to the front. Out of range: no-op. */
    void setCurrentPage(int index);
    /*! Brings the page with \a id to the front. Unknown id: no-op. */
    void setCurrentPage(const QString &id);

Q_SIGNALS:
    /*!
     * Emitted when the page on screen changes, which also covers the case where
     * another page was inserted or removed and the index of the visible page
     * shifted. \a id is the stable name of the page, empty when there is none.
     */
    void currentPageChanged(int index, const QString &id);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Q_DISABLE_COPY(QxAppShell)
    QX_DECLARE_PRIVATE(QxAppShell)
};

QX_APP_END_NAMESPACE

#endif   // QXAPPSHELL_H
