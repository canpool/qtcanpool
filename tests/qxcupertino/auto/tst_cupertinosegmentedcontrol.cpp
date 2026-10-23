/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinosegmentedcontrol.h"

#include "qxcupertino/cupertinotokens.h"

#include "qxtheme/qxthememotion.h"

#include <QApplication>
#include <QImage>
#include <QSignalSpy>

QX_CUPERTINO_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A row of three in a three-hundred wide control: the pill is two in from every
// edge, so a segment is (300 - 4) / 3 wide and the middle of the first one is
// at about fifty.
constexpr int kWidth = 300;

// Where the pill is read: inside it, in the band above the labels it carries.
// The middle of the row is where the words are, and a glyph is not the pill.
int pillRow(QWidget *widget)
{
    return widget->height() / 2 - 12;
}

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

bool isWhite(const QColor &colour)
{
    return colour.lightness() > 240;
}

/*! The brightest pixel in the column at \a x, over the row's height. A line
 *  between two segments is darker than the row it is drawn on and the pill is
 *  white, so this answers "is the pill here" without naming either colour. */
int brightestInColumn(QWidget *widget, int x)
{
    const QImage image = widget->grab().toImage();
    int brightest = 0;
    for (int y = widget->height() / 2 - 8; y <= widget->height() / 2 + 8; ++y) {
        brightest = qMax(brightest, image.pixelColor(x, y).lightness());
    }
    return brightest;
}

/*! The darkest pixel in the column at \a x: a separator shows up here and a
 *  pill passing over it takes it away. */
int darkestInColumn(QWidget *widget, int x)
{
    const QImage image = widget->grab().toImage();
    int darkest = 255;
    for (int y = widget->height() / 2 - 8; y <= widget->height() / 2 + 8; ++y) {
        darkest = qMin(darkest, image.pixelColor(x, y).lightness());
    }
    return darkest;
}
}   // namespace

class tst_CupertinoSegmentedControl : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theRowIsOneStopAndTheArrowsWalkIt();
    void aClickPicksTheSegmentUnderIt();
    void thePillSlidesAndCoversTheLineItPasses();
    void currentIndexRoundTrips();
    void oneSegmentAndNoneAtAllDoNotBreakIt();
private:
    qreal m_wasScale = 1.0;
};

void tst_CupertinoSegmentedControl::cleanup()
{
    QxTheme::setMotionScale(m_wasScale);
    QxTheme::setMotionReduced(false);
}

/*! The whole row is a single tab stop, and the arrows move the pick rather than
 *  the focus - which is the difference between this control and a row of radio
 *  buttons. */
