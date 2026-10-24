/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTIMEPICKER_H
#define FLUENTTIMEPICKER_H

#include "qxfluent_global.h"

#include "fluenttextbox.h"

#include <QtCore/QTime>

QX_FLUENT_BEGIN_NAMESPACE

class FluentTimePickerPrivate;

/*!
 * Fluent's time picker: a field that says which time is in force, and the
 * hours and the minutes laid out in two columns beside it to change it in.
 *
 * The field does not take a keyboard - it says the time, and the two columns
 * of the flyout are where the time is changed: an hour out of the day's
 * twenty-four, a minute out of the hour's sixty, each a click away and the
 * wheel away. A time is finished when one of its two has been taken, and it
 * comes back to the field.
 */
class QX_FLUENT_EXPORT FluentTimePicker : public FluentTextBox
{
    Q_OBJECT
    Q_PROPERTY(QTime time READ time WRITE setTime NOTIFY timeChanged)
public:
    explicit FluentTimePicker(QWidget *parent = Q_NULLPTR);
    ~FluentTimePicker() override;

    /*! The time in force. */
    QTime time() const;

    /*! The flyout the columns open into, for the reader of the room. */
    QWidget *flyout() const;

public Q_SLOTS:
    void setTime(QTime time);
    /*! Opens the two columns, as a click on the field does. */
    void openFlyout();
Q_SIGNALS:
    /*! The time in force changed - by a pick or by \c setTime. */
    void timeChanged(QTime time);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(FluentTimePicker)
    QX_DECLARE_PRIVATE(FluentTimePicker)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTIMEPICKER_H
