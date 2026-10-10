/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef ACCEPTANCEPLUGIN_H
#define ACCEPTANCEPLUGIN_H

#include "qxplugin/qxplugin.h"

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

/*!
 * The acceptance plugin (roadmap DoD #1).
 *
 * It exists to answer one question with a compiler rather than with a promise:
 * can a plugin be built *outside* this repository, against nothing but an
 * installed SDK? So it deliberately uses no internal CMake API, includes no
 * header that is not installed, and touches no source-tree path.
 *
 * What it does once loaded is the smallest real thing a plugin can do: take a
 * dock and a status line from QxPluginContext and publish itself into the
 * object pool, so that a peer - or the host's own verification - can reach it
 * by name without including this header.
 */
class AcceptancePlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "acceptance.json")
public:
    // The leading :: is required here, not stylistic: this class inherits the
    // name QxPlugin, which hides the namespace of the same name, so
    // QxPlugin::QxPluginContext would be read as a member of the class.
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    void shutdown() override;

public Q_SLOTS:
    /*! Reachable by name through the meta-object, which is how a peer uses it. */
    bool isReady() const;
private:
    QWidget *m_panel = Q_NULLPTR;
};

#endif   // ACCEPTANCEPLUGIN_H
