/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialbreadcrumbs.h"

#include "qxcontrols/qxcontrol.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! A path of three steps, short enough that every bar shows it whole. */
QStringList shortPath()
{
    return QStringList() << QStringLiteral("Home") << QStringLiteral("Projects") << QStringLiteral("Qx");
}

/*! A path of ten steps: longer than the eight a bar shows before folding. */
QStringList longPath()
{
    return QStringList() << QStringLiteral("Home") << QStringLiteral("Library") << QStringLiteral("Projects")
                         << QStringLiteral("qtcanpool") << QStringLiteral("src") << QStringLiteral("libs")
                         << QStringLiteral("qxmaterial") << QStringLiteral("widgets") << QStringLiteral("Tests")
                         << QStringLiteral("Settings");
}

/*! The places the bar is showing, in reading order. A place that is folded away
 *  is hidden rather than taken apart, so the pool is read through what is on
 *  show. */
QVector<QxControls::QxControl *> places(MaterialBreadcrumbs *bar)
{
    QVector<QxControls::QxControl *> shown;
    const QList<QxControls::QxControl *> all = bar->findChildren<QxControls::QxControl *>();
    for (QxControls::QxControl *place : all) {
        if (!place->isHidden()) {
            shown.append(place);
        }
    }
    return shown;
}

/*! How far apart two colours are, as a reader sees them. */
int distance(const QColor &one, const QColor &other)
{
    return qAbs(one.red() - other.red()) + qAbs(one.green() - other.green()) + qAbs(one.blue() - other.blue());
}

/*! The darkest pixel of \a room: which colour a step is written in is read from
 *  the ink, and the darkest pixel is the ink rather than the anti-aliasing
 *  round it. */
QColor darkestPixel(const QImage &image, const QRect &room)
{
    QColor darkest(Qt::white);
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (pixel.lightness() < darkest.lightness()) {
                darkest = pixel;
            }
        }
    }
    return darkest;
}

/*! How many pixels of \a room are not \a background, allowance made for the one
 *  or two of anti-aliasing that are almost it. Written as a difference rather
 *  than as "darker than", because a bar in a dark theme is ink on dark and the
 *  comparison that says "there is something here" has to hold in both. */
int pixelsApartFrom(const QImage &image, const QRect &room, const QColor &background, int slack)
{
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            if (distance(image.pixelColor(x, y), background) > slack) {
                ++count;
            }
        }
    }
    return count;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(800, 400);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}

/*! A bar showing \a path, moved off the page's own corner - where the offscreen
 *  platform keeps its pointer, so a bar built at (0, 0) has a step hovered
 *  before any test asks for one. */
MaterialBreadcrumbs *makeBar(QWidget *page, const QStringList &path)
{
    auto *bar = new MaterialBreadcrumbs(page);
    bar->setItems(path);
    const QSize hint = bar->sizeHint();
    bar->resize(hint);
    bar->move(0, 60);
    bar->show();
    QTest::qWaitForWindowExposed(bar);
    return bar;
}
}   // namespace

class tst_MaterialBreadcrumbs : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theStepsStandInTheOrderTheyWereGiven();
    void theBarAsksForTheWidthItsPathNeeds();
    void theStepsAreTheTabStops();
    void whereTheUserIsIsNotAWayBack();
    void aStepOfThePathIsAWayBack();
    void theWaysBackWearTheAccent();
    void theSeparatorStandsBetweenTheSteps();
    void theFoldKeepsBothEndsOfThePath();
    void unrollingShowsEveryStep();
    void unrollingHappensOnce();
    void theKeyboardLandsOnTheRevealedStep();
    void aPathThatFitsIsNeverFolded();
    void aFoldThatWouldHideNothingIsNotAFold();
    void whereTheUserIsIsNeverFoldedAway();
    void aNewPathIsShownFolded();
    void theBarAnswersForItsPath();
};

void tst_MaterialBreadcrumbs::initTestCase()
{
    qtcGiveFonts();
    QxTheme::QxThemeManager::instance();
}

/*! Three steps, left to right, in the order the path was given in - and each of
 *  them as tall as the row, so the whole step is a target rather than its
 *  words. */
void tst_MaterialBreadcrumbs::theStepsStandInTheOrderTheyWereGiven()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());

    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(shown.count(), 3);
    QCOMPARE(shown.at(0)->accessibleName(), QStringLiteral("Home"));
    QCOMPARE(shown.at(1)->accessibleName(), QStringLiteral("Projects"));
    QCOMPARE(shown.at(2)->accessibleName(), QStringLiteral("Qx"));

    for (int i = 0; i < shown.count(); ++i) {
        QCOMPARE(shown.at(i)->height(), bar->height());
        if (i > 0) {
            // Left to right, with the separator's room between them.
            QVERIFY(shown.at(i)->x() > shown.at(i - 1)->x() + shown.at(i - 1)->width());
        }
    }
}

/*! The width it asks for is the path it shows: the last step ends where the
 *  hint ends, so a layout gives the bar the room its steps need and no more. */
