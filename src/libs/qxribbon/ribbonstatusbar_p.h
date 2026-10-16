/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonstatusbar.h"

#include <QHash>
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonStatusBarPrivate
{
    QX_DECLARE_PUBLIC(RibbonStatusBar)
public:
    RibbonStatusBarPrivate();

    // action -> the left-section widget it drives (a label for a text action, a
    // frame for a separator). Empty until addAction()/addSeparator() runs.
    QHash<QAction *, QWidget *> m_left;
    // action -> the right-side button addPermanentAction() put up for it.
    QHash<QAction *, QToolButton *> m_permanent;
};

QX_RIBBON_END_NAMESPACE
