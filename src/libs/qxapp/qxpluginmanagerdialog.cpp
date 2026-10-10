/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxpluginmanagerdialog.h"
#include "qxpluginmanagerdialog_p.h"

#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>
#include <utility>

QX_APP_BEGIN_NAMESPACE

QX_CORE_USE_NAMESPACE
QX_PLUGIN_USE_NAMESPACE

namespace
{

/* Column of the load order, of the switch, of the version and of the state. */
const int kOrderColumn = 0;
const int kPluginColumn = 1;
const int kVersionColumn = 2;
const int kStatusColumn = 3;

/*! Settings keys the dialog owns; a host reads them through the static helpers. */
QString disabledKey()
{
    return QStringLiteral("plugins/disabled");
}

QString enabledKey()
{
    return QStringLiteral("plugins/enabled");
}

/*! What an empty value reads as, so a row never looks like it was forgotten. */
QString emptyMark()
{
    return QStringLiteral("-");
}

QString joined(const QStringList &values)
{
    return values.isEmpty() ? emptyMark() : values.join(QStringLiteral(", "));
}

/*!
 * How far a plugin got, in words. QxPluginState::Disabled means two very
 * different things, and the error string is what tells them apart: a plugin that
 * agreed to stay off says nothing, one that was rejected says why.
 */
QString stateTextOf(QxPluginSpec *spec)
{
    const auto text = [](const char *source) {
        return QCoreApplication::translate("QxPluginManagerDialog", source);
    };

    switch (spec->state()) {
    case QxPluginState::Invalid:
        return text("Metadata is invalid");
    case QxPluginState::Read:
        return text("Not resolved");
    case QxPluginState::Resolved:
        return text("Resolved");
    case QxPluginState::Loaded:
        return text("Loaded");
    case QxPluginState::Initialized:
        return text("Running");
    case QxPluginState::Stopped:
        return text("Stopped");
    case QxPluginState::Disabled:
        return spec->error().isEmpty() ? text("Switched off") : text("Failed");
    }
    return QString();
}

/*! A read-only value of the detail form. */
QLabel *addValueRow(QFormLayout *form, const QString &title)
{
    QLabel *value = new QLabel;
    value->setWordWrap(true);
    value->setTextInteractionFlags(Qt::TextSelectableByMouse);
    form->addRow(title, value);
    return value;
}

}   // namespace

QxPluginManagerDialogPrivate::QxPluginManagerDialogPrivate() = default;

void QxPluginManagerDialogPrivate::init()
{
    Q_Q(QxPluginManagerDialog);

    q->setWindowTitle(QxPluginManagerDialog::tr("Plugins"));
    q->resize(840, 540);

    QVBoxLayout *outer = new QVBoxLayout(q);

    QHBoxLayout *body = new QHBoxLayout();
    outer->addLayout(body, 1);

    m_tree = new QTreeWidget(q);
    m_tree->setRootIsDecorated(false);
    m_tree->setUniformRowHeights(true);
    m_tree->setAllColumnsShowFocus(true);
    m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setColumnCount(4);
    m_tree->setHeaderLabels(QStringList{QxPluginManagerDialog::tr("Order"), QxPluginManagerDialog::tr("Plugin"),
                                        QxPluginManagerDialog::tr("Version"), QxPluginManagerDialog::tr("Status")});
    m_tree->header()->setSectionResizeMode(kPluginColumn, QHeaderView::Stretch);
    m_tree->setColumnWidth(kOrderColumn, 64);
    m_tree->setColumnWidth(kVersionColumn, 90);
    m_tree->setColumnWidth(kStatusColumn, 130);
    body->addWidget(m_tree, 3);

    QVBoxLayout *side = new QVBoxLayout();
    body->addLayout(side, 2);

    QGroupBox *details = new QGroupBox(QxPluginManagerDialog::tr("Details"), q);
    QFormLayout *form = new QFormLayout(details);
    m_id = addValueRow(form, QxPluginManagerDialog::tr("Plugin"));
    m_description = addValueRow(form, QxPluginManagerDialog::tr("Description"));
    m_version = addValueRow(form, QxPluginManagerDialog::tr("Version"));
    m_vendor = addValueRow(form, QxPluginManagerDialog::tr("Vendor"));
    m_file = addValueRow(form, QxPluginManagerDialog::tr("File"));
    m_order = addValueRow(form, QxPluginManagerDialog::tr("Load order"));
    m_state = addValueRow(form, QxPluginManagerDialog::tr("State"));
    m_dependencies = addValueRow(form, QxPluginManagerDialog::tr("Depends on"));
    m_optional = addValueRow(form, QxPluginManagerDialog::tr("Optional"));
    m_dependents = addValueRow(form, QxPluginManagerDialog::tr("Required by"));
    m_error = addValueRow(form, QxPluginManagerDialog::tr("Error"));
    side->addWidget(details);

    QGroupBox *diagnosticsBox = new QGroupBox(QxPluginManagerDialog::tr("Diagnostics"), q);
    QVBoxLayout *diagnosticsLayout = new QVBoxLayout(diagnosticsBox);
    m_diagnostics = new QLabel(diagnosticsBox);
    m_diagnostics->setWordWrap(true);
    m_diagnostics->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_diagnostics->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    diagnosticsLayout->addWidget(m_diagnostics);
    side->addWidget(diagnosticsBox, 1);

    QLabel *hint = new QLabel(
        QxPluginManagerDialog::tr("Switching a plugin on or off takes effect the next time the application starts."),
        q);
    hint->setWordWrap(true);
    outer->addWidget(hint);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, q);
    outer->addWidget(m_buttons);

    QObject::connect(m_buttons, &QDialogButtonBox::accepted, q, [q]() {
        q->accept();
    });
    QObject::connect(m_buttons, &QDialogButtonBox::rejected, q, [q]() {
        q->reject();
    });
    // Applied is called through the dialog rather than bound straight to the
    // button: clicked() carries a checked argument apply() does not take.
    QObject::connect(m_buttons->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, q, [q]() {
        q->apply();
    });

    QObject::connect(m_tree, &QTreeWidget::currentItemChanged, q, [this](QTreeWidgetItem *current, QTreeWidgetItem *) {
        const int row = current ? m_tree->indexOfTopLevelItem(current) : -1;
        showSpec(m_specs.value(row, Q_NULLPTR));
    });
    QObject::connect(m_tree, &QTreeWidget::itemChanged, q, [this](QTreeWidgetItem *item) {
        noteSwitch(item);
    });

    updateButtons();
}

