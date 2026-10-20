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
#include <QScreen>
#include <QScrollArea>
#include <QSignalSpy>

QX_RIBBON_USE_NAMESPACE

namespace
{
/*! The popup the gallery manages, as a widget - it is not part of the public API. */
QWidget *galleryPopup(RibbonGallery *gallery)
{
    const QList<QWidget *> children = gallery->findChildren<QWidget *>();
    for (QWidget *w : children) {
        if (w->windowFlags() & Qt::Popup) {
            return w;
        }
    }
    return nullptr;
}

/*!
 * The previews inside \a popup, in the order the groups were added.
 *
 * RibbonGalleryPreview has no Q_OBJECT, so it has no class name to look for; the
 * scroll area's content is where the previews (and the group titles) live, and a
 * title is the one thing there that is a QLabel.
 */
QList<QWidget *> popupPreviews(QWidget *popup)
{
    QList<QWidget *> previews;
    if (!popup) {
        return previews;
    }
    for (QScrollArea *area : popup->findChildren<QScrollArea *>()) {
        QWidget *content = area->widget();
        if (!content) {
            continue;
        }
        for (QWidget *w : content->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
            if (!w->inherits("QLabel")) {
                previews.append(w);
            }
        }
    }
    return previews;
}

QString rectText(const QRect &r)
{
    return QStringLiteral("%1,%2 %3x%4").arg(r.x()).arg(r.y()).arg(r.width()).arg(r.height());
}
}   // namespace

