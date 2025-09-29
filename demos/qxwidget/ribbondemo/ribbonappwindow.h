/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon/ribbonmainwindow.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonAppWindowPrivate;

class RibbonAppWindow : public RibbonMainWindow
{
    Q_OBJECT
public:
    RibbonAppWindow(QWidget *parent = Q_NULLPTR);
    ~RibbonAppWindow() Q_DECL_OVERRIDE;

    void updateWindowFlags(Qt::WindowFlags flags) Q_DECL_OVERRIDE;

protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonAppWindow)
};

QX_RIBBON_END_NAMESPACE
