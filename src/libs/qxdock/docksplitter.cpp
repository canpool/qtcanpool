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
    QX_INIT_PRIVATE(DockSplitter);
    setChildrenCollapsible(false);
}

DockSplitter::DockSplitter(Qt::Orientation orientation, QWidget *parent)
    : QSplitter(orientation, parent)
{
    QX_INIT_PRIVATE(DockSplitter);
}

DockSplitter::~DockSplitter()
{
    QX_FINI_PRIVATE();
}

bool DockSplitter::hasVisibleContent() const
{
    // Deliberately not cached. The answer changes on every show or hide of a
    // child, and a splitter has a handful of children at most, so this walk -
    // which stops at the first visible one - is cheaper than keeping a cache
    // honest. It is asked once per layout pass, from
    // hideEmptyParentSplitters() as it climbs the splitter tree and from
    // DockContainer::contentRect() for the root splitter, which is not a rate
    // that justifies the invalidation machinery a cache would need.
    for (int i = 0; i < count(); ++i) {
        if (!widget(i)->isHidden()) {
            return true;
        }
    }

    return false;
}

QWidget *DockSplitter::firstWidget() const
{
    return (count() > 0) ? widget(0) : nullptr;
}

QWidget *DockSplitter::lastWidget() const
{
    return (count() > 0) ? widget(count() - 1) : nullptr;
}

/**
 * Returns true if the splitter contains central widget of dock window.
 */
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
