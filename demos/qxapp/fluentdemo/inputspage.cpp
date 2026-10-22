/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "inputspage.h"

#include "qxfluent/fluentrating.h"
#include "qxfluent/fluentslider.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluenttokens.h"

#include <QLabel>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The three settings a screen like this is usually for. They are here because
// three of a control read as a list, and one of a control reads as a sample.
struct Setting {
    const char *name;
    bool on;
};
}   // namespace

InputsPage::InputsPage(QWidget *parent)
    : DemoPage(tr("Input"), tr("What the user chooses, and what the screen says back about the choice."), parent)
{
    addSection(tr("Settings"));

    const Setting settings[] = {
        {QT_TR_NOOP("Wi-Fi"), true}, {QT_TR_NOOP("Bluetooth"), false}, {QT_TR_NOOP("Battery saver"), false}};
    for (const Setting &setting : settings) {
        auto *control = new FluentSwitch(this);
        control->setText(tr(setting.name));
        control->setOn(setting.on);
        addRow(tr(setting.name), control);
    }

    addSection(tr("A value in a range"));

    addNote(tr("Fluent's slider says where its rail is divided: a mark at every interval, because a value "
               "in a range is easier to set when the range is visible."));
    auto *slider = new FluentSlider(this);
    slider->setRange(0, 100);
    slider->setTickInterval(25);
    slider->setValue(40);
    m_volume = new QLabel(tr("40 of 100"), this);
    m_volume->setFont(FluentType::font(FluentType::Ramp::Body));
    connect(slider, &FluentSlider::valueChanged, this, [this](int value) {
        m_volume->setText(tr("%1 of 100").arg(value));
    });
    addRow(tr("Volume"), slider);
    addRow(QString(), m_volume);

    addSection(tr("A rating"));

    addNote(tr("A rating is a value the user sets by pointing, and it is read back the same way - which is "
               "why it carries an accessible face of its own."));
    auto *rating = new FluentRating(this);
    rating->setCount(5);
    rating->setValue(3);
    m_rating = new QLabel(tr("3 of 5"), this);
    m_rating->setFont(FluentType::font(FluentType::Ramp::Body));
    connect(rating, &FluentRating::valueChanged, this, [this](int value) {
        m_rating->setText(tr("%1 of 5").arg(value));
    });
    addRow(tr("Stars"), rating);
    addRow(QString(), m_rating);

    addStretch();
}
