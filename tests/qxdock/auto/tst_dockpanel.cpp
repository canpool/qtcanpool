#include "tst_global.h"

#include "qxdock/dockpanel.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"
#include "qxdock/docksplitter.h"

QX_DOCK_USE_NAMESPACE

class tst_DockPanel : public QObject
{
    Q_OBJECT
private slots:
    void dockWidget();
    void splitter();
    void centralWidget();
};

void tst_DockPanel::dockWidget()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");
    DockWidget *dw5 = new DockWidget("dw5");
    DockWidget *dw6 = new DockWidget("dw6");
    DockWidget *dw7 = new DockWidget("dw7");
    DockWidget *dw8 = new DockWidget("dw8");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw2);
    DockPanel *p3 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw3, p2);
    DockPanel *p4 = wd.addDockWidget(Qx::RightDockWidgetArea, dw4, p3);
    DockPanel *p5 = wd.addDockWidget(Qx::TopDockWidgetArea, dw5);
    DockPanel *p6 = wd.addDockWidget(Qx::BottomDockWidgetArea, dw6);
    DockPanel *p7 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw7);
    DockPanel *p8 = wd.addDockWidget(Qx::RightDockWidgetArea, dw8);

    // container
    QCOMPARE(p1->dockWindow(), &wd);
    QCOMPARE(p1->dockContainer(), &wd);
    QCOMPARE(p2, p3);
    QCOMPARE(p2->dockContainer(), p4->dockContainer());

    // widget
    QCOMPARE(p1->dockWidgetsCount(), 1);
    QCOMPARE(p1->dockWidgetsCount(), p1->dockWidgets().count());
    QCOMPARE(p2->dockWidgetsCount(), 2);
    QCOMPARE(p2->dockWidgetsCount(), p2->dockWidgets().count());
    QCOMPARE(p4->dockWidgetsCount(), 1);
    QCOMPARE(p4->dockWidgetsCount(), p4->dockWidgets().count());

    QCOMPARE(p2->currentDockWidget(), dw3);
    QCOMPARE(p2->currentIndex(), 1);
    p2->setCurrentDockWidget(dw2);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(p2->currentIndex(), 0);

    QCOMPARE(p2->openDockWidgetsCount(), 2);
    QCOMPARE(p2->openDockWidgetsCount(), p2->openedDockWidgets().count());
    QCOMPARE(p2->indexOfFirstOpenDockWidget(), 0);

    dw2->toggleView(false);
    QCOMPARE(p2->openDockWidgetsCount(), 1);
    QCOMPARE(p2->openDockWidgetsCount(), p2->openedDockWidgets().count());
    QCOMPARE(p2->indexOfFirstOpenDockWidget(), 1);

    // features
    QCOMPARE(dw2->features(), DockWidget::DefaultDockWidgetFeatures);
    QCOMPARE(dw3->features(), DockWidget::DefaultDockWidgetFeatures);
    QCOMPARE(p2->features(), DockWidget::DefaultDockWidgetFeatures);
    QCOMPARE(p2->features(Qx::DockBitwiseOr), DockWidget::DefaultDockWidgetFeatures);

    // areas
    QCOMPARE(p5->allowedAreas(), Qx::AllDockAreas);
    p5->setAllowedAreas(Qx::CenterDockWidgetArea);
    QCOMPARE(p5->allowedAreas(), Qx::CenterDockWidgetArea);

    // flags
    QCOMPARE(p6->dockAreaFlags(), DockPanel::DefaultFlags);
    p6->setDockAreaFlags(DockPanel::HideSingleWidgetTitleBar);
    QCOMPARE(p6->dockAreaFlags(), DockPanel::HideSingleWidgetTitleBar);
    p6->setDockAreaFlag(DockPanel::HideSingleWidgetTitleBar, false);
    QCOMPARE(p6->dockAreaFlags(), DockPanel::DefaultFlags);

    // central widget
    QCOMPARE(p1->containsCentralWidget(), false);
    QCOMPARE(p2->containsCentralWidget(), false);

    // topLevelArea
    QCOMPARE(p7->isTopLevelArea(), false);
    QCOMPARE(p8->isTopLevelArea(), false);
}

void tst_DockPanel::splitter()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");
    DockWidget *dw5 = new DockWidget("dw5");
    DockWidget *dw6 = new DockWidget("dw6");
    DockWidget *dw7 = new DockWidget("dw7");
    DockWidget *dw8 = new DockWidget("dw8");

    /*
     * Qx::TopDockWidgetArea    - Qt::Vertical
     * Qx::RightDockWidgetArea  - Qt::Horizontal
     * Qx::CenterDockWidgetArea - Qt::Vertical
     * Qx::BottomDockWidgetArea - Qt::Vertical
     * Qx::LeftDockWidgetArea   - Qt::Horizontal
     */

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Horizontal);

    DockPanel *p2 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw2);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical); // follow second
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Vertical);

    DockPanel *p3 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw3, p2);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p3->parentSplitter()->orientation(), Qt::Vertical); // p2 == p3

    DockPanel *p4 = wd.addDockWidget(Qx::RightDockWidgetArea, dw4, p3);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal); // p3 is added to p4 splitter
    QCOMPARE(p4->parentSplitter()->orientation(), Qt::Horizontal);

    DockPanel *p5 = wd.addDockWidget(Qx::TopDockWidgetArea, dw5);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p4->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p5->parentSplitter()->orientation(), Qt::Vertical);

    DockPanel *p6 = wd.addDockWidget(Qx::BottomDockWidgetArea, dw6);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p4->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p5->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p6->parentSplitter()->orientation(), Qt::Vertical);

    DockPanel *p7 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw7);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p4->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p5->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p6->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p7->parentSplitter()->orientation(), Qt::Horizontal);

    DockPanel *p8 = wd.addDockWidget(Qx::RightDockWidgetArea, dw8);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p4->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p5->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p6->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p7->parentSplitter()->orientation(), Qt::Horizontal);
    QCOMPARE(p8->parentSplitter()->orientation(), Qt::Horizontal);
}

void tst_DockPanel::centralWidget()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");

    DockPanel *p1 = wd.setCentralWidget(dw1);

    QCOMPARE(p1->containsCentralWidget(), true);
    QCOMPARE(p1->isCentralWidgetArea(), true);

    QCOMPARE(p1->isTopLevelArea(), true);
}

TEST_ADD(tst_DockPanel)

#include "tst_dockpanel.moc"
