/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * DockTitleBar is assembled by DockPanel and never constructed by a caller, so
 * the class has no behaviour of its own worth testing - what it does have is a
 * fixed set of parts that other code reaches for by name: the tab bar, the three
 * buttons and the label an auto-hidden panel shows instead.
 *
 * Every one of them is created unconditionally; what changes with the
 * DockManager configuration flags is whether they are visible. That distinction
 * is what this pins down, because a caller doing
 * titleBar->button(Qx::TitleBarButtonAutoHide)->setChecked(true) has to be able
 * to rely on the button existing even when auto hiding is switched off.
 */
#include "tst_global.h"

#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
// docktitlebar.h only forward declares DockTabBar, and the parent check below
// needs the real type.
#include "qxdock/docktabbar.h"
#include "qxdock/docktitlebar.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

QX_DOCK_USE_NAMESPACE

class tst_DockTitleBar : public QObject
{
    Q_OBJECT

private slots:
    void everyPartExistsWhateverTheConfiguration();
    void belongsToItsPanel();
    void reservesTheFocusRuleOnlyWhenTheFlagIsOn();
};

void tst_DockTitleBar::everyPartExistsWhateverTheConfiguration()
{
    DockWindow window;
    DockWidget *dw = new DockWidget(QStringLiteral("titled"));
    QVERIFY(window.addDockWidget(Qx::CenterDockWidgetArea, dw));

    DockPanel *panel = dw->dockPanel();
    QVERIFY(panel);

    DockTitleBar bar(panel);

    QVERIFY(bar.tabBar());
    QVERIFY(bar.autoHideTitleLabel());

    QVERIFY(bar.button(Qx::TitleBarButtonTabsMenu));
    QVERIFY(bar.button(Qx::TitleBarButtonUndock));
    QVERIFY(bar.button(Qx::TitleBarButtonClose));
    QVERIFY(bar.button(Qx::TitleBarButtonAutoHide));
    QVERIFY(bar.button(Qx::TitleBarButtonMinimize));

    // Asking for a button that is not in the enum is a null, not a crash.
    QVERIFY(!bar.button(static_cast<Qx::DockTitleBarButton>(-1)));
}

void tst_DockTitleBar::belongsToItsPanel()
{
    DockWindow window;
    DockWidget *dw = new DockWidget(QStringLiteral("titled"));
    QVERIFY(window.addDockWidget(Qx::CenterDockWidgetArea, dw));

    DockPanel *panel = dw->dockPanel();
    DockTitleBar bar(panel);

    QCOMPARE(bar.dockPanel(), panel);

    // The tab bar is built for that panel - the constructor argument, not the
    // QWidget parent: the layout takes ownership, so the parent is the title bar.
    QCOMPARE(bar.tabBar()->parentWidget(), &bar);
}

/*!
 * The sheet reserves the two pixels the focused title bar paints in the accent,
 * and it can only ask about that through a property: the reservation is for
 * every panel, focused or not, so the focus state cannot be what selects it -
 * a panel that has not been in focus yet carries no "focused" property at all,
 * and would be the one panel whose content jumps when it gains the focus.
 *
 * So the property has to follow the flag, and only the flag. An application
 * that never asks for focus highlighting gets none of it.
 */
void tst_DockTitleBar::reservesTheFocusRuleOnlyWhenTheFlagIsOn()
{
    DockWindow window;
    DockWidget *dw = new DockWidget(QStringLiteral("focus rule"));
    QVERIFY(window.addDockWidget(Qx::CenterDockWidgetArea, dw));

    DockPanel *panel = dw->dockPanel();
    QVERIFY(panel);

    const bool wasOn = DockManager::testConfigFlag(DockManager::FocusHighlighting);

    DockManager::setConfigFlag(DockManager::FocusHighlighting, false);
    {
        DockTitleBar bar(panel);
        QVERIFY2(!bar.property("focusHighlighting").isValid(),
                 "the focus rule is reserved without DockManager::FocusHighlighting");
    }

    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    {
        DockTitleBar bar(panel);
        QCOMPARE(bar.property("focusHighlighting").toBool(), true);
    }

    DockManager::setConfigFlag(DockManager::FocusHighlighting, wasOn);
}

TEST_ADD(tst_DockTitleBar)

#include "tst_docktitlebar.moc"
