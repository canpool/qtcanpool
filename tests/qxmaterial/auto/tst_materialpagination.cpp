/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialpagination.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QEnterEvent>
#endif
#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The bar's own measures, read the way a reader reads them.
constexpr int kItemSize = 32;
constexpr int kCount = 10;

/*! How many pixels of \a room are the accent itself, allowance made for the
 *  one or two of anti-aliasing either side of it. */
int accentPixels(const QImage &image, const QRect &room)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (qAbs(pixel.red() - accent.red()) <= 8 && qAbs(pixel.green() - accent.green()) <= 8 &&
                qAbs(pixel.blue() - accent.blue()) <= 8) {
                ++count;
            }
        }
    }
    return count;
}

/*! Whether \a pixel is \a expected, allowing for the anti-aliasing of whatever
 *  was drawn over it. */
bool alike(const QColor &pixel, const QColor &expected)
{
    return qAbs(pixel.red() - expected.red()) <= 3 && qAbs(pixel.green() - expected.green()) <= 3 &&
           qAbs(pixel.blue() - expected.blue()) <= 3;
}

/*! The darkest pixel of \a room: an arrow nobody can use is drawn with less
 *  ink, and how many pixels are dark is not what says which is which -
 *  anti-aliasing counts as ink. */
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

/*! Tell \a widget the pointer has arrived, the way the platform would. */
void pointerArrives(QWidget *widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF here(5.0, 5.0);
    QEnterEvent enter(here, here, here);
    QApplication::sendEvent(widget, &enter);
#else
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
#endif
}

/*! The list a caller reads back as a sentence, so that a mismatch reports the
 *  whole row rather than one number. */
QString spoken(const QList<int> &items)
{
    QStringList parts;
    for (int item : items) {
        parts.append(item < 0 ? QStringLiteral("...") : QString::number(item));
    }
    return parts.join(QLatin1Char(' '));
}
}   // namespace

class tst_MaterialPagination : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theBarShowsThePagesAroundTheOneInForce();
    void theEndsAreAlwaysThereHoweverLongTheList();
    void howManySiblingsIsHowManyPages();
    void pressingAPageGoesToIt();
    void theArrowsGoOnePageAtATime();
    void theArrowsAreShutAtTheEndsOfTheList();
    void thePageInForceWearsTheAccent();
    void thePointerLightsThePlaceItIsOn();
    void theKeyboardWalksThePages();
    void oneStopOnTheWayThroughTheKeyboard();
    void thePlacesAnswerForThemselves();
    void aLongerOrShorterListKeepsThePageItCan();
};

namespace
{
/*! A bar of ten pages on the first of them. */
MaterialPagination *makeBar(QWidget *page)
{
    auto *bar = new MaterialPagination(page);
    bar->setCount(kCount);
    bar->resize(bar->sizeHint());
    // Away from where the pointer of the offscreen platform parks itself: the
    // hover wash is driven by the real cursor, and a bar built under it would be
    // washed before the test said anything about the pointer.
    bar->move(0, 60);
    bar->show();
    QTest::qWaitForWindowExposed(bar);
    return bar;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(420, 200);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}

/*! The place that stands where \a index says. The bar lays its places out
 *  itself, so the only honest way for a test to reach one is its rectangle. */
QWidget *placeAt(MaterialPagination *bar, int index)
{
    return bar->childAt(bar->itemRect(index).center());
}

QWidget *arrowAt(MaterialPagination *bar, const QRect &room)
{
    return bar->childAt(room.center());
}
}   // namespace

void tst_MaterialPagination::initTestCase()
{
    // The labels are drawn with whatever font the machine has, and the tests
    // read pixels, so there has to be one.
    qtcGiveFonts();
}

/*! MUI's arithmetic: the ends, the page in force with its neighbours, and a
 *  mark wherever pages were left out - one place per thirty-two pixels. */
