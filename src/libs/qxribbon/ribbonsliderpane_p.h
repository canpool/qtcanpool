/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonsliderpane.h"

#include <QToolButton>

class QSlider;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonSliderPanePrivate
{
    QX_DECLARE_PUBLIC(RibbonSliderPane)
public:
    RibbonSliderPanePrivate();

    void init();
    // Draws the minus/plus glyphs from the palette, so the buttons read on a dark
    // theme; called from init() and again on every palette change.
    void updateButtonIcons();

    QToolButton *m_minusButton;
    QSlider *m_slider;
    QToolButton *m_plusButton;
};

QX_RIBBON_END_NAMESPACE
