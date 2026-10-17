/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "colorbutton.h"
#include "qxribbon/ribbonutils.h"

QX_RIBBON_USE_NAMESPACE

ColorButton::ColorButton(const QColor &color, QWidget *parent)
    : QToolButton(parent)
    , m_color(color)
{
    setAutoRaise(true);
    setMouseTracking(true);
    setIcon(colorIcon(m_color, iconSize()));

    connect(this, &QToolButton::clicked, this, [=](bool checked) {
        emit colorClicked(m_color, checked);
    });
}

const QColor &ColorButton::color() const
{
    return m_color;
}

void ColorButton::setColor(const QColor &color)
{
    m_color = color;
    setIcon(colorIcon(m_color, iconSize()));
}
