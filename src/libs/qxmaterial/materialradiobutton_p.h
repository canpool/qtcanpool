/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALRADIOBUTTON_P_H
#define QXMATERIALRADIOBUTTON_P_H

#include "materialcheckable_p.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRadioButton;

class MaterialRadioButtonPrivate : public MaterialCheckablePrivate
{
    Q_DISABLE_COPY(MaterialRadioButtonPrivate)
    Q_DECLARE_PUBLIC(MaterialRadioButton)
public:
    MaterialRadioButtonPrivate(MaterialRadioButton *q);
    ~MaterialRadioButtonPrivate();

    void init();
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALRADIOBUTTON_P_H
