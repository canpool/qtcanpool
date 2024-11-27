/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

class QMenu;

QX_DOCK_BEGIN_NAMESPACE

class DockGroupManager;
class DockPanel;
class DockSubWindow;

class DockGroupPrivate;

class QX_DOCK_EXPORT DockGroup : public QWidget
{
    Q_OBJECT
public:
    using Super = QWidget;
public:
    DockGroup(QWidget *parent, bool canCreateNewGroups, DockGroupManager *manager);
    ~DockGroup();

    DockPanel *addTabWidget(QWidget *widget, const QString &name, DockPanel *after);
    DockPanel *addTabWidget(QWidget *widget, const QString &name, QIcon icon, DockPanel *after);

    DockGroup *createGroup(const QString &groupName, DockPanel *insertPos);
    DockGroup *createGroup(const QString &groupName, QIcon icon, DockPanel *insertPos);

    DockGroup *topLevelGroup() const;
    void destroyGroup(DockGroup *group);

    DockSubWindow *subWindow() const;
    DockGroupManager *groupManager() const;

    bool canCreateNewGroups() const;
    bool isTopLevel();

    QString getGroupNameError(const QString &groupName);

    /** Manually fill a given view menu */
    void setupViewMenu(QMenu *menu);
    /** Attach a view menu that will be automatically fill */
    void attachViewMenu(QMenu *menu);
    void setupMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo);

    void setNewPerspectiveDefaultName(const QString &defaultName);
private slots:
    void autoFillAttachedViewMenu();
    void createPerspective();
private:
    DockGroup(QWidget *parent, DockGroup *topLevelGroup, DockGroupManager *manager);
    void fillPerspectivesMenu(QMenu *menu);
private:
    QX_DECLARE_PRIVATE(DockGroup)
};

QX_DOCK_END_NAMESPACE
