/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "settingspage.h"

#include "qxfluent/fluentcombobox.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluenttokens.h"
#include "qxfluent/fluentwindowmaterial.h"

#include "qxtheme/qxthememanager.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QWidget>

QX_THEME_USE_NAMESPACE

bool SettingsPage::paneOpen() const
{
    return m_paneOpen;
}

void SettingsPage::setPaneOpen(bool open)
{
    if (m_paneOpen == open) {
        return;
    }
    m_paneOpen = open;
    // Blocked, so that being told is not a request: this switch reports the
    // window's answer, it does not make one.
    QSignalBlocker blocker(m_paneSwitch);
    m_paneSwitch->setOn(open);
}

void SettingsPage::reportMaterial(bool applied)
{
    m_materialState->setText(applied ? tr("applied") : tr("not applied"));
}

SettingsPage::SettingsPage(QWidget *parent)
    : DemoPage(tr("Settings"), tr("What this application is, as opposed to what it is showing."), parent)
{
    addSection(tr("The pane"));

    auto *pane = new FluentSwitch(this);
    m_paneSwitch = pane;
    pane->setText(tr("Show the words beside the icons"));
    pane->setOn(m_paneOpen);
    connect(pane, &FluentSwitch::toggled, this, [this](bool on) {
        m_paneOpen = on;
        Q_EMIT paneOpenChanged(on);
    });
    addRow(tr("Open"), pane);

    addSection(tr("Motion"));

    QxThemeManager *engine = QxThemeManager::instance();
    auto *motion = new FluentSwitch(this);
    motion->setText(tr("Reduce motion"));
    motion->setOn(engine->reduceMotion());
    connect(motion, &FluentSwitch::toggled, this, [engine](bool on) {
        engine->setReduceMotion(on);
    });
    addRow(tr("Less of it"), motion);
    addNote(tr("The switch is the theme engine's, so every control that moves asks it: a breathing skeleton "
               "holds the middle of its breath, and a ring that does not know how far it has got stops "
               "sweeping."));

    addSection(tr("The window itself"));

    addNote(tr("A window material is not a control and has no rectangle of its own: it is a choice about "
               "the window everything else sits in, asked of the platform - which on anything other than "
               "Windows answers no, and that is worth showing rather than hiding."));

    auto *materialRow = new QWidget(this);
    auto *materialLayout = new QHBoxLayout(materialRow);
    materialLayout->setContentsMargins(QMargins());
    materialLayout->setSpacing(8);
    // The answers are the materials in their own order, so a row's index is its
    // material: a payload beside each answer is a second list to keep in step.
    auto *materialBox = new FluentComboBox(materialRow);
    materialBox->addItem(tr("None"));
    materialBox->addItem(tr("Mica"));
    materialBox->addItem(tr("Mica Alt"));
    materialBox->addItem(tr("Acrylic"));
    // A family's choice starts on nothing shown, unlike Qt's combo, which puts
    // the first answer in the face by itself: the row has to say where it is.
    materialBox->setCurrentIndex(int(FluentWindowMaterial::Material::None));
    m_materialState = new QLabel(tr("not applied"), materialRow);
    materialLayout->addWidget(materialBox);
    materialLayout->addWidget(m_materialState);
    materialLayout->addStretch(1);
    addBlock(materialRow);

    connect(materialBox, &FluentComboBox::currentIndexChanged, this, [this](int index) {
        const auto wanted = static_cast<FluentWindowMaterial::Material>(index);
        // What the platform answers comes back through reportMaterial(): a page
        // cannot ask, only the window can.
        Q_EMIT materialRequested(wanted);
    });

    addSection(tr("Theme"));

    // The ids are the answers, in the engine's order; the box shows the names.
    const QStringList ids = engine->themeIds();
    auto *themeBox = new FluentComboBox(this);
    for (const QString &id : ids) {
        themeBox->addItem(engine->themeInfo(id).name);
    }
    themeBox->setCurrentIndex(ids.indexOf(engine->theme()));
    connect(themeBox, &FluentComboBox::currentIndexChanged, this, [engine, ids](int index) {
        // The engine answering with the theme that is already in force says so
        // by not emitting, which is what keeps this from going round.
        engine->setTheme(ids.value(index));
    });
    connect(engine, &QxThemeManager::themeChanged, this, [themeBox, ids](const QString &id) {
        // Following the system changes the theme without the box being touched,
        // so what is showing is read back from the engine.
        themeBox->setCurrentIndex(ids.indexOf(id));
    });
    addRow(tr("In force"), themeBox);

    addStretch();
}
