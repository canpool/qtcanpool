/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALICONBUTTON_P_H
#define QXMATERIALICONBUTTON_P_H

#include "qxmaterial_global.h"
#include <QColor>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialIconButton;
class MaterialRippleOverlay;

class MaterialIconButtonPrivate
{
    Q_DISABLE_COPY(MaterialIconButtonPrivate)
    Q_DECLARE_PUBLIC(MaterialIconButton)
public:
    MaterialIconButtonPrivate(MaterialIconButton *q);
    virtual ~MaterialIconButtonPrivate();

    void init();
    void updateRipple();

    MaterialIconButton *const q_ptr;
    MaterialRippleOverlay *rippleOverlay;
    QColor color;
    QColor disabledColor;
    bool useThemeColors;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALICONBUTTON_P_H
