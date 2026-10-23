/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOSWITCH_H
#define CUPERTINOSWITCH_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoSwitchPrivate;

/*!
 * Cupertino's switch: a slim capsule with a white knob in it.
 *
 * Qt has a check box and no switch, and the two are not the same control: a
 * switch says that what it stands for is in force *now* - it is how a settings
 * page says Wi-Fi is on - where a check box says a choice has been made and
 * will be collected later.
 *
 * Two proportions make the drawing this family's rather than another's. The
 * track is slim - a bead's height taller than the knob, and the knob nearly
 * fills it - so the control reads as a line with a bead on it rather than as a
 * filled bar. And the fill does not follow the knob: the track is the off grey
 * until the knob has crossed the middle and the on green from there, one
 * change and not a blend. The travel is what moves; the colour is what it
 * says, and the two are not interpolated into each other.
 *
 * The on green is green in every theme. It comes from the family's semantic
 * colours (cupertinotokens.h) and not from the theme's accent, because a green
 * switch that turned blue under another theme would be a different design
 * language wearing these shapes.
 *
 * There is no sentence in the control. In this family a switch is the capsule
 * and the sentence beside it is a label of its own, laid out by whoever owns
 * the row - which is also why the widget does not paint its own background
 * around the track.
 */
class QX_CUPERTINO_EXPORT CupertinoSwitch : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn NOTIFY toggled)
public:
    explicit CupertinoSwitch(QWidget *parent = Q_NULLPTR);
    ~CupertinoSwitch() override;

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
    Q_DISABLE_COPY(CupertinoSwitch)
    QX_DECLARE_PRIVATE(CupertinoSwitch)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOSWITCH_H
