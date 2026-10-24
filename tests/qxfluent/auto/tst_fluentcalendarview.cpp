/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcalendarview.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QSet>
#include <QtGui/QPainter>
#include <QAccessible>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The month's own measures, read the way a reader reads them.
constexpr int kCell = 40;
constexpr int kBlockPitch = 336;

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

/*! A point inside a day's cell where only the cell's own ground can be: below
 *  the number and clear of the ring, the bar and the corners. */
QPoint ground(const QRect &cell)
{
    return QPoint(cell.left() + 5, cell.bottom() - 5);
}
}   // namespace

class tst_FluentCalendarView : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theViewOpensOnTheMonthItIsTold();
    void theGridIsFortySquareAndRunsInWeeks();
    void theMonthsStandOneUnderTheOther();
    void aMonthFarOffIsScrolledTo();

    void theMonthNamePullsBackToItsYear();
    void theYearShowsTwelveMonthsThreeByFour();
    void theYearNamePullsBackToItsDecade();
    void theDecadeShowsTenYears();
    void aMonthInTheYearOpensThatMonth();
    void aYearInTheDecadeOpensThatYear();

    void thePointerChoosesOneDay();
    void severalDaysAreChosenAndUnchosen();
    void aRangeIsDrawnAsARun();
    void todayKeepsItsRing();

    void theKeyboardWalksTheDays();
    void theKeyboardWalksTheYear();
    void takingTheDayInForceChoosesIt();

    void theAccessibleFaceIsATable();
};

namespace
{
/*! A view laid out the size a caller would give it: seven cells wide, and tall
 *  enough for one month and the top of the next. */
FluentCalendarView *makeView(QWidget *page)
{
    auto *view = new FluentCalendarView(page);
    view->resize(300, 420);
    view->show();
    QTest::qWaitForWindowExposed(view);
    return view;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(320, 440);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}
}   // namespace

void tst_FluentCalendarView::initTestCase()
{
    // The grid is drawn with whatever font the machine has; the tests read
    // pixels, so there has to be one.
    qtcGiveFonts();
}

/*! A view put on a month shows that month, from its first day. */
void tst_FluentCalendarView::theViewOpensOnTheMonthItIsTold()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);

    view->setVisibleMonth(QDate(2026, 3, 1));

    QVERIFY(view->displayMode() == FluentCalendarView::DisplayMode::Month);
    QCOMPARE(view->visibleMonth(), QDate(2026, 3, 1));
    // A month that does not exist is refused, and the view stays where it is:
    // a February that never happened is not a month to show.
    view->setVisibleMonth(QDate(2026, 2, 31));
    QCOMPARE(view->visibleMonth(), QDate(2026, 3, 1));
}

/*! A day is forty square, the days of a week stand side by side and the days
 *  of the next week stand under them. */
void tst_FluentCalendarView::theGridIsFortySquareAndRunsInWeeks()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));

    const QRect monday = view->dayRect(QDate(2026, 3, 2));
    const QRect tuesday = view->dayRect(QDate(2026, 3, 3));
    const QRect nextMonday = view->dayRect(QDate(2026, 3, 9));

    QCOMPARE(monday.size(), QSize(kCell, kCell));
    QCOMPARE(tuesday.left() - monday.left(), kCell);
    QCOMPARE(tuesday.top(), monday.top());
    QCOMPARE(nextMonday.top() - monday.top(), kCell);
    QCOMPARE(nextMonday.left(), monday.left());
}

/*! The months run on under one another rather than one at a time: that is what
 *  makes this a calendar view and not the grid a date picker opens. */
void tst_FluentCalendarView::theMonthsStandOneUnderTheOther()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));

    const QRect march = view->dayRect(QDate(2026, 3, 2));
    const QRect april = view->dayRect(QDate(2026, 4, 6));
    const QRect february = view->dayRect(QDate(2026, 2, 2));

    QCOMPARE(april.top() - march.top(), kBlockPitch);
    QCOMPARE(march.top() - february.top(), kBlockPitch);
    // Down the months is down one column: a month does not drift sideways.
    QCOMPARE(april.left(), march.left());
}

/*! Being told a month a decade away is being scrolled to it, not merely
 *  remembering it. */
void tst_FluentCalendarView::aMonthFarOffIsScrolledTo()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2031, 7, 1));

    const QRect first = view->dayRect(QDate(2031, 7, 1));
    QVERIFY(first.top() >= 0);
    QVERIFY2(first.top() < 100, qPrintable(QStringLiteral("top %1").arg(first.top())));
}

