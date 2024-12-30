#include "tst_global.h"

#include "qxdock/dockcontainer.h"
#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockfloatingcontainer.h"
#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockmanager.h"

QX_DOCK_USE_NAMESPACE

class tst_DockContainer : public QObject
{
    Q_OBJECT
private slots:
    void addDockWidget();
    void zOrder();
    void floating();
    void closeOtherAreas();
    void autohide();
};

void tst_DockContainer::addDockWidget()
{
    DockWindow wd;
    DockContainer c(&wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockPanel *p1 = c.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = c.addDockWidget(Qx::CenterDockWidgetArea, dw2);
    DockPanel *p3 = c.addDockWidget(Qx::CenterDockWidgetArea, dw3, p2);
    DockPanel *p4 = c.addDockWidget(Qx::RightDockWidgetArea, dw4, p3);

    QCOMPARE(dw1->dockPanel(), p1);
    QCOMPARE(dw2->dockPanel(), p2);
    QCOMPARE(dw3->dockPanel(), p3);
    QCOMPARE(dw4->dockPanel(), p4);

    QCOMPARE(p2, p3);
    QCOMPARE(p3->currentDockWidget(), dw3);
    QCOMPARE(p2->openDockWidgetsCount(), 2);

    QCOMPARE(c.hasOpenDockPanels(), true);
    QCOMPARE(c.openedDockPanels().count(), 3);
    QCOMPARE(c.dockPanelCount(), 3);
    QCOMPARE(c.visibleDockPanelCount(), 3);
    QCOMPARE(c.dockPanel(0), p1);
    QCOMPARE(c.dockPanel(1), p2);
    QCOMPARE(c.dockPanel(2), p4);

    QCOMPARE(c.hasTopLevelDockWidget(), false);
    QCOMPARE(c.openedDockWidgets().count(), 4);

    // remove
    c.removeDockWidget(dw3);
    QCOMPARE(dw3->dockPanel(), nullptr);
    QCOMPARE(p3->currentDockWidget(), dw2);

    QCOMPARE(c.hasTopLevelDockWidget(), false);
    QCOMPARE(c.openedDockWidgets().count(), 3);

    // hide
    p2->setVisible(false);
    QCOMPARE(c.hasOpenDockPanels(), true);
    QCOMPARE(c.openedDockPanels().count(), 2);
    QCOMPARE(c.dockPanelCount(), 3);
    QCOMPARE(c.visibleDockPanelCount(), 2);
    QCOMPARE(c.hasTopLevelDockWidget(), false);

    p4->setVisible(false);
    QCOMPARE(c.hasOpenDockPanels(), true);
    QCOMPARE(c.openedDockPanels().count(), 1);
    QCOMPARE(c.dockPanelCount(), 3);
    QCOMPARE(c.visibleDockPanelCount(), 1);
    QCOMPARE(c.hasTopLevelDockWidget(), true);
}

void tst_DockContainer::zOrder()
{
    DockWindow wd;
    DockContainer c1(&wd);
    DockContainer c2(&wd);

    QCOMPARE(wd.zOrderIndex(), 0);
    QCOMPARE(c1.zOrderIndex(), 0);
    QCOMPARE(c2.zOrderIndex(), 0);
    QCOMPARE(c1.isInFrontOf(&wd), false);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");

    DockPanel *p1 = c1.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = c2.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);

    QCOMPARE(p1->dockContainer(), &c1);
    QCOMPARE(p2->dockContainer(), &c1); // not c2 ??

    c2.setVisible(true);
    QCOMPARE(c2.zOrderIndex(), 1);
    c2.setVisible(false);
    QCOMPARE(c2.zOrderIndex(), 1);
    c2.setVisible(true);
    QCOMPARE(c2.zOrderIndex(), 1);

    c2.hide();
    QCOMPARE(c2.zOrderIndex(), 1);
    c2.show();
    QCOMPARE(c2.zOrderIndex(), 1);

    c2.raise();
    QCOMPARE(c2.zOrderIndex(), 1);

    QEvent activateEvent(QEvent::WindowActivate);
    QApplication::sendEvent(&c2, &activateEvent);
    QCOMPARE(c2.zOrderIndex(), 2);

    QCOMPARE(c2.isInFrontOf(&c1), true);
}

void tst_DockContainer::floating()
{
    DockWindow wd;
    DockContainer c1(&wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");

    c1.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    QCOMPARE(c1.isFloating(), false);
    QCOMPARE(c1.floatingWidget(), nullptr);

    DockFloatingContainer *fc2 = wd.addDockWidgetFloating(dw2);

    DockContainer *c2 = fc2->dockContainer();
    QCOMPARE(c2->isFloating(), true);
    QCOMPARE(c2->floatingWidget(), fc2);
}

void tst_DockContainer::closeOtherAreas()
{
    DockWindow wd;
    DockContainer c(&wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");

    DockPanel *p1 = c.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = c.addDockWidget(Qx::CenterDockWidgetArea, dw2);
    DockPanel *p3 = c.addDockWidget(Qx::RightDockWidgetArea, dw3);

    QCOMPARE(c.hasOpenDockPanels(), true);
    QCOMPARE(c.openedDockPanels().count(), 3);
    QCOMPARE(c.dockPanelCount(), 3);
    QCOMPARE(c.visibleDockPanelCount(), 3);

    c.closeOtherAreas(p1);
    Q_UNUSED(p2);
    Q_UNUSED(p3);

    QCOMPARE(c.hasOpenDockPanels(), true);
    QCOMPARE(c.openedDockPanels().count(), 1);
    QCOMPARE(c.dockPanelCount(), 3);
    QCOMPARE(c.visibleDockPanelCount(), 1);
}

void tst_DockContainer::autohide()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw1);
    DockAutoHideContainer *ahc2 = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw2);
    DockAutoHideContainer *ahc3 = wd.addAutoHideDockWidget(Qx::DockSideBarRight, dw3);
    DockAutoHideContainer *ahc4 = wd.addAutoHideDockWidget(Qx::DockSideBarBottom, dw4);

    QCOMPARE(ahc1->autoHideSideBar(), wd.autoHideSideBar(Qx::DockSideBarTop));
    QCOMPARE(ahc2->autoHideSideBar(), wd.autoHideSideBar(Qx::DockSideBarLeft));
    QCOMPARE(ahc3->autoHideSideBar(), wd.autoHideSideBar(Qx::DockSideBarRight));
    QCOMPARE(ahc4->autoHideSideBar(), wd.autoHideSideBar(Qx::DockSideBarBottom));

    QCOMPARE(wd.autoHideWidgets().count(), 4);
}

TEST_ADD(tst_DockContainer)

#include "tst_dockcontainer.moc"
