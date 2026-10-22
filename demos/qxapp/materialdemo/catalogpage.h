/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CATALOGPAGE_H
#define CATALOGPAGE_H

#include "demopage.h"

/*!
 * The catalogue: every control the library ships, once, on one screen.
 *
 * The templates show what a screen built out of these controls can be; this
 * shows what there is to build one out of. The difference is the order: here a
 * control is the subject and its own row is the demonstration, there it is a
 * means to a screen. Both are needed, and neither replaces the other - a
 * catalogue is also where a change in what a control paints is first noticed.
 */
class CatalogPage : public DemoPage
{
    Q_OBJECT
public:
    explicit CatalogPage(QWidget *parent = Q_NULLPTR);
private:
    /*! Asks a question over the page, so that a dialog is demonstrated as what
     *  it is: a thing that interrupts. */
    void openDialog();
};

#endif   // CATALOGPAGE_H
