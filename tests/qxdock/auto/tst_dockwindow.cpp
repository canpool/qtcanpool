#include "tst_global.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockcontainer.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockfloatingcontainer.h"

#include <QLabel>
#include <QDebug>

QX_DOCK_USE_NAMESPACE

class tst_DockWindow : public QObject
{
    Q_OBJECT
private slots:
    void addDockWidget_other();
    void addDockWidget();
    void addDockWidgetTab();
    void addDockWidgetToContainer();
    void removeDockWidget();
    void addAutoHideDockWidget();
    void addDockWidgetFloating();
};

void tst_DockWindow::addDockWidget_other()
{
    DockWindow window;

    DockWidget *w = new DockWidget("Label 1");
    QLabel *l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("I am happy to join with you today in what will go down in history as the greatest demonstration "
               "for freedom in the history of our nation.");
    w->setWidget(l);
    DockPanel *centerPanel = window.addDockWidget(Qx::CenterDockWidgetArea, w);

    QCOMPARE(centerPanel, w->dockPanel());
    QCOMPARE(centerPanel->dockWidgetsCount(), 1);
    QCOMPARE(centerPanel->openDockWidgetsCount(), 1);
    QCOMPARE(centerPanel->currentDockWidget(), w);
    QCOMPARE(centerPanel->currentDockWidget(), centerPanel->dockWidget(centerPanel->currentIndex()));

    DockContainer *container = centerPanel->dockContainer();
    QCOMPARE(container, w->dockContainer());
    QCOMPARE(container->dockPanelCount(), 1);

    DockPanel *centerPanel_2 = window.addDockWidget(Qx::CenterDockWidgetArea, w);
    QVERIFY(centerPanel_2 != centerPanel);
    QCOMPARE(centerPanel->dockWidgetsCount(), 0);   // w was removed from centerPanel
    QVERIFY(centerPanel != w->dockPanel());
    QCOMPARE(centerPanel->dockContainer(), nullptr);   // centerPanel was removed from it's container
    QCOMPARE(centerPanel_2, w->dockPanel());
    QCOMPARE(centerPanel_2->currentDockWidget(), w);

    // WARNING: centerPanel called deleteLater but not deleted.
    // The centerPanel can no longer be used and may become a wild pointer

    DockWidget *w2 = new DockWidget("empty");
    DockPanel *centerPanel_3 = window.addDockWidget(Qx::CenterDockWidgetArea, w2, centerPanel);
    QVERIFY(centerPanel_3 != centerPanel);
    QCOMPARE(centerPanel->dockContainer(), nullptr);
    QCOMPARE(window.dockWidgetsMap().count(), 2);
    QCOMPARE(window.dockWidgetsMap().value("Label 1"), w);

    DockPanel *topPanel = window.addDockWidget(Qx::TopDockWidgetArea, w2, centerPanel);
    QCOMPARE(centerPanel_3->dockWidgetsCount(), 0); // w2 was removed from centerPanel_3
    QVERIFY(topPanel != centerPanel);
    QVERIFY(topPanel != centerPanel_3);
    QCOMPARE(topPanel->currentDockWidget(), w2);
}

void tst_DockWindow::addDockWidget()
{
    // add dock widget
    {
        DockWindow wd;

        int i = 0;
        connect(&wd, &DockWindow::dockWidgetAdded, this, [&i](DockWidget*){
            ++i;
        });

        int j = 0;
        connect(&wd, &DockWindow::dockAreasAdded, this, [&j](){
            // dock panel added
            ++j;
        });

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        DockPanel *p1 = wd.addDockWidget(Qx::NoDockWidgetArea, dw1);    // same as TopDockWidgetArea
        DockPanel *p2 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw2);
        DockPanel *p3 = wd.addDockWidget(Qx::RightDockWidgetArea, dw3);
        DockPanel *p4 = wd.addDockWidget(Qx::TopDockWidgetArea, dw4);
        DockPanel *p5 = wd.addDockWidget(Qx::BottomDockWidgetArea, dw5);
        DockPanel *p6 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw6);
        DockPanel *p7 = wd.addDockWidget(Qx::LeftAutoHideArea, dw7);    // same as TopDockWidgetArea

        QCOMPARE(dw1->dockPanel(), p1);
        QCOMPARE(dw2->dockPanel(), p2);
        QCOMPARE(dw3->dockPanel(), p3);
        QCOMPARE(dw4->dockPanel(), p4);
        QCOMPARE(dw5->dockPanel(), p5);
        QCOMPARE(dw6->dockPanel(), p6);
        QCOMPARE(dw7->dockPanel(), p7);

        QCOMPARE(wd.findDockWidget("dw1"), dw1);
        QCOMPARE(wd.findDockWidget("dw2"), dw2);
        QCOMPARE(wd.findDockWidget("dw3"), dw3);
        QCOMPARE(wd.findDockWidget("dw4"), dw4);
        QCOMPARE(wd.findDockWidget("dw5"), dw5);
        QCOMPARE(wd.findDockWidget("dw6"), dw6);
        QCOMPARE(wd.findDockWidget("dw7"), dw7);

        QCOMPARE(wd.dockContainers().count(), 1);
        QCOMPARE(wd.dockWidgetsMap().count(), 7);
        QCOMPARE(i, 7);
        QCOMPARE(j, 7);
        QCOMPARE(wd.centralWidget(), nullptr);
    }

    // add dock widget to panel
    {
        DockWindow wd;

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        DockPanel *p1 = wd.addDockWidget(Qx::NoDockWidgetArea, dw1);
        DockPanel *p2 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw2, p1);
        DockPanel *p3 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw3, p1);
        DockPanel *p4 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw4, p1, 0);
        DockPanel *p5 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw5, p1);
        DockPanel *p6 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw6, p1);
        DockPanel *p7 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw7, p1, 0);

        QVERIFY(p1 != p2);
        QVERIFY(p2 != p3);
        QVERIFY(p2 != p4);
        QVERIFY(p3 != p4);

        QCOMPARE(p1, p5);
        QCOMPARE(p1, p6);
        QCOMPARE(p1, p7);

        QCOMPARE(wd.openedDockPanels().count(), 4); // p1,p2,p3,p4
        QCOMPARE(p1->dockWidgets().count(), 4);

        QString text = "";
        for (DockWidget *w : p1->dockWidgets()) {
            text += w->objectName() + ",";
        }
        QCOMPARE(text, "dw7,dw1,dw5,dw6,");
    }
}

