/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "filetreeplugin.h"

#include "qxplugin/qxplugincontext.h"

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
    return true;
}

void FileTreePlugin::shutdown()
{
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
