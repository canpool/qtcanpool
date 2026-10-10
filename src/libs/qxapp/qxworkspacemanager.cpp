/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxworkspacemanager.h"
#include "qxworkspacemanager_p.h"

#include "qxappshell.h"

#include "qxdock/dockwindow.h"

#include <QtCore/QDebug>

QX_CORE_USE_NAMESPACE
QX_DOCK_USE_NAMESPACE

QX_APP_BEGIN_NAMESPACE

namespace
{

/*!
 * The group the shell keeps its layout in. Workspaces are stored inside it,
 * next to the keys saveLayout() writes, so one shell has one configuration
 * section - it is the same string as the shell's own kGroup.
 */
const char *const kGroup = "ui";
const char *const kNamesKey = "workspaceNames";
const char *const kCurrentKey = "currentWorkspace";
/*! The sub-group every workspace sits in: ui/workspace/<name>. */
const char *const kWorkspaceGroup = "workspace";
const char *const kDockStateKey = "dockState";
const char *const kPageKey = "currentPage";

}   // namespace

QxWorkspaceManagerPrivate::QxWorkspaceManagerPrivate() = default;

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxWorkspaceManagerPrivate::settings() const
{
    // The shell creates its settings object on first use and owns it, so this
    // is never a borrowed pointer that has to be kept alive here.
    return m_shell ? m_shell->settings() : Q_NULLPTR;
}

QStringList QxWorkspaceManagerPrivate::names() const
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = this->settings();
    if (!settings) {
        return QStringList();
    }

    settings->beginGroup(QString::fromLatin1(kGroup));
    const QStringList list = settings->stringListValue(QString::fromLatin1(kNamesKey));
    settings->endGroup();
    return list;
}

void QxWorkspaceManagerPrivate::setNames(const QStringList &list)
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = this->settings();
    if (!settings) {
        return;
    }

    settings->beginGroup(QString::fromLatin1(kGroup));
    if (list.isEmpty()) {
        // An empty list is a key no reader has to special-case.
        settings->remove(QString::fromLatin1(kNamesKey));
    } else {
        settings->setValue(QString::fromLatin1(kNamesKey), list);
    }
    settings->endGroup();
}

QString QxWorkspaceManagerPrivate::normalize(const QString &name)
{
    return name.trimmed();
}

QString QxWorkspaceManagerPrivate::current() const
{
    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = this->settings();
    if (!settings) {
        return QString();
    }

    settings->beginGroup(QString::fromLatin1(kGroup));
    const QString name = settings->stringValue(QString::fromLatin1(kCurrentKey));
    settings->endGroup();
    return name;
}

void QxWorkspaceManagerPrivate::setCurrent(const QString &name)
{
    Q_Q(QxWorkspaceManager);

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = this->settings();
    if (!settings) {
        return;
    }

    const QString previous = current();
    if (previous == name) {
        return;
    }

    settings->beginGroup(QString::fromLatin1(kGroup));
    if (name.isEmpty()) {
        settings->remove(QString::fromLatin1(kCurrentKey));
    } else {
        settings->setValue(QString::fromLatin1(kCurrentKey), name);
    }
    settings->endGroup();
    settings->sync();

    Q_EMIT q->currentWorkspaceChanged(name);
}

QxWorkspaceManager::QxWorkspaceManager(QxAppShell *shell, QObject *parent)
    : QObject(parent)
    , d_ptr(new QxWorkspaceManagerPrivate())
{
    d_ptr->setPublic(this);
    d_ptr->m_shell = shell;
}

QxWorkspaceManager::~QxWorkspaceManager()
{
    QX_FINI_PRIVATE();
}

QxAppShell *QxWorkspaceManager::shell() const
{
    Q_D(const QxWorkspaceManager);
    return d->m_shell;
}

QX_CORE_PREPEND_NAMESPACE(QxSettings) * QxWorkspaceManager::settings() const
{
    Q_D(const QxWorkspaceManager);
    return d->settings();
}

int QxWorkspaceManager::count() const
{
    Q_D(const QxWorkspaceManager);
    return d->names().count();
}

QStringList QxWorkspaceManager::workspaceNames() const
{
    Q_D(const QxWorkspaceManager);
    return d->names();
}

bool QxWorkspaceManager::contains(const QString &name) const
{
    Q_D(const QxWorkspaceManager);
    return d->names().contains(QxWorkspaceManagerPrivate::normalize(name));
}

QString QxWorkspaceManager::currentWorkspace() const
{
    Q_D(const QxWorkspaceManager);
    return d->current();
}

bool QxWorkspaceManager::saveWorkspace(const QString &name)
{
    Q_D(QxWorkspaceManager);
    if (!d->m_shell) {
        qWarning("QxWorkspaceManager: there is no shell to save a layout of");
        return false;
    }

    const QString key = QxWorkspaceManagerPrivate::normalize(name);
    if (key.isEmpty()) {
        qWarning("QxWorkspaceManager: a workspace needs a name");
        return false;
    }

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->settings();

    QStringList list = d->names();
    if (!list.contains(key)) {
        list.append(key);
    }
    d->setNames(list);

    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->beginGroup(QString::fromLatin1(kWorkspaceGroup));
    settings->beginGroup(key);
    settings->setValue(QString::fromLatin1(kDockStateKey), d->m_shell->dockWindow()->saveState());
    settings->setValue(QString::fromLatin1(kPageKey), d->m_shell->currentPageId());
    settings->endGroup();
    settings->endGroup();
    settings->endGroup();
    settings->sync();

    // Saving is also picking: the panels on screen are what the name now means.
    d->setCurrent(key);
    Q_EMIT workspaceSaved(key);
    return true;
}

