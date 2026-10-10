/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include <QToolButton>

QX_APP_BEGIN_NAMESPACE

class QxMenuAccessButtonPrivate;

/* QxMenuAccessButton */
class QX_APP_EXPORT QxMenuAccessButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QxMenuAccessButton(QWidget *parent = Q_NULLPTR);
    virtual ~QxMenuAccessButton();
public:
    virtual QSize sizeHint() const;
public Q_SLOTS:
    void setOrientation(Qt::Orientation orientation);
private:
    QX_DECLARE_PRIVATE(QxMenuAccessButton)
};

QX_APP_END_NAMESPACE
