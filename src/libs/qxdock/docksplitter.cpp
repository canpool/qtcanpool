/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docksplitter.h"
#include "dockpanel.h"

QX_DOCK_BEGIN_NAMESPACE

class DockSplitterPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSplitter)
public:
    DockSplitterPrivate();
};

DockSplitterPrivate::DockSplitterPrivate()
{

}

DockSplitter::DockSplitter(QWidget *parent)
    : QSplitter(parent)
{
    QX_INIT_PRIVATE(DockSplitter)
}

DockSplitter::DockSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
{
    QX_INIT_PRIVATE(DockSplitter)
}

DockSplitter::~DockSplitter()
{
    QX_FINI_PRIVATE()
}

bool DockSplitter::hasVisibleContent() const
{
    // TODO Cache or precalculate this to speed up
    for (int i = 0; i < count(); ++i) {
        if (!widget(i)->isHidden()) {
            return true;
        }
    }

    return false;
}

bool DockSplitter::isResizingWithContainer() const
{
    for (auto panel : findChildren<DockPanel *>()) {
        if (panel->isCentralWidgetArea()) {
            return true;
        }
    }

    return false;
}

QX_DOCK_END_NAMESPACE
