/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "selectionpage.h"

#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtokens.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// A row of chips reads as a set of things the user is carrying: a filter
// applied, a tag, a suggestion offered. Eight between them, because two chips
// that touch read as one wide one.
QWidget *chipRow(const QList<MaterialChip *> &chips)
{
    auto *row = new QWidget;
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(8);
    for (MaterialChip *chip : chips) {
        layout->addWidget(chip);
    }
    layout->addStretch(1);
    return row;
}
}   // namespace

SelectionPage::SelectionPage(QWidget *parent)
    : DemoPage(tr("Selection"), tr("What the user chooses, and what the screen says back about the choice."), parent)
{
    addSection(tr("Settings"));

    // Three of a control read as a list, and one of a control reads as a
    // sample - so there are three, and they are the three a screen like this
    // is usually for.
    struct Setting {
        const char *name;
        bool on;
    };
    const Setting settings[] = {
        {QT_TR_NOOP("Wi-Fi"), true}, {QT_TR_NOOP("Bluetooth"), false}, {QT_TR_NOOP("Battery saver"), false}};
    for (const Setting &setting : settings) {
        auto *control = new MaterialSwitch(this);
        control->setText(tr(setting.name));
        control->setOn(setting.on);
        addRow(tr(setting.name), control);
    }

    addSection(tr("One of a fixed number of answers"));

    addNote(tr("A segmented button is a row inside a toolbar or a header, where a group of radio buttons "
               "would arrive with a frame and a shape of its own. What says which one is in force is the "
               "fill, and the arrows move it - the row is one stop on the way through the keyboard, not "
               "one per part."));
    auto *segmented = new MaterialSegmentedButton(this);
    segmented->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    segmented->setCurrentIndex(1);
    addRow(tr("Range"), segmented);

    addSection(tr("A value in a range"));

    addNote(tr("Material's slider says nothing of where its rail is divided: the handle's position is the "
               "answer, and a mark at every interval would be a second answer to a question already "
               "answered. The handle is bigger than the track it covers, and lifts off it."));
    auto *slider = new MaterialSlider(this);
    slider->setRange(0, 100);
    slider->setValue(40);
    m_value = new QLabel(tr("40 of 100"), this);
    m_value->setFont(MaterialType::font(MaterialType::Scale::Body));
    connect(slider, &MaterialSlider::valueChanged, this, [this](int value) {
        m_value->setText(tr("%1 of 100").arg(value));
    });
    addRow(tr("Volume"), slider);
    addRow(QString(), m_value);

    addSection(tr("Choices the user is carrying"));

    addNote(tr("A button in a row of buttons reads as a toolbar; a row of chips reads as a set of things "
               "the user is carrying. An assist chip does something when it is pressed and holds nothing; "
               "a filter chip holds a state, is filled while it is in force and carries a mark."));
    auto *assist = new MaterialChip(MaterialChip::Kind::Assist, this);
    assist->setText(tr("Suggest an edit"));
    auto *widgets = new MaterialChip(MaterialChip::Kind::Filter, this);
    widgets->setText(tr("Widgets"));
    // A filter chip that is in force is filled and carries a mark, so "which of
    // these are on" is readable without pressing each one to find out.
    widgets->setChecked(true);
    auto *tools = new MaterialChip(MaterialChip::Kind::Filter, this);
    tools->setText(tr("Tools"));
    addBlock(chipRow({assist, widgets, tools}));

    addStretch();
}
