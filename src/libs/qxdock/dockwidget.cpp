/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwidget.h"
#include "dockwindow.h"

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWidget)
public:
    DockWidgetPrivate();
public:
    DockWindow *m_window = nullptr;
};

DockWidgetPrivate::DockWidgetPrivate()
{
}

DockWidget::DockWidget(QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockWidget)
}

DockWidget::~DockWidget()
{
    QX_FINI_PRIVATE()
}

void DockWidget::setDockWindow(DockWindow *window)
{
    Q_D(DockWidget);
    d->m_window = window;
}

QX_DOCK_END_NAMESPACE
