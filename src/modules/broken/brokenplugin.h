/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef BROKENPLUGIN_H
#define BROKENPLUGIN_H

#include "qxplugin/qxplugin.h"

#include <QtCore/QObject>

/*!
 * Sample module whose metadata requires "nonexistent".
 *
 * It is here to show what a module that cannot run looks like - in the plugin
 * manager dialog it appears with its reason, and the host keeps starting. It is
 * never built: see CMakeLists.txt in this directory.
 */
class BrokenPlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "broken.json")
public:
    /*! Never reached: the manager sets the module aside before instantiating it. */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
};

#endif   // BROKENPLUGIN_H
