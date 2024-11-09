/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockOverlayPrivate;

class QX_DOCK_EXPORT DockOverlay : public QWidget
{
    Q_OBJECT
public:
    enum OverlayMode {
        PanelOverlayMode,
        ContainerOverlayMode
    };
public:
    explicit DockOverlay(QWidget *parent, OverlayMode mode = PanelOverlayMode);
    virtual ~DockOverlay();

private:
    QX_DECLARE_PRIVATE(DockOverlay)
};

QX_DOCK_END_NAMESPACE
