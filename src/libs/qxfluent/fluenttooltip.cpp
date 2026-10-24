/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttooltip.h"

#include "fluenttokens.h"

QX_FLUENT_BEGIN_NAMESPACE

FluentToolTip::FluentToolTip(QWidget *parent)
    : QxControls::QxToolTip(parent)
{
}

FluentToolTip::~FluentToolTip() = default;

qreal FluentToolTip::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Small);
}

QX_FLUENT_END_NAMESPACE
