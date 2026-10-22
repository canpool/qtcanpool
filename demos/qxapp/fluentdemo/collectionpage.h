/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef COLLECTIONPAGE_H
#define COLLECTIONPAGE_H

#include "demopage.h"

/*! The screen where a lot is put on one page: surfaces that lift, things that
 *  fold away, a path back to where the user came from, and several things open
 *  at once. */
class CollectionPage : public DemoPage
{
    Q_OBJECT
public:
    explicit CollectionPage(QWidget *parent = Q_NULLPTR);
};

#endif   // COLLECTIONPAGE_H
