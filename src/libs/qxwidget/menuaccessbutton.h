/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxglobal.h"
#include <QToolButton>

QX_BEGIN_NAMESPACE

class MenuAccessButtonPrivate;

/* MenuAccessButton */
class QX_WIDGET_EXPORT MenuAccessButton : public QToolButton
{
    Q_OBJECT
public:
    explicit MenuAccessButton(QWidget* parent = Q_NULLPTR);
    virtual ~MenuAccessButton();
public:
    virtual QSize sizeHint() const;
public slots:
    void setOrientation(Qt::Orientation orientation);
private:
    QX_DECLARE_PRIVATE(MenuAccessButton)
};

QX_END_NAMESPACE
