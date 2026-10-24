/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTDATEPICKER_H
#define FLUENTDATEPICKER_H

#include "qxfluent_global.h"

#include "fluenttextbox.h"

#include <QtCore/QDate>

QX_FLUENT_BEGIN_NAMESPACE

class FluentCalendarFlyout;
class FluentDatePickerPrivate;

/*!
 * Fluent's date picker: a field that says which day is in force, and the
 * month laid out beside it to change that day in.
 *
 * The writing is not the point here, so the field does not take a keyboard:
 * it says the day, and the calendar flyout - a month as the grid it is, with
 * the months either side a click away - is where the day is changed. A day
 * taken, by a click or by Enter on the walked-to day, comes back to the
 * field.
 */
class QX_FLUENT_EXPORT FluentDatePicker : public FluentTextBox
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
public:
    explicit FluentDatePicker(QWidget *parent = Q_NULLPTR);
    ~FluentDatePicker() override;

    /*! The day in force. */
    QDate date() const;

    /*! The month the flyout is showing. */
    QDate shownMonth() const;

    /*! The flyout the calendar opens into, for the reader of the room. */
    FluentCalendarFlyout *calendar() const;

public Q_SLOTS:
    void setDate(QDate date);
    /*! Opens the calendar flyout, as a click on the field does. */
    void openCalendar();
Q_SIGNALS:
    /*! The day in force changed - by a pick or by \c setDate. */
    void dateChanged(QDate date);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(FluentDatePicker)
    QX_DECLARE_PRIVATE(FluentDatePicker)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTDATEPICKER_H
