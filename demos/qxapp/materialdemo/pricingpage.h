/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef PRICINGPAGE_H
#define PRICINGPAGE_H

#include "demopage.h"

/*!
 * Pricing: three answers side by side, and one of them recommended.
 *
 * It is the template that is a comparison and not a story, and what makes it
 * one is that the three surfaces are the same shape - same width, same rows in
 * the same order - so that the eye can go across them. The middle one is lifted
 * rather than outlined, because "which one do you want" is a question a screen
 * is allowed to have an opinion about, and one surface standing one step above
 * the page is the whole of how Material says so.
 */
class PricingPage : public DemoPage
{
    Q_OBJECT
public:
    explicit PricingPage(QWidget *parent = Q_NULLPTR);
};

#endif   // PRICINGPAGE_H
