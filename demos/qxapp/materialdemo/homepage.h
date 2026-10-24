/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "demopage.h"

class BarChart;

/*!
 * The dashboard: the screen an application opens on when what it is about is a
 * number that moves.
 *
 * It is the first of six templates, and a template here is a whole screen and
 * not a sample of one control: a row of figures that are the answer to "how is
 * it going", a chart that says what the figures are made of, and a list of what
 * came in. Each piece is a card, because on Material a surface is how one thing
 * is told apart from the next.
 *
 * The chart is drawn here rather than brought in: a demo that pulled a plotting
 * library in would be showing that library, and what a control set has to prove
 * is that a screen can be built out of it alone.
 */
class HomePage : public DemoPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = Q_NULLPTR);
private:
    BarChart *m_chart = Q_NULLPTR;
};

#endif   // HOMEPAGE_H
