/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"

#include <QAction>
#include <QApplication>
#include <QKeyEvent>
#include <QSignalSpy>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonGallery : public QObject
{
    Q_OBJECT
private slots:
    void group();
    void keyboard();
    void geometry();
    void scrolling();
};

void tst_RibbonGallery::group()
{
    RibbonGallery gallery;

    QCOMPARE(gallery.currentViewGroup(), nullptr);

    RibbonGalleryGroup *g1 = gallery.addGalleryGroup();

    // The group the strip shows is the group itself - the gallery keeps no second
    // view of it, which is what used to make the two disagree.
    QCOMPARE(gallery.currentViewGroup(), g1);
    QCOMPARE(g1->itemCount(), 0);

    QList<QAction *> actions = {
        new QAction(tr("action1"), this),
        new QAction(tr("action2"), this),
        new QAction(tr("action3"), this),
        new QAction(tr("action4"), this),
    };
    RibbonGalleryGroup *g2 = gallery.addGalleryGroup(tr("group2"), actions);

    QCOMPARE(gallery.currentViewGroup(), g2);
    QCOMPARE(g2->itemCount(), 4);
    QCOMPARE(g2->itemAction(1), actions.at(1));
    QCOMPARE(g2->groupTitle(), tr("group2"));

    RibbonGalleryGroup *g3 = new RibbonGalleryGroup();
    gallery.addGalleryGroup(g3);

    QCOMPARE(gallery.currentViewGroup(), g3);

    gallery.setCurrentViewGroup(g2);
    QCOMPARE(gallery.currentViewGroup(), g2);
}

void tst_RibbonGallery::keyboard()
{
    RibbonGallery gallery;
    QList<QAction *> actions = {
        new QAction(tr("action1"), this),
        new QAction(tr("action2"), this),
        new QAction(tr("action3"), this),
    };
    RibbonGalleryGroup *view = gallery.addGalleryGroup(tr("group"), actions);
    QVERIFY(view);
    QCOMPARE(view->itemCount(), 3);

    auto pressKey = [&gallery](int key) {
        QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
        QApplication::sendEvent(&gallery, &event);
    };

    // With nothing selected yet, the first Down selects the first one, the second moves on.
    pressKey(Qt::Key_Down);
    QCOMPARE(view->selectedIndex(), 0);
    pressKey(Qt::Key_Down);
    QCOMPARE(view->selectedIndex(), 1);

    // Enter activates the selected item, exactly as a click would.
    QSignalSpy triggered(actions.at(1), &QAction::triggered);
    pressKey(Qt::Key_Return);
    QCOMPARE(triggered.count(), 1);

    // The movement clamps at both ends instead of wrapping or falling off the group.
    pressKey(Qt::Key_Down);
    QCOMPARE(view->selectedIndex(), 2);
    pressKey(Qt::Key_Down);
    QCOMPARE(view->selectedIndex(), 2);
    pressKey(Qt::Key_Up);
    QCOMPARE(view->selectedIndex(), 1);

    // PageUp / PageDown are the keyboard form of the two scroll buttons; they must not
    // change the selected item.
    pressKey(Qt::Key_PageDown);
    pressKey(Qt::Key_PageUp);
    QCOMPARE(view->selectedIndex(), 1);
}

void tst_RibbonGallery::geometry()
{
    RibbonGalleryGroup group;
    QList<QAction *> actions;
    for (int i = 0; i < 6; ++i) {
        actions << new QAction(tr("action%1").arg(i), &group);
    }
    group.addItems(actions);
    group.resize(300, 80);
    group.show();

    QVERIFY(group.columnCount() >= 1);
    QVERIFY(group.rowCount() >= 1);

    // An item's rectangle is the grid's and the grid knows nothing about the state:
    // selecting one cannot move it, which is what made the strip jump under the
    // pointer when the sheet gave the hovered state a border of its own.
    QList<QRect> before;
    for (int i = 0; i < group.itemCount(); ++i) {
        before << group.itemRect(i);
    }
    group.selectByIndex(2);
    QList<QRect> after;
    for (int i = 0; i < group.itemCount(); ++i) {
        after << group.itemRect(i);
    }
    QCOMPARE(before, after);

    // The grid is square-ish and bounded by what the caller asked for. Its height
    // is what is left between the spacings rather than the whole height: a grid
    // that is merely height/rows tall runs the last row past the bottom edge, and
    // the bottom line of those items is then never painted.
    // Wide enough for the six of them to sit on one row: the grid is sized for the
    // rows on show, and rows beyond those are reached by scrolling, so the fit is
    // asserted for a strip that really is one row deep.
    group.resize(600, 80);
    group.setGridMinimumWidth(40);
    group.recalcGridSize(80);
    QVERIFY(group.gridSize().width() >= 40);
    QCOMPARE(group.gridSize().height(), 80 - 2 * group.spacing());
    QCOMPARE(group.rowCount(), 1);
    for (int i = 0; i < group.itemCount(); ++i) {
        QVERIFY(group.itemRect(i).bottom() < group.height());
    }
}

void tst_RibbonGallery::scrolling()
{
    RibbonGallery gallery;
    QList<QAction *> actions;
    for (int i = 0; i < 30; ++i) {
        actions << new QAction(tr("action%1").arg(i), &gallery);
    }
    RibbonGalleryGroup *group = gallery.addGalleryGroup(tr("group"), actions);
    gallery.resize(400, 90);
    gallery.show();

    // One row on show means the rest are reached by scrolling, and the range stops
    // where the last row is visible instead of running off the end.
    QCOMPARE(group->viewportRows(), 1);
    QVERIFY(group->rowCount() > 1);
    QCOMPARE(group->scrollRow(), 0);

    gallery.pageDown();
    QCOMPARE(group->scrollRow(), 1);
    gallery.pageUp();
    QCOMPARE(group->scrollRow(), 0);
    // Already at the top, and a scroll button must not walk backwards out of it.
    gallery.pageUp();
    QCOMPARE(group->scrollRow(), 0);

    group->selectByIndex(group->itemCount() - 1);
    QCOMPARE(group->scrollRow(), group->maxScrollRow());

    // A group standing on its own shows every row and has nothing to scroll.
    RibbonGalleryGroup alone;
    alone.addItems(actions);
    alone.resize(300, 200);
    alone.show();
    QCOMPARE(alone.viewportRows(), 0);
    QCOMPARE(alone.maxScrollRow(), 0);
}

TEST_ADD(tst_RibbonGallery)

#include "tst_ribbongallery.moc"
