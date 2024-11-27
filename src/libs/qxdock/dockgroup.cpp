/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockgroup.h"
#include "dockgroupmanager.h"
#include "docksubwindow.h"
#include "dockgroupmanager_p.h"
#include "dockpanel.h"

#include <QVBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QMenu>
#include <QInputDialog>

QX_DOCK_BEGIN_NAMESPACE

class DockGroupPrivate
{
public:
    QX_DECLARE_PUBLIC(DockGroup)
public:
    DockGroupPrivate();
public:
    DockGroup *m_topLevelGroup;
    DockGroupManager *m_mgr;
    DockSubWindow *m_subWindow;
    bool m_canCreateNewGroups;
    QString m_newPerspectiveDefaultName;
};

DockGroupPrivate::DockGroupPrivate()
{
}

DockGroup::DockGroup(QWidget *parent, bool canCreateNewGroups, DockGroupManager *manager)
    : DockGroup(parent, nullptr, manager)
{
    Q_D(DockGroup);
    d->m_canCreateNewGroups = canCreateNewGroups;
    d->m_topLevelGroup = this;
}

DockGroup::DockGroup(QWidget *parent, DockGroup *topLevelGroup, DockGroupManager *manager)
    : Super(parent)
{
    QX_INIT_PRIVATE(DockGroup);
    Q_D(DockGroup);
    d->m_topLevelGroup = topLevelGroup;
    d->m_canCreateNewGroups = topLevelGroup ? topLevelGroup->d_ptr->m_canCreateNewGroups : false;
    d->m_mgr = manager;
    d->m_subWindow = new DockSubWindow(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->m_subWindow);
}

DockGroup::~DockGroup()
{
    QX_FINI_PRIVATE();
}

DockPanel *DockGroup::addTabWidget(QWidget *widget, const QString &name, DockPanel *after)
{
    return addTabWidget(widget, name, QIcon(), after);
}

DockPanel *DockGroup::addTabWidget(QWidget *widget, const QString &name, QIcon icon, DockPanel *after)
{
    for (auto existing : topLevelGroup()->subWindow()->allDockWidgets(true, true)) {
        if (existing.second->objectName() == name) {
            QMessageBox::critical(this, "Error", "Name '" + name + "' already in use");
            return nullptr;
        }
    }

    DockWidget *dw = new DockWidget(name);
    dw->setWidget(widget);
    dw->setIcon(icon);

    Q_D(DockGroup);
    // Add the dock widget to the center dock widget area
    return d->m_subWindow->addDockWidget(Qx::CenterDockWidgetArea, dw, after);
}

DockGroup *DockGroup::createGroup(const QString &groupName, DockPanel *insertPos)
{
    return createGroup(groupName, QIcon(), insertPos);
}

DockGroup *DockGroup::createGroup(const QString &groupName, QIcon icon, DockPanel *insertPos)
{
    Q_D(DockGroup);
    QString error = getGroupNameError(groupName);
    if (!error.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", error);
        return nullptr;
    }

    DockGroup *child = new DockGroup(this, d->m_topLevelGroup, d->m_mgr);
    child->setObjectName(groupName);

    DockWidget *dw = new DockWidget(groupName);
    dw->setWidget(child);
    dw->setIcon(icon);

    insertPos = d->m_subWindow->addDockWidget(Qx::CenterDockWidgetArea, dw, insertPos);

    return child;
}

DockGroup *DockGroup::topLevelGroup() const
{
    Q_D(const DockGroup);
    return d->m_topLevelGroup;
}

void DockGroup::destroyGroup(DockGroup *group)
{
    auto topLevelGroup = group->topLevelGroup();

    if (topLevelGroup && topLevelGroup != group) {
        // reaffect all child docks to top-level
        for (auto dockwidget :
             group->subWindow()->getWidgetsInGUIOrder())   // don't use allDockWidgets to preserve sub-groups
        {
            MoveDockWidgetAction::move(dockwidget, topLevelGroup->subWindow());
        }
        assert(group->subWindow()->allDockWidgets(true, true).empty());

        // find widget's parent:
        for (auto dockwidget : topLevelGroup->subWindow()->allDockWidgets(true, true)) {
            if (dockwidget.second->widget() == group) {
                dockwidget.first->removeDockWidget(dockwidget.second);
                delete dockwidget.second;
                // delete group; automatically deleted when dockWidget is deleted
                group = nullptr;
                break;
            }
        }

        assert(group == nullptr);
    } else {
        assert(false);
    }
}

