/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QPushButton>

QX_DOCK_BEGIN_NAMESPACE

class DockButtonPrivate;

class QX_DOCK_EXPORT DockButton : public QPushButton
{
    Q_OBJECT
public:
    using QPushButton::QPushButton;

    enum Orientation {
        Horizontal,
        VerticalTopToBottom,
        VerticalBottomToTop
    };
public:
    explicit DockButton(QWidget *parent = nullptr);
    virtual ~DockButton();

    virtual QSize sizeHint() const override;

    Orientation buttonOrientation() const;
    void setButtonOrientation(Orientation orientation);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    QX_DECLARE_PRIVATE(DockButton)
};

QX_DOCK_END_NAMESPACE
