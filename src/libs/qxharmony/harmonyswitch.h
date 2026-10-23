/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYSWITCH_H
#define HARMONYSWITCH_H

#include "qxharmony_global.h"

#include "qxcontrols/qxcontrol.h"

QX_HARMONY_BEGIN_NAMESPACE

class HarmonySwitchPrivate;

/*!
 * Harmony's switch: a capsule with a knob that stretches while it is held.
 *
 * Qt has a check box and no switch, and the two are not the same control: a
 * switch says that what it stands for is in force *now*, where a check box says
 * a choice has been made and will be collected later.
 *
 * What makes this drawing this family's is not its outline - a capsule with a
 * knob in it is what every family draws - but what the knob does. Held, the
 * knob stretches into a rounded rectangle of the family's medium corner, one
 * state rather than a continuum of shapes between a circle and a pill, because
 * a control that changed its outline with the distance travelled would be
 * drawing the pointer rather than the state. Let go, it lands with the springy
 * curve: it passes its end a little and settles back, which is the family's
 * physical reading of a movement that arrives.
 *
 * The on colour is the brand's blue in every theme: it comes from the family's
 * semantic colours (harmonytokens.h) rather than from the theme's accent,
 * because a switch that turned another colour under another theme would be a
 * different design language wearing these shapes.
 *
 * There is no sentence in the control: a switch here is the capsule and the
 * words beside it are a label of their own, laid out by whoever owns the row.
 */
class QX_HARMONY_EXPORT HarmonySwitch : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn NOTIFY toggled)
public:
    explicit HarmonySwitch(QWidget *parent = Q_NULLPTR);
    ~HarmonySwitch() override;

    /*! True while the switch is on. */
    bool isOn() const;

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
    bool hitTest(const QPointF &pos) const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(HarmonySwitch)
    QX_DECLARE_PRIVATE(HarmonySwitch)
};

QX_HARMONY_END_NAMESPACE

#endif   // HARMONYSWITCH_H
