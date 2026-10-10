/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "notebookplugin.h"

#include "qxplugin/qxplugincontext.h"

#include "qxcore/qxsettings.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QStyle>

using namespace QxPlugin;

namespace
{
const QString kPageId = QStringLiteral("notebook");
const QString kDocumentKey = QStringLiteral("plugins/notebook/document");
}   // namespace

bool NotebookPlugin::initialize(QxPluginContext *context, QString *errorString)
{
    Q_UNUSED(errorString);

    m_context = context;
    m_editor = new QPlainTextEdit;
    m_editor->setObjectName(QStringLiteral("notebookEditor"));
    m_editor->setPlainText(context->settings()->value(kDocumentKey).toString());

    const QIcon icon = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    context->addPage(kPageId, icon, tr("Notebook"), m_editor);
    return true;
}

void NotebookPlugin::shutdown()
{
    if (m_context && m_editor) {
        m_context->settings()->setValue(kDocumentKey, m_editor->toPlainText());
    }
    m_context = Q_NULLPTR;
    m_editor = Q_NULLPTR;
}

void NotebookPlugin::showPage()
{
    if (m_context) {
        m_context->setCurrentPage(kPageId);
    }
}