void tst_MaterialPagination::theBarShowsThePagesAroundTheOneInForce()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);

    QCOMPARE(bar->page(), 1);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 2 3 4 5 ... 10"));
    QCOMPARE(bar->itemCount(), 7);
    QCOMPARE(bar->height(), kItemSize);
    QCOMPARE(bar->sizeHint(), QSize(9 * kItemSize, kItemSize));
    QCOMPARE(bar->minimumSizeHint(), bar->sizeHint());

    // The places stand side by side and the arrows take the ends of the row.
    QCOMPARE(bar->itemRect(1).left(), bar->itemRect(0).left() + kItemSize);
    QCOMPARE(bar->itemRect(2).left(), bar->itemRect(1).left() + kItemSize);
    QCOMPARE(bar->previousRect().right() + 1, bar->itemRect(0).left());
    QCOMPARE(bar->itemRect(bar->itemCount() - 1).right() + 1, bar->nextRect().left());
    QCOMPARE(bar->nextRect().right(), bar->width() - 1);

    // In the middle the window has both marks; at the far end the window has
    // moved with the page rather than the page moving to the end of the row.
    bar->setPage(5);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 ... 4 5 6 ... 10"));
    bar->setPage(10);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 ... 6 7 8 9 10"));

    // A place that is not there has no rectangle, and neither has an index
    // before the first one.
    QCOMPARE(bar->itemRect(-1), QRect());
    QCOMPARE(bar->itemRect(bar->itemCount()), QRect());
}

/*! However long the list, the bar is the same handful of places: the first page
 *  and the last one are always on it. */
void tst_MaterialPagination::theEndsAreAlwaysThereHoweverLongTheList()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    bar->setCount(1000);
    bar->setPage(500);

    const QList<int> items = bar->items();
    QCOMPARE(items.first(), 1);
    QCOMPARE(items.last(), 1000);
    QCOMPARE(spoken(items), QStringLiteral("1 ... 499 500 501 ... 1000"));

    // Two ends, the page in force with one either side, and a mark between the
    // two runs: seven places, and two arrows around them.
    QCOMPARE(items.count(), 7);
    QCOMPARE(bar->sizeHint().width(), 9 * kItemSize);
}

/*! How many pages stand beside the one in force is the caller's to say. */
void tst_MaterialPagination::howManySiblingsIsHowManyPages()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    bar->setPage(5);

    // No neighbours: the page in force stands alone between the two marks.
    bar->setSiblingCount(0);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 ... 5 ... 10"));

    // Two on each side. One page left out is not worth a mark: the page itself
    // is drawn in the place the mark would have taken.
    bar->setSiblingCount(2);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 2 3 4 5 6 7 ... 10"));

    // No fixed ends: the marks take their place at both ends of the row.
    bar->setSiblingCount(1);
    bar->setBoundaryCount(0);
    QCOMPARE(spoken(bar->items()), QStringLiteral("... 4 5 6 ..."));

    // A count below zero is not fewer than none.
    bar->setBoundaryCount(-3);
    QCOMPARE(bar->boundaryCount(), 0);
}

/*! Pressing a page goes to it, and says so once - the page already on screen is
 *  not a page anyone went to. */
void tst_MaterialPagination::pressingAPageGoesToIt()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    QSignalSpy spy(bar, &MaterialPagination::pageChanged);

    QTest::mouseClick(placeAt(bar, 2), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), 3);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 3);

    // The row has moved with the page, so the page on screen is somewhere else
    // now; pressing where it is again is not a change.
    const int at = bar->items().indexOf(3);
    QVERIFY(at >= 0);
    QTest::mouseClick(placeAt(bar, at), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(spy.count(), 1);

    // A page that is not there is refused, not clamped.
    bar->setPage(99);
    bar->setPage(0);
    QCOMPARE(bar->page(), 3);
    QCOMPARE(spy.count(), 1);
}

/*! The arrows step one page at a time and stop where the list stops. */
void tst_MaterialPagination::theArrowsGoOnePageAtATime()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    bar->setPage(5);

    QTest::mouseClick(arrowAt(bar, bar->nextRect()), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), 6);
    QTest::mouseClick(arrowAt(bar, bar->previousRect()), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), 5);
}

