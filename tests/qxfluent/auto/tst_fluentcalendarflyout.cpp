/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcalendarflyout.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QKeyEvent>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The calendar's own measures: a cell of forty, a head and a row of day
// names above the grid. They are read here the way a reader reads them -
// from the surface's room and the shape of the grid.
constexpr qreal kCell = 40.0;
constexpr qreal kHeadHeight = 48.0;
constexpr qreal kDayNamesHeight = 32.0;
constexpr int kColumns = 7;

/*! The accent, read where the control reads it - a probe against a guessed
 *  shade would break on every accent the theme is allowed to carry. */
bool isAccent(const QColor &color)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    return qAbs(color.red() - accent.red()) <= 8 && qAbs(color.green() - accent.green()) <= 8 &&
           qAbs(color.blue() - accent.blue()) <= 8;
}
}   // namespace

class tst_FluentCalendarFlyout : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void openIsDrawnOverThePage();
    void monthNavigationMovesTheGrid();
    void theDayInForceWearsTheWashAndTodayItsRing();
    void theKeyboardWalksTheDays();
    void itIsCompleteWhenMotionIsReduced();
private:
    QWidget *m_anchor = nullptr;
};

void tst_FluentCalendarFlyout::init()
{
    m_anchor = new QWidget;
    m_anchor->resize(120, 32);
    m_anchor->show();
}

void tst_FluentCalendarFlyout::cleanup()
{
    delete m_anchor;
    m_anchor = nullptr;
}

/*! Opened, the month stands over the page: the surface is drawn and its
 *  grid is written on it. */
void tst_FluentCalendarFlyout::openIsDrawnOverThePage()
{
    FluentCalendarFlyout flyout(m_anchor);
    flyout.open();

    QVERIFY(flyout.isVisible());
    // The flyout stands below its anchor, in the room of its surface.
    QVERIFY(flyout.geometry().top() >= m_anchor->geometry().bottom());
    // And the surface carries ink the page did not have.
    const QImage image = flyout.grab().toImage();
    const QColor page = flyout.palette().color(QPalette::Window);
    int inkRows = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++inkRows;
                break;
            }
        }
    }
    QVERIFY(inkRows > 1);
}

/*! The months either side are a click away, and walking between them moves
 *  the grid without moving the day in force. */
void tst_FluentCalendarFlyout::monthNavigationMovesTheGrid()
{
    FluentCalendarFlyout flyout(m_anchor);
    const QDate month(2026, 3, 1);
    const QDate day(2026, 3, 15);
    flyout.setMonth(month);
    flyout.setSelectedDate(day);
    flyout.open();

    const QImage onMarch = flyout.grab().toImage();
    QCOMPARE(flyout.month(), month);

    flyout.showNextMonth();
    QCOMPARE(flyout.month(), QDate(2026, 4, 1));
    QCOMPARE(flyout.selectedDate(), day);
    const QImage onApril = flyout.grab().toImage();
    QVERIFY(onApril != onMarch);

    flyout.showPreviousMonth();
    QCOMPARE(flyout.month(), month);
    const QImage backOnMarch = flyout.grab().toImage();
    QCOMPARE(backOnMarch, onMarch);
}

/*! The day in force is washed in the accent, and today keeps its own ring -
 *  whatever else it is - when the two are not the same day. */
