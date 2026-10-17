/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxapp/qxpropertyeditor.h"
#include "qxapp/qxsettingsdialog.h"
#include "qxcore/qxsettings.h"

#include <QAction>
#include <QHBoxLayout>
#include <QHash>
#include <QIcon>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QSettings>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

QX_APP_USE_NAMESPACE
using QxCore::QxSettings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(660, 480);
    setWindowTitle(tr("QxProperty / QxPropertyEditor / QxSettingsDialog"));

    // One file-backed settings object shared by both views: the editor reads its
    // baseline from it and writes back into it, and the dialog edits the very
    // same keys, so an edit made in one shows up in the other after a restart.
    m_settings = new QxSettings(QStringLiteral("qtcanpool_example.ini"), QSettings::IniFormat, this);

    auto *settingsAction = new QAction(tr("Settings dialog..."), this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
    menuBar()->addAction(settingsAction);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    buildEditor();
    layout->addWidget(m_editor, 1);

    m_stateLabel = new QLabel(tr("No pending changes"), central);

    m_applyButton = new QPushButton(tr("Apply"), central);
    auto *defaultsButton = new QPushButton(tr("Restore defaults"), central);
    auto *discardButton = new QPushButton(tr("Discard edits"), central);

    auto *buttons = new QHBoxLayout;
    buttons->addWidget(m_applyButton);
    buttons->addWidget(defaultsButton);
    buttons->addWidget(discardButton);
    buttons->addStretch();

    layout->addWidget(m_stateLabel);
    layout->addLayout(buttons);
    setCentralWidget(central);

    m_applyButton->setEnabled(false);
    connect(m_editor, &QxPropertyEditor::modifiedChanged, this, [this](bool modified) {
        m_applyButton->setEnabled(modified);
        m_stateLabel->setText(modified ? tr("Unsaved changes") : tr("No pending changes"));
    });
    connect(m_editor, &QxPropertyEditor::valueChanged, this, [this](const QString &key, const QVariant &value) {
        statusBar()->showMessage(tr("%1 set to %2").arg(key, value.toString()), 4000);
    });
    connect(m_applyButton, &QPushButton::clicked, this, &MainWindow::applyEdits);
    // The two ways back: reset() is what a fresh install would look like,
    // resetToInitial() is "undo what I just typed".
    connect(defaultsButton, &QPushButton::clicked, m_editor, &QxPropertyEditor::reset);
    connect(discardButton, &QPushButton::clicked, m_editor, &QxPropertyEditor::resetToInitial);

    buildSettingsDialog();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildEditor()
{
    m_editor = new QxPropertyEditor(this);

    QxProperty theme;
    theme.key = QStringLiteral("appearance/theme");
    theme.label = tr("Theme");
    theme.type = QxProperty::Enum;
    theme.choices = QStringList{tr("Light"), tr("Dark")};
    theme.defaultValue = QStringLiteral("Light");

    QxProperty showSplash;
    showSplash.key = QStringLiteral("appearance/showSplash");
    showSplash.label = tr("Show splash screen");
    showSplash.type = QxProperty::Bool;
    showSplash.defaultValue = true;

    QxProperty fontSize;
    fontSize.key = QStringLiteral("editor/fontSize");
    fontSize.label = tr("Font size");
    fontSize.type = QxProperty::Int;
    fontSize.defaultValue = 12;

    QxProperty lineNumbers;
    lineNumbers.key = QStringLiteral("editor/lineNumbers");
    lineNumbers.label = tr("Show line numbers");
    lineNumbers.type = QxProperty::Bool;
    lineNumbers.defaultValue = true;

    QxProperty workspace;
    workspace.key = QStringLiteral("editor/workspace");
    workspace.label = tr("Workspace");
    workspace.type = QxProperty::Path;
    workspace.defaultValue = QString();

    m_properties = QList<QxProperty>{theme, showSplash, fontSize, lineNumbers, workspace};

    // addGroup() opens a section, and everything added after it lands inside,
    // which is how a long form stays readable.
    m_editor->addGroup(tr("Appearance"), QList<QxProperty>{theme, showSplash});
    m_editor->addGroup(tr("Editor"), QList<QxProperty>{fontSize, lineNumbers, workspace});

    // What the ini file holds becomes the baseline, so a value saved by a
    // previous run is what "discard edits" returns to.
    QHash<QString, QVariant> stored;
    for (const QxProperty &property : m_properties) {
        stored.insert(property.key, m_settings->value(property.key, property.defaultValue));
    }
    m_editor->setAllValues(stored);
}

void MainWindow::buildSettingsDialog()
{
    m_dialog = new QxSettingsDialog(m_settings, this);
    m_dialog->addPage(QStringLiteral("appearance"), QIcon(), tr("Appearance"),
                      QList<QxProperty>{m_properties.value(0), m_properties.value(1)});
    m_dialog->addPage(QStringLiteral("editor"), QIcon(), tr("Editor"),
                      QList<QxProperty>{m_properties.value(2), m_properties.value(3), m_properties.value(4)});
}

void MainWindow::applyEdits()
{
    const QHash<QString, QVariant> values = m_editor->allValues();
    for (auto it = values.constBegin(); it != values.constEnd(); ++it) {
        m_settings->setValue(it.key(), it.value());
    }
    m_settings->sync();

    // Applied values are the new baseline: the modified flag clears, and
    // "discard edits" from now on means "back to what was just saved".
    m_editor->setAllValues(values);
    m_stateLabel->setText(
        tr("Saved %1 value(s) to %2").arg(static_cast<int>(values.size())).arg(m_settings->fileName()));
}

void MainWindow::openSettings()
{
    m_dialog->exec();
    // The dialog writes through the same settings object, so pick up whatever
    // it changed and make that the editor's baseline too.
    QHash<QString, QVariant> stored;
    for (const QxProperty &property : m_properties) {
        stored.insert(property.key, m_settings->value(property.key, property.defaultValue));
    }
    m_editor->setAllValues(stored);
}