/*! The arrows are always drawn and shut where there is nowhere to go: a bar
 *  that dropped them would move its buttons under the pointer. */
void tst_MaterialPagination::theArrowsAreShutAtTheEndsOfTheList()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);

    QCOMPARE(arrowAt(bar, bar->previousRect())->isEnabled(), false);
    QCOMPARE(arrowAt(bar, bar->nextRect())->isEnabled(), true);

    const QImage frame = bar->grab().toImage();
    const QColor shut = darkestPixel(frame, bar->previousRect().adjusted(2, 2, -2, -2));
    const QColor open = darkestPixel(frame, bar->nextRect().adjusted(2, 2, -2, -2));
    QVERIFY2(shut.lightness() > open.lightness(),
             qPrintable(QStringLiteral("%1 vs %2").arg(shut.lightness()).arg(open.lightness())));

    // A shut arrow answers no press.
    QTest::mouseClick(arrowAt(bar, bar->previousRect()), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), 1);

    bar->setPage(kCount);
    QCOMPARE(arrowAt(bar, bar->previousRect())->isEnabled(), true);
    QCOMPARE(arrowAt(bar, bar->nextRect())->isEnabled(), false);
    QTest::mouseClick(arrowAt(bar, bar->nextRect()), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), kCount);
}

/*! The page on screen sits in a circle of the accent, and the pages beside it do
 *  not - the circle is what says which part of the list is on the screen. */
void tst_MaterialPagination::thePageInForceWearsTheAccent()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);

    const QImage frame = bar->grab().toImage();
    const QRect on = bar->itemRect(0);
    const QRect off = bar->itemRect(1);
    const QColor page_ = bar->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor fill = QxControls::QxStateLayer::layered(page_, accent, 0.12);

    // Inside the circle and above the label, which is where the fill shows.
    const QPoint inForce = on.center() + QPoint(0, -11);
    const QPoint resting = off.center() + QPoint(0, -11);
    const QColor washed = frame.pixelColor(inForce);
    QVERIFY2(alike(washed, fill), qPrintable(QStringLiteral("fill %1 vs %2").arg(washed.name()).arg(fill.name())));
    QVERIFY(alike(frame.pixelColor(resting), page_));

    // And its number is the accent, while its neighbour's is the page's ink.
    QVERIFY(accentPixels(frame, on) > 0);
    QCOMPARE(accentPixels(frame, off), 0);
}

/*! The pointer arriving, and the place being pushed, are two different washes. */
void tst_MaterialPagination::thePointerLightsThePlaceItIsOn()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    QWidget *second = placeAt(bar, 1);
    const QPoint probe = bar->itemRect(1).center() + QPoint(0, -11);
    const QColor page_ = bar->palette().color(QPalette::Window);
    const QColor ink = bar->palette().color(QPalette::WindowText);

    pointerArrives(second);
    const QColor hovering = bar->grab().toImage().pixelColor(probe);
    QVERIFY2(alike(hovering, QxControls::QxStateLayer::layered(page_, ink, QxControls::QxStateLayer::hoverOpacity)),
             qPrintable(hovering.name()));

    QTest::mousePress(second, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    const QColor pressing = bar->grab().toImage().pixelColor(probe);
    QVERIFY2(alike(pressing, QxControls::QxStateLayer::layered(page_, ink, QxControls::QxStateLayer::pressedOpacity)),
             qPrintable(pressing.name()));

    // Letting go over the place is a click, which goes to that page.
    QTest::mouseRelease(second, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(bar->page(), 2);
}

/*! A paginator is walked, not tabbed through: the arrows move a page at a time
 *  and Home and End go to the ends. */
void tst_MaterialPagination::theKeyboardWalksThePages()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    QWidget *current = placeAt(bar, 0);
    current->setFocus();
    QVERIFY(current->hasFocus());

    QTest::keyClick(current, Qt::Key_Right);
    QCOMPARE(bar->page(), 2);
    QTest::keyClick(current, Qt::Key_PageDown);
    QCOMPARE(bar->page(), 3);

    // The keyboard travelled with the page rather than staying on a place that
    // is now showing something else.
    const int at = bar->items().indexOf(bar->page());
    QVERIFY(at >= 0);
    QVERIFY(placeAt(bar, at)->hasFocus());

    QTest::keyClick(placeAt(bar, at), Qt::Key_End);
    QCOMPARE(bar->page(), kCount);
    QTest::keyClick(placeAt(bar, at), Qt::Key_Right);
    QCOMPARE(bar->page(), kCount);
    QTest::keyClick(placeAt(bar, at), Qt::Key_Home);
    QCOMPARE(bar->page(), 1);
    QTest::keyClick(placeAt(bar, at), Qt::Key_Left);
    QCOMPARE(bar->page(), 1);
}