void tst_FluentCalendarFlyout::theDayInForceWearsTheWashAndTodayItsRing()
{
    FluentCalendarFlyout flyout(m_anchor);
    // Two days in the month this machine thinks it is, and never the same
    // day: the wash and the ring are read off one grid.
    const QDate today = QDate::currentDate();
    const QDate picked(today.year(), today.month(), today.day() == 1 ? 2 : 1);
    flyout.setSelectedDate(picked);
    flyout.open();

    const QRectF content = flyout.contentRect();
    const qreal gridTop = content.top() + kHeadHeight + kDayNamesHeight;
    auto cellOf = [&gridTop](const QDate &date) {
        const QDate first(date.year(), date.month(), 1);
        const int index = first.dayOfWeek() - 1 + date.day() - 1;
        const int row = index / kColumns;
        const int column = index % kColumns;
        return QRectF(column * kCell, gridTop + row * kCell, kCell, kCell);
    };

    // The day in force: filled in the accent.
    const QImage image = flyout.grab().toImage();
    const QRectF pickedCell = cellOf(picked).translated(content.left(), 0);
    int wash = 0;
    for (int x = int(pickedCell.left()) + 12; x < int(pickedCell.right()) - 12; ++x) {
        for (int y = int(pickedCell.top()) + 12; y < int(pickedCell.bottom()) - 12; ++y) {
            if (isAccent(image.pixelColor(x, y))) {
                ++wash;
            }
        }
    }
    QVERIFY(wash > 0);

    // Today: a ring, not a fill - the accent stands at the cell's rim and
    // the middle of the cell stays ink.
    const QRectF todayCell = cellOf(today).translated(content.left(), 0);
    int rim = 0;
    for (int x = int(todayCell.left()) + 2; x < int(todayCell.right()) - 2; ++x) {
        for (int y = int(todayCell.top()) + 2; y < int(todayCell.bottom()) - 2; ++y) {
            if (isAccent(image.pixelColor(x, y))) {
                ++rim;
            }
        }
    }
    QVERIFY(rim > 0);
}

/*! The keyboard walks the days - a step, a week - Enter takes the day it
 *  stands on, Escape puts the month away. */
void tst_FluentCalendarFlyout::theKeyboardWalksTheDays()
{
    FluentCalendarFlyout flyout(m_anchor);
    const QDate day(2026, 3, 15);
    flyout.setSelectedDate(day);
    flyout.open();

    QSignalSpy changedSpy(&flyout, &FluentCalendarFlyout::selectedDateChanged);
    QSignalSpy pickedSpy(&flyout, &FluentCalendarFlyout::datePicked);
    QSignalSpy dismissedSpy(&flyout, &QxControls::QxPopupSurface::dismissed);

    QTest::keyClick(&flyout, Qt::Key_Right);
    QCOMPARE(flyout.selectedDate(), day.addDays(1));
    QCOMPARE(changedSpy.count(), 1);
    QTest::keyClick(&flyout, Qt::Key_Left);
    QCOMPARE(flyout.selectedDate(), day);
    QTest::keyClick(&flyout, Qt::Key_Down);
    QCOMPARE(flyout.selectedDate(), day.addDays(7));
    QTest::keyClick(&flyout, Qt::Key_Up);
    QCOMPARE(flyout.selectedDate(), day);

    QTest::keyClick(&flyout, Qt::Key_Return);
    QCOMPARE(pickedSpy.count(), 1);
    QCOMPARE(pickedSpy.at(0).at(0).toDate(), day);
    QVERIFY(!flyout.isVisible());
    QCOMPARE(dismissedSpy.count(), 1);

    flyout.open();
    QTest::keyClick(&flyout, Qt::Key_Escape);
    QVERIFY(!flyout.isVisible());
    QCOMPARE(pickedSpy.count(), 1);
    QCOMPARE(dismissedSpy.count(), 2);
}

/*! Reduced motion is not a thinner month: the flyout carries no entrance to
 *  skip, and the grid it draws is the grid, complete. */
void tst_FluentCalendarFlyout::itIsCompleteWhenMotionIsReduced()
{
    QxTheme::setMotionReduced(true);
    FluentCalendarFlyout flyout(m_anchor);
    flyout.setSelectedDate(QDate::currentDate());
    flyout.open();

    QVERIFY(flyout.isVisible());
    const QImage image = flyout.grab().toImage();
    const QColor page = flyout.palette().color(QPalette::Window);
    int inkRows = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++inkRows;
                break;
            }
        }
    }
    QVERIFY(inkRows > 1);
    QxTheme::setMotionReduced(false);
}

void tst_FluentCalendarFlyout::theKeyboardLandsOnIt()
{
    FluentCalendarFlyout widget(m_anchor);
    widget.show();
    widget.open();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentCalendarFlyout)

#include "tst_fluentcalendarflyout.moc"
