/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcalendarflyout.h"
#include "qxfluent/fluentdatepicker.h"

#include "qxcontrols/qxpopupsurface.h"

#include <QSignalSpy>
#include <QtWidgets/QLineEdit>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

class tst_FluentDatePicker : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theFieldSaysTheDay();
    void aDayTakenComesBackToTheField();
    void openingShowsTheMonthInForce();
    void nothingIsSaidForNoDay();
    void aFieldNobodyCanOpenIsDrawnFainter();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentDatePicker::init()
{
    m_page = new QWidget;
    m_page->resize(200, 32);
    m_page->show();
}

void tst_FluentDatePicker::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

/*! The field does not take a keyboard - it says the day the calendar has
 *  taken. */
void tst_FluentDatePicker::theFieldSaysTheDay()
{
    FluentDatePicker picker(m_page);
    QVERIFY(picker.lineEdit()->isReadOnly());

    QSignalSpy changedSpy(&picker, &FluentDatePicker::dateChanged);
    picker.setDate(QDate(2026, 3, 15));
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-03-15"));
    QCOMPARE(changedSpy.count(), 1);

    // The day in force said twice is said once.
    picker.setDate(QDate(2026, 3, 15));
    QCOMPARE(changedSpy.count(), 1);
}

/*! A day taken in the calendar - by Enter on the walked-to day - comes back
 *  to the field, and the month goes away with it. */
void tst_FluentDatePicker::aDayTakenComesBackToTheField()
{
    FluentDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));

    picker.openCalendar();
    QVERIFY(picker.calendar()->isVisible());

    QSignalSpy changedSpy(&picker, &FluentDatePicker::dateChanged);
    QSignalSpy dismissedSpy(picker.calendar(), &QxControls::QxPopupSurface::dismissed);

    QTest::keyClick(picker.calendar(), Qt::Key_Return);
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(changedSpy.count(), 0);
    QVERIFY(!picker.calendar()->isVisible());
    QCOMPARE(dismissedSpy.count(), 1);

    // And a different day comes back as a different day.
    picker.openCalendar();
    picker.calendar()->setSelectedDate(QDate(2026, 5, 20));
    QTest::keyClick(picker.calendar(), Qt::Key_Return);
    QCOMPARE(picker.date(), QDate(2026, 5, 20));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-05-20"));
    QCOMPARE(changedSpy.count(), 1);
}

/*! Opened, the calendar stands below the field, on the month the day in
 *  force belongs to. */
void tst_FluentDatePicker::openingShowsTheMonthInForce()
{
    FluentDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));
    picker.openCalendar();

    QVERIFY(picker.calendar()->isVisible());
    QCOMPARE(picker.shownMonth(), QDate(2026, 3, 1));
    QVERIFY(picker.calendar()->geometry().top() >= m_page->geometry().bottom());
}

/*! No day is no answer: an invalid day leaves the field as it stood. */
void tst_FluentDatePicker::nothingIsSaidForNoDay()
{
    FluentDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));

    QSignalSpy changedSpy(&picker, &FluentDatePicker::dateChanged);
    picker.setDate(QDate());
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-03-15"));
    QCOMPARE(changedSpy.count(), 0);
}

/*! A picker nobody can open is drawn a step down. Pinned to the active colours
 *  first, so what differs between the two frames is the face's own hand.
 *
 *  Only the frame is asserted: the field is read-only by design, so a press on it
 *  can only open the calendar, and that flyout has its own cases in
 *  `tst_FluentCalendarFlyout`.
 */
void tst_FluentDatePicker::aFieldNobodyCanOpenIsDrawnFainter()
{
    FluentDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));
    picker.setGeometry(0, 0, 160, 32);
    picker.show();
    qtcPinDisabledColours(&picker);
    // The typing inside is a QLineEdit of its own, and Qt greys that one through
    // its palette: pinned too, so the only thing left to move is the face.
    qtcPinDisabledColours(picker.lineEdit());

    const QImage live = picker.grab().toImage();
    picker.setEnabled(false);
    const QImage faded = picker.grab().toImage();
    QVERIFY(faded != live);
}

TEST_ADD(tst_FluentDatePicker)

#include "tst_fluentdatepicker.moc"
