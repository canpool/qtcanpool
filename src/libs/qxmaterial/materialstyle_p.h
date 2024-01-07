/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTYLE_P_H
#define QXMATERIALSTYLE_P_H

#include "qxmaterial_global.h"

// for internal
#include "materialnamespace.h"
#include "materialstyle.h"

QX_MATERIAL_BEGIN_NAMESPACE

#define MATERIAL_DISABLE_THEME_COLORS                                                                                  \
    if (d->useThemeColors == true) {                                                                                   \
        d->useThemeColors = false;                                                                                     \
    }

class MaterialStyle;
class MaterialTheme;

class MaterialStylePrivate
{
    QX_DECLARE_PUBLIC(MaterialStyle)
public:
    MaterialStylePrivate();
    ~MaterialStylePrivate();

    void init();

    MaterialTheme *theme;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTYLE_P_H