void tst_DockWindow::addDockWidgetTab()
{
    // add dock widget
    {
        DockWindow wd;

        int j = 0;
        connect(&wd, &DockWindow::dockAreasAdded, this, [&j](){
            // dock panel added
            ++j;
        });

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        // widget same as TopDockWidgetArea, panel same as CenterDockWidgetArea
        DockPanel *p1 = wd.addDockWidgetTab(Qx::NoDockWidgetArea, dw1);
        DockPanel *p2 = wd.addDockWidgetTab(Qx::LeftDockWidgetArea, dw2);
        DockPanel *p3 = wd.addDockWidgetTab(Qx::RightDockWidgetArea, dw3);
        DockPanel *p4 = wd.addDockWidgetTab(Qx::TopDockWidgetArea, dw4);
        DockPanel *p5 = wd.addDockWidgetTab(Qx::BottomDockWidgetArea, dw5);
        DockPanel *p6 = wd.addDockWidgetTab(Qx::CenterDockWidgetArea, dw6);
        // widget same as TopDockWidgetArea, panel same as CenterDockWidgetArea
        DockPanel *p7 = wd.addDockWidgetTab(Qx::LeftAutoHideArea, dw7);

        QCOMPARE(dw1->dockPanel(), p1);
        QCOMPARE(dw2->dockPanel(), p2);
        QCOMPARE(dw3->dockPanel(), p3);
        QCOMPARE(dw4->dockPanel(), p4);
        QCOMPARE(dw5->dockPanel(), p5);
        QCOMPARE(dw6->dockPanel(), p6);
        QCOMPARE(dw7->dockPanel(), p7);

        QCOMPARE(p1, p6);
        QCOMPARE(p6, p7);

        QCOMPARE(wd.findDockWidget("dw1"), dw1);
        QCOMPARE(wd.findDockWidget("dw2"), dw2);
        QCOMPARE(wd.findDockWidget("dw3"), dw3);
        QCOMPARE(wd.findDockWidget("dw4"), dw4);
        QCOMPARE(wd.findDockWidget("dw5"), dw5);
        QCOMPARE(wd.findDockWidget("dw6"), dw6);
        QCOMPARE(wd.findDockWidget("dw7"), dw7);

        QCOMPARE(wd.dockContainers().count(), 1);
        QCOMPARE(wd.dockWidgetsMap().count(), 7);
        QCOMPARE(j, 5); // p1,p2,p3,p4,p5
        QCOMPARE(wd.centralWidget(), nullptr);
    }

    // add dock widget to panel
    {
        DockWindow wd;

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");

        DockPanel *p1 = wd.addDockWidgetTab(dw1, nullptr);
        DockPanel *p2 = wd.addDockWidgetTab(dw2, p1);
        DockPanel *p3 = wd.addDockWidgetTab(dw3, p1, 0);
        DockPanel *p4 = wd.addDockWidgetTab(dw4, p1, 10);

        QCOMPARE(p1, p2);
        QCOMPARE(p2, p3);
        QCOMPARE(p3, p4);

        QCOMPARE(wd.openedDockPanels().count(), 1);
        QCOMPARE(p1->dockWidgets().count(), 4);

        QString text = "";
        for (DockWidget *w : p1->dockWidgets()) {
            text += w->objectName() + ",";
        }
        QCOMPARE(text, "dw3,dw1,dw2,dw4,");
    }
}

