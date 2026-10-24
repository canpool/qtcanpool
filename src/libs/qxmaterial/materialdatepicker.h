/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALDATEPICKER_H
#define MATERIALDATEPICKER_H

#include "qxmaterial_global.h"

#include "materialtextfield.h"

#include <QtCore/QDate>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDatePickerPrivate;

/*!
 * Material's date picker: the field that says which day is in force, and the
 * month laid out beside it to change that day in.
 *
 * The field is a text field because a day is written as one, and it does not
 * take a keyboard because the writing is not the point: the day is chosen in
 * the calendar - a month as the grid it is, the months either side a click
 * away - and comes back to the field already written. A day taken, by a click
 * or by Enter on the walked-to day, is the answer; a day that is not a day is
 * not an answer, and the field goes on saying what it said.
 *
 * \sa QxControls::QxCalendarFlyout, QxControls::QxPopupSurface
 */
class QX_MATERIAL_EXPORT MaterialDatePicker : public MaterialTextField
{
    Q_OBJECT
    Q_PROPERTY(QDate date READ date WRITE setDate NOTIFY dateChanged)
public:
    explicit MaterialDatePicker(QWidget *parent = Q_NULLPTR);
    ~MaterialDatePicker() override;

    /*! The day in force. */
    QDate date() const;

    /*! The month the calendar is showing; the first of it. */
    QDate shownMonth() const;

    /*! The calendar the field opens, for the reader of the room. It is the
     *  flyout a reader can also put up on its own. */
    QWidget *calendar() const;

public Q_SLOTS:
    void setDate(QDate date);
    /*! Opens the calendar, as a click on the field does. */
    void openCalendar();
Q_SIGNALS:
    /*! The day in force changed - by a pick or by \c setDate. A day said twice
     *  is said once. */
    void dateChanged(QDate date);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialDatePicker)
    QX_DECLARE_PRIVATE(MaterialDatePicker)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALDATEPICKER_H
