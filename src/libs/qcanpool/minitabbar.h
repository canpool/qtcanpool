/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qcanpool.h"
#include <QTabBar>

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT MiniTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit MiniTabBar(QWidget *parent = nullptr);
    ~MiniTabBar();

protected:
    virtual QSize tabSizeHint(int index) const;
    virtual void paintEvent(QPaintEvent *event) override;
};

QCANPOOL_END_NAMESPACE
