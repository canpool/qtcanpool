/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef NOTEBOOKPLUGIN_H
#define NOTEBOOKPLUGIN_H

#include "qxplugin/qxplugin.h"

#include <QtCore/QObject>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
QT_END_NAMESPACE

/*!
 * Sample module (Level 1): a page in the central workspace.
 *
 * Where output demonstrates docks, this one demonstrates pages: addPage() puts
 * a widget in the host's workspace and an entry with the same icon and title in
 * its navigation, and setCurrentPage() brings it to the front. The page is not
 * a dock - it is part of the fixed middle of the shell, around which the docks
 * are arranged.
 *
 * The document also survives a restart: whatever was typed is kept in the
 * settings object the context hands out, under the page's own key. A plugin
 * only ever reaches the configuration through QxPluginContext::settings().
 */
class NotebookPlugin : public QxPlugin::QxPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QX_PLUGIN_IID FILE "notebook.json")
public:
    // The leading :: is required: the inherited name QxPlugin hides the
    // namespace of the same name. See the note in outputplugin.h.
    /*! Adds the page and restores what was typed last time. */
    bool initialize(::QxPlugin::QxPluginContext *context, QString *errorString) override;
    /*! Writes the document back to the settings the context handed out. */
    void shutdown() override;

    /*! Brings the page to the front; the shell ignores an unknown id. */
    void showPage();
private:
    ::QxPlugin::QxPluginContext *m_context = Q_NULLPTR;
    QPlainTextEdit *m_editor = Q_NULLPTR;
};

#endif   // NOTEBOOKPLUGIN_H
