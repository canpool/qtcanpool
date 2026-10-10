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

    // Publishing is the whole of the agreement with filetree: it asks the pool
    // for an object named "output" and connects to its appendLine(). Neither
    // side includes the other's header, and the name is the contract.
    setObjectName(QStringLiteral("output"));
    context->addObject(this);
    return true;
}

void OutputPlugin::shutdown()
{
    // Taken out before the plugin goes away, rather than left for the pool to
    // notice a dead pointer: whoever was connected to this object gets a
    // well-defined moment for it.
    if (context()) {
        context()->removeObject(this);
    }
    m_view = Q_NULLPTR;
}

void OutputPlugin::appendLine(const QString &line)
{
    if (m_view) {
        m_view->appendPlainText(line);
    }
}
