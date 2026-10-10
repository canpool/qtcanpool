/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxpluginmanagerdialog.h"

#include "qxcore/qxsettings.h"
#include "qxplugin/qxplugin_global.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QPointer>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

QX_PLUGIN_BEGIN_NAMESPACE
class QxPluginSpec;
QX_PLUGIN_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

/*!
 * The state behind QxPluginManagerDialog: the list, the switches it shows and
 * the two lists the settings hold.
 *
 * The switch of a plugin lives in m_switch while the dialog is open, so reading
 * it back does not mean walking the tree. m_saved* is what the settings held when
 * the dialog was last loaded, which is what isModified() and revert() compare
 * against - the switches themselves are never written until apply().
 */
class QxPluginManagerDialogPrivate
{
    QX_DECLARE_PUBLIC(QxPluginManagerDialog)
public:
    QxPluginManagerDialogPrivate();

    void init();

    /*! Fills the list from the manager and the switches from the settings. */
    void rebuild();
    /*! Points the detail pane and the diagnostics area at \a spec. */
    void showSpec(::QxPlugin::QxPluginSpec *spec);

    ::QxPlugin::QxPluginSpec *specById(const QString &id) const;
    /*! What the plugin \a id would do if the settings were applied as they are. */
    bool savedState(::QxPlugin::QxPluginSpec *spec) const;
    bool isModified() const;
    void updateButtons();
    /*! Records a switch the user just flipped. */
    void noteSwitch(QTreeWidgetItem *item);

    QPointer<::QxPlugin::QxPluginManager> m_manager;
    QPointer<QX_CORE_PREPEND_NAMESPACE(QxSettings)> m_settings;

    QTreeWidget *m_tree = Q_NULLPTR;
    QDialogButtonBox *m_buttons = Q_NULLPTR;

    QLabel *m_id = Q_NULLPTR;
    QLabel *m_description = Q_NULLPTR;
    QLabel *m_version = Q_NULLPTR;
    QLabel *m_vendor = Q_NULLPTR;
    QLabel *m_file = Q_NULLPTR;
    QLabel *m_order = Q_NULLPTR;
    QLabel *m_state = Q_NULLPTR;
    QLabel *m_dependencies = Q_NULLPTR;
    QLabel *m_optional = Q_NULLPTR;
    QLabel *m_dependents = Q_NULLPTR;
    QLabel *m_error = Q_NULLPTR;
    QLabel *m_diagnostics = Q_NULLPTR;

    /*! The specs as the list shows them, row for row. */
    QList<::QxPlugin::QxPluginSpec *> m_specs;
    /*! The switch of every displayed plugin, by id. */
    QHash<QString, bool> m_switch;
    /*! What the settings held when the dialog was loaded. */
    QStringList m_savedDisabled;
    QStringList m_savedEnabled;
    /*! Set while the tree is being filled, so itemChanged stays quiet. */
    bool m_updating = false;
};

QX_APP_END_NAMESPACE
