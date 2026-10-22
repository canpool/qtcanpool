/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALPROGRESSRING_H
#define MATERIALPROGRESSRING_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialProgressRingPrivate;

/*!
 * Material's progress ring: an arc that says how far along something is, or an
 * arc that only says something is happening.
 *
 * QProgressBar is a styled widget and its indeterminate mode is the style's to
 * draw, so an application gets a block of Windows 95 sweeping a track. The two
 * modes here are not the same drawing either: one is an arc with an end, over
 * the track it has not reached; the other is an arc without one.
 *
 * The second is where this stops being the Fluent ring: that one is a fixed
 * arc that turns, while Material's grows and shrinks as it goes round - one
 * movement rather than two, and a length that keeps changing is what says the
 * wait has not settled. Neither is a transition, and both are the thing the
 * reduce-motion switch is about, so both stop when the user asked for less
 * motion.
 */
class QX_MATERIAL_EXPORT MaterialProgressRing : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool indeterminate READ isIndeterminate WRITE setIndeterminate)
public:
    explicit MaterialProgressRing(QWidget *parent = Q_NULLPTR);
    ~MaterialProgressRing() override;

    /*! How far along, 0 to 100. It is kept whatever the mode is, but it only
     *  shows once the ring is determinate - and a ring is born indeterminate,
     *  so a first value has to come with setIndeterminate(false). */
    int value() const;
    /*! True while the ring says only that something is happening. */
    bool isIndeterminate() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    /*! Emitted when how far along changed. */
    void valueChanged(int value);

public Q_SLOTS:
    void setValue(int value);
    void setIndeterminate(bool on);
protected:
    void paintSurface(QPainter *painter) override;
    void changeEvent(QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    void updateSweeping();

    Q_DISABLE_COPY(MaterialProgressRing)
    QX_DECLARE_PRIVATE(MaterialProgressRing)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALPROGRESSRING_H
