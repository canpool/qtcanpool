/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockwindow.h"

class QMenu;

QX_DOCK_BEGIN_NAMESPACE

class DockGroup;
class DockWidget;
class DockPanel;

class DockSubWindowPrivate;

class QX_DOCK_EXPORT DockSubWindow : public DockWindow
{
    Q_OBJECT
public:
    using Super = DockWindow;
public:
    explicit DockSubWindow(DockGroup *parent);
    virtual ~DockSubWindow() override;

    void fillViewMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo);
    void fillMoveMenu(QMenu *menu, const std::vector<DockSubWindow *> &moveTo);

    void addPerspectiveRec(const QString &name);
    void openPerspectiveRec(const QString &name);
    void removePerspectivesRec();
    void loadPerspectivesRec(QSettings &Settings);
    void savePerspectivesRec(QSettings &Settings) const;

    static DockSubWindow *subWindow(DockWidget *widget);
    void childWindows(std::vector<DockSubWindow *> &subs, bool rec) const;
    std::vector<DockSubWindow *> allSubWindows(bool includeThis, bool rec) const;
    std::vector<std::pair<DockSubWindow *, DockWidget *>> allDockWidgets(bool includeThis, bool rec) const;

    QString getGroupName();
    QString getPersistGroupName();
    static QString getGroupNameFromPersistGroupName(QString persistGroupName);
    DockGroup *group();

    QMap<QString, QStringList> getGroupContents();
    DockPanel *getInsertDefaultPos();
    std::vector<DockWidget *> getWidgetsInGUIOrder() const;
private:
    QX_DECLARE_PRIVATE(DockSubWindow)
};

QX_DOCK_END_NAMESPACE
