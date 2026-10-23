/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinopage.h"

#include "qxcupertino/cupertinoactivityindicator.h"
#include "qxcupertino/cupertinobutton.h"
#include "qxcupertino/cupertinosegmentedcontrol.h"
#include "qxcupertino/cupertinoslider.h"
#include "qxcupertino/cupertinoswitch.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QX_CUPERTINO_USE_NAMESPACE

namespace
{
/*!
 * The three levels of a button in one row.
 *
 * A button's level is the whole of what tells one from another here, so a row
 * that showed one level would be hiding two thirds of the control. The row is a
 * plain widget holding the three, which is the same bargain the window material
 * row on the Fluent page makes: the row is one control's row, and what it
 * carries is every face that control has.
 */
QWidget *buttonRow(QWidget *parent)
{
    auto *row = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    auto *filled = new CupertinoButton(QStringLiteral("Continue"), row);
    auto *tinted = new CupertinoButton(QStringLiteral("Later"), row);
    tinted->setRole(CupertinoButton::Role::Tinted);
    auto *plain = new CupertinoButton(QStringLiteral("Cancel"), row);
    plain->setRole(CupertinoButton::Role::Plain);

    layout->addWidget(filled);
    layout->addWidget(tinted);
    layout->addWidget(plain);
    layout->addStretch(1);
    return row;
}
}   // namespace

CupertinoPage::CupertinoPage(QWidget *parent)
    : GalleryPage(QStringLiteral("cupertino"), parent)
{
    addSection(tr("Input"));

    // The switch is the family's only control without a label: the capsule is
    // the whole of it, and the sentence beside it belongs to the page.
    auto *onOff = addControl<CupertinoSwitch>(QStringLiteral("cupertinoSwitch"), QStringLiteral("CupertinoSwitch"),
                                              new CupertinoSwitch(this));
    onOff->setOn(true);

    // A slider with no ticks, which is what this family draws: the track says
    // how far and the knob says where.
    auto *slider = addControl<CupertinoSlider>(QStringLiteral("cupertinoSlider"), QStringLiteral("CupertinoSlider"),
                                               new CupertinoSlider(this));
    slider->setRange(0, 100);
    slider->setValue(40);

    addSection(tr("Selection"));

    // One row of three, one of them picked, and a white pill under the pick -
    // the whole row is one keyboard stop, so the arrows move the pick.
    auto *segments = addControl<CupertinoSegmentedControl>(QStringLiteral("cupertinoSegmentedControl"),
                                                           QStringLiteral("CupertinoSegmentedControl"),
                                                           new CupertinoSegmentedControl(this));
    segments->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    segments->setCurrentIndex(1);

    addSection(tr("Actions"));

    // The three levels side by side: what a level is worth is only legible next
    // to the other two.
    addControl<QWidget>(QStringLiteral("cupertinoButton"), QStringLiteral("CupertinoButton"), buttonRow(this));

    addSection(tr("Status"));

    // The mark a page shows while it waits. There is no value in it and no way
    // to stop it from here - a wait belongs to whatever is doing the waiting -
    // so the row is the mark and nothing else.
    addControl<CupertinoActivityIndicator>(QStringLiteral("cupertinoActivityIndicator"),
                                           QStringLiteral("CupertinoActivityIndicator"),
                                           new CupertinoActivityIndicator(this));
}
