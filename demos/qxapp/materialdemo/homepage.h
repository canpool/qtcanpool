/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "demopage.h"

/*! The first screen: what this application is, the three surfaces Material
 *  puts things on, and the answer a surface gives when it is pressed. */
class HomePage : public DemoPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = Q_NULLPTR);
};

#endif   // HOMEPAGE_H
