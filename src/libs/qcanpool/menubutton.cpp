/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "menubutton.h"

#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>

QCANPOOL_BEGIN_NAMESPACE

MenuButton::MenuButton(QWidget *parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
    setFocusPolicy(Qt::NoFocus);
    setPopupMode(QToolButton::InstantPopup);
}

MenuButton::MenuButton(const QString &text, QWidget *parent)
    : MenuButton(parent)
{
    setText(text);
}

MenuButton::MenuButton(const QIcon &icon, const QString &text, QWidget *parent)
    : MenuButton(text, parent)
{
    setIcon(icon);
}

MenuButton::~MenuButton()
{

}

void MenuButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    // We do not need to draw both extension arrows
    opt.features &= ~QStyleOptionToolButton::HasMenu;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

QCANPOOL_END_NAMESPACE
