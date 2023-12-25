/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALCHECKBOX_P_H
#define QXMATERIALCHECKBOX_P_H

#include "materialcheckable_p.h"
#include "materialcheckbox.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCheckBoxPrivate : public MaterialCheckablePrivate
{
    Q_DISABLE_COPY(MaterialCheckBoxPrivate)
    Q_DECLARE_PUBLIC(MaterialCheckBox)
public:
    MaterialCheckBoxPrivate(MaterialCheckBox *q);
    ~MaterialCheckBoxPrivate();

    void init();
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALCHECKBOX_P_H
