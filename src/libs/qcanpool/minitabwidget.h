/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qcanpool.h"
#include <QTabWidget>

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT MiniTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit MiniTabWidget(QWidget *parent = nullptr);
};

QCANPOOL_END_NAMESPACE
