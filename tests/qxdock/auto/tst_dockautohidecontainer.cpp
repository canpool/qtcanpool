#include "tst_global.h"

#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QCoreApplication>
#include <QPointer>

QX_DOCK_USE_NAMESPACE

class tst_DockAutoHideContainer : public QObject
{
    Q_OBJECT
private slots:
    void autoHideStaysPinnedOnAnEmptyWindow();
    void sizeObeysTheContentRectAndItsMargin();
};

/*!
 * Making a widget auto hidden on a window that has nothing else open used to
 * unpin it on the spot. DockPanel::insertDockWidget() toggled the not yet
 * visible auto hide panel open, DockWidgetPrivate::showDockWidget() read "auto
 * hide panel with nothing opened" as "unpin me" and called
 * moveContentsToParent(), and that deleted the auto hide container, docked the
 * widget like any other one and left the panel it had created behind empty in
 * the root splitter. Everything happens while the widget is being inserted, so
 * no shown window is needed - and a window whose only dock widgets are auto
 * hidden is precisely the state that triggers it.
 *
 * The order of the checks is deliberate: DockWidget::autoHideArea() dereferences
 * the auto hide container without checking it, so on the old code it crashed
 * rather than failed. Every check above it fails first, which keeps this a red
 * test instead of a segfault that would take the other test objects with it.
 */
void tst_DockAutoHideContainer::autoHideStaysPinnedOnAnEmptyWindow()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow window;
    window.resize(800, 600);

    DockWidget *widget = new DockWidget("widget");
    QPointer<DockAutoHideContainer> container = window.addAutoHideDockWidget(Qx::DockSideBarLeft, widget);
    QVERIFY(!container.isNull());

    // No ordinary dock panel may be left behind in the root splitter.
    QCOMPARE(window.dockPanelCount(), 0);
    QVERIFY(widget->dockPanel() != Q_NULLPTR);
    QCOMPARE(widget->dockPanel()->isAutoHide(), true);

    // The container is destroyed with deleteLater(), so it can still look alive
    // well after it has been thrown away.
    QCoreApplication::sendPostedEvents(Q_NULLPTR, QEvent::DeferredDelete);
    QVERIFY2(!container.isNull(), "the auto hide container was unpinned right after it was created");

    QCOMPARE(widget->isAutoHide(), true);
    QCOMPARE(int(widget->autoHideArea()), int(Qx::DockSideBarLeft));
    QCOMPARE(window.autoHideWidgets().count(), 1);
}

/*!
 * An auto hide container is sized from two limits that are computed rather than
 * stored, and neither is reachable from outside - DockResizeHandle exposes setters
 * for its minimum and maximum and no getters, and DockAutoHideContainer::updateSize()
 * is protected. So they are asserted through the one public door, setSize().
 *
 * The upper limit is the dock container's content rect minus the resize margin
 * (30), which is what keeps a sliver of the dock area visible; the same number is
 * what DockAutoHideContainerPrivate::updateResizeHandleSizeLimitMax() hands to the
 * resize handle, so this pins both at once. It is asserted as a relationship and
 * not as an absolute number on purpose: before the first layout the content rect
 * is not the window's size yet, and it is the relationship that carries the meaning.
 *
 * The lower limit is *not* applied here. Dragging is kept above 64 by the resize
 * handle, but setSize() writes the container's size directly, so a caller that
 * asks for 1 gets 1. That asymmetry is the point of the second half.
 */
void tst_DockAutoHideContainer::sizeObeysTheContentRectAndItsMargin()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow window;
    window.resize(800, 600);

    DockWidget *widget = new DockWidget("widget");
    QPointer<DockAutoHideContainer> container = window.addAutoHideDockWidget(Qx::DockSideBarLeft, widget);
    QVERIFY(!container.isNull());

    const int contentWidth = window.contentRect().width();
    QVERIFY2(contentWidth > 30, "the content rect must be wide enough for the margin to leave room");

    // Asking for more than the content rect allows is clamped to it minus the
    // margin, rather than obeyed.
    container->setSize(100000);
    QCOMPARE(container->width(), contentWidth - 30);

    // Asking for less than the resize handle's minimum is obeyed anyway: the
    // minimum guards a drag, not this call.
    container->setSize(1);
    QCOMPARE(container->width(), 1);
}

TEST_ADD(tst_DockAutoHideContainer)

#include "tst_dockautohidecontainer.moc"
