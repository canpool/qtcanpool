/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockpanel.h"
#include "docktab.h"
#include "docktabbar.h"
#include "docksidebar.h"
#include "docksidetab.h"
#include "dockwindow_p.h"

#include <QAccessibleWidget>

QX_DOCK_BEGIN_NAMESPACE

namespace
{

/*!
 * \brief Accessible interface for a dock panel body.
 *
 * DockPanel is a painted QFrame with no text of its own, so without an explicit
 * role the default accessibility reports a generic Client. Report Pane instead so
 * assistive tools announce the panel as a named section, and let the default child
 * enumeration expose the tab bar and the dock widget it currently shows.
 */
class DockPanelAccessible : public QAccessibleWidget
{
public:
    explicit DockPanelAccessible(DockPanel *panel)
        : QAccessibleWidget(panel, QAccessible::Pane)
    {
    }
};

/*!
 * \brief Accessible interface for a dock tab.
 *
 * DockTab is a painted QFrame (not a button) whose text lives in an inner label,
 * so the default role would be Client. Report PageTab; the text itself is mirrored
 * into the accessible name by DockTab::setText().
 */
class DockTabAccessible : public QAccessibleWidget
{
public:
    explicit DockTabAccessible(DockTab *tab)
        : QAccessibleWidget(tab, QAccessible::PageTab)
    {
    }
};

/*!
 * \brief Accessible interface for an auto-hide side tab.
 *
 * DockSideTab is a DockButton (a QPushButton), which the default factory reports
 * as PushButton; report PageTab so it is announced as what it is.
 */
class DockSideTabAccessible : public QAccessibleWidget
{
public:
    explicit DockSideTabAccessible(DockSideTab *tab)
        : QAccessibleWidget(tab, QAccessible::PageTab)
    {
    }
};

/*!
 * \brief Accessible interface for the tab bar of a dock panel.
 */
class DockTabBarAccessible : public QAccessibleWidget
{
public:
    explicit DockTabBarAccessible(DockTabBar *tabBar)
        : QAccessibleWidget(tabBar, QAccessible::PageTabList)
    {
    }
};

/*!
 * \brief Accessible interface for the auto-hide side bar.
 *
 * The side bar is a vertical tab bar holding the auto-hide tabs, so PageTabList
 * matches what it is; the default Client role hid that.
 */
class DockSideBarAccessible : public QAccessibleWidget
{
public:
    explicit DockSideBarAccessible(DockSideBar *sideBar)
        : QAccessibleWidget(sideBar, QAccessible::PageTabList)
    {
    }
};

/*!
 * \brief Creates the dock-specific interfaces; returns null for anything else so
 *        the built-in factories keep handling the remaining widgets.
 */
QAccessibleInterface *dockAccessibleFactory(const QString &className, QObject *object)
{
    Q_UNUSED(className)
    if (auto *panel = qobject_cast<DockPanel *>(object)) {
        return new DockPanelAccessible(panel);
    }
    if (auto *tab = qobject_cast<DockTab *>(object)) {
        return new DockTabAccessible(tab);
    }
    if (auto *sideTab = qobject_cast<DockSideTab *>(object)) {
        return new DockSideTabAccessible(sideTab);
    }
    if (auto *tabBar = qobject_cast<DockTabBar *>(object)) {
        return new DockTabBarAccessible(tabBar);
    }
    if (auto *sideBar = qobject_cast<DockSideBar *>(object)) {
        return new DockSideBarAccessible(sideBar);
    }
    return Q_NULLPTR;
}

}   // namespace

void installDockAccessibility()
{
    static bool installed = false;
    if (installed) {
        return;
    }
    installed = true;
    QAccessible::installFactory(&dockAccessibleFactory);
}

QX_DOCK_END_NAMESPACE
