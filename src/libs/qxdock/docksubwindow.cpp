/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "docksubwindow.h"
#include "dockgroupmanager_p.h"
#include "dockgroup.h"
#include "dockpanel.h"

#include <QMenu>
#include <QSettings>

QX_DOCK_BEGIN_NAMESPACE

class DockSubWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSubWindow)
public:
    DockSubWindowPrivate();
public:
    DockGroup *m_group;
};

DockSubWindowPrivate::DockSubWindowPrivate()
{
}

DockSubWindow::DockSubWindow(DockGroup *parent)
    : Super(parent)
{
    QX_INIT_PRIVATE(DockSubWindow);
    Q_D(DockSubWindow);
    d->m_group = parent;
}

DockSubWindow::~DockSubWindow()
{
    QX_FINI_PRIVATE();
}

void DockSubWindow::fillViewMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo)
{
    Q_D(DockSubWindow);
    auto widgetsMap = dockWidgetsMap();
    for (auto iter = widgetsMap.begin(); iter != widgetsMap.end(); ++iter) {
        auto widget = iter.value()->widget();
        auto action = iter.value()->toggleViewAction();

        DockGroup *grp = dynamic_cast<DockGroup *>(widget);
        if (grp) {
            auto subMenu = menu->addMenu(iter.key());

            subMenu->addAction(action);
            subMenu->addSeparator();

            grp->setupMenu(subMenu, moveTo);
        } else {
            menu->addAction(action);
        }
    }

    if (d->m_group->canCreateNewGroups()) {
        // see how this works, to create it in the right place,
        // and also to have load perspective work when some groups are missing
        menu->addSeparator();

        menu->addAction(new CreateChildDockAction(d->m_group, menu));

        if (d->m_group->topLevelGroup()->subWindow() != this)
            menu->addAction(new DestroyGroupAction(d->m_group, menu));
    }
}

void DockSubWindow::fillMoveMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo)
{
    auto widgetsMap = dockWidgetsMap();
    for (auto iter = widgetsMap.begin(); iter != widgetsMap.end(); ++iter) {
        auto subMenu = menu->addMenu(iter.key());

        for (auto sw : moveTo) {
            // iterate over all possible target managers
            if (sw == this) {
                // if dock is already in mgr, no reason to move it there
            } else if (sw == subWindow(iter.value())) {
                // if target is the group itself, can't move it there, would make no sense
            } else {
                subMenu->addAction(new MoveDockWidgetAction(iter.value(), sw, subMenu));
            }
        }
    }
}

void DockSubWindow::addPerspectiveRec(const QString &name)
{
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);

    for (auto child : childrens)
        child->addPerspective(name);
}

void DockSubWindow::openPerspectiveRec(const QString &name)
{
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);

    for (auto child : childrens)
        child->openPerspective(name);
}

void DockSubWindow::removePerspectivesRec()
{
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);

    for (auto child : childrens)
        child->removePerspectives(child->perspectiveNames());
}

void DockSubWindow::loadPerspectivesRec(QSettings &settings)
{
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);

    for (auto child : childrens) {
        settings.beginGroup(child->getPersistGroupName());
        child->loadPerspectives(settings);
        settings.endGroup();
    }
}

void DockSubWindow::savePerspectivesRec(QSettings &settings) const
{
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);

    for (auto child : childrens) {
        settings.beginGroup(child->getPersistGroupName());
        child->savePerspectives(settings);
        settings.endGroup();
    }
}

DockSubWindow *DockSubWindow::subWindow(DockWidget *widget)
{
    DockGroup *group = widget ? dynamic_cast<DockGroup *>(widget->widget()) : nullptr;
    return (group) ? group->subWindow() : nullptr;
}

void DockSubWindow::childWindows(std::vector<DockSubWindow *> &subs, bool rec) const
{
    auto widgets = getWidgetsInGUIOrder();
    for (auto widget : widgets) {
        DockSubWindow *sw = subWindow(widget);
        if (sw) {
            subs.push_back(sw);
            if (rec) {   // recursion
                sw->childWindows(subs, rec);
            }
        }
    }
}

std::vector<DockSubWindow *> DockSubWindow::allSubWindows(bool includeThis, bool rec) const
{
    std::vector<DockSubWindow *> sws;
    if (includeThis)
        sws.push_back(const_cast<DockSubWindow *>(this));
    childWindows(sws, rec);
    return sws;
}

std::vector<std::pair<DockSubWindow *, DockWidget *>> DockSubWindow::allDockWidgets(bool includeThis, bool rec) const
{
    std::vector<std::pair<DockSubWindow *, DockWidget *>> widgets;
    for (auto sw : allSubWindows(includeThis, rec)) {
        for (auto widget : sw->getWidgetsInGUIOrder())
            widgets.push_back(std::make_pair(sw, widget));
    }
    return widgets;
}

QString DockSubWindow::getGroupName()
{
    Q_D(DockSubWindow);
    return d->m_group->objectName();
}

#define CHILD_PREFIX QString("Child-")
QString DockSubWindow::getPersistGroupName()
{
    QString group = "Top";
    if (!getGroupName().isEmpty())
        group = CHILD_PREFIX + getGroupName();
    return group;
}

QString DockSubWindow::getGroupNameFromPersistGroupName(QString persistGroupName)
{
    if (persistGroupName.startsWith(CHILD_PREFIX)) {
        persistGroupName = persistGroupName.mid(CHILD_PREFIX.size());
    } else {
        assert(false);
    }
    return persistGroupName;
}

DockGroup *DockSubWindow::group()
{
    Q_D(DockSubWindow);
    return d->m_group;
}

QMap<QString, QStringList> DockSubWindow::getGroupContents()
{
    QMap<QString, QStringList> result;
    std::vector<DockSubWindow *> childrens = allSubWindows(true, true);
    for (auto child : childrens) {
        result[child->getPersistGroupName()] = child->dockWidgetsMap().keys();
    }
    return result;
}

DockPanel *DockSubWindow::getInsertDefaultPos()
{
    DockPanel *defaultPos = nullptr;
    if (dockPanelCount() != 0)
        defaultPos = dockPanel(dockPanelCount() - 1);
    return defaultPos;
}

std::vector<DockWidget *> DockSubWindow::getWidgetsInGUIOrder() const
{
    std::vector<DockWidget *> result;
    result.reserve(dockWidgetsMap().size());
    for (int i = 0; i != dockPanelCount(); ++i) {
        for (auto widget : dockPanel(i)->dockWidgets())
            result.push_back(widget);
    }
    return result;
}

QX_DOCK_END_NAMESPACE
