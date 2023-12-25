/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALRADIOBUTTON_H
#define QXMATERIALRADIOBUTTON_H

#include "materialcheckable.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRadioButtonPrivate;

class QX_MATERIAL_EXPORT MaterialRadioButton : public MaterialCheckable
{
    Q_OBJECT
public:
    explicit MaterialRadioButton(QWidget *parent = 0);
    ~MaterialRadioButton();
protected:
    void setupProperties();
private:
    Q_DISABLE_COPY(MaterialRadioButton)
    Q_DECLARE_PRIVATE(MaterialRadioButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALRADIOBUTTON_H