void tst_MaterialBreadcrumbs::theBarAsksForTheWidthItsPathNeeds()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());

    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(bar->sizeHint().width(), shown.last()->geometry().right() + 1);
    // A step is not shortened to fit: the least room it takes is the room it
    // asks for.
    QCOMPARE(bar->minimumSizeHint(), bar->sizeHint());
}

/*! The keyboard walks the ways back; where the user is is not a stop, because
 *  there is nowhere to go from it. */
void tst_MaterialBreadcrumbs::theStepsAreTheTabStops()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());

    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(shown.at(0)->focusPolicy(), Qt::TabFocus);
    QCOMPARE(shown.at(1)->focusPolicy(), Qt::TabFocus);
    QCOMPARE(shown.at(2)->focusPolicy(), Qt::NoFocus);
}

/*! Choosing the last step is not a move: pressing it answers nothing, and it is
 *  not a link to an assistive tool either. */
void tst_MaterialBreadcrumbs::whereTheUserIsIsNotAWayBack()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());
    const QVector<QxControls::QxControl *> shown = places(bar);

    QSignalSpy spy(bar, &MaterialBreadcrumbs::itemClicked);
    QTest::mouseClick(shown.at(2), Qt::LeftButton);
    QTest::keyClick(shown.at(2), Qt::Key_Return);
    QCOMPARE(spy.count(), 0);

    QAccessibleInterface *here = QAccessible::queryAccessibleInterface(shown.at(2));
    QVERIFY(here);
    QCOMPARE(here->role(), QAccessible::StaticText);

    QAccessibleInterface *back = QAccessible::queryAccessibleInterface(shown.at(1));
    QVERIFY(back);
    QCOMPARE(back->role(), QAccessible::Link);
}

/*! A step before the last is a way back, by the pointer and by the keyboard. */
void tst_MaterialBreadcrumbs::aStepOfThePathIsAWayBack()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());
    const QVector<QxControls::QxControl *> shown = places(bar);

    QSignalSpy spy(bar, &MaterialBreadcrumbs::itemClicked);
    QTest::mouseClick(shown.at(0), Qt::LeftButton);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
    QCOMPARE(spy.at(0).at(1).toString(), QStringLiteral("Home"));

    shown.at(1)->setFocus();
    QVERIFY(shown.at(1)->hasFocus());
    QTest::keyClick(shown.at(1), Qt::Key_Return);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), 1);
    QCOMPARE(spy.at(1).at(1).toString(), QStringLiteral("Projects"));

    // Space answers on release, the way a button does.
    QTest::keyClick(shown.at(1), Qt::Key_Space);
    QCOMPARE(spy.count(), 3);
}

/*! Material says a link is the accent colour, so a way back is accent ink and
 *  the step the user is on is the page's own ink. */
void tst_MaterialBreadcrumbs::theWaysBackWearTheAccent()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());
    const QVector<QxControls::QxControl *> shown = places(bar);

    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor ink = bar->palette().color(QPalette::WindowText);
    const QImage frame = bar->grab().toImage();
    const QColor back = darkestPixel(frame, shown.at(0)->geometry());
    const QColor here = darkestPixel(frame, shown.at(2)->geometry());

    QVERIFY2(distance(back, accent) < distance(here, accent), qPrintable(QStringLiteral("the way back is not accent")));
    QVERIFY2(distance(here, ink) < distance(back, ink), qPrintable(QStringLiteral("where the user is is not ink")));
}

/*! The separator is the room between two steps, drawn: it belongs to neither of
 *  them, and it is in that room rather than inside either step. */
void tst_MaterialBreadcrumbs::theSeparatorStandsBetweenTheSteps()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());
    const QVector<QxControls::QxControl *> shown = places(bar);

    const QRect left = shown.at(0)->geometry();
    const QRect right = shown.at(1)->geometry();
    const QRect room(left.right() + 1, left.top(), right.left() - left.right() - 1, left.height());
    QVERIFY(room.width() > 0);

    const QImage frame = bar->grab().toImage();
    // The bar's own corner is padding inside the first step, so it is what the
    // row is made of - and the separator is the ink that is not it.
    const QColor blank = frame.pixelColor(0, 0);
    QVERIFY(pixelsApartFrom(frame, room, blank, 40) > 0);
}

/*! A path longer than the bar shows keeps both ends and folds the middle: the
 *  first step, the mark, and where the user is. */
void tst_MaterialBreadcrumbs::theFoldKeepsBothEndsOfThePath()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(shown.count(), 3);
    QCOMPARE(shown.at(0)->accessibleName(), QStringLiteral("Home"));
    QCOMPARE(shown.at(2)->accessibleName(), QStringLiteral("Settings"));

    // The middle is the mark, and what it does is unroll the path.
    QAccessibleInterface *mark = QAccessible::queryAccessibleInterface(shown.at(1));
    QVERIFY(mark);
    QCOMPARE(mark->role(), QAccessible::Button);
    QCOMPARE(shown.at(1)->accessibleName(), QStringLiteral("Show path"));
    QCOMPARE(shown.at(1)->focusPolicy(), Qt::TabFocus);

    // A folded path is not one the user unrolled.
    QVERIFY(!bar->isExpanded());
}

