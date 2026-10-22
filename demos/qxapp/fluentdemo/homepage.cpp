/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "homepage.h"

#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentinfobar.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluentteachingtip.h"
#include "qxfluent/fluenttokens.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

HomePage::HomePage(QWidget *parent)
    : DemoPage(tr("Home"),
               tr("An application built out of QxFluent: a navigation pane, a command bar, "
                  "and every control the library ships, on a screen that uses them for something."),
               parent)
{
    addSection(tr("What this is"));

    // A card is the surface a screen is made of: what it does is lift whatever
    // is put on it, which is why the text goes inside it rather than beside it.
    m_card = new FluentCard(this);
    m_card->setElevation(FluentElevation::Level::Depth4);
    m_card->setFixedHeight(120);
    auto *cardLayout = new QVBoxLayout(m_card);
    // The margin the shadow is drawn in is the only part of the card's
    // rectangle its contents may use.
    const int inset = FluentCard::shadowMargin(FluentElevation::Level::Depth4);
    cardLayout->setContentsMargins(inset, inset, inset, inset);
    auto *cardText = new QLabel(tr("Every control on these screens is a QxFluent control. Nothing here is "
                                   "a QPushButton with a style sheet, and nothing is a QWidget subclass "
                                   "that Qt drew."),
                                m_card);
    cardText->setWordWrap(true);
    cardText->setFont(FluentType::font(FluentType::Ramp::Body));
    cardLayout->addWidget(cardText);
    addBlock(m_card);

    addSection(tr("A message that stays"));

    m_info = new FluentInfoBar(this);
    m_info->setSeverity(FluentInfoBar::Severity::Informational);
    m_info->setTitle(tr("Nothing was written to disk"));
    m_info->setMessage(tr("An info bar is dismissed by the user, not by a timer, so it can say something "
                          "that has to be read."));
    m_info->setOpen(true);
    addBlock(m_info);

    m_showInfo = new FluentSwitch(this);
    m_showInfo->setText(tr("Show the message"));
    m_showInfo->setOn(true);
    connect(m_showInfo, &FluentSwitch::toggled, this, [this](bool on) {
        m_info->setOpen(on);
    });
    addRow(tr("Open"), m_showInfo);

    addSection(tr("Pointing at things"));

    addNote(tr("A teaching tip knows what it is about: it is given the thing it points at and opens on the "
               "side there is room for."));
    m_tipButton = new QPushButton(tr("Point at me"), this);
    connect(m_tipButton, &QPushButton::clicked, this, [this]() {
        if (!m_tip) {
            m_tip = new FluentTeachingTip(this);
            m_tip->setTitle(tr("A teaching tip"));
            m_tip->setMessage(tr("It points at the thing it is about, and closes when the pointer goes "
                                 "somewhere else."));
        }
        m_tip->setTarget(m_tipButton);
        m_tip->setPreferredSide(FluentTeachingTip::Side::Bottom);
        m_tip->setOpen(true);
    });
    addRow(tr("Tip"), m_tipButton);

    addStretch();
}
