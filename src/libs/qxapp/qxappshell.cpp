/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxappshell.h"
#include "qxnavigationbar.h"
#include "qxtoastmanager.h"
#include "qxworkspacemanager.h"

#include "qxcore/qxsettings.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QSignalBlocker>
#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>

QX_CORE_USE_NAMESPACE
QX_DOCK_USE_NAMESPACE

QX_APP_BEGIN_NAMESPACE

/*! Where settings land when the application never named itself. */
#ifndef QX_APP_FALLBACK_ORGANIZATION
#define QX_APP_FALLBACK_ORGANIZATION "qtcanpool"
#endif

namespace
{

const char *const kGroup = "ui";
const char *const kGeometryKey = "windowGeometry";
const char *const kWindowStateKey = "windowState";
const char *const kDockStateKey = "dockState";
const char *const kCurrentPageKey = "currentPage";

/*! Width of the busy indicator in the status bar. */
const int kProgressWidth = 140;

}   // namespace

class QxAppShellPrivate
{
    QX_DECLARE_PUBLIC(QxAppShell)
public:
    /*! One entry of the central workspace: a rail entry and a stacked page. */
    struct Page {
        QString id;
        QString title;
        QIcon icon;
        QWidget *m_widget = Q_NULLPTR;
    };

    QxAppShellPrivate();
    void init();
    /*!
     * Moves the rail and the page stack to m_currentIndex and emits
     * currentPageChanged() when the visible page - index and id - differs from
     * the pair it had before.
     */
    void resyncCurrentPage(int previousIndex, const QString &previousId);
    int indexOfPage(const QString &id) const;
    QString pageId(int index) const;
    /*! Creates the settings object on first use; never returns null. */
    QX_CORE_PREPEND_NAMESPACE(QxSettings) * ensureSettings() const;
    /*! Creates the toast stack on first use; never returns null. */
    QxToastManager *ensureToastManager() const;
    /*! Creates the workspace manager on first use; never returns null. */
    QxWorkspaceManager *ensureWorkspaceManager() const;
public:
    QxNavigationBar *m_navigationBar = Q_NULLPTR;
    QX_DOCK_PREPEND_NAMESPACE(DockWindow) *m_dockWindow = Q_NULLPTR;
    QStackedWidget *m_pageStack = Q_NULLPTR;
    QLabel *m_statusLabel = Q_NULLPTR;
    QProgressBar *m_progress = Q_NULLPTR;
    QVector<Page> m_pages;
    mutable QX_CORE_PREPEND_NAMESPACE(QxSettings) *m_settings = Q_NULLPTR;
    mutable QxToastManager *m_toastManager = Q_NULLPTR;
    mutable QxWorkspaceManager *m_workspaceManager = Q_NULLPTR;
    int m_currentIndex = -1;
    bool m_busy = false;
    bool m_autoSaveLayout = true;
};

QxAppShellPrivate::QxAppShellPrivate() = default;

void QxAppShellPrivate::init()
{
    Q_Q(QxAppShell);

    // The rail and the dock area share the window's central widget, with the
    // rail pinned to the left edge, so the docks stay inside the dock area.
    QWidget *central = new QWidget(q);
    QHBoxLayout *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_navigationBar = new QxNavigationBar(central);
    layout->addWidget(m_navigationBar);

    m_dockWindow = new DockWindow(central);
    layout->addWidget(m_dockWindow, 1);

    q->setCentralWidget(central);

    // The pages are the dock window's central widget, so every dock added
    // later ends up around them. qxdock requires the central widget to be the
    // very first dock widget, hence the order here.
    m_pageStack = new QStackedWidget();
    DockWidget *workspace = new DockWidget(QString());
    workspace->setObjectName(QStringLiteral("qxapp_workspace"));
    workspace->setWidget(m_pageStack);
    m_dockWindow->setCentralWidget(workspace);

    QObject::connect(m_navigationBar, &QxNavigationBar::currentChanged, q, [this](int index) {
        if (index < -1 || index >= m_pages.count()) {
            return;
        }
        const int previousIndex = m_currentIndex;
        const QString previousId = pageId(previousIndex);
        m_currentIndex = index;
        resyncCurrentPage(previousIndex, previousId);
    });

    // Status bar: a permanent message line plus an indicator that only shows up
    // while the shell is busy.
    m_statusLabel = new QLabel(q->statusBar());
    q->statusBar()->addWidget(m_statusLabel, 1);

    m_progress = new QProgressBar(q->statusBar());
    m_progress->setRange(0, 0);
    m_progress->setTextVisible(false);
    m_progress->setFixedWidth(kProgressWidth);
    m_progress->hide();
    q->statusBar()->addPermanentWidget(m_progress);
}

