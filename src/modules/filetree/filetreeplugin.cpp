/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "filetreeplugin.h"

#include "qxplugin/qxplugincontext.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtWidgets/QTreeWidget>

using namespace QxPlugin;

namespace
{
const int kPathRole = Qt::UserRole;
}   // namespace

bool FileTreePlugin::initialize(QxPluginContext *context, QString *errorString)
{
    Q_UNUSED(errorString);

    // Loaded once, a plugin library stays loaded: a second manager in the same
    // process is handed the same instance back and initialize() runs on it again.
    // Dropping the previous connection first is what stops every line that goes
    // to the output panel from being delivered twice, three times, four...
    if (m_sinkConnection) {
        QObject::disconnect(m_sinkConnection);
        m_sinkConnection = QMetaObject::Connection();
    }

    m_view = new QTreeWidget;
    m_view->setObjectName(QStringLiteral("fileTreeView"));
    m_view->setHeaderHidden(true);

    QTreeWidgetItem *root = new QTreeWidgetItem(m_view, QStringList(tr("qtcanpool")));
    root->setData(0, kPathRole, QStringLiteral("."));
    for (const QString &name : {QStringLiteral("src"), QStringLiteral("demos"), QStringLiteral("tests")}) {
        QTreeWidgetItem *item = new QTreeWidgetItem(root, QStringList(name));
        item->setData(0, kPathRole, name);
    }
    root->setExpanded(true);

    // The tree is what the module contributes; the host decides where a left
    // dock lands and how it can be moved.
    context->addDock(QxPluginContext::LeftDock, QStringLiteral("filetree"), tr("Files"), m_view);

    connect(m_view, &QTreeWidget::itemActivated, this, [this](QTreeWidgetItem *item, int) {
        if (item) {
            emit fileActivated(item->data(0, kPathRole).toString());
        }
    });

    // Where the lines go, if anywhere is listening. output is an optional
    // dependency - the metadata says so - because this module works without it;
    // all that is lost is the reporting. Nothing here includes the other
    // module's header, and asking the pool is the whole of the coupling: the
    // connection is made by member name through the meta-object.
    QObject *sink = context->objectByName(QStringLiteral("output"));
    if (sink) {
        m_sinkConnection = QObject::connect(this, SIGNAL(fileActivated(QString)), sink, SLOT(appendLine(QString)));
        if (!m_sinkConnection) {
            // Something answers to "output" but does not take a line. Worth
            // saying out loud: a pool lookup that silently found the wrong
            // object would be a lot harder to notice than a warning here.
            qWarning("filetree: the pooled 'output' offers no appendLine(QString)");
        }
    }
    return true;
}

void FileTreePlugin::shutdown()
{
    if (m_sinkConnection) {
        QObject::disconnect(m_sinkConnection);
        m_sinkConnection = QMetaObject::Connection();
    }
    m_view = Q_NULLPTR;
}

void FileTreePlugin::activateFile(const QString &path)
{
    // Selecting the entry keeps the view in step when the call came from the
    // host rather than from a click.
    if (m_view) {
        const QList<QTreeWidgetItem *> hits =
            m_view->findItems(QFileInfo(path).fileName(), Qt::MatchExactly | Qt::MatchRecursive, 0);
        if (!hits.isEmpty()) {
            m_view->setCurrentItem(hits.first());
        }
    }
    emit fileActivated(path);
}
