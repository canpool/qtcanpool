/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxwidget_global.h"
#include <QTabWidget>

QX_WIDGET_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT MiniTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MiniTabWidget(QWidget *parent = nullptr);
};

QX_WIDGET_END_NAMESPACE
