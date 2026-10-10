#include "tst_global.h"

#include "qxdock/dockpanel.h"
#include "qxdock/docktab.h"
#include "qxdock/docktabbar.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QScrollBar>

QX_DOCK_USE_NAMESPACE

namespace
{

/*
 * Builds a dock window whose tab bar is guaranteed to overflow: six dock
 * widgets with a wide minimum size in a narrow window. Overflow is what makes
 * "scroll the current tab into view" observable at all, and relying on the
 * natural width of the titles would leave that up to the font.
 */
DockWindow *makeCrowdedWindow(DockPanel **panelOut, DockTabBar **tabBarOut)
{
    auto *wd = new DockWindow;
    DockPanel *panel = nullptr;
    for (int i = 0; i < 6; ++i) {
        auto *dw = new DockWidget(QString("dock widget %1").arg(i));
        panel = wd->addDockWidget(Qx::CenterDockWidgetArea, dw, panel);
    }

    auto *tabBar = panel->findChild<DockTabBar *>();
    Q_ASSERT(tabBar);
    for (int i = 0; i < tabBar->count(); ++i) {
        tabBar->tab(i)->setMinimumWidth(200);
    }

    wd->resize(320, 240);
    wd->show();
    QTest::qWait(50);

    if (panelOut) {
        *panelOut = panel;
    }
    if (tabBarOut) {
        *tabBarOut = tabBar;
    }
    return wd;
}

/* Position of the tab inside the scroll area's viewport. */
QPoint tabPosInViewport(DockTabBar *tabBar, int index)
{
    return tabBar->tab(index)->mapTo(tabBar->viewport(), QPoint(0, 0));
}

}   // namespace

class tst_DockTabBar : public QObject
{
    Q_OBJECT
private slots:
    void scrollsCurrentTabIntoView();
    void scrollSurvivesTabDeletion();
};

/*
 * Switching to a tab that sits outside the visible area has to scroll it back
 * into view - that is the feature issue #520 asked for. It is deferred to the
 * next event loop pass, so the assertion has to let the event loop turn.
 */
void tst_DockTabBar::scrollsCurrentTabIntoView()
{
    DockTabBar *tabBar = nullptr;
    QScopedPointer<DockWindow> wd(makeCrowdedWindow(nullptr, &tabBar));
    QVERIFY(tabBar);
    QVERIFY(tabBar->count() > 1);
    QVERIFY(tabBar->areTabsOverflowing());

    const int last = tabBar->count() - 1;

    // Scrolling to the end: the last tab has to be fully inside the viewport.
    tabBar->setCurrentIndex(last);
    QTest::qWait(50);
    QCOMPARE(tabBar->currentIndex(), last);
    const QPoint tail = tabPosInViewport(tabBar, last);
    QVERIFY2(tail.x() >= 0, "the active tab is scrolled off to the left");
    QVERIFY2(tail.x() + tabBar->tab(last)->width() <= tabBar->viewport()->width(),
             "the active tab is not fully inside the viewport");

    // ... and back to the beginning.
    tabBar->setCurrentIndex(0);
    QTest::qWait(50);
    QCOMPARE(tabBar->currentIndex(), 0);
    QCOMPARE(tabPosInViewport(tabBar, 0).x(), 0);
}

/*
 * Regression test for the block that used to be commented out in
 * DockTabBarPrivate::updateTabs().
 *
 * It deferred the scroll with a lambda that captured the tab bar *by
 * reference*, so the lambda body read a stack slot that no longer held the tab
 * bar by the time the event loop ran - and the program went down with it.
 * A second hazard sits next to it: the tab itself is a raw pointer, and the
 * tab (and its dock widget) can be destroyed before the deferred call is
 * delivered.
 *
 * So both have to survive the event loop turn: the deferred call may only be
 * delivered when its tab bar is still alive, and it may only touch a tab that
 * is still alive.
 */
void tst_DockTabBar::scrollSurvivesTabDeletion()
{
    DockTabBar *tabBar = nullptr;
    QScopedPointer<DockWindow> wd(makeCrowdedWindow(nullptr, &tabBar));
    QVERIFY(tabBar);
    QVERIFY(tabBar->count() > 1);

    const int last = tabBar->count() - 1;
    const int count = tabBar->count();

    // Queues the deferred "scroll into view" for the last tab.
    tabBar->setCurrentIndex(last);
    QCOMPARE(tabBar->currentIndex(), last);

    // The tab goes away before the event loop gets its turn. The tab object is
    // a child of its dock widget, so deleting the widget deletes the tab.
    DockWidget *victim = tabBar->tab(last)->dockWidget();
    QVERIFY(victim);
    victim->deleteDockWidget();

    // Both the deferred scroll and the deferred deletion are delivered here.
    // Under the old code this is where the process died.
    QTest::qWait(50);

    QCOMPARE(tabBar->count(), count - 1);
    QVERIFY(tabBar->currentTab() != nullptr);
    QVERIFY(tabBar->currentTab()->dockWidget() != victim);
}

TEST_ADD(tst_DockTabBar)

#include "tst_docktabbar.moc"
