/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALCHECKBOX_H
#define QXMATERIALCHECKBOX_H

#include "materialcheckable.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCheckBoxPrivate;

class QX_MATERIAL_EXPORT MaterialCheckBox : public MaterialCheckable
{
    Q_OBJECT
public:
    explicit MaterialCheckBox(QWidget *parent = 0);
    ~MaterialCheckBox();
private:
    Q_DISABLE_COPY(MaterialCheckBox)
    Q_DECLARE_PRIVATE(MaterialCheckBox)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALCHECKBOX_H