bool QxWorkspaceManager::applyWorkspace(const QString &name)
{
    Q_D(QxWorkspaceManager);
    if (!d->m_shell) {
        qWarning("QxWorkspaceManager: there is no shell to apply a layout to");
        return false;
    }

    const QString key = QxWorkspaceManagerPrivate::normalize(name);
    if (!d->names().contains(key)) {
        qWarning("QxWorkspaceManager: there is no workspace called \"%s\"", qPrintable(key));
        return false;
    }

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->settings();
    settings->beginGroup(QString::fromLatin1(kGroup));
    settings->beginGroup(QString::fromLatin1(kWorkspaceGroup));
    settings->beginGroup(key);
    const QByteArray dockState = settings->value(QString::fromLatin1(kDockStateKey)).toByteArray();
    const QString pageId = settings->stringValue(QString::fromLatin1(kPageKey));
    settings->endGroup();
    settings->endGroup();
    settings->endGroup();

    if (dockState.isEmpty()) {
        qWarning("QxWorkspaceManager: the workspace \"%s\" has no layout stored", qPrintable(key));
        return false;
    }
    if (!d->m_shell->dockWindow()->restoreState(dockState)) {
        // The usual reason is a dock of the arrangement that no longer exists.
        // There is nothing to fall back to: a half-applied arrangement is worse
        // than the one on screen.
        qWarning("QxWorkspaceManager: the layout of \"%s\" does not fit this shell", qPrintable(key));
        return false;
    }

    d->m_shell->setCurrentPage(pageId);
    d->setCurrent(key);
    Q_EMIT workspaceApplied(key);
    return true;
}

bool QxWorkspaceManager::removeWorkspace(const QString &name)
{
    Q_D(QxWorkspaceManager);

    const QString key = QxWorkspaceManagerPrivate::normalize(name);
    QStringList list = d->names();
    if (!list.removeOne(key)) {
        return false;
    }
    d->setNames(list);

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->settings();
    if (settings) {
        settings->beginGroup(QString::fromLatin1(kGroup));
        settings->beginGroup(QString::fromLatin1(kWorkspaceGroup));
        // Removes the key and everything below it, which is the whole workspace.
        settings->remove(key);
        settings->endGroup();
        settings->endGroup();
        settings->sync();
    }

    if (d->current() == key) {
        d->setCurrent(QString());
    }
    Q_EMIT workspaceRemoved(key);
    return true;
}

bool QxWorkspaceManager::renameWorkspace(const QString &from, const QString &to)
{
    Q_D(QxWorkspaceManager);

    const QString oldKey = QxWorkspaceManagerPrivate::normalize(from);
    const QString newKey = QxWorkspaceManagerPrivate::normalize(to);
    if (oldKey.isEmpty() || newKey.isEmpty()) {
        qWarning("QxWorkspaceManager: a workspace needs a name");
        return false;
    }

    QStringList list = d->names();
    const int index = list.indexOf(oldKey);
    if (index < 0) {
        return false;
    }
    if (oldKey == newKey) {
        return true;
    }
    if (list.contains(newKey)) {
        qWarning("QxWorkspaceManager: \"%s\" is already taken", qPrintable(newKey));
        return false;
    }

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->settings();
    if (settings) {
        settings->beginGroup(QString::fromLatin1(kGroup));
        settings->beginGroup(QString::fromLatin1(kWorkspaceGroup));

        settings->beginGroup(oldKey);
        const QVariant dockState = settings->value(QString::fromLatin1(kDockStateKey));
        const QString pageId = settings->stringValue(QString::fromLatin1(kPageKey));
        settings->endGroup();

        settings->remove(oldKey);
        settings->beginGroup(newKey);
        settings->setValue(QString::fromLatin1(kDockStateKey), dockState);
        settings->setValue(QString::fromLatin1(kPageKey), pageId);
        settings->endGroup();

        settings->endGroup();
        settings->endGroup();
    }

    list.replace(index, newKey);
    d->setNames(list);
    if (settings) {
        settings->sync();
    }

    if (d->current() == oldKey) {
        d->setCurrent(newKey);
    }
    // A rename is a removal and a save, told with the two signals every
    // consumer already handles; a signal of its own would make each of them
    // write a third branch for the same outcome.
    Q_EMIT workspaceRemoved(oldKey);
    Q_EMIT workspaceSaved(newKey);
    return true;
}

void QxWorkspaceManager::clearWorkspaces()
{
    Q_D(QxWorkspaceManager);

    const QStringList list = d->names();
    if (list.isEmpty()) {
        return;
    }

    QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = d->settings();
    if (settings) {
        settings->beginGroup(QString::fromLatin1(kGroup));
        settings->beginGroup(QString::fromLatin1(kWorkspaceGroup));
        for (const QString &name : list) {
            settings->remove(name);
        }
        settings->endGroup();
        settings->endGroup();
    }

    d->setNames(QStringList());
    d->setCurrent(QString());
    if (settings) {
        settings->sync();
    }

    for (const QString &name : list) {
        Q_EMIT workspaceRemoved(name);
    }
}

QX_APP_END_NAMESPACE
