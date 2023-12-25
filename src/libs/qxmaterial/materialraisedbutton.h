/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALRAISEDBUTTON_H
#define QXMATERIALRAISEDBUTTON_H

#include "materialflatbutton.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRaisedButtonPrivate;

class QX_MATERIAL_EXPORT MaterialRaisedButton : public MaterialFlatButton
{
    Q_OBJECT
public:
    explicit MaterialRaisedButton(QWidget *parent = 0);
    explicit MaterialRaisedButton(const QString &text, QWidget *parent = 0);
    ~MaterialRaisedButton();
protected:
    MaterialRaisedButton(MaterialRaisedButtonPrivate &d, QWidget *parent = 0);

    bool event(QEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialRaisedButton)
    Q_DECLARE_PRIVATE(MaterialRaisedButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALRAISEDBUTTON_H
