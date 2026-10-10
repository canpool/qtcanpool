/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "outputplugin.h"

#include "qxplugin/qxplugincontext.h"

#include <QtWidgets/QPlainTextEdit>

using namespace QxPlugin;

bool OutputPlugin::initialize(QxPluginContext *context, QString *errorString)
{
    Q_UNUSED(errorString);

    m_view = new QPlainTextEdit;
    m_view->setObjectName(QStringLiteral("outputView"));
    m_view->setReadOnly(true);
    m_view->setPlainText(tr("output: ready"));

    // The context names the area, the id and the title; the host decides what a
    // dock is and where "bottom" lands. The panel widget goes into the host's
    // ownership here, which is why shutdown() only forgets the pointer.
    context->addDock(QxPluginContext::BottomDock, QStringLiteral("output"), tr("Output"), m_view);
    context->setStatusMessage(tr("Output panel ready"));
    return true;
}

void OutputPlugin::shutdown()
{
    m_view = Q_NULLPTR;
}

void OutputPlugin::appendLine(const QString &line)
{
    if (m_view) {
        m_view->appendPlainText(line);
    }
}
