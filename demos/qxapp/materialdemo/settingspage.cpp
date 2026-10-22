/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "settingspage.h"

#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtokens.h"

#include "qxtheme/qxthememanager.h"

#include <QComboBox>
#include <QLabel>

QX_THEME_USE_NAMESPACE

SettingsPage::SettingsPage(QWidget *parent)
    : DemoPage(tr("Settings"), tr("What this application is, as opposed to what it is showing."), parent)
{
    QxThemeManager *engine = QxThemeManager::instance();

    addSection(tr("The top of the screen"));

    addNote(tr("Which of the four shapes the bar takes is a question about the screen, not about taste: the "
               "two at fifty-six name it beside its icons, and the two taller ones give the name a row of "
               "its own - which is what a screen that is a place wants, and is why they are taller rather "
               "than bolder."));

    auto *variantBox = new QComboBox(this);
    variantBox->addItem(tr("Small"), int(MaterialTopAppBar::Variant::Small));
    variantBox->addItem(tr("Center aligned"), int(MaterialTopAppBar::Variant::CenterAligned));
    variantBox->addItem(tr("Medium"), int(MaterialTopAppBar::Variant::Medium));
    variantBox->addItem(tr("Large"), int(MaterialTopAppBar::Variant::Large));
    connect(variantBox, QOverload<int>::of(&QComboBox::activated), this, [this, variantBox](int index) {
        Q_EMIT variantRequested(static_cast<MaterialTopAppBar::Variant>(variantBox->itemData(index).toInt()));
    });
    addRow(tr("Shape"), variantBox);

    auto *scrolled = new MaterialSwitch(this);
    scrolled->setText(tr("Say content has moved under the bar"));
    connect(scrolled, &MaterialSwitch::toggled, this, [this](bool on) {
        Q_EMIT scrolledChanged(on);
    });
    addRow(tr("Scrolled under"), scrolled);
    addNote(tr("What the bar says about it is a change of surface, not a line and not a shadow: at rest it "
               "is the page, and once content has moved under it, it is the surface one step away. That is "
               "the whole of what Material calls on-scroll elevation."));

    addSection(tr("Motion"));

    auto *motion = new MaterialSwitch(this);
    motion->setText(tr("Reduce motion"));
    motion->setOn(engine->reduceMotion());
    connect(motion, &MaterialSwitch::toggled, this, [engine](bool on) {
        engine->setReduceMotion(on);
    });
    addRow(tr("Less of it"), motion);
    addNote(tr("The switch is the theme engine's, so every control that moves asks it: a breathing skeleton "
               "holds the middle of its breath, a ring that does not know how far it has got stops "
               "sweeping, and a snackbar arrives without the rise."));

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
