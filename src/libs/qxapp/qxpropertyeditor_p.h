/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxproperty.h"
#include "qxpropertyeditor.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QVariant>

QT_BEGIN_NAMESPACE
class QFormLayout;
class QVBoxLayout;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

/*!
 * The state behind QxPropertyEditor: one row per property, the baseline the
 * values are compared against, and the flag that keeps the controls from feeding
 * their own programmatic update back into the model.
 *
 * A row keeps two widget pointers because a couple of types need a container:
 * the field is what the form lays out, and the input is the control that really
 * holds the value - the same widget for every type except Path, whose field is a
 * line edit next to a browse button.
 */
class QxPropertyEditorPrivate
{
    QX_DECLARE_PUBLIC(QxPropertyEditor)
public:
    QxPropertyEditorPrivate();

    struct Row {
        QxProperty property;
        QWidget *field = Q_NULLPTR;
        QWidget *input = Q_NULLPTR;
    };

    void init();
    int indexOf(const QString &key) const;
    /*! Builds the control for \a property and adds it to \a form. */
    void insertRow(QFormLayout *form, const QxProperty &property);
    /*! Creates the control for \a property; \a input receives the value holder. */
    QWidget *createField(const QxProperty &property, QWidget **input);
    /*! Feeds every change of the control at \a index back into the model. */
    void connectField(int index);

    QVariant readField(const Row &row) const;
    void writeField(const Row &row, const QVariant &value);
    /*! Stores \a value on the row, updates its control and reports the change. */
    void setRowValue(int index, const QVariant &value);
    void updateModified();
public:
    QVBoxLayout *m_layout = Q_NULLPTR;
    /*! The form the next property is added to, i.e. the last group opened. */
    QFormLayout *m_form = Q_NULLPTR;
    QList<Row> m_rows;
    QHash<QString, QVariant> m_initial;
    bool m_modified = false;
    /*! True while the controls are being filled, so their signals are ignored. */
    bool m_updating = false;
};

QX_APP_END_NAMESPACE