void tst_CupertinoSegmentedControl::theRowIsOneStopAndTheArrowsWalkIt()
{
    CupertinoSegmentedControl control;
    control.resize(kWidth, control.sizeHint().height());
    control.setItems({QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")});

    QCOMPARE(control.count(), 3);
    QCOMPARE(control.currentIndex(), 0);
    QCOMPARE(control.focusPolicy(), Qt::StrongFocus);
    // One stop means one widget: the segments are not controls of their own.
    QCOMPARE(control.findChildren<QWidget *>().count(), 0);

    control.setFocus();
    QTest::keyClick(&control, Qt::Key_Right);
    QCOMPARE(control.currentIndex(), 1);
    QTest::keyClick(&control, Qt::Key_Left);
    QCOMPARE(control.currentIndex(), 0);
    QTest::keyClick(&control, Qt::Key_End);
    QCOMPARE(control.currentIndex(), 2);
    QTest::keyClick(&control, Qt::Key_Right);
    QCOMPARE(control.currentIndex(), 2);
    QTest::keyClick(&control, Qt::Key_Home);
    QCOMPARE(control.currentIndex(), 0);
    QTest::keyClick(&control, Qt::Key_Left);
    QCOMPARE(control.currentIndex(), 0);
}

/*! A click picks the segment it lands on, and says so once. */
void tst_CupertinoSegmentedControl::aClickPicksTheSegmentUnderIt()
{
    CupertinoSegmentedControl control;
    control.resize(kWidth, control.sizeHint().height());
    control.setItems({QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")});

    QSignalSpy changed(&control, &CupertinoSegmentedControl::currentIndexChanged);

    QTest::mouseClick(&control, Qt::LeftButton, Qt::NoModifier, QPoint(250, control.height() / 2));
    QCOMPARE(control.currentIndex(), 2);
    QCOMPARE(control.currentText(), QStringLiteral("Month"));
    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.at(0).at(0).toInt(), 2);

    QTest::mouseClick(&control, Qt::LeftButton, Qt::NoModifier, QPoint(150, control.height() / 2));
    QCOMPARE(control.currentIndex(), 1);
    QCOMPARE(changed.count(), 2);

    // A click on the segment already picked changes nothing and says nothing.
    QTest::mouseClick(&control, Qt::LeftButton, Qt::NoModifier, QPoint(150, control.height() / 2));
    QCOMPARE(changed.count(), 2);
}

/*! The pill is what moves, and the line it passes over goes under it for as long
 *  as it is there: the pick is the position of a thing rather than the state of
 *  one of several. */
void tst_CupertinoSegmentedControl::thePillSlidesAndCoversTheLineItPasses()
{
    CupertinoSegmentedControl control;
    control.resize(kWidth, control.sizeHint().height());
    control.setItems({QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")});

    // The pill starts under the first segment, so the first line is a line and
    // the column above the second segment is the row's own grey.
    const qreal segment = (kWidth - 4.0) / 3.0;
    const int firstLine = int(2.0 + segment) + 1;
    const int secondSegment = int(2.0 + segment * 1.5);
    QVERIFY(isWhite(pixel(&control, int(2.0 + segment * 0.5), pillRow(&control))));
    QVERIFY(darkestInColumn(&control, firstLine) < brightestInColumn(&control, secondSegment));

    // A slow slide, so a sample lands with the pill halfway between the first
    // and the second segment - over the line.
    m_wasScale = QxTheme::motionScale();
    QxTheme::setMotionScale(20.0);
    const int travel = CupertinoMotion::forDuration(CupertinoMotion::Duration::Normal).duration;
    QVERIFY(travel > 4000);

    control.setCurrentIndex(1);
    QTest::qWait(travel / 2);
    QVERIFY(isWhite(pixel(&control, firstLine, pillRow(&control))));

    // And once it has arrived, the line is back and the pill is under the second
    // segment.
    QTest::qWait(travel);
    QVERIFY(brightestInColumn(&control, firstLine) < 240);
    QVERIFY(isWhite(pixel(&control, int(2.0 + segment * 1.5), pillRow(&control))));
}

/*! A pick is an index into the labels: it can be asked for, set back, and asked
 *  for again - and a pick outside the labels is the nearest one there is. */
void tst_CupertinoSegmentedControl::currentIndexRoundTrips()
{
    CupertinoSegmentedControl control;
    control.setItems({QStringLiteral("Day"), QStringLiteral("Week")});
    QCOMPARE(control.items().count(), 2);
    QCOMPARE(control.currentText(), QStringLiteral("Day"));

    control.setCurrentIndex(1);
    QCOMPARE(control.currentIndex(), 1);
    QCOMPARE(control.currentText(), QStringLiteral("Week"));

    control.setCurrentIndex(9);
    QCOMPARE(control.currentIndex(), 1);
    control.setCurrentIndex(-4);
    QCOMPARE(control.currentIndex(), 0);

    // Labels can be replaced under a pick, and the pick stays where it still
    // names a segment.
    control.setCurrentIndex(1);
    control.addItem(QStringLiteral("Month"));
    QCOMPARE(control.count(), 3);
    QCOMPARE(control.currentIndex(), 1);
    QCOMPARE(control.currentText(), QStringLiteral("Week"));

    control.setItems({QStringLiteral("Only")});
    QCOMPARE(control.currentIndex(), 0);
    QCOMPARE(control.currentText(), QStringLiteral("Only"));
}

/*! One segment and none at all are states a segmented control is put in by a
 *  data change, and neither is a crash: there is simply nothing to move. */
void tst_CupertinoSegmentedControl::oneSegmentAndNoneAtAllDoNotBreakIt()
{
    CupertinoSegmentedControl control;
    control.resize(kWidth, control.sizeHint().height());

    // Nothing at all.
    QCOMPARE(control.count(), 0);
    QCOMPARE(control.currentIndex(), -1);
    QVERIFY(control.currentText().isEmpty());
    QVERIFY(!control.grab().isNull());
    control.setCurrentIndex(0);
    QCOMPARE(control.currentIndex(), -1);
    QTest::keyClick(&control, Qt::Key_Right);
    QTest::mouseClick(&control, Qt::LeftButton, Qt::NoModifier, QPoint(40, control.height() / 2));
    QCOMPARE(control.currentIndex(), -1);

    // One, which is a pick that cannot move.
    control.setItems({QStringLiteral("Only")});
    QCOMPARE(control.count(), 1);
    QCOMPARE(control.currentIndex(), 0);
    QTest::keyClick(&control, Qt::Key_Right);
    QCOMPARE(control.currentIndex(), 0);
    QTest::keyClick(&control, Qt::Key_Home);
    QCOMPARE(control.currentIndex(), 0);
    QVERIFY(!control.grab().isNull());
}

TEST_ADD(tst_CupertinoSegmentedControl)

#include "tst_cupertinosegmentedcontrol.moc"