void QxAppShellPrivate::resyncCurrentPage(int previousIndex, const QString &previousId)
{
    Q_Q(QxAppShell);

    {
        // The rail is an output here, not an input: writing the index back must
        // not run the selection logic a second time.
        const QSignalBlocker blocker(m_navigationBar);
        m_navigationBar->setCurrentIndex(m_currentIndex);
    }
    if (m_currentIndex >= 0) {
        m_pageStack->setCurrentIndex(m_currentIndex);
    }

    const QString id = pageId(m_currentIndex);
    if (m_currentIndex != previousIndex || id != previousId) {
        Q_EMIT q->currentPageChanged(m_currentIndex, id);
    }
}

int QxAppShellPrivate::indexOfPage(const QString &id) const
{
    for (int i = 0; i < m_pages.count(); ++i) {
        if (m_pages.at(i).id == id) {
            return i;
        }
    }
    return -1;
}

QString QxAppShellPrivate::pageId(int index) const
{
    if (index < 0 || index >= m_pages.count()) {
        return QString();
    }
    return m_pages.at(index).id;
}

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxAppShellPrivate::ensureSettings() const
{
    if (!m_settings) {
        QString organization = QCoreApplication::organizationName();
        if (organization.isEmpty()) {
            organization = QString::fromLatin1(QX_APP_FALLBACK_ORGANIZATION);
        }
        QString application = QCoreApplication::applicationName();
        if (application.isEmpty()) {
            application = organization;
        }
        m_settings = new QX_CORE_PREPEND_NAMESPACE(QxSettings)(organization, application, q_ptr);
    }
    return m_settings;
}

QxToastManager *QxAppShellPrivate::ensureToastManager() const
{
    if (!m_toastManager) {
        // The toasts are children of the shell, which is what puts them above
        // the ribbon, the rail and the docks without touching a window manager.
        m_toastManager = new QxToastManager(q_ptr, q_ptr);
    }
    return m_toastManager;
}

QxWorkspaceManager *QxAppShellPrivate::ensureWorkspaceManager() const
{
    if (!m_workspaceManager) {
        m_workspaceManager = new QxWorkspaceManager(q_ptr, q_ptr);
    }
    return m_workspaceManager;
}

QxAppShell::QxAppShell(QWidget *parent)
    : RibbonAppWindow(parent)
    , d_ptr(new QxAppShellPrivate())
{
    d_ptr->setPublic(this);
    Q_D(QxAppShell);
    d->init();
}

QxAppShell::~QxAppShell()
{
    QX_FINI_PRIVATE();
}

QxNavigationBar *QxAppShell::navigationBar() const
{
    Q_D(const QxAppShell);
    return d->m_navigationBar;
}

QX_DOCK_PREPEND_NAMESPACE(DockWindow) * QxAppShell::dockWindow() const
{
    Q_D(const QxAppShell);
    return d->m_dockWindow;
}

void QxAppShell::addPage(const QString &id, const QIcon &icon, const QString &title, QWidget *page)
{
    Q_D(QxAppShell);
    insertPage(d->m_pages.count(), id, icon, title, page);
}

