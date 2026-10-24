/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "ledgerpage.h"

#include "qxmaterial/materialtable.h"
#include "qxmaterial/materialtokens.h"

#include <QtCore/QCoreApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! The rooms the ledger is about. The names are the album's photographs,
 *  because a ledger in this window is the record of that album's prints. */
struct Entry {
    const char *name;
    const char *status;
    const char *when;
    const char *amount;
};

const Entry kEntries[] = {
    {QT_TR_NOOP("Morning fog"), QT_TR_NOOP("Paid"), QT_TR_NOOP("12 Oct"), QT_TR_NOOP("120.00")},
    {QT_TR_NOOP("The long beach"), QT_TR_NOOP("Refunded"), QT_TR_NOOP("11 Oct"), QT_TR_NOOP("80.00")},
    {QT_TR_NOOP("Three fields"), QT_TR_NOOP("Paid"), QT_TR_NOOP("09 Oct"), QT_TR_NOOP("120.00")},
    {QT_TR_NOOP("Blossom"), QT_TR_NOOP("Pending"), QT_TR_NOOP("07 Oct"), QT_TR_NOOP("200.00")},
    {QT_TR_NOOP("Dusk over the mill"), QT_TR_NOOP("Paid"), QT_TR_NOOP("02 Oct"), QT_TR_NOOP("120.00")}};

/*! The page's own words are asked for by its own name, so that a translator
 *  finds them under "LedgerPage" rather than spread over three contexts. */
QString tr(const char *text)
{
    return QCoreApplication::translate("LedgerPage", text);
}
}   // namespace

LedgerPage::LedgerPage(QWidget *parent)
    : DemoPage(tr("Ledger"),
               tr("What was ordered, read as a table: rows of the same four things, and a heading that "
                  "says what each of them holds."),
               parent)
{
    addSection(tr("What was ordered"));

    addNote(tr("This is the screen a family has to own a control for. A page can put a heading over rows "
               "and keep the columns at fixed widths - the other window in this demo does exactly that, "
               "because it has no table - but columns that line up, a line under every row, and a keyboard "
               "that walks the cells are one thing, and that thing is a table view. What the page decides is "
               "only what the columns are called and what stands in them."));

    // Five lines and a heading: as many as the table is tall by default, so the
    // page has no scrolling to do and no half-empty frame in it.
    auto *table = new MaterialTable(this);
    table->setHeaders({tr("Photograph"), tr("Status"), tr("Ordered"), tr("Paid")});
    // The columns share the table's width, which is the one thing the control
    // leaves to the page: a table wider than its columns is a table with a hole
    // in the right-hand side of it.
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (const Entry &entry : kEntries) {
        table->addRow({tr(entry.name), tr(entry.status), tr(entry.when), tr(entry.amount)});
    }
    addBlock(table);

    // A total is the one thing a table has that a list of cards does not: the
    // bottom line is read against the column above it, and that only works when
    // the column is a column.
    auto *foot = new QLabel(tr("Five of the twelve, 640.00 paid in all"), this);
    foot->setFont(MaterialType::font(MaterialType::Scale::Label));
    foot->setForegroundRole(QPalette::PlaceholderText);
    addBlock(foot);

    addStretch();
}
