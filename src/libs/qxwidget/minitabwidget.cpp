/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "minitabwidget.h"
#include "minitabbar.h"

QX_WIDGET_BEGIN_NAMESPACE

MiniTabWidget::MiniTabWidget(QWidget *parent)
    : QTabWidget{parent}
{
    setTabBar(new MiniTabBar(this));
}

QX_WIDGET_END_NAMESPACE
