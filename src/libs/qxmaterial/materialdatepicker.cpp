/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialdatepicker.h"

#include "materialtokens.h"

#include "qxcontrols/qxcalendarflyout.h"

#include <QtWidgets/QLineEdit>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 * The month this family opens: the shared grid, written in this family's type
 * and turned by this family's corner for a surface that sits on the page.
 */
class MaterialCalendarFlyout : public QxControls::QxCalendarFlyout
{
public:
    explicit MaterialCalendarFlyout(QWidget *anchor)
        : QxControls::QxCalendarFlyout(anchor)
    {
        setSurfaceRadius(MaterialShape::radius(MaterialShape::Radius::Large));
    }
protected:
    /*! The month's name is what the surface is about, so it is the heavier cut
     *  of the body type; the weekdays over the grid are the small print. */
    QFont headFont() const override
    {
        return MaterialType::font(MaterialType::Scale::Body, true);
    }
    QFont weekdayFont() const override
    {
        return MaterialType::font(MaterialType::Scale::Label);
    }
};

class MaterialDatePickerPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialDatePicker)

    MaterialCalendarFlyout *calendar = nullptr;
    // The day said last. The calendar already carries what was taken by the
    // time the picker hears of it, so "what changed" is remembered here.
    QDate last;
};

MaterialDatePicker::MaterialDatePicker(QWidget *parent)
    : MaterialTextField(parent)
    , d_ptr(new MaterialDatePickerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The writing is not the point: the field says the day, the calendar
    // changes it. What the field does keep is the label, which names what the
    // day is a day of.
    lineEdit()->setReadOnly(true);
    lineEdit()->installEventFilter(this);
    d_ptr->calendar = new MaterialCalendarFlyout(this);
    connect(d_ptr->calendar, &QxControls::QxCalendarFlyout::datePicked, this, [this](QDate date) {
        setDate(date);
    });
    d_ptr->last = d_ptr->calendar->selectedDate();
}

MaterialDatePicker::~MaterialDatePicker() = default;

QDate MaterialDatePicker::date() const
{
    return d_ptr->calendar->selectedDate();
}

QDate MaterialDatePicker::shownMonth() const
{
    return d_ptr->calendar->month();
}

QWidget *MaterialDatePicker::calendar() const
{
    return d_ptr->calendar;
}

void MaterialDatePicker::setDate(QDate date)
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

void MaterialDatePicker::openCalendar()
{
    if (!isEnabled()) {
        return;
    }
    d_ptr->calendar->setSelectedDate(date().isValid() ? date() : QDate::currentDate());
    d_ptr->calendar->open();
}

bool MaterialDatePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonPress) {
        openCalendar();
        return true;
    }
    return MaterialTextField::eventFilter(watched, event);
}

QX_MATERIAL_END_NAMESPACE
