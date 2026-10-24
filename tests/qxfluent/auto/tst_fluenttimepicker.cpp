/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttimepicker.h"

#include "qxcontrols/qxpopupsurface.h"

#include <QSignalSpy>
#include <QtWidgets/QLineEdit>
#include <QWheelEvent>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The columns' own measures, read the way a reader reads them: the padding
// a column keeps from the surface's edge, the row the hour stands in.
constexpr qreal kPadding = 12.0;
constexpr qreal kColumnWidth = 56.0;
constexpr qreal kRowHeight = 32.0;
}   // namespace

class tst_FluentTimePicker : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void theFieldSaysTheTime();
    void aTakeComesBackToTheField();
    void theColumnsComeRound();
    void theWheelTurnsAColumnWithoutSayingSo();
    void aClickTakesWhatItLandsOn();
    void enterTakesItAwayAndEscapePutsItAway();
    void aFieldNobodyCanOpenIsDrawnFainter();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentTimePicker::init()
{
    m_page = new QWidget;
    m_page->resize(200, 32);
    m_page->show();
}

void tst_FluentTimePicker::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

/*! The field does not take a keyboard - it says the time the columns have
 *  taken. */
void tst_FluentTimePicker::theFieldSaysTheTime()
{
    FluentTimePicker picker(m_page);
    QVERIFY(picker.lineEdit()->isReadOnly());

    QSignalSpy changedSpy(&picker, &FluentTimePicker::timeChanged);
    picker.setTime(QTime(9, 5));
    QCOMPARE(picker.time(), QTime(9, 5));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("09:05"));
    QCOMPARE(changedSpy.count(), 1);

    // The time in force said twice is said once, and no time is no answer.
    picker.setTime(QTime(9, 5));
    QCOMPARE(changedSpy.count(), 1);
    picker.setTime(QTime());
    QCOMPARE(picker.time(), QTime(9, 5));
    QCOMPARE(changedSpy.count(), 1);
}

/*! A take in either column - the keyboard's Up and Down - finishes a time
 *  and it comes back to the field. */
void tst_FluentTimePicker::aTakeComesBackToTheField()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(9, 5));
    picker.openFlyout();
    QWidget *columns = picker.flyout();
    QVERIFY(columns->isVisible());

    QSignalSpy changedSpy(&picker, &FluentTimePicker::timeChanged);

    QTest::keyClick(columns, Qt::Key_Down);
    QCOMPARE(picker.time(), QTime(10, 5));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("10:05"));
    QCOMPARE(changedSpy.count(), 1);

    // The other column: Right hands the walk to the minutes, Up takes one
    // off.
    QTest::keyClick(columns, Qt::Key_Right);
    QTest::keyClick(columns, Qt::Key_Up);
    QCOMPARE(picker.time(), QTime(10, 4));
    QCOMPARE(changedSpy.count(), 2);
}

/*! The day's hours and the hour's minutes both come round. */
void tst_FluentTimePicker::theColumnsComeRound()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(23, 59));
    picker.openFlyout();
    QWidget *columns = picker.flyout();

    QTest::keyClick(columns, Qt::Key_Down);
    QCOMPARE(picker.time().hour(), 0);
    QTest::keyClick(columns, Qt::Key_Right);
    QTest::keyClick(columns, Qt::Key_Down);
    QCOMPARE(picker.time(), QTime(0, 0));
}

/*! The wheel turns a column; turning it says nothing - the time in force
 *  only moves when a row is taken. */
void tst_FluentTimePicker::theWheelTurnsAColumnWithoutSayingSo()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(4, 30));
    picker.openFlyout();
    QWidget *columns = picker.flyout();
    const QRectF content = QRectF(columns->contentsRect());

    QSignalSpy changedSpy(&picker, &FluentTimePicker::timeChanged);
    QWheelEvent wheel(QPointF(content.left() + kPadding + kColumnWidth / 2.0, content.top() + 96.0),
                      columns->mapToGlobal(QPoint(int(content.left()) + 40, int(content.top()) + 96)), QPoint(),
                      QPoint(0, 120), Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
    QApplication::sendEvent(columns, &wheel);

    QCOMPARE(changedSpy.count(), 0);
    QCOMPARE(picker.time(), QTime(4, 30));
}

/*! A click takes the row it lands on, and the time it finishes comes back
 *  to the field. */
void tst_FluentTimePicker::aClickTakesWhatItLandsOn()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(4, 30));
    picker.openFlyout();
    QWidget *columns = picker.flyout();
    const QRectF content = QRectF(columns->contentsRect());

    QSignalSpy changedSpy(&picker, &FluentTimePicker::timeChanged);
    // The hour column, two rows down from the top of the view: the wheel
    // aside, the view stood where the taken hour stands mid-column.
    const QPointF row(content.left() + kPadding + kColumnWidth / 2.0,
                      content.top() + 2 * kRowHeight + kRowHeight / 2.0);
    QTest::mouseClick(columns, Qt::LeftButton, Qt::NoModifier, row.toPoint());

    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(picker.time(), QTime(3, 30));
    QCOMPARE(picker.lineEdit()->text(), QStringLiteral("03:30"));
}

/*! Enter is the hand that puts the columns away having said the time;
 *  Escape puts them away all the same. */
void tst_FluentTimePicker::enterTakesItAwayAndEscapePutsItAway()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(9, 5));

    QSignalSpy dismissedSpy(static_cast<QxControls::QxPopupSurface *>(picker.flyout()),
                            &QxControls::QxPopupSurface::dismissed);
    picker.openFlyout();
    QVERIFY(picker.flyout()->isVisible());
    QTest::keyClick(picker.flyout(), Qt::Key_Return);
    QVERIFY(!picker.flyout()->isVisible());
    QCOMPARE(dismissedSpy.count(), 1);

    picker.openFlyout();
    QTest::keyClick(picker.flyout(), Qt::Key_Escape);
    QVERIFY(!picker.flyout()->isVisible());
    QCOMPARE(dismissedSpy.count(), 2);
    QCOMPARE(picker.time(), QTime(9, 5));
}

/*! A time field nobody can open is drawn a step down, for the reason the date
 *  picker's case gives: pinned colours, one frame before and one after, and the
 *  flyout left to the cases that own it.
 */
void tst_FluentTimePicker::aFieldNobodyCanOpenIsDrawnFainter()
{
    FluentTimePicker picker(m_page);
    picker.setTime(QTime(9, 5));
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

void tst_FluentTimePicker::theKeyboardLandsOnIt()
{
    FluentTimePicker widget(m_page);
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentTimePicker)

#include "tst_fluenttimepicker.moc"