/*! Pressing the mark shows every step, in the order the path was given in, and
 *  says so. */
void tst_MaterialBreadcrumbs::unrollingShowsEveryStep()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    QSignalSpy spy(bar, &MaterialBreadcrumbs::expanded);
    QTest::mouseClick(places(bar).at(1), Qt::LeftButton);

    QCOMPARE(spy.count(), 1);
    QVERIFY(bar->isExpanded());
    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(shown.count(), longPath().count());
    for (int i = 0; i < shown.count(); ++i) {
        QCOMPARE(shown.at(i)->accessibleName(), longPath().at(i));
    }
    // Nothing is left to unroll, so no mark is left either.
    for (QxControls::QxControl *place : shown) {
        QVERIFY(place->accessibleName() != QStringLiteral("Show path"));
    }
}

/*! Unrolling happens once, and folding back is the bar's answer to being asked
 *  again: it does not do either by itself. */
void tst_MaterialBreadcrumbs::unrollingHappensOnce()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    QSignalSpy spy(bar, &MaterialBreadcrumbs::expanded);
    bar->expand();
    QCOMPARE(spy.count(), 1);
    bar->expand();
    QCOMPARE(spy.count(), 1);

    // A path the user unrolled is one they chose to see: it stays unrolled.
    QTest::qWait(250);
    QCOMPARE(places(bar).count(), longPath().count());

    bar->collapse();
    QVERIFY(!bar->isExpanded());
    QCOMPARE(places(bar).count(), 3);
    // The fold came back as a fold, not as an unrolling.
    QCOMPARE(spy.count(), 1);
}

/*! Unrolling from the keyboard does not lose the place: the focus goes to the
 *  first step the fold was hiding, because that is what was asked for. */
void tst_MaterialBreadcrumbs::theKeyboardLandsOnTheRevealedStep()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    QxControls::QxControl *mark = places(bar).at(1);
    mark->setFocus();
    QVERIFY(mark->hasFocus());
    QTest::keyClick(mark, Qt::Key_Return);

    QCOMPARE(places(bar).count(), longPath().count());
    QWidget *landed = bar->focusWidget();
    QVERIFY(landed);
    // One step before the fold, and the first one the mark was standing for.
    QCOMPARE(landed->accessibleName(), longPath().at(bar->itemsBeforeCollapse()));
}

/*! A path that fits is a path shown whole - and a bar told never to fold shows
 *  one that does not fit. */
void tst_MaterialBreadcrumbs::aPathThatFitsIsNeverFolded()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    bar->setMaxItems(longPath().count());
    QCOMPARE(places(bar).count(), longPath().count());

    bar->setMaxItems(0);
    QCOMPARE(places(bar).count(), longPath().count());
    QVERIFY(!bar->isExpanded());
}

/*! A fold that would leave nothing out is not a fold: the steps either side of
 *  it are the whole path. */
void tst_MaterialBreadcrumbs::aFoldThatWouldHideNothingIsNotAFold()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    bar->setMaxItems(2);
    bar->setItemsBeforeCollapse(3);
    bar->setItemsAfterCollapse(3);
    // Six steps kept either side of nothing, out of ten.
    bar->setItems(longPath().mid(0, 4));
    QCOMPARE(places(bar).count(), 4);
}

/*! The step the user is on is the reason the bar says anything: it is never
 *  folded away, so a tail of none is read as a tail of one. */
void tst_MaterialBreadcrumbs::whereTheUserIsIsNeverFoldedAway()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    bar->setMaxItems(2);
    bar->setItemsBeforeCollapse(1);
    bar->setItemsAfterCollapse(0);

    const QVector<QxControls::QxControl *> shown = places(bar);
    QCOMPARE(shown.count(), 3);
    QCOMPARE(shown.at(0)->accessibleName(), QStringLiteral("Home"));
    QCOMPARE(shown.at(2)->accessibleName(), QStringLiteral("Settings"));
}

/*! A new path is not the old one: it is shown folded, whatever state the old one
 *  was left in. */
void tst_MaterialBreadcrumbs::aNewPathIsShownFolded()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, longPath());

    bar->expand();
    QCOMPARE(places(bar).count(), longPath().count());

    bar->setItems(QStringList() << shortPath() << QStringLiteral("Leaf"));
    QVERIFY(!bar->isExpanded());
    QCOMPARE(places(bar).count(), shortPath().count() + 1);
}

/*! To an assistive tool the path is a list, and what it reads off it is the path
 *  in one sentence. */
void tst_MaterialBreadcrumbs::theBarAnswersForItsPath()
{
    QWidget *page = makePage();
    MaterialBreadcrumbs *bar = makeBar(page, shortPath());

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(bar);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::List);
    QCOMPARE(bar->accessibleName(), QStringLiteral("Home / Projects / Qx"));
}

TEST_ADD(tst_MaterialBreadcrumbs)

#include "tst_materialbreadcrumbs.moc"
