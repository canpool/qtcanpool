/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include <QPushButton>

QX_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT RibbonApplicationButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RibbonApplicationButton(QWidget *parent = Q_NULLPTR);
    explicit RibbonApplicationButton(const QString &text, QWidget *parent = Q_NULLPTR);
    RibbonApplicationButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);
};

QX_END_NAMESPACE
