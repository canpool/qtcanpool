/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxmenubutton.h"

#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>

QX_APP_BEGIN_NAMESPACE

QxMenuButton::QxMenuButton(QWidget *parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
    setFocusPolicy(Qt::NoFocus);
    setPopupMode(QToolButton::InstantPopup);
}

QxMenuButton::QxMenuButton(const QString &text, QWidget *parent)
    : QxMenuButton(parent)
{
    setText(text);
}

QxMenuButton::QxMenuButton(const QIcon &icon, const QString &text, QWidget *parent)
    : QxMenuButton(text, parent)
{
    setIcon(icon);
}

QxMenuButton::~QxMenuButton()
{
}

void QxMenuButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    // We do not need to draw both extension arrows
    opt.features &= ~QStyleOptionToolButton::HasMenu;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

QX_APP_END_NAMESPACE
