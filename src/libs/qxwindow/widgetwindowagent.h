/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WIDGETWINDOWAGENT_H
#define WIDGETWINDOWAGENT_H

#include "windowagentwidget.h"

QX_WINDOW_BEGIN_NAMESPACE

class QX_WINDOW_EXPORT WidgetWindowAgent : public  WindowAgentWidget
{
    Q_OBJECT
 public:
    explicit WidgetWindowAgent(QObject *parent = nullptr) : WindowAgentWidget(parent) {}
    ~WidgetWindowAgent() = default;
};

QX_WINDOW_END_NAMESPACE

#endif   // WIDGETWINDOWAGENT_H
