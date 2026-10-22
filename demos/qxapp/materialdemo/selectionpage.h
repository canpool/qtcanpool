/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SELECTIONPAGE_H
#define SELECTIONPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

class QLabel;

/*! The screen where things are chosen: a list of settings, one of a fixed
 *  number of answers, a value in a range, and the small things that stand for a
 *  choice already carried. */
class SelectionPage : public DemoPage
{
    Q_OBJECT
public:
    explicit SelectionPage(QWidget *parent = Q_NULLPTR);
private:
    QLabel *m_value = Q_NULLPTR;
};

#endif   // SELECTIONPAGE_H
