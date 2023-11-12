/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QPushButton>

QX_RIBBON_BEGIN_NAMESPACE

class QX_RIBBON_EXPORT RibbonApplicationButton : public QPushButton
{
    Q_OBJECT
public:
    explicit RibbonApplicationButton(QWidget *parent = Q_NULLPTR);
    explicit RibbonApplicationButton(const QString &text, QWidget *parent = Q_NULLPTR);
    RibbonApplicationButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);
};

QX_RIBBON_END_NAMESPACE
