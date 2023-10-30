/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxwidget_global.h"
#include <QTabBar>

QX_WIDGET_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT MiniTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit MiniTabBar(QWidget *parent = nullptr);
    ~MiniTabBar();

protected:
    virtual QSize tabSizeHint(int index) const;
    virtual void paintEvent(QPaintEvent *event) override;
};

QX_WIDGET_END_NAMESPACE
