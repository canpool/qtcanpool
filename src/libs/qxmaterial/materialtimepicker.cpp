/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtimepicker.h"

#include "materialtokens.h"

#include "qxcontrols/qxtimeflyout.h"

#include <QtWidgets/QLineEdit>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 * The columns this family opens: the shared flyout, written in this family's
 * type and turned by this family's corner for a surface that sits on the page.
 */
class MaterialTimeFlyout : public QxControls::QxTimeFlyout
{
public:
    explicit MaterialTimeFlyout(QWidget *anchor)
        : QxControls::QxTimeFlyout(anchor)
    {
        setSurfaceRadius(MaterialShape::radius(MaterialShape::Radius::Large));
    }
protected:
    QFont rowFont() const override
    {
        return MaterialType::font(MaterialType::Scale::Body);
    }
};

class MaterialTimePickerPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialTimePicker)

    MaterialTimeFlyout *flyout = nullptr;
    // The time said last. The columns already carry what was taken by the time
    // the picker hears of it, so "what changed" is remembered here.
    QTime last{0, 0};
};

MaterialTimePicker::MaterialTimePicker(QWidget *parent)
    : MaterialTextField(parent)
    , d_ptr(new MaterialTimePickerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The writing is not the point: the field says the time, the columns
    // change it.
    lineEdit()->setReadOnly(true);
    lineEdit()->installEventFilter(this);
    d_ptr->flyout = new MaterialTimeFlyout(this);
    connect(d_ptr->flyout, &QxControls::QxTimeFlyout::timeTaken, this, [this](QTime time) {
        setTime(time);
    });
    d_ptr->last = d_ptr->flyout->time();
}

MaterialTimePicker::~MaterialTimePicker() = default;

QTime MaterialTimePicker::time() const
{
    return d_ptr->flyout->time();
}

QWidget *MaterialTimePicker::flyout() const
{
    return d_ptr->flyout;
}

void MaterialTimePicker::setTime(QTime time)
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

void MaterialTimePicker::openFlyout()
{
    if (!isEnabled()) {
        return;
    }
    d_ptr->flyout->setTime(time().isValid() ? time() : QTime::currentTime());
    d_ptr->flyout->open();
}

bool MaterialTimePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonPress) {
        openFlyout();
        return true;
    }
    return MaterialTextField::eventFilter(watched, event);
}

QX_MATERIAL_END_NAMESPACE