QxPluginSpec *QxPluginManagerDialogPrivate::specById(const QString &id) const
{
    for (QxPluginSpec *spec : std::as_const(m_specs)) {
        if (spec->id() == id)
            return spec;
    }
    return Q_NULLPTR;
}

bool QxPluginManagerDialogPrivate::savedState(QxPluginSpec *spec) const
{
    if (!spec)
        return false;
    if (m_savedDisabled.contains(spec->id()))
        return false;
    if (m_savedEnabled.contains(spec->id()))
        return true;
    return spec->isEnabledByDefault();
}

bool QxPluginManagerDialogPrivate::isModified() const
{
    for (QxPluginSpec *spec : std::as_const(m_specs)) {
        if (m_switch.value(spec->id(), spec->isEnabledByDefault()) != savedState(spec))
            return true;
    }
    return false;
}

void QxPluginManagerDialogPrivate::updateButtons()
{
    if (!m_buttons)
        return;
    if (QAbstractButton *apply = m_buttons->button(QDialogButtonBox::Apply))
        apply->setEnabled(isModified());
}

void QxPluginManagerDialogPrivate::noteSwitch(QTreeWidgetItem *item)
{
    if (m_updating || !item)
        return;
    const int row = m_tree->indexOfTopLevelItem(item);
    if (row < 0 || row >= m_specs.count())
        return;
    m_switch.insert(m_specs.at(row)->id(), item->checkState(kPluginColumn) == Qt::Checked);
    updateButtons();
}

void QxPluginManagerDialogPrivate::rebuild()
{
    if (!m_tree)
        return;

    // Filling the list moves the switches about, and every move is a change the
    // user did not make; m_updating keeps those out of m_switch.
    m_updating = true;
    m_tree->clear();
    m_specs.clear();
    m_switch.clear();

    QList<QxPluginSpec *> specs;
    QHash<QxPluginSpec *, int> orderOf;
    if (!m_manager.isNull()) {
        specs = m_manager->allSpecs();
        const QList<QxPluginSpec *> ordered = m_manager->specs();
        for (int i = 0; i < ordered.count(); ++i)
            orderOf.insert(ordered.at(i), i);
    }

    // The plugins that load come first, in the order they load in; everything
    // else - switched off, failed, unresolved - follows by name.
    const auto rank = [&orderOf](QxPluginSpec *spec) {
        return orderOf.value(spec, -1);
    };
    std::stable_sort(specs.begin(), specs.end(), [&rank](QxPluginSpec *left, QxPluginSpec *right) {
        const int leftRank = rank(left);
        const int rightRank = rank(right);
        if (leftRank >= 0 && rightRank < 0)
            return true;
        if (leftRank < 0 && rightRank >= 0)
            return false;
        if (leftRank >= 0 && rightRank >= 0 && leftRank != rightRank)
            return leftRank < rightRank;
        return QString::compare(left->id(), right->id(), Qt::CaseInsensitive) < 0;
    });

    for (QxPluginSpec *spec : std::as_const(specs)) {
        const int order = orderOf.value(spec, -1);
        QTreeWidgetItem *item = new QTreeWidgetItem(m_tree);
        item->setText(kOrderColumn, order < 0 ? emptyMark() : QString::number(order + 1));
        item->setText(kPluginColumn, spec->id());
        item->setText(kVersionColumn, spec->version().toString());
        item->setText(kStatusColumn, stateTextOf(spec));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        const bool on = savedState(spec);
        item->setCheckState(kPluginColumn, on ? Qt::Checked : Qt::Unchecked);
        if (!spec->error().isEmpty())
            item->setToolTip(kPluginColumn, spec->error());

        m_specs.append(spec);
        m_switch.insert(spec->id(), on);
    }
    m_updating = false;

    if (!m_specs.isEmpty())
        m_tree->setCurrentItem(m_tree->topLevelItem(0));
    else
        showSpec(Q_NULLPTR);

    m_diagnostics->setText(!m_manager.isNull() && m_manager->hasError() ? m_manager->errorString()
                                                                        : QxPluginManagerDialog::tr("No errors."));
    updateButtons();
}

