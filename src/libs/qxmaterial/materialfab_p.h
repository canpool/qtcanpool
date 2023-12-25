/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALFAB_P_H
#define QXMATERIALFAB_P_H

#include "materialfab.h"
#include "materialraisedbutton_p.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialFloatingActionButton;

class MaterialFloatingActionButtonPrivate : public MaterialRaisedButtonPrivate
{
    Q_DISABLE_COPY(MaterialFloatingActionButtonPrivate)
    Q_DECLARE_PUBLIC(MaterialFloatingActionButton)
public:
    enum {
        DefaultDiameter = 56,
        MiniDiameter = 40
    };

    enum {
        DefaultIconSize = 24,
        MiniIconSize = 18
    };

    MaterialFloatingActionButtonPrivate(MaterialFloatingActionButton *q);
    ~MaterialFloatingActionButtonPrivate();

    void init();
    QRect fabGeometry() const;
    void setupProperties();

    inline int diameter() const;
    inline int iconSize() const;

    Qt::Corner corner;
    bool mini;
    int offsX;
    int offsY;
};

inline int MaterialFloatingActionButtonPrivate::diameter() const
{
    return mini ? MiniDiameter : DefaultDiameter;
}

inline int MaterialFloatingActionButtonPrivate::iconSize() const
{
    return mini ? MiniIconSize : DefaultIconSize;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALFAB_P_H
