/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "harmonypage.h"

#include "qxharmony/harmonybutton.h"
#include "qxharmony/harmonycard.h"
#include "qxharmony/harmonyslider.h"
#include "qxharmony/harmonyswitch.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_HARMONY_USE_NAMESPACE

namespace
{
/*!
 * The three levels of a button in one row.
 *
 * A level is only legible next to the other two, so the row carries all three -
 * the same bargain the button row on the Cupertino page makes, and the same one
 * the window material row on the Fluent page makes: one control's row, every
 * face that control has.
 */
QWidget *buttonRow(QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto *primary = new HarmonyButton(QStringLiteral("Continue"), row);
    auto *secondary = new HarmonyButton(QStringLiteral("Later"), row);
    secondary->setKind(HarmonyButton::Kind::Secondary);
    auto *text = new HarmonyButton(QStringLiteral("Cancel"), row);
    text->setKind(HarmonyButton::Kind::Text);

    layout->addWidget(primary);
    layout->addWidget(secondary);
    layout->addWidget(text);
    layout->addStretch(1);
    return row;
}
}   // namespace

HarmonyPage::HarmonyPage(QWidget *parent)
    : GalleryPage(QStringLiteral("harmony"), parent)
{
    addSection(tr("Input"));

    // The switch says yes or no with a knob that overshoots and settles back:
    // the movement is the family's, and it is why the row is here.
    auto *onOff = addControl<HarmonySwitch>(QStringLiteral("harmonySwitch"), QStringLiteral("HarmonySwitch"),
                                            new HarmonySwitch(this));
    onOff->setOn(true);

    // A slider that floats its number above the knob while it is being dragged,
    // which is the whole reason this control is taller than a track.
    auto *slider = addControl<HarmonySlider>(QStringLiteral("harmonySlider"), QStringLiteral("HarmonySlider"),
                                             new HarmonySlider(this));
    slider->setRange(0, 100);
    slider->setValue(40);

    addSection(tr("Display"));

    // The card is the family's surface: a corner you can see and a lift under
    // it. It stays at the level it comes with - the room it reserves is the room
    // its own lift needs - and its one line of text is inset by that same room,
    // which is the only part of the rectangle its contents may use.
    auto *card =
        addControl<HarmonyCard>(QStringLiteral("harmonyCard"), QStringLiteral("HarmonyCard"), new HarmonyCard(this));
    card->setFixedSize(240, 96);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(HarmonyCard::shadowRoom(), HarmonyCard::shadowRoom(), HarmonyCard::shadowRoom(),
                                   HarmonyCard::shadowRoom());
    cardLayout->addWidget(new QLabel(tr("A card is a surface the page is not."), card));

    addSection(tr("Actions"));

    // The three levels side by side, and the middle one is the family's own
    // answer: a quiet neutral capsule with the word in the brand's blue.
    addControl<QWidget>(QStringLiteral("harmonyButton"), QStringLiteral("HarmonyButton"), buttonRow(this));
}
