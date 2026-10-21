/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXCONTROLACCESSIBLE_H
#define QXCONTROLACCESSIBLE_H

#include "qxcontrols_global.h"

#include <QtWidgets/QAccessibleWidget>

QX_CONTROLS_BEGIN_NAMESPACE

class QxControl;

/*!
 * The accessible face of a control that paints itself.
 *
 * A control drawn in code is invisible to an assistive tool in the way that
 * matters: nothing about it says what it is or what is true of it, because
 * the pixels that say both are the control's own. Qt answers from the widget
 * underneath - a Client with a rectangle - so every control of this kind has
 * to say its role and its state somewhere, and saying it once here is the
 * point: a control answers two questions (what are you, what is true of you)
 * and the plumbing around the answers is the same for all of them.
 *
 * The role comes from QxControl::accessibleRole() and the state from
 * QxControl::fillAccessibleState(), so a control that wants to be announced
 * as a switch answers one call and inherits the rest.
 */
class QX_CONTROLS_EXPORT QxControlAccessible : public QAccessibleWidget
{
public:
    QxControlAccessible(QxControl *control, QAccessible::Role role);

    QAccessible::State state() const override;
};

/*!
 * Installs the factory that gives every QxControl an accessible face.
 * Idempotent, and called by QxControl's own constructor, so a control does
 * not have to remember to do it.
 */
void QX_CONTROLS_EXPORT installControlAccessibility();

QX_CONTROLS_END_NAMESPACE

#endif   // QXCONTROLACCESSIBLE_H
