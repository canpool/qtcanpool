/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtooltip.h"

#include "materialtokens.h"

QX_MATERIAL_BEGIN_NAMESPACE

MaterialTooltip::MaterialTooltip(QWidget *parent)
    : QxControls::QxToolTip(parent)
{
}

MaterialTooltip::~MaterialTooltip() = default;

qreal MaterialTooltip::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Small);
}

QX_MATERIAL_END_NAMESPACE
