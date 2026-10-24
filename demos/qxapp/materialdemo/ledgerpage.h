/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef LEDGERPAGE_H
#define LEDGERPAGE_H

#include "demopage.h"

/*!
 * The ledger: what was ordered, as a table.
 *
 * The one screen in this demo that needed a control the family did not have
 * until now. A page can arrange rows of four things out of a list - a header
 * above them, fixed widths, a line between - and that is what the other window
 * does, because its family has no table. This one has one, so the page uses it:
 * a table is the one arrangement that cannot be improvised for long, since the
 * keyboard, the column widths and the alignment all have to agree.
 */
class LedgerPage : public DemoPage
{
    Q_OBJECT
public:
    explicit LedgerPage(QWidget *parent = Q_NULLPTR);
};

#endif   // LEDGERPAGE_H
