/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "demopage.h"

// The variant and the scrolled state are the window's to answer, so the class
// that carries them comes in whole rather than as a forward declaration: an
// enum inside a class cannot be named through one.
#include "qxmaterial/materialtopappbar.h"

QX_MATERIAL_USE_NAMESPACE

/*! The screen where the application is configured.
 *
 *  What the page asks for is the window's, not its own: which shape the top app
 *  bar takes is a question about the window, so the page says what was asked
 *  for and leaves the doing to it - which is also why it can be lifted into
 *  another application without being rewritten.
 */
class SettingsPage : public DemoPage
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = Q_NULLPTR);
Q_SIGNALS:
    void variantRequested(MaterialTopAppBar::Variant variant);
    void scrolledChanged(bool scrolled);
};

#endif   // SETTINGSPAGE_H
