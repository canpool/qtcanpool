/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "brokenplugin.h"

#include "qxplugin/qxplugincontext.h"

using namespace QxPlugin;

bool BrokenPlugin::initialize(QxPluginContext *context, QString *errorString)
{
    Q_UNUSED(context);
    if (errorString) {
        *errorString = QStringLiteral("unreachable: the manager rejects the module before initialize()");
    }
    return false;
}
