/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "settingspage.h"

#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluenttokens.h"
#include "qxfluent/fluentwindowmaterial.h"

#include "qxtheme/qxthememanager.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

QX_THEME_USE_NAMESPACE

bool SettingsPage::paneOpen() const
{
    return m_paneOpen;
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
    auto *materialBox = new QComboBox(materialRow);
    materialBox->addItem(tr("None"), int(FluentWindowMaterial::Material::None));
    materialBox->addItem(tr("Mica"), int(FluentWindowMaterial::Material::Mica));
    materialBox->addItem(tr("Mica Alt"), int(FluentWindowMaterial::Material::MicaAlt));
    materialBox->addItem(tr("Acrylic"), int(FluentWindowMaterial::Material::Acrylic));
    m_materialState = new QLabel(tr("not applied"), materialRow);
    materialLayout->addWidget(materialBox);
    materialLayout->addWidget(m_materialState);
    materialLayout->addStretch(1);
    addBlock(materialRow);

    connect(materialBox, QOverload<int>::of(&QComboBox::activated), this, [this, materialBox](int index) {
        const auto wanted = static_cast<FluentWindowMaterial::Material>(materialBox->itemData(index).toInt());
        // What the platform answers comes back through reportMaterial(): a page
        // cannot ask, only the window can.
        Q_EMIT materialRequested(wanted);
    });

    addSection(tr("Theme"));

    auto *themeBox = new QComboBox(this);
    const QStringList ids = engine->themeIds();
    for (const QString &id : ids) {
        themeBox->addItem(engine->themeInfo(id).name, id);
    }
    themeBox->setCurrentIndex(ids.indexOf(engine->theme()));
    connect(themeBox, QOverload<int>::of(&QComboBox::activated), this, [engine, themeBox](int index) {
        engine->setTheme(themeBox->itemData(index).toString());
    });
    connect(engine, &QxThemeManager::themeChanged, this, [themeBox](const QString &id) {
        // Following the system changes the theme without the box being touched,
        // so what is showing is read back from the engine.
        themeBox->setCurrentIndex(themeBox->findData(id));
    });
    addRow(tr("In force"), themeBox);

    addStretch();
}
