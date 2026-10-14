/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "titlebutton.h"

#include <QtGui/QMouseEvent>

TitleButton::TitleButton(QWidget *parent)
    : QToolButton(parent)
{
    // A title bar button is not part of the tab order, and it must not take the
    // focus away from the window it belongs to when it is clicked.
    setFocusPolicy(Qt::NoFocus);
}

void TitleButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    // The base class is called first: it is what keeps the button's own state
    // consistent with the press/release pairs Qt has already delivered, and
    // QToolButton emits clicked() from there.
    QToolButton::mouseDoubleClickEvent(event);
    Q_EMIT doubleClicked();
}
