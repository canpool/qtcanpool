/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPROPERTYEDITOR_H
#define QXPROPERTYEDITOR_H

#include "qxapp_global.h"
#include "qxproperty.h"

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtWidgets/QWidget>

QX_APP_BEGIN_NAMESPACE

class QxPropertyEditorPrivate;

/*!
 * A form built from a list of QxProperty descriptors.
 *
 * The editor is the key/value half of a settings dialog: it turns descriptors
 * into controls, keeps the current value of every one of them, and reports when
 * those values no longer match what they started as. It knows nothing about
 * where a value came from or where it should end up - reading and writing a
 * configuration file is the dialog's job (see QxSettingsDialog), and that split
 * is what keeps this class testable without a settings backend behind it.
 *
 * @code
 * QxPropertyEditor editor;
 * editor.addGroup(tr("Appearance"), {theme, fontSize});
 * editor.addProperty(showLineNumbers);
 * editor.setAllValues(loaded);        // the baseline
 * connect(&editor, &QxPropertyEditor::modifiedChanged, this, [](bool on) { applyButton->setEnabled(on); });
 * @endcode
 *
 * Sections work the way a form is read: addGroup() opens one and the properties
 * added after it land inside it, until the next addGroup(). A property added
 * before any group goes into the plain form at the top.
 *
 * Two kinds of "back to normal" are offered, because they answer different
 * questions: reset() goes back to the defaults a caller declared - what a fresh
 * installation would have - while resetToInitial() goes back to the values
 * handed to setAllValues(), which is what "discard my edits" means.
 */
class QX_APP_EXPORT QxPropertyEditor : public QWidget
{
    Q_OBJECT
public:
    explicit QxPropertyEditor(QWidget *parent = Q_NULLPTR);
    ~QxPropertyEditor() override;

    int count() const;
    /*! The keys of the properties, in the order they were added. */
    QStringList keys() const;
    bool contains(const QString &key) const;

    /*!
     * Adds \a property to the section the last addGroup() opened, or to the
     * plain form when there is none yet. A property whose key is already known
     * is refused: two controls writing the same key would fight over it.
     */
    void addProperty(const QxProperty &property);
    /*! Opens a section titled \a title and adds \a properties to it. */
    void addGroup(const QString &title, const QList<QxProperty> &properties);

    /*! The current value of \a key, or an invalid QVariant when there is no such property. */
    QVariant value(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);
    /*!
     * Sets every key of \a values that this editor knows and takes the result as
     * the new baseline, so isModified() is false right after this. Keys that are
     * not properties here are reported and skipped rather than stored.
     */
    void setAllValues(const QHash<QString, QVariant> &values);
    /*! The current value of every property. */
    QHash<QString, QVariant> allValues() const;

    /*! Whether the current values differ from the ones setAllValues() took as a baseline. */
    bool isModified() const;

public Q_SLOTS:
    /*! Sets every property back to its defaultValue. */
    void reset();
    /*! Sets every property back to the values setAllValues() was given. */
    void resetToInitial();

Q_SIGNALS:
    /*! A value changed, whether by the user or by the application. */
    void valueChanged(const QString &key, const QVariant &value);
    /*! isModified() changed. */
    void modifiedChanged(bool modified);
private:
    Q_DISABLE_COPY(QxPropertyEditor)
    QX_DECLARE_PRIVATE(QxPropertyEditor)
};

QX_APP_END_NAMESPACE

#endif   // QXPROPERTYEDITOR_H
