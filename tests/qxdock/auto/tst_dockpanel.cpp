#include "tst_global.h"

#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"
#include "qxdock/docksplitter.h"
#include "qxdock/docktitlebar.h"

QX_DOCK_USE_NAMESPACE

class tst_DockPanel : public QObject
{
    Q_OBJECT
private slots:
    void dockWidget();
    void splitter();
    void centralWidget();
    void topLevelArea();
    void minimumSizeHintKeepsTheTitleBar();
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
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);   // follow second
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Vertical);

    DockPanel *p3 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw3, p2);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p3->parentSplitter()->orientation(), Qt::Vertical);   // p2 == p3

    DockPanel *p4 = wd.addDockWidget(Qx::RightDockWidgetArea, dw4, p3);
    QCOMPARE(p1->parentSplitter()->orientation(), Qt::Vertical);
    QCOMPARE(p2->parentSplitter()->orientation(), Qt::Horizontal);   // p3 is added to p4 splitter
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

/*!
 * isTopLevelArea() is named after the place such a panel usually has, but what it
 * answers is narrower and worth spelling out: "am I the only visible dock panel in
 * my container". DockContainer::topLevelDockPanel() returns null unless exactly
 * one panel passes the isHidden() test, and DockPanel::isTopLevelArea() compares
 * itself against that. (An older note had it as rootSplitter() == this, which the
 * code never said; the mistake is what this case exists to keep from coming back.)
 *
 * Two consequences the name does not suggest, both asserted below: the answer can
 * change without the panel itself changing - hiding a neighbour is enough - and a
 * container with no registered panel has no top level area even when a panel is
 * on screen, which is exactly the situation of an auto hide panel.
 */
void tst_DockPanel::topLevelArea()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    {
        DockWindow wd;

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");

        DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
        DockPanel *p2 = wd.addDockWidget(Qx::RightDockWidgetArea, dw2);

        // Two visible panels: neither one is "the one and only", so neither is top
        // level, however central its position looks.
        QCOMPARE(wd.openedDockPanels().count(), 2);
        QCOMPARE(p1->isTopLevelArea(), false);
        QCOMPARE(p2->isTopLevelArea(), false);

        // Hiding the second panel's only widget leaves one visible panel, and the
        // survivor answers true from then on. p2 is deliberately not touched again:
        // hiding the last widget of a panel is allowed to remove the panel.
        dw2->toggleView(false);
        QCOMPARE(wd.openedDockPanels().count(), 1);
        QCOMPARE(wd.openedDockPanels().constFirst(), p1);
        QCOMPARE(p1->isTopLevelArea(), true);
    }

    {
        // The auto hide container's panel is created directly and never goes
        // through DockContainer::addDockPanel(), so it is not in the container's
        // panel list at all - the window reports no dock panel while that panel is
        // on screen. With nothing to compare against, the panel is not top level.
        DockWindow wd;

        DockWidget *dw = new DockWidget("dw");
        DockAutoHideContainer *container = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw);
        QVERIFY(container != Q_NULLPTR);

        QCOMPARE(wd.dockPanelCount(), 0);
        QVERIFY(dw->dockPanel() != Q_NULLPTR);
        QCOMPARE(dw->dockPanel()->isTopLevelArea(), false);
    }
}

/*!
 * The hint counts the title bar in when the panel has one. The question is
 * "does this panel have a title bar" - a matter of intent - so it is asked with
 * isHidden(), the way the rest of this library asks it (openedDockPanels(),
 * topLevelDockPanel(), the drop overlay). isVisible() would also go false for
 * the whole tree while the window has not been shown yet, and a layout that
 * happens to ask during that window would be handed a bound one title bar
 * short of what it has to place.
 *
 * The damage is smaller than the ribbon case was - the hint is asked again
 * rather than cached, so the bound corrects itself once the window is shown -
 * but this library has no demo and no golden image, so a wrong bound here is
 * not something a screenshot would have caught either way.
 */
void tst_DockPanel::minimumSizeHintKeepsTheTitleBar()
{
    DockWindow wd;

    DockWidget *dw = new DockWidget(QStringLiteral("dw"));
    QWidget *content = new QWidget();
    content->setMinimumSize(60, 40);
    dw->setWidget(content);

    DockPanel *p = wd.addDockWidget(Qx::LeftDockWidgetArea, dw);

    // The window is still hidden, so the whole tree reports isVisible() false
    // while the panel does have a title bar.
    QVERIFY(!wd.isVisible());
    QVERIFY(!p->titleBar()->isHidden());

    const int titleBarHeight = p->titleBar()->minimumSizeHint().height();
    QVERIFY(titleBarHeight > 0);

    const QSize whileHidden = p->minimumSizeHint();
    QCOMPARE(whileHidden.height(), dw->minimumSizeHint().height() + titleBarHeight);

    // Showing the window must not change the answer.
    wd.show();
    QCOMPARE(p->minimumSizeHint(), whileHidden);
}

TEST_ADD(tst_DockPanel)
#include "tst_dockpanel.moc"
