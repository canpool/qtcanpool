/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxsettingsdialog.h"
#include "qxsettingsdialog_p.h"

#include "qxpropertyeditor.h"

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QShowEvent>
#include <QStackedWidget>
#include <QVBoxLayout>

QX_APP_BEGIN_NAMESPACE

QX_CORE_USE_NAMESPACE

namespace
{

/*! Width the list of pages settles for. */
const int kListMinimumWidth = 120;
const int kListMaximumWidth = 240;
/*! Edge of the icon of a page entry. */
const int kIconSize = 24;

}   // namespace

QxSettingsDialogPrivate::QxSettingsDialogPrivate() = default;

void QxSettingsDialogPrivate::init()
{
    Q_Q(QxSettingsDialog);

    QVBoxLayout *outer = new QVBoxLayout(q);

    QHBoxLayout *body = new QHBoxLayout();
    outer->addLayout(body, 1);

    m_list = new QListWidget(q);
    m_list->setIconSize(QSize(kIconSize, kIconSize));
    m_list->setUniformItemSizes(true);
    m_list->setMinimumWidth(kListMinimumWidth);
    m_list->setMaximumWidth(kListMaximumWidth);
    // The list is an entry point, not a control to fill in: it must not take the
    // focus away from the page the user came to edit.
    m_list->setFocusPolicy(Qt::NoFocus);
    body->addWidget(m_list);

    m_stack = new QStackedWidget(q);
    body->addWidget(m_stack, 1);

    QObject::connect(m_list, &QListWidget::currentRowChanged, q, [this, q](int row) {
        if (row < 0 || row >= m_pages.count()) {
            return;
        }
        m_stack->setCurrentIndex(row);
        Q_EMIT q->currentPageChanged(m_pages.at(row).id);
    });

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply |
                                         QDialogButtonBox::RestoreDefaults,
                                     q);
    outer->addWidget(m_buttons);

    // Both of these are overrides, so they are called by name rather than through
    // a pointer to the base slot.
    QObject::connect(m_buttons, &QDialogButtonBox::accepted, q, [q]() {
        q->accept();
    });
    QObject::connect(m_buttons, &QDialogButtonBox::rejected, q, [q]() {
        q->reject();
    });
    // Applied and reset are called through the dialog rather than bound straight
    // to the button: clicked() carries a checked argument these two do not take.
    QObject::connect(m_buttons->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, q, [q]() {
        q->apply();
    });
    QObject::connect(m_buttons->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, q, [q]() {
        q->reset();
    });

    updateButtons();
}

int QxSettingsDialogPrivate::indexOf(const QString &id) const
{
    for (int i = 0; i < m_pages.count(); ++i) {
        if (m_pages.at(i).id == id) {
            return i;
        }
    }
    return -1;
}

void QxSettingsDialogPrivate::loadPage(const Page &page)
{
    if (!page.editor || !m_settings) {
        return;
    }

    QHash<QString, QVariant> values;
    for (const QxProperty &property : page.properties) {
        // A key that was never written reads as its default, which is what makes
        // a fresh installation look like a configured one.
        values.insert(property.key, m_settings->value(property.key, property.defaultValue));
    }
    page.editor->setAllValues(values);
}

void QxSettingsDialogPrivate::load()
{
    for (const Page &page : m_pages) {
        loadPage(page);
    }
}

void QxSettingsDialogPrivate::write() const
{
    if (!m_settings) {
        return;
    }

    for (const Page &page : m_pages) {
        if (!page.editor) {
            continue;
        }
        const QHash<QString, QVariant> values = page.editor->allValues();
        for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
            m_settings->setValue(it.key(), it.value());
        }
    }
    m_settings->sync();
}

bool QxSettingsDialogPrivate::isModified() const
{
    for (const Page &page : m_pages) {
        if (page.editor && page.editor->isModified()) {
            return true;
        }
    }
    return false;
}

void QxSettingsDialogPrivate::updateButtons()
{
    if (!m_buttons) {
        return;
    }
    if (QAbstractButton *apply = m_buttons->button(QDialogButtonBox::Apply)) {
        apply->setEnabled(isModified());
    }
}

QxSettingsDialog::QxSettingsDialog(QxSettings *settings, QWidget *parent)
    : QDialog(parent)
    , d_ptr(new QxSettingsDialogPrivate())
{
    Q_D(QxSettingsDialog);
    d->setPublic(this);
    d->m_settings = settings;
    d->init();
}

QxSettingsDialog::~QxSettingsDialog()
{
    QX_FINI_PRIVATE();
}

QxSettings *QxSettingsDialog::settings() const
{
    Q_D(const QxSettingsDialog);
    return d->m_settings;
}