class tst_RibbonGallery : public QObject
{
    Q_OBJECT
private slots:
    void group();
    void keyboard();
    void geometry();
    void scrolling();
    void popupItemsKeepTheirSize();
    void popupStaysOnScreen();
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

/*
 * The popup's items are the strip's items, whatever the strip is showing.
 *
 * A group that asks for three display rows divides the strip's height by three,
 * so its own grid is a third as tall. The popup used to take that grid as the
 * size of an item for every group it showed, which made the whole popup shrink a
 * step each time an item in another group was picked - the strip follows the
 * choice, so the group it followed answered with a smaller grid. The popup is
 * laid out by its own width, and its items are one row of the strip's worth big.
 */
void tst_RibbonGallery::popupItemsKeepTheirSize()
{
    RibbonGallery gallery;
    QList<QAction *> oneRow;
    QList<QAction *> threeRows;
    for (int i = 0; i < 30; ++i) {
        oneRow << new QAction(tr("one%1").arg(i), &gallery);
        threeRows << new QAction(tr("three%1").arg(i), &gallery);
    }
    RibbonGalleryGroup *one = gallery.addGalleryGroup(tr("one"), oneRow);
    RibbonGalleryGroup *three = gallery.addGalleryGroup(tr("three"), threeRows);
    three->setGalleryGroupStyle(RibbonGalleryGroup::IconOnly);
    three->setDisplayRow(RibbonGalleryGroup::DisplayThreeRow);
    gallery.setCurrentViewGroup(one);

    // The width is chosen so that five pixels decide a column: the height a group
    // needs depends on how many columns its width buys, and the old measurement
    // guessed a width five pixels short of the one the layout then gave the
    // preview. Only near a column boundary does that guess cost a whole row, which
    // is the band of empty surface this asserts is not there.
    gallery.resize(577, 64);
    gallery.show();
    QCoreApplication::processEvents();

    gallery.showMoreDetail();
    QCoreApplication::processEvents();
    QWidget *popup = galleryPopup(&gallery);
    QVERIFY(popup);
    QVERIFY(popup->isVisible());

    const QList<QWidget *> before = popupPreviews(popup);
    QCOMPARE(before.count(), 2);
    QList<int> heights;
    for (QWidget *preview : before) {
        heights << preview->height();
    }
    QVERIFY(heights.at(0) > 0);

    // And no row of empty space under the group: the height is what its items
    // need at the width the preview actually has, not at a width guessed before
    // the layout ran. A guess one column too narrow is one row too much, which is
    // a band of empty surface under the last group.
    const int spacing = one->spacing();
    const int gridHeight = gallery.contentsRect().height() - 2 * spacing;
    const int gridWidth = qMax(gridHeight, one->gridMinimumWidth());
    const int columns = qMax(1, (before.at(0)->width() + spacing) / (gridWidth + spacing));
    const int rows = (one->itemCount() + columns - 1) / columns;
    const int needed = rows * (gridHeight + spacing) - spacing + 2 * spacing;
    QCOMPARE(before.at(0)->height(), needed);
    popup->hide();

    // The strip now shows the three-row group, so its own items are smaller -
    // that is what DisplayRow asks for, and the assertion is the premise of this
    // test rather than a restatement of the fix.
    gallery.setCurrentViewGroup(three);
    QCoreApplication::processEvents();
    QVERIFY2(three->gridSize().height() < one->gridSize().height(),
             qPrintable(QStringLiteral("strip grids: one=%1 three=%2")
                            .arg(one->gridSize().height())
                            .arg(three->gridSize().height())));

    gallery.showMoreDetail();
    QCoreApplication::processEvents();
    const QList<QWidget *> after = popupPreviews(popup);
    QCOMPARE(after.count(), 2);
    for (int i = 0; i < after.count() && i < heights.count(); ++i) {
        QCOMPARE(after.at(i)->height(), heights.at(i));
    }
}

/*
 * The popup opens under the strip only when it fits there: against the bottom of
 * the screen it has to open upwards, and against the right edge it has to stay
 * on the screen sideways. It used to be placed at the strip's left edge and under
 * it unconditionally, with no answer for either edge - when it flipped up it
 * flipped by its whole height from the strip's bottom, which leaves the popup
 * hanging off the top of the screen whenever the window sits low.
 */
void tst_RibbonGallery::popupStaysOnScreen()
{
    RibbonGallery gallery;
    QList<QAction *> actions;
    for (int i = 0; i < 60; ++i) {
        actions << new QAction(tr("action%1").arg(i), &gallery);
    }
    RibbonGalleryGroup *group = gallery.addGalleryGroup(tr("group"), actions);
    QVERIFY(group);
    QScreen *screen = gallery.screen();
    QVERIFY(screen);
    const QRect avail = screen->availableGeometry();
    gallery.resize(qMin(400, qMax(120, avail.width() - 20)), 90);
    gallery.show();
    QCoreApplication::processEvents();

    // Against the bottom edge: there is no room below, and the popup is taller
    // than what is left of the screen, so it has to end up above the strip.
    gallery.move(avail.left() + 10, avail.bottom() - gallery.height() + 1);
    QCoreApplication::processEvents();
    gallery.showMoreDetail();
    QCoreApplication::processEvents();
    QWidget *popup = galleryPopup(&gallery);
    QVERIFY(popup);
    QVERIFY2(avail.contains(popup->geometry()),
             qPrintable(
                 QStringLiteral("bottom edge: popup %1, screen %2").arg(rectText(popup->geometry()), rectText(avail))));
    QVERIFY(popup->geometry().bottom() <= gallery.mapToGlobal(QPoint(0, 0)).y());
    popup->hide();

    // Against the right edge: the width comes from the gallery, which is nearly
    // as wide as the window, so it has to be moved back onto the screen.
    gallery.move(avail.right() - gallery.width() + 1, avail.top() + 10);
    QCoreApplication::processEvents();
    gallery.showMoreDetail();
    QCoreApplication::processEvents();
    QVERIFY2(avail.contains(popup->geometry()),
             qPrintable(
                 QStringLiteral("right edge: popup %1, screen %2").arg(rectText(popup->geometry()), rectText(avail))));
}

TEST_ADD(tst_RibbonGallery)

#include "tst_ribbongallery.moc"
