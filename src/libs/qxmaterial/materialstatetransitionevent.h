/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTATETRANSITIONEVENT_H
#define QXMATERIALSTATETRANSITIONEVENT_H

#include "qxmaterial_global.h"
#include <QEvent>

QX_MATERIAL_BEGIN_NAMESPACE

enum MaterialStateTransitionType {
    // Snackbar
    SnackbarShowTransition = 1,
    SnackbarHideTransition,
    SnackbarWaitTransition,
    SnackbarNextTransition,
    // FlatButton
    FlatButtonPressedTransition,
    FlatButtonCheckedTransition,
    FlatButtonUncheckedTransition,
    // CollapsibleMenu
    CollapsibleMenuExpand,
    CollapsibleMenuCollapse,
    // Slider
    SliderChangedToMinimum,
    SliderChangedFromMinimum,
    SliderNoFocusMouseEnter,
    SliderNoFocusMouseLeave,
    // Dialog
    DialogShowTransition,
    DialogHideTransition,
    //
    MaxTransitionType = 65535
};

struct MaterialStateTransitionEvent : public QEvent {
    MaterialStateTransitionEvent(MaterialStateTransitionType type) : QEvent(QEvent::Type(QEvent::User + 1)), type(type)
    {
    }

    MaterialStateTransitionType type;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTATETRANSITIONEVENT_H
