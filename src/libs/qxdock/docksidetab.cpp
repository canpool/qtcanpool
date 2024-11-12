/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "docksidetab.h"
#include "docksidebar.h"
#include "dockmanager.h"
#include "dockwidget.h"
#include "dockutils.h"

QX_DOCK_BEGIN_NAMESPACE

class DockSideTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSideTab)
public:
    DockSideTabPrivate();
    void updateOrientation();
public:
    DockSideBar *m_sideBar = nullptr;
    DockWidget *m_dockWidget = nullptr;
    Qt::Orientation m_orientation{Qt::Vertical};
};

DockSideTabPrivate::DockSideTabPrivate()
{
}

void DockSideTabPrivate::updateOrientation()
{
    Q_Q(DockSideTab);
    bool IconOnly = DockManager::testAutoHideConfigFlag(DockManager::AutoHideSideBarsIconOnly);
    if (IconOnly && !q->icon().isNull()) {
        q->setText("");
        q->setOrientation(Qt::Horizontal);
    } else {
        auto area = m_sideBar->sideBarArea();
        q->setOrientation((area == Qx::DockSideBarBottom || area == Qx::DockSideBarTop) ? Qt::Horizontal
                                                                                        : Qt::Vertical);
    }
}

DockSideTab::DockSideTab(QWidget *parent)
    : DockButton(parent)
{
    QX_INIT_PRIVATE(DockSideTab);
    setAttribute(Qt::WA_NoMousePropagation);
    setFocusPolicy(Qt::NoFocus);
}

DockSideTab::~DockSideTab()
{
    QX_FINI_PRIVATE();
}

DockWidget *DockSideTab::dockWidget() const
{
    Q_D(const DockSideTab);
    return d->m_dockWidget;
}

void DockSideTab::setDockWidget(DockWidget *w)
{
    Q_D(DockSideTab);
    if (!w) {
        return;
    }
    d->m_dockWidget = w;
    setText(w->windowTitle());
    setIcon(d->m_dockWidget->icon());
    setToolTip(w->windowTitle());
}

Qt::Orientation DockSideTab::orientation() const
{
    Q_D(const DockSideTab);
    return d->m_orientation;
}

void DockSideTab::setOrientation(Qt::Orientation orientation)
{
    Q_D(DockSideTab);
    d->m_orientation = orientation;
    if (orientation == Qt::Horizontal) {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    }
    DockButton::setButtonOrientation((Qt::Horizontal == orientation) ? DockButton::Horizontal
                                                                     : DockButton::VerticalTopToBottom);
    updateStyle();
}

void DockSideTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
    update();
}

DockSideBar *DockSideTab::sideBar() const
{
    Q_D(const DockSideTab);
    return d->m_sideBar;
}

int DockSideTab::tabIndex() const
{
    Q_D(const DockSideTab);
    if (!d->m_sideBar) {
        return -1;
    }

    return d->m_sideBar->indexOfTab(*this);
}

void DockSideTab::setSideBar(DockSideBar *sideBar)
{
    Q_D(DockSideTab);
    d->m_sideBar = sideBar;
    if (d->m_sideBar) {
        d->updateOrientation();
    }
}

void DockSideTab::removeFromSideBar()
{
    Q_D(DockSideTab);
    if (d->m_sideBar == nullptr) {
        return;
    }
    d->m_sideBar->removeTab(this);
    setSideBar(nullptr);
}

QX_DOCK_END_NAMESPACE
