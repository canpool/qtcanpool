/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "collectionpage.h"

#include "qxfluent/fluentbreadcrumbbar.h"
#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentexpander.h"
#include "qxfluent/fluenttabview.h"
#include "qxfluent/fluenttokens.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*! One card at one elevation, carrying one line.
 *
 *  A card is a surface, and a surface with nothing on it is a shape rather than
 *  an answer - so each of these says what it is for, inset by the margin the
 *  shadow is drawn in, which is the only part of the rectangle the contents may
 *  use. */
QxFluent::FluentCard *elevatedCard(FluentElevation::Level level, const QString &text, QWidget *parent)
{
    auto *card = new QxFluent::FluentCard(parent);
    card->setElevation(level);
    card->setFixedSize(220, 96);
    auto *layout = new QVBoxLayout(card);
    const int inset = QxFluent::FluentCard::shadowMargin(level);
    layout->setContentsMargins(inset, inset, inset, inset);
    auto *label = new QLabel(text, card);
    label->setWordWrap(true);
    label->setFont(FluentType::font(FluentType::Ramp::Body));
    layout->addWidget(label);
    return card;
}
}   // namespace

CollectionPage::CollectionPage(QWidget *parent)
    : DemoPage(tr("Collection"), tr("How a lot of content sits on one screen without becoming a list."), parent)
{
    addSection(tr("Surfaces"));

    addNote(tr("Elevation is not decoration: it is how far above the page a surface sits, and the shadow is "
               "how a Fluent screen says it."));

    auto *cards = new QWidget(this);
    auto *cardLayout = new QHBoxLayout(cards);
    cardLayout->setContentsMargins(QMargins());
    cardLayout->setSpacing(16);
    cardLayout->addWidget(elevatedCard(FluentElevation::Level::Depth4, tr("At rest on the page."), cards));
    cardLayout->addWidget(elevatedCard(FluentElevation::Level::Depth8, tr("Lifted: a card that is hovered."), cards));
    cardLayout->addWidget(elevatedCard(FluentElevation::Level::Depth16, tr("A menu, or a flyout."), cards));
    cardLayout->addStretch(1);
    addBlock(cards);

    addSection(tr("Folding things away"));

    auto *first = new FluentExpander(this);
    first->setTitle(tr("What an expander hides"));
    auto *firstBody = new QLabel(tr("The header is the summary; the body is what the summary was of."));
    firstBody->setMinimumHeight(48);
    first->setContent(firstBody);
    first->setExpanded(true);
    addBlock(first);

    auto *second = new FluentExpander(this);
    second->setTitle(tr("Another one, closed"));
    auto *secondBody = new QLabel(tr("Two of them read as a list of questions; one reads as a control."));
    secondBody->setMinimumHeight(48);
    second->setContent(secondBody);
    addBlock(second);

    addSection(tr("Where the user is"));

    auto *crumbs = new FluentBreadcrumbBar(this);
    crumbs->setItems(QStringList{tr("Home"), tr("Library"), tr("Album")});
    addBlock(crumbs);
    auto *path = new QLabel(tr("Nothing clicked yet."), this);
    path->setFont(FluentType::font(FluentType::Ramp::Caption));
    connect(crumbs, &FluentBreadcrumbBar::itemClicked, this, [path](int index, const QString &text) {
        Q_UNUSED(index)
        path->setText(tr("Clicked: %1").arg(text));
    });
    addBlock(path);

    addSection(tr("Several things open at once"));

    addNote(tr("A tab is not a destination: it is something open, and it keeps what was being done in it "
               "when another one is looked at."));
    auto *tabs = new FluentTabView(this);
    tabs->addTab(new QLabel(tr("The first thing being worked on.")), tr("First"));
    tabs->addTab(new QLabel(tr("The second, kept while the first is looked at.")), tr("Second"));
    tabs->addTab(new QLabel(tr("A third, to show the row is a row.")), tr("Third"));
    tabs->setMinimumHeight(160);
    addBlock(tabs);

    addStretch();
}
