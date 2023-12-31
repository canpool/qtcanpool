/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "windowitemdelegate_p.h"

QX_WINDOW_BEGIN_NAMESPACE

WindowItemDelegate::WindowItemDelegate() = default;

WindowItemDelegate::~WindowItemDelegate() = default;

void WindowItemDelegate::resetQtGrabbedControl(QObject *host) const
{
    Q_UNUSED(host);
}

QX_WINDOW_END_NAMESPACE
