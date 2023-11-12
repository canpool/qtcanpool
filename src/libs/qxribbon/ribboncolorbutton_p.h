/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "ribbonbutton_p.h"
#include "ribboncolorbutton.h"

#include <QStyleOptionToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonColorButton;

class RibbonColorButtonPrivate : public RibbonButtonPrivate
{
public:
    Q_DECLARE_PUBLIC(RibbonColorButton)
public:
    RibbonColorButtonPrivate();

    QPixmap createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize) Q_DECL_OVERRIDE;
public:
    QColor m_color;
};

QX_RIBBON_END_NAMESPACE
