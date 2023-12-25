/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAVATAR_P_H
#define QXMATERIALAVATAR_P_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QChar>
#include <QColor>
#include <QIcon>
#include <QImage>
#include <QPixmap>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAvatar;

class MaterialAvatarPrivate
{
    Q_DISABLE_COPY(MaterialAvatarPrivate)
    Q_DECLARE_PUBLIC(MaterialAvatar)
public:
    MaterialAvatarPrivate(MaterialAvatar *q);
    ~MaterialAvatarPrivate();

    void init();

    MaterialAvatar *const q_ptr;
    int size;
    QxMaterial::AvatarType type;
    QChar letter;
    QImage image;
    QIcon icon;
    QPixmap pixmap;
    bool useThemeColors;
    QColor textColor;
    QColor backgroundColor;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAVATAR_P_H
