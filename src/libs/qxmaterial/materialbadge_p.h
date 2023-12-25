/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALBADGE_P_H
#define QXMATERIALBADGE_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QIcon>
#include <QSize>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialBadge;

class MaterialBadgePrivate
{
    Q_DISABLE_COPY(MaterialBadgePrivate)
    Q_DECLARE_PUBLIC(MaterialBadge)
public:
    MaterialBadgePrivate(MaterialBadge *q);
    ~MaterialBadgePrivate();

    void init();

    MaterialBadge *const q_ptr;
    QString text;
    QColor textColor;
    QColor backgroundColor;
    QSize size;
    QIcon icon;
    qreal x;
    qreal y;
    int padding;
    bool useThemeColors;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALBADGE_P_H
