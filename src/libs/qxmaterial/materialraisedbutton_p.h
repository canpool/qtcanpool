/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALRAISEDBUTTON_P_H
#define QXMATERIALRAISEDBUTTON_P_H

#include "materialflatbutton_p.h"
#include "materialraisedbutton.h"

class QStateMachine;
class QState;
class QGraphicsDropShadowEffect;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRaisedButton;

class MaterialRaisedButtonPrivate : public MaterialFlatButtonPrivate
{
    Q_DISABLE_COPY(MaterialRaisedButtonPrivate)
    Q_DECLARE_PUBLIC(MaterialRaisedButton)
public:
    MaterialRaisedButtonPrivate(MaterialRaisedButton *q);
    ~MaterialRaisedButtonPrivate();

    void init();

    QStateMachine *shadowStateMachine;
    QState *normalState;
    QState *pressedState;
    QGraphicsDropShadowEffect *effect;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALRAISEDBUTTON_P_H