void QxPluginManagerDialogPrivate::showSpec(QxPluginSpec *spec)
{
    const QList<QLabel *> labels{m_id,    m_description,  m_version,  m_vendor,     m_file, m_order,
                                 m_state, m_dependencies, m_optional, m_dependents, m_error};

    if (!spec) {
        for (QLabel *label : labels)
            label->clear();
        return;
    }

    int order = -1;
    if (!m_manager.isNull())
        order = m_manager->specs().indexOf(spec);

    // What depends on this one: the other half of the picture the detail pane is
    // for, and the thing a reader cannot get by looking at one plugin alone.
    QStringList dependents;
    for (QxPluginSpec *other : std::as_const(m_specs)) {
        if (other == spec)
            continue;
        if (other->dependencies().contains(spec->id()))
            dependents.append(other->id());
        else if (other->optionalDependencies().contains(spec->id()))
            dependents.append(QxPluginManagerDialog::tr("%1 (optional)").arg(other->id()));
    }

    m_id->setText(spec->id());
    m_description->setText(spec->description().isEmpty() ? emptyMark() : spec->description());
    if (spec->compatVersion() == spec->version())
        m_version->setText(spec->version().toString());
    else
        m_version->setText(QxPluginManagerDialog::tr("%1 (host compatible from %2)")
                               .arg(spec->version().toString(), spec->compatVersion().toString()));
    m_vendor->setText(spec->vendor().isEmpty() ? emptyMark() : spec->vendor());
    m_file->setText(spec->filePath().isEmpty() ? emptyMark() : spec->filePath());
    m_order->setText(order < 0 ? emptyMark() : QString::number(order + 1));
    m_state->setText(stateTextOf(spec));
    m_dependencies->setText(joined(spec->dependencies()));
    m_optional->setText(joined(spec->optionalDependencies()));
    m_dependents->setText(joined(dependents));
    m_error->setText(spec->error().isEmpty() ? emptyMark() : spec->error());
}

QxPluginManagerDialog::QxPluginManagerDialog(QxPluginManager *manager, QxSettings *settings, QWidget *parent)
    : QDialog(parent)
    , d_ptr(new QxPluginManagerDialogPrivate())
{
    Q_D(QxPluginManagerDialog);
    d->setPublic(this);
    d->m_manager = manager;
    d->m_settings = settings;
    d->init();
    // Loaded here as well as on show, so a caller that never exec()s it still
    // gets a dialog that tells the truth.
    reload();
}

QxPluginManagerDialog::~QxPluginManagerDialog()
{
    Q_D(QxPluginManagerDialog);
    // The tree is a child of this dialog and outlives the private half, which
    // QX_FINI_PRIVATE deletes right now. Tearing the tree down would otherwise
    // call back into state that has just gone away.
    if (d->m_tree)
        d->m_tree->blockSignals(true);
    QX_FINI_PRIVATE();
}

QStringList QxPluginManagerDialog::disabledPlugins(QxSettings *settings)
{
    return settings ? settings->stringListValue(disabledKey()) : QStringList();
}

void QxPluginManagerDialog::setDisabledPlugins(QxSettings *settings, const QStringList &ids)
{
    if (!settings)
        return;
    if (ids.isEmpty())
        settings->remove(disabledKey());
    else
        settings->setValue(disabledKey(), ids);
}

QStringList QxPluginManagerDialog::enabledPlugins(QxSettings *settings)
{
    return settings ? settings->stringListValue(enabledKey()) : QStringList();
}

