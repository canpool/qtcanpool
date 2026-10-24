/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcalendarflyout.h"

#include "fluenttokens.h"

QX_FLUENT_BEGIN_NAMESPACE

FluentCalendarFlyout::FluentCalendarFlyout(QWidget *anchor)
    : QxControls::QxCalendarFlyout(anchor)
{
    // A calendar is a surface that sits on the page, and Fluent's surfaces that
    // sit on the page are the large corner.
    setSurfaceRadius(FluentShape::radius(FluentShape::Radius::Large));
}

FluentCalendarFlyout::~FluentCalendarFlyout() = default;

QFont FluentCalendarFlyout::headFont() const
{
    return FluentType::font(FluentType::Ramp::Body);
}

QFont FluentCalendarFlyout::weekdayFont() const
{
    return FluentType::font(FluentType::Ramp::Caption);
}

QX_FLUENT_END_NAMESPACE