/*! The name of a month is the way out of the month: pressing it pulls the view
 *  back to that year's twelve. */
void tst_FluentCalendarView::theMonthNamePullsBackToItsYear()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));

    int changes = 0;
    connect(view, &FluentCalendarView::displayModeChanged, this, [&changes]() {
        ++changes;
    });

    const QRect day = view->dayRect(QDate(2026, 3, 1));
    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, QPoint(day.center().x(), day.top() - 48));

    QVERIFY(view->displayMode() == FluentCalendarView::DisplayMode::Year);
    QCOMPARE(changes, 1);
    QCOMPARE(view->visibleMonth(), QDate(2026, 3, 1));
}

/*! The year is its twelve months, three across and four down. */
void tst_FluentCalendarView::theYearShowsTwelveMonthsThreeByFour()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Year);

    QSet<int> lefts;
    QSet<int> tops;
    for (int month = 1; month <= 12; ++month) {
        const QRect cell = view->monthRect(QDate(2026, month, 1));
        QVERIFY2(cell.isValid(), qPrintable(QStringLiteral("month %1").arg(month)));
        lefts.insert(cell.left());
        tops.insert(cell.top());
    }
    QCOMPARE(lefts.size(), 3);
    QCOMPARE(tops.size(), 4);
    // The months of another year are not on this page.
    QVERIFY(!view->monthRect(QDate(2025, 3, 1)).isValid());
}

/*! Pressing the year pulls it back once more, to the ten years around it. */
void tst_FluentCalendarView::theYearNamePullsBackToItsDecade()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Year);

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, QPoint(view->width() / 2, 22));

    QVERIFY(view->displayMode() == FluentCalendarView::DisplayMode::Decade);
}

/*! The decade is ten years, two across and five down, running from the year
 *  the decade starts in. */
void tst_FluentCalendarView::theDecadeShowsTenYears()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Decade);

    QSet<int> lefts;
    QSet<int> tops;
    for (int year = 2020; year <= 2029; ++year) {
        const QRect cell = view->yearRect(year);
        QVERIFY2(cell.isValid(), qPrintable(QStringLiteral("year %1").arg(year)));
        lefts.insert(cell.left());
        tops.insert(cell.top());
    }
    QCOMPARE(lefts.size(), 2);
    QCOMPARE(tops.size(), 5);
    QVERIFY(!view->yearRect(2019).isValid());
    QVERIFY(!view->yearRect(2030).isValid());
}

/*! A month pressed in the year is that month's days, and the view goes back
 *  down to them. */
void tst_FluentCalendarView::aMonthInTheYearOpensThatMonth()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Year);

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->monthRect(QDate(2026, 5, 1)).center());

    QVERIFY(view->displayMode() == FluentCalendarView::DisplayMode::Month);
    QCOMPARE(view->visibleMonth(), QDate(2026, 5, 1));
}

/*! A year pressed in the decade is that year's twelve months. */
void tst_FluentCalendarView::aYearInTheDecadeOpensThatYear()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Decade);

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->yearRect(2023).center());

    QVERIFY(view->displayMode() == FluentCalendarView::DisplayMode::Year);
    QCOMPARE(view->visibleMonth().year(), 2023);
}

/*! A day pressed is the day in force, and the signal says so once. */
void tst_FluentCalendarView::thePointerChoosesOneDay()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));

    int changes = 0;
    QDate reported;
    connect(view, &FluentCalendarView::selectedDateChanged, this, [&](QDate date) {
        ++changes;
        reported = date;
    });

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 12)).center());

    QCOMPARE(view->selectedDate(), QDate(2026, 3, 12));
    QCOMPARE(changes, 1);
    QCOMPARE(reported, QDate(2026, 3, 12));

    // Pressing the same day again is not a change of what is in force.
    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 12)).center());
    QCOMPARE(changes, 1);
}

/*! Several days at once: a press adds a day, another press on a day already
 *  chosen takes it back. */
void tst_FluentCalendarView::severalDaysAreChosenAndUnchosen()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setSelectionMode(FluentCalendarView::SelectionMode::Multiple);

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 10)).center());
    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 12)).center());
    QCOMPARE(view->selectedDates(), QList<QDate>() << QDate(2026, 3, 10) << QDate(2026, 3, 12));

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 10)).center());
    QCOMPARE(view->selectedDates(), QList<QDate>() << QDate(2026, 3, 12));
}

