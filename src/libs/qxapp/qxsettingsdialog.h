/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXSETTINGSDIALOG_H
#define QXSETTINGSDIALOG_H

#include "qxapp_global.h"
#include "qxproperty.h"

#include "qxcore/qxsettings.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QListWidget;
class QShowEvent;
class QStackedWidget;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxPropertyEditor;
class QxSettingsDialogPrivate;

/*!
 * A settings dialog: a list of pages on the left, a key/value form on the right
 * and the three things a settings dialog is expected to do - apply, discard and
 * go back to the defaults.
 *
 * The dialog is the half that knows about storage. QxPropertyEditor keeps the
 * values and knows nothing about where they came from; this class reads them out
 * of a QxSettings object when it is shown and writes them back when they are
 * applied.
 *
 * @code
 * QxSettingsDialog dialog(shell.settings());
 * dialog.addPage(QStringLiteral("general"), generalIcon, tr("General"), generalProperties);
 * dialog.addPage(QStringLiteral("editor"), editorIcon, tr("Editor"), editorProperties);
 * dialog.addGroup(QStringLiteral("editor"), tr("Indentation"), indentationProperties);
 * dialog.exec();
 * @endcode
 *
 * The key of a property is the path it is stored under, so it is the part that
 * has to be stable and untranslated; the title of a page and the label of a
 * property are presentation and may be translated freely. A key that is not in
 * the settings object reads as the defaultValue of its descriptor, which is what
 * makes a first run look like a configured one.
 *
 * Migration is deliberately not here. Upgrading a stored layout from one release
 * to the next is start-up work - it has to run before anything reads the
 * configuration, and it may need to touch keys no dialog shows - so it stays
 * with QxSettings::migrate() and the application's start-up path.
 *
 * The dialog does not own the settings object; the application does, and it
 * usually keeps one instance for the whole run.
 */
class QX_APP_EXPORT QxSettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit QxSettingsDialog(QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings, QWidget *parent = Q_NULLPTR);
    ~QxSettingsDialog() override;

    QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const;

    /*!
     * Adds a page: an entry in the list on the left and a form on the right
     * holding \a properties. The id addresses the page in addGroup() and in
     * editor(), so it has to be stable and unique.
     */
    void addPage(const QString &id, const QIcon &icon, const QString &title, const QList<QxProperty> &properties);
    /*!
     * Opens a group inside the page \a pageId and adds \a properties to it. A
     * page can hold both grouped and ungrouped properties, so this is a way to
     * structure a long page, not an alternative to addPage().
     */
    void addGroup(const QString &pageId, const QString &title, const QList<QxProperty> &properties);

    QStringList pageIds() const;
    /*! The form of the page \a pageId, or null when there is no such page. */
    QxPropertyEditor *editor(const QString &pageId) const;
    QString currentPageId() const;

    /*! Whether anything on any page differs from what it was loaded as. */
    bool isModified() const;

public Q_SLOTS:
    /*! Reads every page back out of the settings object; the dialog does this itself whenever it is shown. */
    void reload();
    /*! Sets every property of every page back to the default its descriptor declares. */
    void reset();
    /*! Writes every value to the settings object and syncs it. */
    void apply();
    void setCurrentPage(const QString &id);
    /*! Applies and closes. */
    void accept() override;
    /*! Discards the edits and closes. */
    void reject() override;

Q_SIGNALS:
    /*! The values have been written and synced. */
    void applied();
    void currentPageChanged(const QString &id);
protected:
    void showEvent(QShowEvent *event) override;
private:
    Q_DISABLE_COPY(QxSettingsDialog)
    QX_DECLARE_PRIVATE(QxSettingsDialog)
};

QX_APP_END_NAMESPACE

#endif   // QXSETTINGSDIALOG_H
