/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;

class DockContainerPrivate;

class QX_DOCK_EXPORT DockContainer : public QWidget
{
    Q_OBJECT
public:
    explicit DockContainer(DockWindow *window, QWidget *parent = nullptr);
    virtual ~DockContainer();

protected:
    void createRootSplitter();

private:
    QX_DECLARE_PRIVATE(DockContainer)
};

QX_DOCK_END_NAMESPACE
