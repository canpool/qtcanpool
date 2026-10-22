/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CHECKOUTPAGE_H
#define CHECKOUTPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

class QStackedWidget;

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialSegmentedButton;
QX_MATERIAL_END_NAMESPACE

/*!
 * Checkout: one decision taken in three steps, and never all three at once.
 *
 * The template is the one screen that is a sequence, and what it has to say is
 * where the user is in it: a row of the three steps that is also the way back,
 * and a card that holds only the step in force. The row is a segmented button
 * because that is a control made of "one of these is in force", which is what a
 * stepper is - and unlike a row of buttons it cannot be read as three things to
 * do at the same time.
 */
class CheckoutPage : public DemoPage
{
    Q_OBJECT
public:
    explicit CheckoutPage(QWidget *parent = Q_NULLPTR);
private:
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialSegmentedButton) *m_steps = Q_NULLPTR;
    QStackedWidget *m_pages = Q_NULLPTR;
};

#endif   // CHECKOUTPAGE_H
