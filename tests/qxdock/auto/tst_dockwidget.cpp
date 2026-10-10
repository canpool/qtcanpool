#include "tst_global.h"

#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockmanager.h"
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
    void autoHideSideTab();
};

void tst_DockWidget::feature()
{
    DockWidget dw("dock1");
    QCOMPARE(DockWidget::DefaultDockWidgetFeatures, dw.features());
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

TEST_ADD(tst_DockWidget)

#include "tst_dockwidget.moc"