/*! One stop on the way through the keyboard, on the page in force. */
void tst_MaterialPagination::oneStopOnTheWayThroughTheKeyboard()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);

    QCOMPARE(placeAt(bar, 0)->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(placeAt(bar, 1)->focusPolicy(), Qt::NoFocus);
    QCOMPARE(arrowAt(bar, bar->nextRect())->focusPolicy(), Qt::NoFocus);

    bar->setPage(3);
    const int at = bar->items().indexOf(3);
    QCOMPARE(placeAt(bar, at)->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(placeAt(bar, 0)->focusPolicy(), Qt::NoFocus);
}

/*! To an assistive tool each place is what it is: a button that goes somewhere,
 *  or a mark that only says pages were left out. */
void tst_MaterialPagination::thePlacesAnswerForThemselves()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);

    QAccessibleInterface *onScreen = QAccessible::queryAccessibleInterface(placeAt(bar, 0));
    QVERIFY(onScreen);
    QCOMPARE(onScreen->role(), QAccessible::Button);
    QCOMPARE(onScreen->text(QAccessible::Name), QStringLiteral("1"));
    QVERIFY(onScreen->state().selected);

    QAccessibleInterface *elsewhere = QAccessible::queryAccessibleInterface(placeAt(bar, 1));
    QVERIFY(elsewhere);
    QCOMPARE(elsewhere->text(QAccessible::Name), QStringLiteral("2"));
    QCOMPARE(elsewhere->state().selected, false);

    // The mark that stands for the pages left out is not something to press.
    const int mark = bar->items().indexOf(-1);
    QVERIFY(mark >= 0);
    QAccessibleInterface *ellipsis = QAccessible::queryAccessibleInterface(placeAt(bar, mark));
    QVERIFY(ellipsis);
    QCOMPARE(ellipsis->role(), QAccessible::StaticText);

    QAccessibleInterface *next = QAccessible::queryAccessibleInterface(arrowAt(bar, bar->nextRect()));
    QVERIFY(next);
    QCOMPARE(next->text(QAccessible::Name), QStringLiteral("Next page"));
}

/*! A list that loses pages keeps the page it can, and says the page moved -
 *  nobody asked for a page, but the page changed all the same. */
void tst_MaterialPagination::aLongerOrShorterListKeepsThePageItCan()
{
    QWidget *page = makePage();
    MaterialPagination *bar = makeBar(page);
    bar->setPage(kCount);
    QSignalSpy spy(bar, &MaterialPagination::pageChanged);

    bar->setCount(3);
    QCOMPARE(bar->count(), 3);
    QCOMPARE(bar->page(), 3);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 3);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1 2 3"));

    // A paginator with no pages is not drawn, so the count never reaches nought.
    bar->setCount(0);
    QCOMPARE(bar->count(), 1);
    QCOMPARE(bar->page(), 1);
    QCOMPARE(spoken(bar->items()), QStringLiteral("1"));

    // Growing again does not move the page it is on.
    spy.clear();
    bar->setCount(kCount);
    QCOMPARE(bar->page(), 1);
    QCOMPARE(spy.count(), 0);
}

TEST_ADD(tst_MaterialPagination)

#include "tst_materialpagination.moc"
