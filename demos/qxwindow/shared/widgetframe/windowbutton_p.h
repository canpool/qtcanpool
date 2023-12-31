/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWBUTTONPRIVATE_H
#define WINDOWBUTTONPRIVATE_H

#include "windowbutton.h"

QX_WINDOW_BEGIN_NAMESPACE

class WindowButtonPrivate
{
    Q_DECLARE_PUBLIC(WindowButton)
public:
    WindowButtonPrivate();
    virtual ~WindowButtonPrivate();

    void init();

    WindowButton *q_ptr;

    QIcon iconNormal;
    QIcon iconChecked;
    QIcon iconDisabled;

    void reloadIcon();
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWBUTTONPRIVATE_H