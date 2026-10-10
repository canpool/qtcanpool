/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxplugin.h"

QX_PLUGIN_BEGIN_NAMESPACE

QxPlugin::QxPlugin(QObject *parent)
    : QObject(parent)
    , m_context(Q_NULLPTR)
{
}

QxPlugin::~QxPlugin()
{
}

void QxPlugin::extensionsInitialized()
{
}

void QxPlugin::shutdown()
{
}

int QxPlugin::interfaceVersion() const
{
    return 1;
}

QxPluginContext *QxPlugin::context() const
{
    return m_context;
}

QX_PLUGIN_END_NAMESPACE
