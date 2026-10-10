/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include <QToolButton>

QX_APP_BEGIN_NAMESPACE

class QX_APP_EXPORT QxMenuButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QxMenuButton(QWidget *parent = Q_NULLPTR);
    explicit QxMenuButton(const QString &text, QWidget *parent = Q_NULLPTR);
    QxMenuButton(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);
    ~QxMenuButton();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

QX_APP_END_NAMESPACE
