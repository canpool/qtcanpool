/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTIMEPICKER_H
#define MATERIALTIMEPICKER_H

#include "qxmaterial_global.h"

#include "materialtextfield.h"

#include <QtCore/QTime>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTimePickerPrivate;

/*!
 * Material's time picker: the field that says which time is in force, and the
 * two columns - the day's hours, the hour's minutes - laid out beside it to
 * change that time in.
 *
 * The field does not take a keyboard, for the same reason the date picker's
 * does not: a time is read off the columns, not typed into a field whose
 * grammar would then have to be argued about. A value taken in either column
 * is a time, and comes back to the field already written as \c HH:mm; the
 * columns stay up until the keyboard or a click outside puts them away,
 * because a time has two halves and the other half may be next.
 *
 * \sa QxControls::QxTimeFlyout, QxControls::QxPopupSurface
 */
class QX_MATERIAL_EXPORT MaterialTimePicker : public MaterialTextField
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
public:
    explicit MaterialTimePicker(QWidget *parent = Q_NULLPTR);
    ~MaterialTimePicker() override;

    /*! The time in force. */
    QTime time() const;

    /*! The columns the field opens, for the reader of the room. */
    QWidget *flyout() const;

public Q_SLOTS:
    void setTime(QTime time);
    /*! Opens the columns, as a click on the field does. */
    void openFlyout();
Q_SIGNALS:
    /*! The time in force changed - by a take or by \c setTime. A time said
     *  twice is said once. */
    void timeChanged(QTime time);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialTimePicker)
    QX_DECLARE_PRIVATE(MaterialTimePicker)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTIMEPICKER_H
