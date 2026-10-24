/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "orderpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QSignalBlocker>
#include <QtCore/QStringList>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kGap = 16;
constexpr int kCardPadding = 16;
constexpr int kCardWidth = 520;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! The surface every step stands on: one card of one width, with the room a
 *  card keeps inside it.
 *
 *  The width is fixed rather than taken from the page so that the three steps
 *  are the same size: a stepper whose card changes width as the user walks
 *  through it looks like three screens rather than one. */
MaterialCard *stepCard(QWidget *parent)
{
    auto *card = new MaterialCard(MaterialCard::Variant::Outlined, parent);
    card->setFixedWidth(kCardWidth);
    return card;
}

QVBoxLayout *stepLayout(MaterialCard *card)
{
    auto *layout = new QVBoxLayout(card);
    const int inset = MaterialCard::shadowMargin(MaterialCard::Variant::Outlined) + kCardPadding;
    layout->setContentsMargins(inset, inset, inset, inset);
    layout->setSpacing(12);
    return layout;
}
}   // namespace

OrderPage::OrderPage(QWidget *parent)
    : DemoPage(tr("Order"), tr("One decision taken in three steps, and never all three at once."), parent)
{
    m_steps = new MaterialSegmentedButton(this);
    m_steps->setItems(QStringList{tr("Address"), tr("Payment"), tr("Review")});
    m_steps->setCurrentIndex(0);
    // As wide as the card it steps through: a stepper that spans the window is
    // three segments that happen to be far apart, and the eye reads the width
    // before it reads the steps.
    auto *stepRow = new QWidget(this);
    auto *stepRowLayout = new QHBoxLayout(stepRow);
    stepRowLayout->setContentsMargins(QMargins());
    stepRowLayout->addWidget(m_steps);
    stepRowLayout->addStretch(1);
    m_steps->setFixedWidth(kCardWidth);
    addBlock(stepRow);

    m_pages = new QStackedWidget(this);
    addBlock(m_pages);

    // Step one: where it goes.
    auto *address = stepCard(m_pages);
    QVBoxLayout *addressLayout = stepLayout(address);
    addressLayout->addWidget(line(tr("Where it goes"), MaterialType::Scale::Title, true, address));
    auto *name = new MaterialTextField(address);
    name->setVariant(MaterialTextField::Variant::Outlined);
    name->setLabel(tr("Full name"));
    addressLayout->addWidget(name);
    auto *street = new MaterialTextField(address);
    street->setVariant(MaterialTextField::Variant::Outlined);
    street->setLabel(tr("Street"));
    addressLayout->addWidget(street);
    auto *same = new MaterialSwitch(address);
    same->setText(tr("The bill goes to the same address"));
    same->setOn(true);
    addressLayout->addWidget(same);
    m_pages->addWidget(address);

    // Step two: how it is paid for. Three filter chips, because one of them is
    // in force and the other two are not - which is what a chip holds and a
    // button does not.
    auto *payment = stepCard(m_pages);
    QVBoxLayout *paymentLayout = stepLayout(payment);
    paymentLayout->addWidget(line(tr("How it is paid for"), MaterialType::Scale::Title, true, payment));

    auto *methods = new QWidget(payment);
    auto *methodLayout = new QHBoxLayout(methods);
    methodLayout->setContentsMargins(QMargins());
    methodLayout->setSpacing(8);
    QList<MaterialChip *> chips;
    const char *ways[] = {QT_TR_NOOP("Card"), QT_TR_NOOP("Bank"), QT_TR_NOOP("Wallet")};
    for (const char *way : ways) {
        auto *chip = new MaterialChip(MaterialChip::Kind::Filter, methods);
        chip->setText(tr(way));
        chips.append(chip);
        methodLayout->addWidget(chip);
    }
    methodLayout->addStretch(1);
    chips.first()->setChecked(true);

    // One of them, always: the choice is a set of one, so putting a second in
    // force takes the first one back rather than adding to it. The blocker is
    // what keeps that from being two signals about one decision.
    for (MaterialChip *chip : chips) {
        connect(chip, &MaterialChip::toggled, this, [chips, chip](bool on) {
            if (!on) {
                return;
            }
            for (MaterialChip *other : chips) {
                if (other != chip) {
                    const QSignalBlocker blocker(other);
                    other->setChecked(false);
                }
            }
        });
    }
    paymentLayout->addWidget(methods);

    auto *cardNumber = new MaterialTextField(payment);
    cardNumber->setVariant(MaterialTextField::Variant::Filled);
    cardNumber->setLabel(tr("Card number"));
    paymentLayout->addWidget(cardNumber);
    m_pages->addWidget(payment);

    // Step three: what is being bought, and the way out.
    auto *review = stepCard(m_pages);
    QVBoxLayout *reviewLayout = stepLayout(review);
    reviewLayout->addWidget(line(tr("What is being bought"), MaterialType::Scale::Title, true, review));

    auto *items = new QGridLayout;
    items->setContentsMargins(QMargins());
    items->setHorizontalSpacing(kGap);
    items->setVerticalSpacing(6);
    struct Item {
        const char *text;
        int cents;
    };
    const Item bought[] = {{QT_TR_NOOP("Album, twelve photographs"), 1800},
                           {QT_TR_NOOP("One print, 30 by 40"), 2400},
                           {QT_TR_NOOP("Shipping"), 500}};
    int total = 0;
    int row = 0;
    for (const Item &item : bought) {
        items->addWidget(line(tr(item.text), MaterialType::Scale::Body, false, review), row, 0);
        QLabel *money = line(tr("$%1").arg(item.cents / 100.0, 0, 'f', 2), MaterialType::Scale::Body, false, review);
        money->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        items->addWidget(money, row, 1);
        total += item.cents;
        ++row;
    }
    items->addWidget(line(tr("Total"), MaterialType::Scale::Title, true, review), row, 0);
    QLabel *sum = line(tr("$%1").arg(total / 100.0, 0, 'f', 2), MaterialType::Scale::Title, true, review);
    sum->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    items->addWidget(sum, row, 1);
    items->setColumnStretch(0, 1);
    reviewLayout->addLayout(items);

    auto *place = new MaterialChip(MaterialChip::Kind::Assist, review);
    place->setText(tr("Place order"));
    connect(place, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("Order placed, in the sense that a demo can place one: nothing was charged "
                                   "and nothing will arrive."));
    });
    reviewLayout->addWidget(place, 0, Qt::AlignLeft);
    m_pages->addWidget(review);

    // The row of steps is the only way between them, which is what makes it the
    // stepper and not a decoration: what it reports, the stack follows.
    connect(m_steps, &MaterialSegmentedButton::currentIndexChanged, m_pages, &QStackedWidget::setCurrentIndex);

    addNote(tr("A step that has been left is still a step that can be gone back to, so the row stays on "
               "screen for all three - a stepper that only ever shows where the user is would be a label."));

    addStretch();
}
