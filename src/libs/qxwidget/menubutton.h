/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxglobal.h"
#include <QToolButton>

QX_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT MenuButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MenuButton(QWidget *parent = Q_NULLPTR);
    explicit MenuButton(const QString &text, QWidget *parent = Q_NULLPTR);
    MenuButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);
    virtual ~MenuButton();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

QX_END_NAMESPACE
