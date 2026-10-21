/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxcontrolaccessible.h"

#include "qxcontrol.h"

QX_CONTROLS_BEGIN_NAMESPACE

namespace
{
/*!
 * Gives an accessible face to every control of this base and leaves every
 * other widget to the factories that were installed before it.
 */
QAccessibleInterface *controlAccessibleFactory(const QString &className, QObject *object)
{
    Q_UNUSED(className)
    if (QxControl *control = qobject_cast<QxControl *>(object)) {
        return new QxControlAccessible(control, control->accessibleRole());
    }
    return Q_NULLPTR;
}
}   // namespace

QxControlAccessible::QxControlAccessible(QxControl *control, QAccessible::Role role)
    : QAccessibleWidget(control, role)
{
}

QAccessible::State QxControlAccessible::state() const
{
    // What the widget underneath already answers - visible, enabled, focusable,
    // focused - arrives here, and the control adds what is true of it beyond
    // that: checked, pressed, selected.
    QAccessible::State state = QAccessibleWidget::state();
    if (QxControl *control = qobject_cast<QxControl *>(widget())) {
        control->fillAccessibleState(&state);
    }
    return state;
}

void installControlAccessibility()
{
    static bool installed = false;
    if (installed) {
        return;
    }
    installed = true;
    QAccessible::installFactory(&controlAccessibleFactory);
}

QX_CONTROLS_END_NAMESPACE
