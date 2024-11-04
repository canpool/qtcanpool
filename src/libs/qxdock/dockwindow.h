/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockcontainer.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWindowPrivate;

class QX_DOCK_EXPORT DockWindow : public DockContainer
{
    Q_OBJECT
public:
    explicit DockWindow(QWidget *parent = nullptr);
    ~DockWindow();

protected:
    void registerDockContainer(DockContainer *container);

private:
    QX_DECLARE_PRIVATE(DockWindow)
    friend class DockContainer;
};

QX_DOCK_END_NAMESPACE
