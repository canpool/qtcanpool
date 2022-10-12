/**
 * Copyright (C) 2021-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbonwindow.h"

#include "ribbonbar.h"

QCANPOOL_BEGIN_NAMESPACE

class RibbonWindowPrivate
{
public:
    RibbonWindowPrivate();

    void init();

public:
    RibbonWindow *q;
    RibbonBar *m_ribbonBar;
};

RibbonWindowPrivate::RibbonWindowPrivate()
    : m_ribbonBar(nullptr)
{

}

void RibbonWindowPrivate::init()
{
    q->setMouseTracking(true);

    m_ribbonBar = new RibbonBar(q);

    q->setMenuWidget(m_ribbonBar);
}

RibbonWindow::RibbonWindow(QWidget *parent)
    : QMainWindow(parent)
    , d(new RibbonWindowPrivate())
{
    d->q = this;
    d->init();
}

RibbonWindow::~RibbonWindow()
{
    delete d;
}

RibbonBar *RibbonWindow::ribbonBar() const
{
    return d->m_ribbonBar;
}

void RibbonWindow::setFrameless(bool frameless)
{
    if (frameless) {
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    } else {
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    }
    d->m_ribbonBar->updateParentFlags();
}

QCANPOOL_END_NAMESPACE
