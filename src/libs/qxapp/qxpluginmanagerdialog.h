/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPLUGINMANAGERDIALOG_H
#define QXPLUGINMANAGERDIALOG_H

#include "qxapp_global.h"
#include "qxplugin/qxplugin_global.h"

#include "qxcore/qxsettings.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE
class QShowEvent;
QT_END_NAMESPACE

QX_PLUGIN_BEGIN_NAMESPACE
class QxPluginManager;
QX_PLUGIN_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxPluginManagerDialogPrivate;

/*!
 * Lists the plugins the manager found and lets their switches be set.
 *
 * A row per plugin with its place in the load order, its version and its state;
 * the row selected shows what that plugin depends on, what depends on it and -
 * when it did not start - why. The diagnostics area below carries the manager's
 * errorString(), which is where a failure that belongs to more than one plugin
 * (a cycle, a cascade) arrives in one piece.
 *
 * A switch is stored as a deviation from the metadata rather than as an absolute
 * state: a plugin that agrees with its EnabledByDefault goes into neither list, so
 * switching off one the metadata turned on and enabling one it switched off both
 * survive a restart. Comparing against the metadata is also what keeps this
 * honest for a plugin that is off by default - the plugin is only off until it is
 * asked for by name.
 *
 * The change takes effect the next time the application starts: the dialog never
 * loads or unloads anything. It writes the two lists, and the host reads them
 * before calling loadPlugins() - which is what the four static helpers are for.
 *
 * @code
 * // at start-up, before loadPlugins()
 * manager.setDisabledPlugins(QxPluginManagerDialog::disabledPlugins(settings));
 * manager.setEnabledPlugins(QxPluginManagerDialog::enabledPlugins(settings));
 * manager.loadPlugins();
 *
 * // later, from a menu entry
 * QxPluginManagerDialog dialog(&manager, settings, &shell);
 * dialog.exec();
 * @endcode
 */
class QX_APP_EXPORT QxPluginManagerDialog : public QDialog
{
    Q_OBJECT
public:
    // What the settings hold ------------------------------------------------
    /*! Ids stored as switched off; empty for a null \a settings. */
    static QStringList disabledPlugins(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings);
    /*! Stores the ids that must not start; an empty list takes the key out again. */
    static void setDisabledPlugins(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings, const QStringList &ids);

    /*! Ids stored as switched on even though their metadata says they are off by default. */
    static QStringList enabledPlugins(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings);
    /*! Stores the ids that must start; an empty list takes the key out again. */
    static void setEnabledPlugins(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings, const QStringList &ids);

    explicit QxPluginManagerDialog(::QxPlugin::QxPluginManager *manager,
                                   QX_CORE_PREPEND_NAMESPACE(QxSettings) *settings = Q_NULLPTR,
                                   QWidget *parent = Q_NULLPTR);
    ~QxPluginManagerDialog() override;

    ::QxPlugin::QxPluginManager *manager() const;

    // The list --------------------------------------------------------------
    /*! Row of the plugin \a id, or -1 when the list does not hold it. */
    int rowOf(const QString &id) const;
    /*! Number of rows, one per plugin the manager knows about. */
    int count() const;
    /*! The switch of the plugin \a id as it is on screen; false for an unknown id. */
    bool isChecked(const QString &id) const;
    /*! Sets the switch of the plugin \a id on screen; unknown id: no-op. */
    void setChecked(const QString &id, bool checked);

    /*! Brings the row of the plugin \a id into view and shows its details. */
    void setCurrentPlugin(const QString &id);

    /*! The manager's errorString(), or a placeholder when there is none. */
    QString diagnostics() const;
    /*! Whether the switches on screen differ from the ones the settings hold. */
    bool isModified() const;

public Q_SLOTS:
    /*! Rebuilds the list from the manager and the switches from the settings. */
    void reload();
    /*! Puts every switch back to what the settings hold. */
    void revert();
    /*! Writes the switches to the settings. */
    void apply();
    /*! Applies and closes. */
    void accept() override;
    /*! Discards and closes. */
    void reject() override;

Q_SIGNALS:
    /*! The switches have been written and synced. */
    void applied();
protected:
    void showEvent(QShowEvent *event) override;
private:
    Q_DISABLE_COPY(QxPluginManagerDialog)
    QX_DECLARE_PRIVATE(QxPluginManagerDialog)
};

QX_APP_END_NAMESPACE

#endif   // QXPLUGINMANAGERDIALOG_H