void QxAppShell::insertPage(int index, const QString &id, const QIcon &icon, const QString &title, QWidget *page)
{
    Q_D(QxAppShell);
    if (!page || index < 0 || index > d->m_pages.count()) {
        return;
    }
    if (id.isEmpty()) {
        qWarning("QxAppShell: a page needs a non-empty id");
        return;
    }
    if (d->indexOfPage(id) >= 0) {
        qWarning("QxAppShell: a page with id '%s' already exists", qPrintable(id));
        return;
    }

    QxAppShellPrivate::Page entry;
    entry.id = id;
    entry.title = title;
    entry.icon = icon;
    entry.m_widget = page;

    const bool wasEmpty = d->m_pages.isEmpty();
    const int current = d->m_currentIndex;
    const QString previous = d->pageId(current);

    d->m_pages.insert(index, entry);
    d->m_pageStack->insertWidget(index, page);
    {
        // The rail is rebuilt underneath us, so its signal must not drive the
        // selection while the page list is only half updated.
        const QSignalBlocker blocker(d->m_navigationBar);
        d->m_navigationBar->insertItem(index, icon, title);
    }

    if (wasEmpty) {
        // The first page of a shell has nothing to compete with.
        d->m_currentIndex = index;
    } else if (index <= current) {
        // The page that was on screen kept its identity but moved up by one.
        d->m_currentIndex = current + 1;
    }
    d->resyncCurrentPage(current, previous);
}

void QxAppShell::removePage(const QString &id)
{
    Q_D(QxAppShell);
    const int index = d->indexOfPage(id);
    if (index < 0) {
        return;
    }

    const int current = d->m_currentIndex;
    const QString previous = d->pageId(current);

    QWidget *page = d->m_pages.at(index).m_widget;
    d->m_pages.removeAt(index);
    d->m_pageStack->removeWidget(page);
    if (page) {
        // Out of the shell the caller owns the page again; it is only hidden.
        page->setParent(Q_NULLPTR);
    }
    {
        const QSignalBlocker blocker(d->m_navigationBar);
        d->m_navigationBar->removeItem(index);
    }

    if (d->m_pages.isEmpty()) {
        d->m_currentIndex = -1;
    } else if (index < current) {
        d->m_currentIndex = current - 1;
    } else if (index == current) {
        // The visible page is gone: fall back to the one that took its place.
        d->m_currentIndex = qMin(current, d->m_pages.count() - 1);
    }
    d->resyncCurrentPage(current, previous);
}

int QxAppShell::pageCount() const
{
    Q_D(const QxAppShell);
    return d->m_pages.count();
}

int QxAppShell::indexOfPage(const QString &id) const
{
    Q_D(const QxAppShell);
    return d->indexOfPage(id);
}

QString QxAppShell::pageId(int index) const
{
    Q_D(const QxAppShell);
    return d->pageId(index);
}

QWidget *QxAppShell::page(const QString &id) const
{
    Q_D(const QxAppShell);
    const int index = d->indexOfPage(id);
    if (index < 0) {
        return Q_NULLPTR;
    }
    return d->m_pages.at(index).m_widget;
}

int QxAppShell::currentPageIndex() const
{
    Q_D(const QxAppShell);
    return d->m_currentIndex;
}

QString QxAppShell::currentPageId() const
{
    Q_D(const QxAppShell);
    return d->pageId(d->m_currentIndex);
}

QWidget *QxAppShell::currentPage() const
{
    Q_D(const QxAppShell);
    return d->m_currentIndex >= 0 ? d->m_pages.at(d->m_currentIndex).m_widget : Q_NULLPTR;
}

QX_DOCK_PREPEND_NAMESPACE(DockWidget) * QxAppShell::addDock(Qx::DockWidgetArea area, const QString &id,
                                                            const QString &title, QWidget *widget)
{
    Q_D(QxAppShell);
    if (id.isEmpty()) {
        qWarning("QxAppShell: a dock needs a non-empty id");
        return Q_NULLPTR;
    }
    if (d->m_dockWindow->findDockWidget(id)) {
        qWarning("QxAppShell: a dock with id '%s' already exists", qPrintable(id));
        return Q_NULLPTR;
    }

    DockWidget *dock = new DockWidget(title);
    dock->setObjectName(id);
    if (widget) {
        dock->setWidget(widget);
    }
    d->m_dockWindow->addDockWidget(area, dock);
    return dock;
}

QX_DOCK_PREPEND_NAMESPACE(DockWidget) * QxAppShell::dock(const QString &id) const
{
    Q_D(const QxAppShell);
    return d->m_dockWindow->findDockWidget(id);
}

void QxAppShell::setStatusMessage(const QString &message)
{
    Q_D(QxAppShell);
    d->m_statusLabel->setText(message);
}

QString QxAppShell::statusMessage() const
{
    Q_D(const QxAppShell);
    return d->m_statusLabel->text();
}

