/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTYLE_P_H
#define QXMATERIALSTYLE_P_H

#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialStyle;
class MaterialTheme;

class MaterialStylePrivate
{
    Q_DISABLE_COPY(MaterialStylePrivate)
    Q_DECLARE_PUBLIC(MaterialStyle)
public:
    MaterialStylePrivate(MaterialStyle *q);
    ~MaterialStylePrivate();

    void init();

    MaterialStyle *const q_ptr;
    MaterialTheme *theme;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTYLE_P_H
