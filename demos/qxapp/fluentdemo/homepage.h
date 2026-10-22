/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "demopage.h"

#include "qxfluent/qxfluent_global.h"

class QPushButton;

QX_FLUENT_BEGIN_NAMESPACE
class FluentCard;
class FluentInfoBar;
class FluentSwitch;
class FluentTeachingTip;
QX_FLUENT_END_NAMESPACE

QX_FLUENT_USE_NAMESPACE

/*! The first screen: what this application is, and three of the ways it says
 *  something to the user - a surface that lifts, a message that stays until it
 *  is answered, and a tip that points at the thing it is about. */
class HomePage : public DemoPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = Q_NULLPTR);
private:
    FluentCard *m_card = Q_NULLPTR;
    FluentInfoBar *m_info = Q_NULLPTR;
    FluentSwitch *m_showInfo = Q_NULLPTR;
    /*! Made the first time it is asked for: a teaching tip is a window of its
     *  own, and one that exists before it is wanted is a second top-level
     *  window a capture would have to explain. */
    FluentTeachingTip *m_tip = Q_NULLPTR;
    QPushButton *m_tipButton = Q_NULLPTR;
};

#endif   // HOMEPAGE_H
