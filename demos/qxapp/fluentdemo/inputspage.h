/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef INPUTSPAGE_H
#define INPUTSPAGE_H

#include "demopage.h"

class QLabel;

/*! The screen where things are chosen: a list of settings, a value in a range,
 *  and a rating - three answers to "what does the user want", and the three
 *  shapes Fluent gives them. */
class InputsPage : public DemoPage
{
    Q_OBJECT
public:
    explicit InputsPage(QWidget *parent = Q_NULLPTR);
private:
    QLabel *m_volume = Q_NULLPTR;
    QLabel *m_rating = Q_NULLPTR;
};

#endif   // INPUTSPAGE_H
