/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTSWITCH_H
#define FLUENTSWITCH_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentSwitchPrivate;

/*!
 * Fluent's toggle switch: a pill with a knob in it, and a sentence beside it.
 *
 * Qt has a check box and no switch, and the two are not the same control: a
 * switch says that what it stands for is in force *now* - it is how a settings
 * page says Wi-Fi is on - where a check box says a choice has been made and
 * will be collected later. Fluent's answer is the pill, and its shape is the
 * whole signal: the track fills with the accent when the thing is on, and the
 * knob travels from one end to the other over a Fluent duration.
 *
 * The label is part of the control and part of the target: a pointer anywhere
 * on it toggles, which is why the wash and the focus ring are told to stay on
 * the track - a hover that tinted the sentence would be dimming the label, not
 * lighting the control.
 */
class QX_FLUENT_EXPORT FluentSwitch : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn NOTIFY toggled)
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    explicit FluentSwitch(QWidget *parent = Q_NULLPTR);
    ~FluentSwitch() override;

    /*! True while the switch is on. */
    bool isOn() const;

    /*! The sentence beside the track; empty means no sentence. */
    QString text() const;
    void setText(const QString &text);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Turns the switch on: \a on = true. Emits toggled() when it changes. */
    void setOn(bool on);
    /*! Flips the switch - what a click and the keyboard both do. */
    void toggle();

Q_SIGNALS:
    /*! Emitted whenever the switch changes state, by the pointer, the keyboard
     *  or the application. */
    void toggled(bool on);
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentSwitch)
    QX_DECLARE_PRIVATE(FluentSwitch)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTSWITCH_H
