/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialdatepicker.h"

#include "qxcontrols/qxpopupsurface.h"

#include <QSignalSpy>
#include <QtWidgets/QLineEdit>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

class tst_MaterialDatePicker : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theFieldSaysTheDay();
    void aDayTakenComesBackToTheField();
    void openingShowsTheMonthInForce();
    void nothingIsSaidForNoDay();
private:
    QWidget *m_page = nullptr;
};

void tst_MaterialDatePicker::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(280, 56);
    m_page->show();
}

void tst_MaterialDatePicker::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

/*! The field does not take a keyboard - it says the day the calendar has
 *  taken, written the way a day is written here. */
void tst_MaterialDatePicker::theFieldSaysTheDay()
{
    MaterialDatePicker picker(m_page);
    QVERIFY(picker.lineEdit()->isReadOnly());

    QSignalSpy changedSpy(&picker, &MaterialDatePicker::dateChanged);
    picker.setDate(QDate(2026, 3, 15));
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-03-15"));
    QCOMPARE(changedSpy.count(), 1);

    // The day in force said twice is said once.
    picker.setDate(QDate(2026, 3, 15));
    QCOMPARE(changedSpy.count(), 1);
}

/*! A day taken in the calendar - here by walking to another day and pressing
 *  Enter - comes back to the field, and the month goes away with it. */
void tst_MaterialDatePicker::aDayTakenComesBackToTheField()
{
    MaterialDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));

    picker.openCalendar();
    QVERIFY(picker.calendar()->isVisible());

    QSignalSpy changedSpy(&picker, &MaterialDatePicker::dateChanged);
    QSignalSpy dismissedSpy(static_cast<QxControls::QxPopupSurface *>(picker.calendar()),
                            &QxControls::QxPopupSurface::dismissed);

    // The day in force is taken as it stands: nothing changed, and the month
    // goes away all the same.
    QTest::keyClick(picker.calendar(), Qt::Key_Return);
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(changedSpy.count(), 0);
    QVERIFY(!picker.calendar()->isVisible());
    QCOMPARE(dismissedSpy.count(), 1);

    // A day walked to - a week on from the one in force - comes back as a
    // different day, written again.
    picker.openCalendar();
    QTest::keyClick(picker.calendar(), Qt::Key_Down);
    QTest::keyClick(picker.calendar(), Qt::Key_Return);
    QCOMPARE(picker.date(), QDate(2026, 3, 22));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-03-22"));
    QCOMPARE(changedSpy.count(), 1);
}

/*! Opened, the calendar stands under the field it belongs to, on the month the
 *  day in force belongs to. */
void tst_MaterialDatePicker::openingShowsTheMonthInForce()
{
    MaterialDatePicker picker(m_page);
    picker.setGeometry(0, 0, 240, 56);
    picker.show();
    picker.setDate(QDate(2026, 3, 15));
    picker.openCalendar();

    QVERIFY(picker.calendar()->isVisible());
    QCOMPARE(picker.shownMonth(), QDate(2026, 3, 1));
    // Under the field, not under the page: the field is the thing the month
    // belongs to and it is what the surface is placed against.
    QVERIFY(picker.calendar()->y() >= picker.mapToGlobal(QPoint(0, picker.height())).y());
}

/*! No day is no answer: an invalid day leaves the field as it stood. */
void tst_MaterialDatePicker::nothingIsSaidForNoDay()
{
    MaterialDatePicker picker(m_page);
    picker.setDate(QDate(2026, 3, 15));

    QSignalSpy changedSpy(&picker, &MaterialDatePicker::dateChanged);
    picker.setDate(QDate());
    QCOMPARE(picker.date(), QDate(2026, 3, 15));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("2026-03-15"));
    QCOMPARE(changedSpy.count(), 0);
}

TEST_ADD(tst_MaterialDatePicker)

#include "tst_materialdatepicker.moc"
