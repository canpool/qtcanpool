/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbonbutton.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonColorButtonPrivate;

class QX_RIBBON_EXPORT RibbonColorButton : public RibbonButton
{
    Q_OBJECT
public:
    explicit RibbonColorButton(QWidget *parent = nullptr);
    explicit RibbonColorButton(QAction *defaultAction, QWidget *parent = nullptr);
    virtual ~RibbonColorButton();

    const QColor &color() const;
Q_SIGNALS:
    void colorChanged(const QColor &color);
    void colorClicked(const QColor &color, bool checked = false);
public Q_SLOTS:
    void setColor(const QColor &color);
private Q_SLOTS:
    void onButtonClicked(bool checked = false);
private:
    Q_DECLARE_PRIVATE(RibbonColorButton)
};

QX_RIBBON_END_NAMESPACE
