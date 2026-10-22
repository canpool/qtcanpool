/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef ALBUMPAGE_H
#define ALBUMPAGE_H

#include "demopage.h"

/*!
 * The album: a page whose subject is a picture, and whose job is to get out of
 * the way of it.
 *
 * The shape is the one a portfolio has: a hero that says what the whole is, a
 * grid of the pieces it is made of, and a footer of the few ways onward. It is
 * here because it is the opposite test of the dashboard - that screen is full
 * of things to read and this one is not, and a control set that can only make
 * a page of rows has not been tested.
 */
class AlbumPage : public DemoPage
{
    Q_OBJECT
public:
    explicit AlbumPage(QWidget *parent = Q_NULLPTR);
};

#endif   // ALBUMPAGE_H
