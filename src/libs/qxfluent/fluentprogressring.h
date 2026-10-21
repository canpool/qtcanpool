/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTPROGRESSRING_H
#define FLUENTPROGRESSRING_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentProgressRingPrivate;

/*!
 * Fluent's progress ring: an arc that says how far along something is, or an
 * arc that only says something is happening.
 *
 * QProgressBar is a styled widget and its indeterminate mode is the style's to
 * draw; a Fluent application gets a Windows 95 block sweeping a track. The
 * ring is the Fluent answer and the two modes are not the same drawing: one is
 * an arc with an end, the other is an arc without one, turning to say that the
 * waiting has not stopped.
 *
 * The turning is the one motion here that is not a transition - nothing is
 * moving from one state to another, it is a loop - so it reads the
 * reduce-motion switch itself and stops, because a ring that turns forever is
 * exactly what that switch is asking not to be shown.
 */
class QX_FLUENT_EXPORT FluentProgressRing : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool indeterminate READ isIndeterminate WRITE setIndeterminate)
public:
    explicit FluentProgressRing(QWidget *parent = Q_NULLPTR);
    ~FluentProgressRing() override;

    /*! How far along, 0 to 100; meaningless while indeterminate. */
    int value() const;
    /*! True while the ring says only that something is happening. */
    bool isIndeterminate() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setIndeterminate(bool on);
protected:
    void paintSurface(QPainter *painter) override;
    void changeEvent(QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    void updateTurning();

    Q_DISABLE_COPY(FluentProgressRing)
    QX_DECLARE_PRIVATE(FluentProgressRing)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTPROGRESSRING_H
