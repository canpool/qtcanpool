/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "minitabwidget.h"
#include "minitabbar.h"

QCANPOOL_BEGIN_NAMESPACE

MiniTabWidget::MiniTabWidget(QWidget *parent)
    : QTabWidget{parent}
{
    setTabBar(new MiniTabBar(this));
}

QCANPOOL_END_NAMESPACE
