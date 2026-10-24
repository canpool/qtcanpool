/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "settingspage.h"

#include "qxmaterial/materialselect.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtokens.h"

#include "qxtheme/qxthememanager.h"

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

    // The answers are the shapes in their own order, so a row's index is its
    // shape: a payload beside each answer is a second list to keep in step.
    // The select carries the name of the choice itself - it is a field, and a
    // field is labelled - so the row does not say it a second time.
    auto *variantBox = new MaterialSelect(this);
    variantBox->setLabel(tr("Shape"));
    variantBox->addItem(tr("Small"));
    variantBox->addItem(tr("Center aligned"));
    variantBox->addItem(tr("Medium"));
    variantBox->addItem(tr("Large"));
    // The bar is built at Small and only this page moves it, so the field starts
    // there: a select that shows nothing is a question already answered
    // somewhere else.
    variantBox->setCurrentIndex(int(MaterialTopAppBar::Variant::Small));
    connect(variantBox, &MaterialSelect::currentIndexChanged, this, [this](int index) {
        Q_EMIT variantRequested(static_cast<MaterialTopAppBar::Variant>(index));
    });
    addRow(QString(), variantBox);

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

    // The ids are the answers, in the engine's order; the field shows the names.
    const QStringList ids = engine->themeIds();
    auto *themeBox = new MaterialSelect(this);
    themeBox->setLabel(tr("In force"));
    for (const QString &id : ids) {
        themeBox->addItem(engine->themeInfo(id).name);
    }
    themeBox->setCurrentIndex(ids.indexOf(engine->theme()));
    connect(themeBox, &MaterialSelect::currentIndexChanged, this, [engine, ids](int index) {
        // The engine answering with the theme that is already in force says so
        // by not emitting, which is what keeps this from going round.
        engine->setTheme(ids.value(index));
    });
    connect(engine, &QxThemeManager::themeChanged, this, [themeBox, ids](const QString &id) {
        // Following the system changes the theme without the box being touched,
        // so what is showing is read back from the engine.
        themeBox->setCurrentIndex(ids.indexOf(id));
    });
    addRow(QString(), themeBox);

    addStretch();
}
