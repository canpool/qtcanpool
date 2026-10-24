/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "homepage.h"

#include "qxfluent/fluentbutton.h"
#include "qxfluent/fluentcard.h"
#include "qxfluent/fluenttokens.h"

#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
// Three to a row, and the columns share whatever width the screen has: an index
// that sized itself to its longest name would widen every other line on the
// page, which is how a list of one name becomes a page that scrolls sideways.
constexpr int kColumns = 3;
}   // namespace

HomePage::HomePage(QWidget *parent)
    : DemoPage(tr("Home"),
               tr("An application built out of QxFluent: a caption of its own, a navigation pane, and every "
                  "control the library ships, on a screen that uses it for something."),
               parent)
{
    addSection(tr("What this is"));

    // A card is the surface a screen is made of: what it does is lift whatever
    // is put on it, which is why the text goes inside it rather than beside it.
    auto *surface = new FluentCard(this);
    surface->setElevation(FluentElevation::Level::Depth4);
    surface->setFixedHeight(120);
    auto *cardLayout = new QVBoxLayout(surface);
    // The margin the shadow is drawn in is the only part of the card's
    // rectangle its contents may use.
    const int inset = FluentCard::shadowMargin(FluentElevation::Level::Depth4);
    cardLayout->setContentsMargins(inset, inset, inset, inset);
    auto *cardText = new QLabel(tr("Every control on these screens is a QxFluent control. Nothing here is a "
                                   "QPushButton with a style sheet, and nothing is a QWidget subclass that Qt "
                                   "drew. The pages are the other half of the control gallery: one control at "
                                   "a time, on a screen that uses it for something."),
                                surface);
    cardText->setWordWrap(true);
    cardText->setFont(FluentType::font(FluentType::Ramp::Body));
    cardLayout->addWidget(cardText);
    addBlock(surface);

    addSection(tr("Every control"));

    addNote(tr("The pane on the left walks down the same list. This is the whole of it at once, and a name "
               "here opens its page."));
    auto *index = new QWidget(this);
    m_index = new QGridLayout(index);
    m_index->setContentsMargins(QMargins());
    m_index->setHorizontalSpacing(8);
    m_index->setVerticalSpacing(8);
    addBlock(index);

    addStretch();
}

void HomePage::setCatalogue(const QStringList &titles, int firstDestination)
{
    m_firstDestination = firstDestination;

    for (int i = 0; i < titles.count(); ++i) {
        auto *entry = new FluentButton(m_index->parentWidget());
        entry->setText(titles.at(i));
        // A button rather than a link: the index is a list of things the
        // application can show, and a button is what Qt puts in the keyboard's
        // way and tells a screen reader is pressable. The quiet kind, because a
        // grid of forty accented commands would say that every screen is the
        // one command a view is allowed to point at.
        entry->setKind(FluentButton::Kind::Subtle);
        entry->setCursor(Qt::PointingHandCursor);
        // Whatever the column gets: a page that is as wide as its longest name
        // is a page the window cannot show whole, so the cell is the column and
        // the name inside it is the page's own business. Three columns of a page
        // that is at least 680 wide hold the longest name in this index with
        // room to spare, which is why nothing is cut here.
        entry->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        const int destination = m_firstDestination + i;
        connect(entry, &FluentButton::clicked, this, [this, destination]() {
            Q_EMIT destinationRequested(destination);
        });
        m_index->addWidget(entry, i / kColumns, i % kColumns);
    }
}
