/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttimepicker.h"

#include "fluenttokens.h"

#include "qxcontrols/qxtimeflyout.h"

#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

/*!
 * The two columns this family's time picker opens into: the shared flyout,
 * written in Fluent's type and turned by Fluent's corners.
 */
class FluentTimeFlyout : public QxControls::QxTimeFlyout
{
public:
    explicit FluentTimeFlyout(QWidget *anchor)
        : QxControls::QxTimeFlyout(anchor)
    {
        setSurfaceRadius(FluentShape::radius(FluentShape::Radius::Large));
    }
protected:
    QFont rowFont() const override
    {
        return FluentType::font(FluentType::Ramp::Body);
    }
};

class FluentTimePickerPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTimePicker)

    FluentTimeFlyout *flyout = nullptr;
    // The time said last. The columns already carry what was taken by the
    // time the picker hears of it, so "what changed" is remembered here.
    QTime last{0, 0};
};

FluentTimePicker::FluentTimePicker(QWidget *parent)
    : FluentTextBox(parent)
    , d_ptr(new FluentTimePickerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The writing is not the point: the field says, the columns change.
    lineEdit()->setReadOnly(true);
    lineEdit()->installEventFilter(this);
    d_ptr->flyout = new FluentTimeFlyout(this);
    connect(d_ptr->flyout, &QxControls::QxTimeFlyout::timeTaken, this, [this](QTime time) {
        setTime(time);
    });
    d_ptr->last = d_ptr->flyout->time();
}

FluentTimePicker::~FluentTimePicker(){QX_FINI_PRIVATE()}

QTime FluentTimePicker::time() const
{
    return d_ptr->flyout->time();
}

QWidget *FluentTimePicker::flyout() const
{
    return d_ptr->flyout;
}

void FluentTimePicker::setTime(QTime time)
{
    if (!time.isValid()) {
        return;
    }
    const QTime previous = d_ptr->last;
    d_ptr->flyout->setTime(time);
    setText(time.toString(QStringLiteral("HH:mm")));
    d_ptr->last = time;
    if (time != previous) {
        Q_EMIT timeChanged(time);
    }
}

void FluentTimePicker::openFlyout()
{
    if (!isEnabled()) {
        return;
    }
    d_ptr->flyout->setTime(time().isValid() ? time() : QTime::currentTime());
    d_ptr->flyout->open();
}

bool FluentTimePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonPress) {
        openFlyout();
        return true;
    }
    return FluentTextBox::eventFilter(watched, event);
}

QX_FLUENT_END_NAMESPACE
