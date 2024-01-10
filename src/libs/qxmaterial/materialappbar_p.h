/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAPPBAR_P_H
#define QXMATERIALAPPBAR_P_H

#include "qxmaterial_global.h"
#include <QColor>

class QHBoxLayout;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAppBar;

class MaterialAppBarPrivate
{
    Q_DISABLE_COPY(MaterialAppBarPrivate)
    QX_DECLARE_PUBLIC(MaterialAppBar)
public:
    MaterialAppBarPrivate();
    ~MaterialAppBarPrivate();

    void init();

public:
    QHBoxLayout *m_layout;
    bool useThemeColors;
    QColor foregroundColor;
    QColor backgroundColor;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAPPBAR_P_H