DockSubWindow *DockGroup::subWindow() const
{
    Q_D(const DockGroup);
    return d->m_subWindow;
}

DockGroupManager *DockGroup::groupManager() const
{
    Q_D(const DockGroup);
    return d->m_mgr;
}

bool DockGroup::canCreateNewGroups() const
{
    Q_D(const DockGroup);
    return d->m_canCreateNewGroups;
}

bool DockGroup::isTopLevel()
{
    return objectName().isEmpty();
}

QString DockGroup::getGroupNameError(const QString &groupName)
{
    Q_D(DockGroup);
    if (groupName.isEmpty()) {
        return "Group must have a non-empty name";
    }

    std::vector<DockSubWindow *> sws = d->m_subWindow->allSubWindows(true, true);
    for (auto sw : sws) {
        if (sw->getGroupName() == groupName)
            return "Group name '" + groupName + "' already used";
    }

    return "";
}

void DockGroup::setupViewMenu(QMenu *menu)
{
    Q_D(DockGroup);
    std::vector<DockSubWindow *> sws = d->m_subWindow->allSubWindows(true, true);

    bool hasPerspectivesMenu = false;
    if (topLevelGroup() == this)
        hasPerspectivesMenu = (d->m_mgr != nullptr);
    else
        assert(false);

    QMenu *organize = menu;
    if (hasPerspectivesMenu)
        organize = menu->addMenu("Organize");

    setupMenu(organize, sws);

    if (hasPerspectivesMenu) {
        QMenu *perspectives = menu->addMenu("Perspectives");
        fillPerspectivesMenu(perspectives);
    }
}

void DockGroup::attachViewMenu(QMenu *menu)
{
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(autoFillAttachedViewMenu()));
}

void DockGroup::setupMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo)
{
    Q_D(DockGroup);
    d->m_subWindow->fillViewMenu(menu, moveTo);
    menu->addSeparator();
    auto moveMenu = menu->addMenu("Move");
    d->m_subWindow->fillMoveMenu(moveMenu, moveTo);
}

void DockGroup::setNewPerspectiveDefaultName(const QString &defaultName)
{
    Q_D(DockGroup);
    d->m_newPerspectiveDefaultName = defaultName;
}

void DockGroup::autoFillAttachedViewMenu()
{
    QMenu *menu = dynamic_cast<QMenu *>(QObject::sender());

    if (menu) {
        menu->clear();
        setupViewMenu(menu);
    } else {
        assert(false);
    }
}

void DockGroup::createPerspective()
{
    Q_D(DockGroup);
    if (!d->m_mgr)
        return;

    QString name = d->m_newPerspectiveDefaultName;
    if (!d->m_newPerspectiveDefaultName.isEmpty()) {
        int index = 2;
        while (d->m_mgr->perspectiveNames().contains(name)) {
            name = d->m_newPerspectiveDefaultName + " (" + QString::number(index) + ")";
            ++index;
        }
    }

    while (true) {
        bool ok = false;
        name = QInputDialog::getText(nullptr, "Create perspective", "Enter perspective name", QLineEdit::Normal, name,
                                     &ok);
        if (ok) {
            if (name.isEmpty()) {
                QMessageBox::critical(nullptr, "Error", "Perspective name cannot be empty");
                continue;
            } else if (d->m_mgr->perspectiveNames().contains(name)) {
                if (QMessageBox::critical(nullptr, "Error", "Perspective '" + name + "' already exists, overwrite it?",
                                          QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
                    continue;
            }

            d->m_mgr->addPerspective(name, this);
            break;
        } else {
            break;
        }
    }
}

void DockGroup::fillPerspectivesMenu(QMenu *menu)
{
    Q_D(DockGroup);
    menu->addAction("Create perspective...", this, SLOT(createPerspective()));

    QStringList perspectiveNames;
    if (d->m_mgr)
        perspectiveNames = d->m_mgr->perspectiveNames();

    if (!perspectiveNames.isEmpty()) {
        QMenu *load = menu->addMenu("Load perspective");
        for (const auto &name : perspectiveNames) {
            load->addAction(new LoadPerspectiveAction(load, name, this));
        }
        QMenu *remove = menu->addMenu("Remove perspective");
        for (const auto &name : perspectiveNames) {
            remove->addAction(new RemovePerspectiveAction(remove, name, this));
        }
    }
}

QX_DOCK_END_NAMESPACE
