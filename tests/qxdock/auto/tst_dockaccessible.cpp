/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxdock/dockpanel.h"
#include "qxdock/docksidebar.h"
#include "qxdock/docksidetab.h"
#include "qxdock/docktab.h"
#include "qxdock/docktabbar.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QAccessible>

QX_DOCK_USE_NAMESPACE

class tst_DockAccessible : public QObject
{
    Q_OBJECT
private slots:
    void names();
    void roles();
};

void tst_DockAccessible::names()
{
    // Accessibility (B2): the dock widget title is mirrored into the accessible
    // name, with the '&' mnemonic stripped, and kept in step with title changes.
    DockWidget dw(QStringLiteral("&Document"));
    QCOMPARE(dw.accessibleName(), QStringLiteral("Document"));

    dw.setWindowTitle(QStringLiteral("&Report"));
    QCOMPARE(dw.accessibleName(), QStringLiteral("Report"));

    // The tab text is mirrored the same way; the initial text comes from the dock
    // widget's title.
    DockTab tab(&dw);
    QCOMPARE(tab.accessibleName(), QStringLiteral("Report"));

    tab.setText(QStringLiteral("&Summary"));
    QCOMPARE(tab.accessibleName(), QStringLiteral("Summary"));
}

void tst_DockAccessible::roles()
{
    // Constructing the window is what registers the dock's QAccessible factory
    // (see installDockAccessibility()), so do that first.
    DockWindow wd;

    DockWidget *dw = new DockWidget(QStringLiteral("center"));
    DockPanel *panel = wd.addDockWidget(Qx::CenterDockWidgetArea, dw);
    QVERIFY(panel);

    DockSideBar sideBar(&wd, Qx::DockSideBarLeft);
    DockSideTab sideTab;

    // The painted panel body is a Pane, its tab and the auto-hide tab are PageTabs,
    // and the tab bar and side bar are PageTabLists - roles the default factories
    // would all have reported as Client or PushButton.
    auto roleOf = [](QObject *o) {
        // The returned interface is owned by QAccessible's cache (its destructor is
        // protected), so it must not be deleted here.
        QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(o);
        return iface ? iface->role() : QAccessible::NoRole;
    };

    QCOMPARE(roleOf(panel), QAccessible::Pane);
    QCOMPARE(roleOf(dw->tab()), QAccessible::PageTab);
    QCOMPARE(roleOf(panel->findChild<DockTabBar *>()), QAccessible::PageTabList);
    QCOMPARE(roleOf(&sideTab), QAccessible::PageTab);
    QCOMPARE(roleOf(&sideBar), QAccessible::PageTabList);
}

TEST_ADD(tst_DockAccessible)

#include "tst_dockaccessible.moc"