void tst_DockWindow::addDockWidgetToContainer()
{
    DockWindow wd;

    DockContainer *c = new DockContainer(&wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");

    DockPanel *p1 = wd.addDockWidgetToContainer(Qx::CenterDockWidgetArea, dw1, c);
    DockPanel *p2 = wd.addDockWidgetToContainer(Qx::CenterDockWidgetArea, dw2, &wd);

    QVERIFY(p1 != p2);

    QCOMPARE(wd.dockContainers().count(), 2);
    QCOMPARE(wd.dockWidgetsMap().count(), 2);

    QCOMPARE(wd.openedDockWidgets().count(), 1);
    QCOMPARE(wd.openedDockPanels().count(), 1);

    QCOMPARE(c->openedDockWidgets().count(), 1);
    QCOMPARE(c->openedDockPanels().count(), 1);
}

void tst_DockWindow::removeDockWidget()
{
    DockWindow wd;

    int i = 0;
    connect(&wd, &DockWindow::dockWidgetAboutToBeRemoved, this, [&i](DockWidget*){
        ++i;
    });
    int j = 0;
    connect(&wd, &DockWindow::dockWidgetRemoved, this, [&j](DockWidget*){
        ++j;
    });

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");

    DockPanel *p1 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidgetTab(Qx::CenterDockWidgetArea, dw2);
    DockPanel *p3 = wd.addDockWidgetTab(dw3, p2);

    QCOMPARE(p1, p2);
    QCOMPARE(p2, p3);

    QCOMPARE(p1->dockWidgets().count(), 3);

    wd.removeDockWidget(dw1);
    QCOMPARE(i, 1);
    QCOMPARE(j, 1);

    QCOMPARE(p1->dockWidgets().count(), 2);
}

void tst_DockWindow::addAutoHideDockWidget()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockAutoHideContainer *ahc = wd.addAutoHideDockWidget(Qx::DockSideBarNone, dw1);
    QCOMPARE(ahc, nullptr);

    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw1);
    DockAutoHideContainer *ahc2 = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw2);
    DockAutoHideContainer *ahc3 = wd.addAutoHideDockWidget(Qx::DockSideBarRight, dw3);
    DockAutoHideContainer *ahc4 = wd.addAutoHideDockWidget(Qx::DockSideBarBottom, dw4);

    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(ahc2->sideBarArea(), Qx::DockSideBarLeft);
    QCOMPARE(ahc3->sideBarArea(), Qx::DockSideBarRight);
    QCOMPARE(ahc4->sideBarArea(), Qx::DockSideBarBottom);

    QCOMPARE(ahc1->dockWidget(), dw1);
    QCOMPARE(ahc2->dockWidget(), dw2);
    QCOMPARE(ahc3->dockWidget(), dw3);
    QCOMPARE(ahc4->dockWidget(), dw4);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);
}

void tst_DockWindow::addDockWidgetFloating()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw1);
    DockFloatingContainer *fc2 = wd.addDockWidgetFloating(dw2);
    DockFloatingContainer *fc3 = wd.addDockWidgetFloating(dw3);

    QCOMPARE(fc1->dockContainer(), dw1->dockContainer());
    QCOMPARE(fc2->dockContainer(), dw2->dockContainer());
    QCOMPARE(fc3->dockContainer(), dw3->dockContainer());

    QCOMPARE(dw1->dockFloatingContainer(), fc1);
    QCOMPARE(dw2->dockFloatingContainer(), fc2);
    QCOMPARE(dw3->dockFloatingContainer(), fc3);

    QCOMPARE(fc1->hasTopLevelDockWidget(), true);
    QCOMPARE(fc2->hasTopLevelDockWidget(), true);
    QCOMPARE(fc3->hasTopLevelDockWidget(), true);

    QCOMPARE(fc1->topLevelDockWidget(), dw1);
    QCOMPARE(fc2->topLevelDockWidget(), dw2);
    QCOMPARE(fc3->topLevelDockWidget(), dw3);

    QCOMPARE(wd.dockWidgetsMap().count(), 3);

    QCOMPARE(dw1->isFloating(), true);
    QCOMPARE(dw2->isInFloatingContainer(), true);
    QCOMPARE(dw3->isInFloatingContainer(), true);
    QCOMPARE(dw3->isFloating(), true);

    fc3->dockContainer()->addDockWidget(Qx::CenterDockWidgetArea, dw4);
    QCOMPARE(dw3->isInFloatingContainer(), true);
    QCOMPARE(dw3->isFloating(), false);
    QCOMPARE(dw4->isInFloatingContainer(), true);
    QCOMPARE(dw4->isFloating(), false);
    QCOMPARE(fc3->dockWidgets().count(), 2);

    // It is still 3, because dw4 was not added via DockWindow
    QCOMPARE(wd.dockWidgetsMap().count(), 3);

    QVERIFY(dw3->dockContainer() == dw4->dockContainer());
    QVERIFY(dw3->dockPanel() != dw4->dockPanel());
}

TEST_ADD(tst_DockWindow)

#include "tst_dockwindow.moc"