void QxSettingsDialog::addPage(const QString &id, const QIcon &icon, const QString &title,
                               const QList<QxProperty> &properties)
{
    Q_D(QxSettingsDialog);

    if (id.isEmpty()) {
        qWarning("QxSettingsDialog: a page needs a non-empty id");
        return;
    }
    if (d->indexOf(id) >= 0) {
        qWarning("QxSettingsDialog: a page with id '%s' already exists", qPrintable(id));
        return;
    }

    QxPropertyEditor *editor = new QxPropertyEditor(d->m_stack);
    d->m_stack->addWidget(editor);
    for (const QxProperty &property : properties) {
        editor->addProperty(property);
    }

    // The list owns the entry; the id it is addressed by lives in the page list
    // below, so the item needs nothing else.
    new QListWidgetItem(icon, title, d->m_list);

    QxSettingsDialogPrivate::Page page;
    page.id = id;
    page.title = title;
    page.editor = editor;
    page.properties = properties;
    d->m_pages.append(page);

    QObject::connect(editor, &QxPropertyEditor::modifiedChanged, this, [d]() {
        d->updateButtons();
    });

    d->loadPage(d->m_pages.last());
    if (d->m_pages.count() == 1) {
        // The first page of a dialog has nothing to compete with.
        d->m_list->setCurrentRow(0);
        d->m_stack->setCurrentIndex(0);
    }
    d->updateButtons();
}

void QxSettingsDialog::addGroup(const QString &pageId, const QString &title, const QList<QxProperty> &properties)
{
    Q_D(QxSettingsDialog);

    const int index = d->indexOf(pageId);
    if (index < 0) {
        qWarning("QxSettingsDialog: there is no page with id '%s'", qPrintable(pageId));
        return;
    }

    QxPropertyEditor *editor = d->m_pages.at(index).editor;
    editor->addGroup(title, properties);
    d->m_pages[index].properties.append(properties);

    // The group brought keys this page had not read yet.
    d->loadPage(d->m_pages.at(index));
    d->updateButtons();
}

QStringList QxSettingsDialog::pageIds() const
{
    Q_D(const QxSettingsDialog);
    QStringList ids;
    for (const QxSettingsDialogPrivate::Page &page : d->m_pages) {
        ids.append(page.id);
    }
    return ids;
}

QxPropertyEditor *QxSettingsDialog::editor(const QString &pageId) const
{
    Q_D(const QxSettingsDialog);
    const int index = d->indexOf(pageId);
    return index < 0 ? Q_NULLPTR : d->m_pages.at(index).editor;
}

QString QxSettingsDialog::currentPageId() const
{
    Q_D(const QxSettingsDialog);
    const int index = d->m_stack->currentIndex();
    if (index < 0 || index >= d->m_pages.count()) {
        return QString();
    }
    return d->m_pages.at(index).id;
}

bool QxSettingsDialog::isModified() const
{
    Q_D(const QxSettingsDialog);
    return d->isModified();
}

void QxSettingsDialog::reload()
{
    Q_D(QxSettingsDialog);
    d->load();
    d->updateButtons();
}

void QxSettingsDialog::reset()
{
    Q_D(QxSettingsDialog);
    for (const QxSettingsDialogPrivate::Page &page : d->m_pages) {
        if (page.editor) {
            page.editor->reset();
        }
    }
    d->updateButtons();
}

void QxSettingsDialog::apply()
{
    Q_D(QxSettingsDialog);

    if (!d->m_settings) {
        qWarning("QxSettingsDialog: there is no settings object to apply to");
        return;
    }

    d->write();

    // What is on screen is what is stored now, so it becomes the new baseline and
    // the apply button goes dark again.
    for (const QxSettingsDialogPrivate::Page &page : d->m_pages) {
        if (page.editor) {
            page.editor->setAllValues(page.editor->allValues());
        }
    }
    d->updateButtons();
    Q_EMIT applied();
}

void QxSettingsDialog::setCurrentPage(const QString &id)
{
    Q_D(QxSettingsDialog);
    const int index = d->indexOf(id);
    if (index < 0) {
        return;
    }
    // The list drives the stack, so going through it keeps both in step.
    d->m_list->setCurrentRow(index);
}

void QxSettingsDialog::accept()
{
    apply();
    QDialog::accept();
}

void QxSettingsDialog::reject()
{
    Q_D(QxSettingsDialog);
    // Discarding means the dialog shows the stored values next time, not the ones
    // that were on screen when it closed.
    for (const QxSettingsDialogPrivate::Page &page : d->m_pages) {
        if (page.editor) {
            page.editor->resetToInitial();
        }
    }
    d->updateButtons();
    QDialog::reject();
}

void QxSettingsDialog::showEvent(QShowEvent *event)
{
    Q_D(QxSettingsDialog);
    // Shown, not constructed: a page added later still has to be read, and a
    // dialog that is opened twice has to show what is stored now rather than
    // what was edited and discarded last time.
    d->load();
    d->updateButtons();
    QDialog::showEvent(event);
}

QX_APP_END_NAMESPACE
