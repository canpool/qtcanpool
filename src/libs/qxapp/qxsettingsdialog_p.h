/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxcore/qxsettings.h"
#include "qxproperty.h"
#include "qxsettingsdialog.h"

#include <QtCore/QList>
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QListWidget;
class QStackedWidget;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxPropertyEditor;

/*!
 * The state behind QxSettingsDialog: the pages, the settings object they are
 * read from and written to, and the buttons.
 *
 * A page keeps the descriptors it was built from as well as its editor, because
 * the descriptor is the only place the default lives - and the default is what a
 * key that has never been written has to read as.
 */
class QxSettingsDialogPrivate
{
    QX_DECLARE_PUBLIC(QxSettingsDialog)
public:
    QxSettingsDialogPrivate();

    struct Page {
        QString id;
        QString title;
        QxPropertyEditor *editor = Q_NULLPTR;
        QList<QxProperty> properties;
    };

    void init();
    int indexOf(const QString &id) const;
    /*! Fills \a page from the settings object, falling back to its defaults. */
    void loadPage(const Page &page);
    void load();
    void write() const;
    bool isModified() const;
    void updateButtons();
public:
    QPointer<QX_CORE_PREPEND_NAMESPACE(QxSettings)> m_settings;
    QListWidget *m_list = Q_NULLPTR;
    QStackedWidget *m_stack = Q_NULLPTR;
    QDialogButtonBox *m_buttons = Q_NULLPTR;
    QList<Page> m_pages;
};

QX_APP_END_NAMESPACE
