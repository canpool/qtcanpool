/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qcanpool.h"
#include <QToolButton>

QCANPOOL_BEGIN_NAMESPACE

class MenuAccessButtonPrivate;

/* MenuAccessButton */
class QCANPOOL_SHARED_EXPORT MenuAccessButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MenuAccessButton(QWidget* parent = Q_NULLPTR);
    virtual ~MenuAccessButton();
public:
    virtual QSize sizeHint() const;
public Q_SLOTS:
    void setOrientation(Qt::Orientation orientation);
private:
    QCP_DECLARE_PRIVATE(MenuAccessButton)
};

QCANPOOL_END_NAMESPACE
