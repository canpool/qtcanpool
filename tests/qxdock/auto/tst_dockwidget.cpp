/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
#include "qxdock/docksidebar.h"
#include "qxdock/docksidetab.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

QX_DOCK_USE_NAMESPACE

class tst_DockWidget : public QObject
{
    Q_OBJECT
private slots:
    void feature();
    void lockedFeatures();
    void deleteContentOnClose();
    void autoHideSideTab();
    void repinAlreadyPinned();
    void repinPanelAlreadyPinned();
};

void tst_DockWidget::feature()
{
    DockWidget dw("dock1");
    QCOMPARE(DockWidget::DefaultDockWidgetFeatures, dw.features());
}

/*
 * DockWidget::features() answers with its own feature set minus whatever the
 * dock window has locked globally, so a freshly built window has to report that
 * nothing is locked yet. Otherwise every dock widget added to it answers
 * features() with arbitrary bits removed, and the first
 * lockDockWidgetFeaturesGlobally() call can be swallowed by its
 * "already in that state" early return.
 *
 * tst_DockWidget::feature() above cannot see this: a dock widget that belongs
 * to no window returns its own set unmasked.
 */
void tst_DockWidget::lockedFeatures()
{
    DockWindow wd;

    DockWidget *dw = new DockWidget("dw1");
    QVERIFY(wd.addDockWidget(Qx::LeftDockWidgetArea, dw));

    QCOMPARE(wd.globallyLockedDockWidgetFeatures(), DockWidget::NoDockWidgetFeatures);
    QCOMPARE(dw->features(), DockWidget::DefaultDockWidgetFeatures);

    // Locking is what removes features, and it has to take effect from the
    // first call on.
    wd.lockDockWidgetFeaturesGlobally(DockWidget::DockWidgetMovable);
    QCOMPARE(wd.globallyLockedDockWidgetFeatures(), DockWidget::DockWidgetMovable);
    QVERIFY(!dw->features().testFlag(DockWidget::DockWidgetMovable));
    QVERIFY(dw->features().testFlag(DockWidget::DockWidgetClosable));
}

/*
 * A dock widget with DeleteContentOnClose is expected to run without content:
 * showDockWidget() rebuilds it from the widget factory, and takeWidget() is the
 * documented way for a caller to take the content back. Closing one that has no
 * content must therefore not try to delete the content it no longer has.
 */
void tst_DockWidget::deleteContentOnClose()
{
    DockWindow wd;

    DockWidget *dw = new DockWidget("dw1");
    dw->setFeature(DockWidget::DockWidgetDeleteOnClose, true);
    dw->setFeature(DockWidget::DeleteContentOnClose, true);

    QWidget *content = new QWidget;
    dw->setWidget(content);
    QVERIFY(wd.addDockWidget(Qx::LeftDockWidgetArea, dw));

    QCOMPARE(dw->takeWidget(), content);
    QCOMPARE(dw->widget(), nullptr);

    dw->toggleView(false);
    QCOMPARE(dw->isClosed(), true);
    QCOMPARE(dw->widget(), nullptr);

    delete content;
}

/*
 * Pins the invariant behind the "toggleView(true) will show sidTab before
 * mainwindow" note that used to sit in DockWidget::toggleViewInternal().
 *
 * That note was attached to a call that toggled the auto-hide container from
 * there. The call is not needed: the tab the user clicks to bring a pinned
 * widget back belongs to the side bar, and the side bar owns its visibility
 * (DockSideBar::insertTab() shows it, removeTab() hides it once the last one is
 * gone). Toggling the dock widget must therefore not add a second tab, not swap
 * the tab object, and not drop the pin.
 */
void tst_DockWidget::autoHideSideTab()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    QVERIFY(wd.addDockWidget(Qx::CenterDockWidgetArea, dw1));

    DockAutoHideContainer *container = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw2);
    QVERIFY(container);
    QCOMPARE(dw2->isAutoHide(), true);
    QCOMPARE(dw2->autoHideContainer(), container);

    DockSideTab *tab = container->autoHideTab();
    DockSideBar *sideBar = container->autoHideSideBar();
    QVERIFY(tab);
    QVERIFY(sideBar);
    QCOMPARE(sideBar->count(), 1);

    dw2->toggleView(false);
    dw2->toggleView(true);

    QCOMPARE(sideBar->count(), 1);
    QCOMPARE(container->autoHideTab(), tab);
    QCOMPARE(dw2->autoHideContainer(), container);
    QCOMPARE(dw2->isAutoHide(), true);

    // The same has to hold across a save/restore round trip, which is where the
    // original note saw the side tab turn up first.
    const QByteArray state = wd.saveState();
    QCOMPARE(wd.restoreState(state), true);

    QCOMPARE(sideBar->count(), 1);
    QCOMPARE(container->autoHideTab(), tab);
}

/*
 * Pinning an already pinned widget has to leave it where it is.
 *
 * The tab context menu offers "Pin" next to "Pin To...", and that entry calls
 * DockWidget::setAutoHide(true) with no area, so the area argument keeps its
 * default of DockSideBarNone. The "nothing changes" guard at the top of
 * setAutoHide() compares that None against the area the widget is actually
 * pinned to, finds them different, and lets the call through to the branch that
 * moves the widget to another side bar - with None, which is not one of the
 * four sides a container ever builds a side bar for.
 */
void tst_DockWidget::repinAlreadyPinned()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    QVERIFY(wd.addDockWidget(Qx::CenterDockWidgetArea, dw1));

    DockWidget *dw2 = new DockWidget("dw2");
    DockAutoHideContainer *container = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw2);
    QVERIFY(container);
    QCOMPARE(dw2->isAutoHide(), true);
    QCOMPARE(dw2->autoHideArea(), Qx::DockSideBarLeft);
    QCOMPARE(container->autoHideSideBar()->count(), 1);

    dw2->setAutoHide(true);

    QCOMPARE(dw2->isAutoHide(), true);
    QCOMPARE(dw2->autoHideArea(), Qx::DockSideBarLeft);
    QCOMPARE(dw2->autoHideContainer(), container);
    QCOMPARE(container->autoHideSideBar()->count(), 1);
}

/*
 * DockPanel::setAutoHide() is the panel level twin of the call above and is
 * part of the exported API, so it has to answer DockSideBarNone the same way.
 *
 * The panel that a pinned widget lives in is the auto hide container's own
 * panel, so it reports isAutoHide() as true and reaches the same "move to
 * another side bar" branch.
 */
void tst_DockWidget::repinPanelAlreadyPinned()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    QVERIFY(wd.addDockWidget(Qx::CenterDockWidgetArea, dw1));

    DockWidget *dw2 = new DockWidget("dw2");
    DockAutoHideContainer *container = wd.addAutoHideDockWidget(Qx::DockSideBarRight, dw2);
    QVERIFY(container);

    DockPanel *panel = dw2->dockPanel();
    QVERIFY(panel);
    QCOMPARE(panel->isAutoHide(), true);

    panel->setAutoHide(true);

    QCOMPARE(panel->isAutoHide(), true);
    QCOMPARE(dw2->isAutoHide(), true);
    QCOMPARE(dw2->autoHideArea(), Qx::DockSideBarRight);
    QCOMPARE(dw2->autoHideContainer(), container);
    QCOMPARE(container->autoHideSideBar()->count(), 1);
}

TEST_ADD(tst_DockWidget)

#include "tst_dockwidget.moc"
