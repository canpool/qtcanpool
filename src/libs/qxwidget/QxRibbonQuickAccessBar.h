/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include "QxRibbonContainers.h"

#include <QMenu>
#include <QToolButton>

QX_BEGIN_NAMESPACE

class QuickAccessBar;
class RibbonQuickAccessBarPrivate;

class QX_WIDGET_EXPORT RibbonQuickAccessBar : public RibbonCtrlContainer
{
    Q_OBJECT
public:
    RibbonQuickAccessBar(QWidget *parent = Q_NULLPTR);
    ~RibbonQuickAccessBar();
public:
    QuickAccessBar *accessBar() const;
private:
    QX_DECLARE_PRIVATE(RibbonQuickAccessBar)
};

QX_END_NAMESPACE
