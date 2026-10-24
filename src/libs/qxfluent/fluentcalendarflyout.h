/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCALENDARFLYOUT_H
#define FLUENTCALENDARFLYOUT_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcalendarflyout.h"

QX_FLUENT_BEGIN_NAMESPACE

/*!
 * Fluent's calendar flyout: the shared month grid, written in Fluent's type and
 * turned by Fluent's corners.
 *
 * Everything the grid does - the weeks, the month before and after, today's
 * ring, the keyboard walking the days - is the shared calendar's; what Fluent
 * answers is the two faces the month is written at, which are Fluent's body
 * and caption, and the corner the surface turns by.
 */
class QX_FLUENT_EXPORT FluentCalendarFlyout : public QxControls::QxCalendarFlyout
{
    Q_OBJECT
public:
    explicit FluentCalendarFlyout(QWidget *anchor);
    ~FluentCalendarFlyout() override;
protected:
    QFont headFont() const override;
    QFont weekdayFont() const override;
private:
    Q_DISABLE_COPY(FluentCalendarFlyout)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCALENDARFLYOUT_H