void QxAppShell::setBusy(bool busy)
{
    Q_D(QxAppShell);
    if (d->m_busy == busy) {
        return;
    }
    d->m_busy = busy;
    d->m_progress->setVisible(busy);
}

bool QxAppShell::isBusy() const
{
    Q_D(const QxAppShell);
    return d->m_busy;
}

QxToastManager *QxAppShell::toastManager() const
{
    Q_D(const QxAppShell);
    return d->ensureToastManager();
}

QxToast *QxAppShell::showToast(const QString &text, QxToast::Level level, int timeoutMs)
{
    Q_D(QxAppShell);
    return d->ensureToastManager()->show(text, level, timeoutMs);
}

QxWorkspaceManager *QxAppShell::workspaceManager() const
{
    Q_D(const QxAppShell);
    return d->ensureWorkspaceManager();
}

QStringList QxAppShell::workspaceNames() const
{
    Q_D(const QxAppShell);
    return d->ensureWorkspaceManager()->workspaceNames();
}

bool QxAppShell::saveWorkspace(const QString &name)
{
    Q_D(QxAppShell);
    return d->ensureWorkspaceManager()->saveWorkspace(name);
}

bool QxAppShell::applyWorkspace(const QString &name)
{
    Q_D(QxAppShell);
    return d->ensureWorkspaceManager()->applyWorkspace(name);
}

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxAppShell::settings() const
{
    Q_D(const QxAppShell);
    return d->ensureSettings();
}

void QxAppShell::setSettings(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings)
{
    Q_D(QxAppShell);
    if (d->m_settings == settings) {
        return;
    }
    delete d->m_settings;
    d->m_settings = settings;
    if (d->m_settings) {
        d->m_settings->setParent(this);
    }
}

void QxAppShell::saveLayout()
{
    Q_D(QxAppShell);
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->setValue(QString::fromLatin1(kGeometryKey), saveGeometry());
    settings->setValue(QString::fromLatin1(kWindowStateKey), QMainWindow::saveState());
    settings->setValue(QString::fromLatin1(kDockStateKey), d->m_dockWindow->saveState());
    settings->setValue(QString::fromLatin1(kCurrentPageKey), currentPageId());
    settings->endGroup();
    settings->sync();
}

void QxAppShell::restoreLayout()
{
    Q_D(QxAppShell);
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->ensureSettings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    const QByteArray geometry = settings->value(QString::fromLatin1(kGeometryKey)).toByteArray();
    const QByteArray windowState = settings->value(QString::fromLatin1(kWindowStateKey)).toByteArray();
    const QByteArray dockState = settings->value(QString::fromLatin1(kDockStateKey)).toByteArray();
    const QString pageId = settings->stringValue(QString::fromLatin1(kCurrentPageKey));
    settings->endGroup();

    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    if (!windowState.isEmpty()) {
        QMainWindow::restoreState(windowState);
    }
    if (!dockState.isEmpty()) {
        d->m_dockWindow->restoreState(dockState);
    }
    if (!pageId.isEmpty()) {
        setCurrentPage(pageId);
    }
}

bool QxAppShell::autoSaveLayout() const
{
    Q_D(const QxAppShell);
    return d->m_autoSaveLayout;
}

void QxAppShell::setAutoSaveLayout(bool on)
{
    Q_D(QxAppShell);
    d->m_autoSaveLayout = on;
}

void QxAppShell::setCurrentPage(int index)
{
    Q_D(QxAppShell);
    if (index < 0 || index >= d->m_pages.count()) {
        return;
    }
    const int previousIndex = d->m_currentIndex;
    const QString previousId = d->pageId(previousIndex);
    d->m_currentIndex = index;
    d->resyncCurrentPage(previousIndex, previousId);
}

void QxAppShell::setCurrentPage(const QString &id)
{
    Q_D(QxAppShell);
    const int index = d->indexOfPage(id);
    if (index < 0) {
        return;
    }
    setCurrentPage(index);
}

void QxAppShell::closeEvent(QCloseEvent *event)
{
    Q_D(QxAppShell);
    if (d->m_autoSaveLayout) {
        saveLayout();
    }
    RibbonAppWindow::closeEvent(event);
}

QX_APP_END_NAMESPACE