void QxPluginManagerDialog::setEnabledPlugins(QxSettings *settings, const QStringList &ids)
{
    if (!settings)
        return;
    if (ids.isEmpty())
        settings->remove(enabledKey());
    else
        settings->setValue(enabledKey(), ids);
}

QxPluginManager *QxPluginManagerDialog::manager() const
{
    Q_D(const QxPluginManagerDialog);
    return d->m_manager;
}

int QxPluginManagerDialog::rowOf(const QString &id) const
{
    Q_D(const QxPluginManagerDialog);
    for (int i = 0; i < d->m_specs.count(); ++i) {
        if (d->m_specs.at(i)->id() == id)
            return i;
    }
    return -1;
}

int QxPluginManagerDialog::count() const
{
    Q_D(const QxPluginManagerDialog);
    return d->m_specs.count();
}

bool QxPluginManagerDialog::isChecked(const QString &id) const
{
    Q_D(const QxPluginManagerDialog);
    QxPluginSpec *spec = d->specById(id);
    if (!spec)
        return false;
    return d->m_switch.value(id, spec->isEnabledByDefault());
}

void QxPluginManagerDialog::setChecked(const QString &id, bool checked)
{
    Q_D(QxPluginManagerDialog);
    const int row = rowOf(id);
    if (row < 0)
        return;
    d->m_updating = true;
    d->m_tree->topLevelItem(row)->setCheckState(kPluginColumn, checked ? Qt::Checked : Qt::Unchecked);
    d->m_updating = false;
    d->m_switch.insert(id, checked);
    d->updateButtons();
}

void QxPluginManagerDialog::setCurrentPlugin(const QString &id)
{
    Q_D(QxPluginManagerDialog);
    const int row = rowOf(id);
    if (row < 0)
        return;
    // Going through the tree keeps the selection, the detail pane and the
    // selectionChanged handler in step.
    d->m_tree->setCurrentItem(d->m_tree->topLevelItem(row));
}

QString QxPluginManagerDialog::diagnostics() const
{
    Q_D(const QxPluginManagerDialog);
    return d->m_diagnostics ? d->m_diagnostics->text() : QString();
}

bool QxPluginManagerDialog::isModified() const
{
    Q_D(const QxPluginManagerDialog);
    return d->isModified();
}

void QxPluginManagerDialog::reload()
{
    Q_D(QxPluginManagerDialog);
    d->m_savedDisabled = disabledPlugins(d->m_settings);
    d->m_savedEnabled = enabledPlugins(d->m_settings);
    d->rebuild();
}

void QxPluginManagerDialog::revert()
{
    Q_D(QxPluginManagerDialog);
    d->m_updating = true;
    for (int i = 0; i < d->m_specs.count(); ++i) {
        QxPluginSpec *spec = d->m_specs.at(i);
        const bool on = d->savedState(spec);
        d->m_tree->topLevelItem(i)->setCheckState(kPluginColumn, on ? Qt::Checked : Qt::Unchecked);
        d->m_switch.insert(spec->id(), on);
    }
    d->m_updating = false;
    d->updateButtons();
}

void QxPluginManagerDialog::apply()
{
    Q_D(QxPluginManagerDialog);
    if (!d->m_settings) {
        qWarning("QxPluginManagerDialog: there is no settings object to apply to");
        return;
    }

    QStringList disabled;
    QStringList enabled;
    for (QxPluginSpec *spec : std::as_const(d->m_specs)) {
        const bool want = d->m_switch.value(spec->id(), spec->isEnabledByDefault());
        // Only the deviations are stored: a plugin that agrees with its metadata
        // belongs in neither list, which keeps both lists small and keeps the
        // metadata the default a fresh installation starts from.
        if (want == spec->isEnabledByDefault())
            continue;
        if (want)
            enabled.append(spec->id());
        else
            disabled.append(spec->id());
    }

    setDisabledPlugins(d->m_settings, disabled);
    setEnabledPlugins(d->m_settings, enabled);
    d->m_settings->sync();

    // What is stored is the new baseline, so the apply button goes dark again.
    d->m_savedDisabled = disabled;
    d->m_savedEnabled = enabled;
    d->updateButtons();
    Q_EMIT applied();
}

void QxPluginManagerDialog::accept()
{
    apply();
    QDialog::accept();
}

void QxPluginManagerDialog::reject()
{
    revert();
    QDialog::reject();
}

void QxPluginManagerDialog::showEvent(QShowEvent *event)
{
    Q_D(QxPluginManagerDialog);
    // Shown, not constructed: a later loadPlugins() finds plugins this dialog has
    // not seen, and one opened twice has to show what is stored now rather than
    // what was edited and discarded last time.
    reload();
    QDialog::showEvent(event);
}

QX_APP_END_NAMESPACE
