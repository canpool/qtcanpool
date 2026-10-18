/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockiconprovider.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QAbstractButton>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPointer>

QX_DOCK_USE_NAMESPACE

class tst_DockAutoHideContainer : public QObject
{
    Q_OBJECT
private slots:
    void autoHideStaysPinnedOnAnEmptyWindow();
    void sizeObeysTheContentRectAndItsMargin();
    void collapseSlidesIntoPlace();
    void glyphsFollowTheSurfaceOnShow();
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

/*!
 * The slide is opt-in, and that is the first thing pinned here: without the flag
 * an auto hide container is in place the moment it is asked to open, which is
 * what every layout built on this library already relies on. With the flag it
 * travels there instead - and, crucially, ends up in exactly the same place, so
 * the flag decides how a container arrives and not where it belongs.
 *
 * The positions are compared rather than the animation, because the animation is
 * the thing that cannot be trusted to be observable: what matters is that the
 * container leaves its side bar and stops where it always did.
 */
void tst_DockAutoHideContainer::collapseSlidesIntoPlace()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow window;
    window.resize(800, 600);

    DockWidget *widget = new DockWidget("widget");
    DockAutoHideContainer *container = window.addAutoHideDockWidget(Qx::DockSideBarRight, widget);
    QVERIFY(container != Q_NULLPTR);

    // Undisturbed, this is where the container belongs.
    container->collapseView(false);
    const QRect place = container->geometry();

    // Without the flag there is nothing to wait for.
    container->collapseView(true);
    QCOMPARE(container->isHidden(), true);
    QCOMPARE(container->geometry(), place);

    container->collapseView(false);
    QCOMPARE(container->isHidden(), false);
    QCOMPARE(container->geometry(), place);

    // With the flag on it comes from the side bar it belongs to: right after the
    // call it is still out of its place and off the window, and once the slide is
    // over it is where it belongs.
    DockManager::setAutoHideConfigFlag(DockManager::AutoHideAnimationEnabled, true);

    container->collapseView(true);
    QCOMPARE(container->isHidden(), true);

    container->collapseView(false);
    QCOMPARE(container->isHidden(), false);
    QVERIFY(container->geometry() != place);
    QVERIFY(container->geometry().x() > place.x());

    QTest::qWait(3 * 200);
    QCOMPARE(container->geometry(), place);

    // Collapsing does not leave the slide running behind a hidden widget.
    container->collapseView(true);
    QCOMPARE(container->isHidden(), true);

    // And with the flag off again there is nothing left of it.
    DockManager::setAutoHideConfigFlag(DockManager::AutoHideAnimationEnabled, false);
    container->collapseView(false);
    QCOMPARE(container->geometry(), place);
}

/*!
 * The title bar's buttons are built before anything knows this container will
 * sit on the accent fill, so they carry the surface text colour - black on the
 * accent in a light scheme, which is exactly where a black glyph disappears.
 * Showing the container re-applies the icons beneath it, and the glyph a
 * caller configured for the accent fill is what comes out.
 */
void tst_DockAutoHideContainer::glyphsFollowTheSurfaceOnShow()
{
    // A colour nothing else in the test uses, so a pixel carrying it is the
    // glyph, not the fill behind it.
    DockStyleColors colors;
    colors.onAccent = QColor(9, 8, 7);
    applyDockIcons(colors);

    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    DockWindow window;
    window.resize(800, 600);
    DockWidget *widget = new DockWidget(QStringLiteral("Glyphs"));
    DockAutoHideContainer *container = window.addAutoHideDockWidget(Qx::DockSideBarLeft, widget);
    QVERIFY(container != Q_NULLPTR);
    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));
    // A fresh auto hide panel starts collapsed; opening it is the Show this
    // test is about.
    container->collapseView(false);

    // The most opaque pixel over every dock button: a glyph is a stroke, so the
    // comparison is against the colour with the best coverage, with a tolerance
    // for antialiasing.
    QColor glyph;
    int best = 0;
    const QList<QAbstractButton *> buttons = window.findChildren<QAbstractButton *>();
    for (const QAbstractButton *button : buttons) {
        const QImage image = button->icon().pixmap(QSize(16, 16)).toImage();
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                const QColor pixel = image.pixelColor(x, y);
                if (pixel.alpha() > best) {
                    best = pixel.alpha();
                    glyph = pixel;
                }
            }
        }
    }
    QVERIFY(best > 200);
    QVERIFY2(qAbs(glyph.red() - 9) <= 3 && qAbs(glyph.green() - 8) <= 3 && qAbs(glyph.blue() - 7) <= 3,
             qPrintable(QStringLiteral("glyph is %1").arg(glyph.name())));

    // Leave the provider and the cached variants as the other tests found them.
    applyDockIcons(QGuiApplication::palette());
}

TEST_ADD(tst_DockAutoHideContainer)

#include "tst_dockautohidecontainer.moc"
