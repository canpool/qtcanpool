/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT && MulanPSL-2.0
**/
#pragma once

#include "qxwidget_global.h"
#include "qxribbon/ribbonmainwindow.h"

QX_WIDGET_BEGIN_NAMESPACE

class RibbonAppWindowPrivate;

class QX_WIDGET_EXPORT RibbonAppWindow : public QX_RIBBON_PREPEND_NAMESPACE(RibbonMainWindow)
{
    Q_OBJECT
public:
    RibbonAppWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::WindowFlags());
    ~RibbonAppWindow() Q_DECL_OVERRIDE;

    void updateWindowFlags(Qt::WindowFlags flags) Q_DECL_OVERRIDE;

protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonAppWindow)
};

QX_WIDGET_END_NAMESPACE
