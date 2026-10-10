/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "acceptanceplugin.h"

#include "qxplugin/qxplugincontext.h"

#include <QtCore/QString>
#include <QtWidgets/QLabel>

using namespace QxPlugin;

bool AcceptancePlugin::initialize(QxPluginContext *context, QString *errorString)
{
    if (!context) {
        if (errorString) {
            *errorString = QStringLiteral("the host handed over no context");
        }
        return false;
    }

    auto *panel = new QLabel(tr("acceptance: built outside the source tree"));
    panel->setObjectName(QStringLiteral("acceptancePanel"));

    // The two host capabilities the plugin contract is really about: somewhere
    // to put a widget, and a line on the status bar. Everything host-specific -
    // what a dock is, where a "bottom" dock lands - stays behind the interface.
    context->addDock(QxPluginContext::BottomDock, QStringLiteral("acceptance"), tr("Acceptance"), panel);
    context->setStatusMessage(tr("Acceptance plugin ready"));

    // Publishing is a choice, not something that happens automatically: a plugin
    // that stays quiet is unreachable, which is the point of the pool.
    setObjectName(QStringLiteral("acceptance"));
    context->addObject(this);

    m_panel = panel;
    return true;
}

void AcceptancePlugin::shutdown()
{
    // Out of the pool before the plugin goes, not after: whoever was connected
    // gets a defined moment for it.
    if (context()) {
        context()->removeObject(this);
    }
    m_panel = Q_NULLPTR;
}

bool AcceptancePlugin::isReady() const
{
    return m_panel != Q_NULLPTR;
}
