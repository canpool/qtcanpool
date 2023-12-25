/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTHEME_P_H
#define QXMATERIALTHEME_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QHash>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTheme;

class MaterialThemePrivate
{
    Q_DISABLE_COPY(MaterialThemePrivate)
    Q_DECLARE_PUBLIC(MaterialTheme)
public:
    MaterialThemePrivate(MaterialTheme *q);
    ~MaterialThemePrivate();

    MaterialTheme *const q_ptr;
    QHash<QString, QColor> colors;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTHEME_P_H
