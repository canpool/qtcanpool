/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "pricingpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QFontMetrics>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! The strings on this page answer to one context, the page's own name: a
 *  sentence that appears twice on the screen is then one translation, and a
 *  translator has one place to look for all of them. */
QString tr(const char *text)
{
    return QCoreApplication::translate("PricingPage", text);
}

constexpr int kGap = 16;
// The width the words get, not the width of the rectangle: the outer rectangle
// also holds the band the shadow is drawn in, and that band is not the same at
// every variant - so a fixed outer width would make the lifted card's column
// narrower than its neighbours' and break the one thing this page is.
constexpr int kContentWidth = 240;
constexpr int kCardPadding = 16;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! One plan, as a column of the same rows in the same order as its neighbours.
 *
 *  \a recommended is not decoration: the card is lifted instead of outlined, so
 *  the one the screen would rather the user chose stands one step above the page
 *  rather than under a label. The label is there anyway, because a difference of
 *  surface is a difference the theme draws and a reader may not see.
 */
MaterialCard *planCard(const QString &name, const QString &price, const QStringList &features, const QString &action,
                       bool recommended, QWidget *parent)
{
    // Elevated rather than filled: the lift is what a theme always draws, and a
    // filled surface is the palette's AlternateBase, which a theme is free to
    // set to the page's own colour - a recommended plan that no one can see is
    // not a recommendation.
    auto *card =
        new MaterialCard(recommended ? MaterialCard::Variant::Elevated : MaterialCard::Variant::Outlined, parent);
    const int inset = MaterialCard::shadowMargin(card->variant()) + kCardPadding;
    card->setFixedWidth(kContentWidth + 2 * inset);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(inset, inset, inset, inset);
    layout->setSpacing(6);

    // The flag stands in all three cards, blank where it says nothing: a row
    // that only the middle card has would push its price down, and the three
    // rows are the whole point of the comparison.
    QLabel *flag = line(recommended ? tr("Recommended") : QString(), MaterialType::Scale::Label, true, card);
    flag->setStyleSheet(QStringLiteral("color: palette(highlight);"));
    flag->setFixedHeight(QFontMetrics(flag->font()).height());
    layout->addWidget(flag);
    layout->addWidget(line(name, MaterialType::Scale::Title, true, card));

    auto *money = new QWidget(card);
    auto *moneyLayout = new QHBoxLayout(money);
    moneyLayout->setContentsMargins(QMargins());
    moneyLayout->setSpacing(4);
    moneyLayout->addWidget(line(price, MaterialType::Scale::Display, true, money));
    QLabel *period = line(tr("/ month"), MaterialType::Scale::Label, false, money);
    period->setAlignment(Qt::AlignBottom);
    moneyLayout->addWidget(period);
    moneyLayout->addStretch(1);
    layout->addWidget(money);

    for (const QString &feature : features) {
        layout->addWidget(line(feature, MaterialType::Scale::Body, false, card));
    }
    layout->addStretch(1);

    // The action is a chip rather than a button because that is the shape this
    // library draws: an assist chip is a button in a pill, which is what a
    // Material button looks like anyway.
    auto *choose = new MaterialChip(MaterialChip::Kind::Assist, card);
    choose->setText(action);
    layout->addWidget(choose, 0, Qt::AlignLeft);
    return card;
}
}   // namespace

PricingPage::PricingPage(QWidget *parent)
    : DemoPage(tr("Pricing"),
               tr("Three answers side by side: what each one costs, what it includes, and the "
                  "one this screen would rather you took."),
               parent)
{
    addNote(tr("A comparison is read across, so the three surfaces keep the same rows in the same order - "
               "name, price, what is included, and the way to take it. The middle one is lifted rather than "
               "outlined: on Material a surface standing one step above the page is how a screen says which "
               "of three it is about."));

    auto *plans = new QWidget(this);
    auto *planLayout = new QHBoxLayout(plans);
    planLayout->setContentsMargins(QMargins());
    planLayout->setSpacing(kGap);

    const auto add = [this, planLayout](MaterialCard *card, const QString &name) {
        QObject::connect(card->findChild<MaterialChip *>(), &QxControl::clicked, this, [this, name]() {
            Q_EMIT messageRequested(tr("Nothing was bought: %1 is a card on a screen, and this window has no "
                                       "shop behind it.")
                                        .arg(name));
        });
        planLayout->addWidget(card);
    };

    add(planCard(tr("Free"), tr("$0"),
                 QStringList{tr("One album"), tr("Twelve photographs a month"), tr("The demo's own themes")},
                 tr("Start"), false, plans),
        tr("Free"));
    add(planCard(tr("Pro"), tr("$12"),
                 QStringList{tr("Unlimited albums"), tr("Two hundred prints a month"),
                             tr("Every theme, and the ones to come")},
                 tr("Choose Pro"), true, plans),
        tr("Pro"));
    add(planCard(tr("Team"), tr("$29"),
                 QStringList{tr("Everything in Pro"), tr("Five people on one account"), tr("A shared archive")},
                 tr("Contact us"), false, plans),
        tr("Team"));
    planLayout->addStretch(1);
    addBlock(plans);

    addNote(tr("The three heights are the same because the rows are: a comparison whose cards end at three "
               "different places makes the reader do the work of lining them up."));

    addStretch();
}