/*! A range is two presses and the run between them, and it is drawn as a run:
 *  the days between are washed, the days outside are not. */
void tst_FluentCalendarView::aRangeIsDrawnAsARun()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setSelectionMode(FluentCalendarView::SelectionMode::Range);

    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 10)).center());
    QTest::mouseClick(view, Qt::LeftButton, Qt::NoModifier, view->dayRect(QDate(2026, 3, 14)).center());

    QCOMPARE(view->selectedDates(), QList<QDate>() << QDate(2026, 3, 10) << QDate(2026, 3, 14));

    const QImage frame = view->grab().toImage();
    const QColor page_ = view->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor washed = QxControls::QxStateLayer::layered(page_, accent, 0.12);

    const QRect between = view->dayRect(QDate(2026, 3, 12));
    QVERIFY2(
        alike(frame.pixelColor(ground(between)), washed),
        qPrintable(
            QStringLiteral("between %1 washed %2").arg(frame.pixelColor(ground(between)).name()).arg(washed.name())));

    const QRect outside = view->dayRect(QDate(2026, 3, 16));
    QVERIFY(alike(frame.pixelColor(ground(outside)), page_));

    // The two ends are solid, not washed.
    const QRect end = view->dayRect(QDate(2026, 3, 10));
    QVERIFY(accentPixels(frame, end.adjusted(6, 6, -6, -6)) > 100);
}

/*! Today stands in its own ring, which is the accent and nothing else. */
void tst_FluentCalendarView::todayKeepsItsRing()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    const QDate today = view->today();
    view->setVisibleMonth(QDate(today.year(), today.month(), 1));

    const QImage frame = view->grab().toImage();
    const QRect cell = view->dayRect(today);
    QVERIFY2(accentPixels(frame, cell) > 0, "today carries no ring");
    // And a day that is not today carries none.
    QCOMPARE(accentPixels(frame, view->dayRect(today.addDays(2))), 0);
}

/*! The keyboard walks the days: across is a day, down is a week. */
void tst_FluentCalendarView::theKeyboardWalksTheDays()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setSelectedDate(QDate(2026, 3, 2));
    view->setFocus();

    QTest::keyClick(view, Qt::Key_Right);
    QCOMPARE(view->currentDate(), QDate(2026, 3, 3));
    QTest::keyClick(view, Qt::Key_Down);
    QCOMPARE(view->currentDate(), QDate(2026, 3, 10));
    QTest::keyClick(view, Qt::Key_PageDown);
    QCOMPARE(view->currentDate(), QDate(2026, 4, 10));
    // Walking is not choosing: the day in force has not moved.
    QCOMPARE(view->selectedDate(), QDate(2026, 3, 2));
}

/*! In the year the keyboard walks months, three to a line. */
void tst_FluentCalendarView::theKeyboardWalksTheYear()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setCurrentDate(QDate(2026, 3, 1));
    view->setDisplayMode(FluentCalendarView::DisplayMode::Year);
    view->setFocus();

    QTest::keyClick(view, Qt::Key_Right);
    QCOMPARE(view->currentDate(), QDate(2026, 4, 1));
    QTest::keyClick(view, Qt::Key_Down);
    QCOMPARE(view->currentDate(), QDate(2026, 7, 1));
}

/*! Taking the day the keyboard stands on chooses it. */
void tst_FluentCalendarView::takingTheDayInForceChoosesIt()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setVisibleMonth(QDate(2026, 3, 1));
    view->setCurrentDate(QDate(2026, 3, 20));
    view->setFocus();

    QTest::keyClick(view, Qt::Key_Return);
    QCOMPARE(view->selectedDate(), QDate(2026, 3, 20));
}

/*! To an assistive tool the calendar is a table of days under a name. */
void tst_FluentCalendarView::theAccessibleFaceIsATable()
{
    QWidget *page = makePage();
    FluentCalendarView *view = makeView(page);
    view->setSelectedDate(QDate(2026, 3, 12));

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(view);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Table);
    QCOMPARE(face->text(QAccessible::Name), QStringLiteral("Calendar"));
    QCOMPARE(face->text(QAccessible::Value), QStringLiteral("2026-03-12"));
    QVERIFY(face->state().focusable);
}

TEST_ADD(tst_FluentCalendarView)

#include "tst_fluentcalendarview.moc"
