/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentdatepicker.h"

#include "fluentcalendarflyout.h"

#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

class FluentDatePickerPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentDatePicker)

    FluentCalendarFlyout *calendar = nullptr;
    // The day said last. The calendar already carries what was taken by the
    // time the picker hears of it, so "what changed" is remembered here.
    QDate last;
};

FluentDatePicker::FluentDatePicker(QWidget *parent)
    : FluentTextBox(parent)
    , d_ptr(new FluentDatePickerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The writing is not the point: the field says, the calendar changes.
    lineEdit()->setReadOnly(true);
    lineEdit()->installEventFilter(this);
    d_ptr->calendar = new FluentCalendarFlyout(this);
    connect(d_ptr->calendar, &FluentCalendarFlyout::datePicked, this, [this](QDate date) {
        setDate(date);
    });
    d_ptr->last = d_ptr->calendar->selectedDate();
}

FluentDatePicker::~FluentDatePicker(){QX_FINI_PRIVATE()}

QDate FluentDatePicker::date() const
{
    return d_ptr->calendar->selectedDate();
}

QDate FluentDatePicker::shownMonth() const
{
    return d_ptr->calendar->month();
}

FluentCalendarFlyout *FluentDatePicker::calendar() const
{
    return d_ptr->calendar;
}

void FluentDatePicker::setDate(QDate date)
{
    if (!date.isValid()) {
        return;
    }
    const QDate previous = d_ptr->last;
    d_ptr->calendar->setSelectedDate(date);
    setText(date.toString(QStringLiteral("yyyy-MM-dd")));
    d_ptr->last = date;
    if (date != previous) {
        Q_EMIT dateChanged(date);
    }
}

void FluentDatePicker::openCalendar()
{
    if (!isEnabled()) {
        return;
    }
    d_ptr->calendar->setSelectedDate(date().isValid() ? date() : QDate::currentDate());
    d_ptr->calendar->open();
}

bool FluentDatePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonPress) {
        openCalendar();
        return true;
    }
    return FluentTextBox::eventFilter(watched, event);
}

QX_FLUENT_END_NAMESPACE
