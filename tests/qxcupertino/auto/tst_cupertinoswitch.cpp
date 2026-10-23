/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinoswitch.h"
#include "qxcupertino/cupertinotokens.h"

#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QElapsedTimer>
#include <QImage>
#include <QSignalSpy>

QX_CUPERTINO_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// The knob's two resting places in a switch sized to its track: two in from
// either end, twenty-seven across, so its middle is at 15 and at 35.
constexpr int kOffCenter = 15;
constexpr int kOnCenter = 35;

// Longer than the longest motion tier, so a travel that is happening has
// finished by the time the pixels are read.
constexpr int kSettleWait = 500;

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! A point on the track the knob never covers, in either resting place or
 *  anywhere between: the capsule's top edge is straight over its middle, and
 *  the knob is a circle that stops two in from it. Where the knob is not, the
 *  fill is - which is the one pixel worth reading in every state. */
QColor fillPixel(QWidget *widget)
{
    return pixel(widget, 25, 1);
}

bool isGreen(const QColor &colour)
{
    return colour.green() > colour.red() + 40 && colour.green() > colour.blue() + 40;
}

bool isGrey(const QColor &colour)
{
    // A grey has one value, not four: the three channels sit close together.
    // Nothing is said about how light it is, because the same fill is drawn on
    // either face of the palette: the off colour is the system's grey thinned
    // over the page, so it is pale on a light page and dark on a dark one.
    const int spread = qMax(qMax(colour.red(), colour.green()), colour.blue()) -
                       qMin(qMin(colour.red(), colour.green()), colour.blue());
    return spread <= 8;
}

bool closeTo(const QColor &a, const QColor &b)
{
    // The knob's own shadow falls on the track beside it, so a pixel on the
    // fill is the fill to within a couple of units - and a blend of the two
    // fill colours is tens of units away, which is the difference this is
    // here to tell apart.
    constexpr int kTolerance = 10;
    return qAbs(a.red() - b.red()) <= kTolerance && qAbs(a.green() - b.green()) <= kTolerance &&
           qAbs(a.blue() - b.blue()) <= kTolerance;
}
}   // namespace

class tst_CupertinoSwitch : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void offIsAPaleTrackWithTheKnobAtOneEnd();
    void onTurnsTheTrackGreenAndMovesTheKnob();
    void theTrackTurnsAtTheMiddleAndNotBefore();
    void reducedMotionPutsTheKnobThereAtOnce();
    void aClickToggles();
    void theEmptyRoomBesideTheTrackIsNotTheControl();
    void theKeyboardToggles();
    void whatItIsAndWhetherItIsOn();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
    qreal m_wasScale = 1.0;
};

void tst_CupertinoSwitch::cleanup()
{
    // The motion switch, the scale and the accessible registry are all
    // process-wide.
    QxTheme::setMotionReduced(m_wasReduced);
    QxTheme::setMotionScale(m_wasScale);
    QAccessible::setActive(m_wasActive);
}

/*! What off looks like: a pale grey capsule with a white knob at the left end
 *  and the off colour showing where the knob has not been. */
void tst_CupertinoSwitch::offIsAPaleTrackWithTheKnobAtOneEnd()
{
    CupertinoSwitch sw;
    QCOMPARE(sw.sizeHint(), QSize(51, 31));
    sw.resize(51, 31);

    QVERIFY(!sw.isOn());
    const QColor knob = pixel(&sw, kOffCenter, 15);
    const QColor track = fillPixel(&sw);

    // The knob is white on both faces of the theme; the track behind it is a
    // grey, not the accent and not the green.
    QVERIFY(knob.red() > 240 && knob.green() > 240 && knob.blue() > 240);
    QVERIFY(isGrey(track));
    QVERIFY(!isGreen(track));
}

/*! Turning it on turns the whole track green and carries the knob to the far
 *  end over this family's normal duration. */
void tst_CupertinoSwitch::onTurnsTheTrackGreenAndMovesTheKnob()
{
    CupertinoSwitch sw;
    sw.resize(51, 31);
    sw.setOn(true);
    QVERIFY(sw.isOn());
    QTest::qWait(kSettleWait);

    // The knob arrived: the far end is white now and the near end shows the
    // track.
    const QColor knob = pixel(&sw, kOnCenter, 15);
    QVERIFY(knob.red() > 240 && knob.green() > 240 && knob.blue() > 240);
    QVERIFY(isGreen(fillPixel(&sw)));
    QVERIFY(isGreen(pixel(&sw, 5, 15)));

    // And it goes back the same way.
    sw.setOn(false);
    QTest::qWait(kSettleWait);
    QVERIFY(isGrey(fillPixel(&sw)));
    QVERIFY(pixel(&sw, kOffCenter, 15).red() > 240);
}

/*! The distinctive half of the drawing: the fill turns over once, when the knob
 *  crosses the middle, and is never a blend of the two colours in between.
 *
 *  A slow travel is used so the samples land well inside it - the twentyfold
 *  scale turns the normal tier into seconds - and every sample is compared with
 *  one of the two rest colours, so a track that faded from grey to green as the
 *  knob moved would fail here even though both ends looked right. */
void tst_CupertinoSwitch::theTrackTurnsAtTheMiddleAndNotBefore()
{
    m_wasScale = QxTheme::motionScale();
    QxTheme::setMotionScale(20.0);

    CupertinoSwitch sw;
    sw.resize(51, 31);
    const QColor offFill = fillPixel(&sw);

    // What the fill looks like once the knob has arrived, taken from a switch
    // that was never in motion.
    QxTheme::setMotionReduced(true);
    CupertinoSwitch settled;
    settled.resize(51, 31);
    settled.setOn(true);
    const QColor onFill = fillPixel(&settled);
    QxTheme::setMotionReduced(m_wasReduced);

    const int travel = CupertinoMotion::forDuration(CupertinoMotion::Duration::Normal).duration;
    QVERIFY(travel > 4000);

    QElapsedTimer clock;
    clock.start();
    sw.setOn(true);

    // Walk through the travel: every sample is one of the two rest colours, and
    // the one it changes to is the green.
    int turnedAt = -1;
    while (clock.elapsed() < travel + 200) {
        QTest::qWait(25);
        const QColor fill = fillPixel(&sw);
        if (closeTo(fill, onFill)) {
            turnedAt = int(clock.elapsed());
            break;
        }
        QVERIFY(closeTo(fill, offFill));
    }

    // The change happens where the knob crosses the middle - half of a travel
    // whose easing is symmetric, so half of its duration - with room for the
    // sampling step on either side.
    QVERIFY(turnedAt > 0);
    QVERIFY(turnedAt > travel * 4 / 10);
    QVERIFY(turnedAt < travel * 6 / 10);
}

/*! The reduce switch is honoured by the travel without the control asking
 *  about it: no duration means the knob is simply there. */
void tst_CupertinoSwitch::reducedMotionPutsTheKnobThereAtOnce()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    CupertinoSwitch sw;
    sw.resize(51, 31);
    sw.setOn(true);
    // No wait: a reduced travel is not a travel.
    QVERIFY(pixel(&sw, kOnCenter, 15).red() > 240);
    QVERIFY(isGreen(pixel(&sw, 5, 15)));

    sw.setOn(false);
    QVERIFY(pixel(&sw, kOffCenter, 15).red() > 240);
    QVERIFY(isGrey(fillPixel(&sw)));
}

/*! A press and its release on the track is a click, and a click toggles. */
void tst_CupertinoSwitch::aClickToggles()
{
    CupertinoSwitch sw;
    sw.resize(51, 31);
    QSignalSpy toggled(&sw, &CupertinoSwitch::toggled);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOffCenter, 15));
    QVERIFY(sw.isOn());
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.at(0).at(0).toBool(), true);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOnCenter, 15));
    QVERIFY(!sw.isOn());
    QCOMPARE(toggled.count(), 2);
}

/*! A switch given a wider cell than it needs is still the capsule and only the
 *  capsule: the room beside the track does not toggle it. */
void tst_CupertinoSwitch::theEmptyRoomBesideTheTrackIsNotTheControl()
{
    CupertinoSwitch sw;
    sw.resize(120, 31);
    QSignalSpy toggled(&sw, &CupertinoSwitch::toggled);

    // The track is centred, so it spans 34 to 85; five is the page.
    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(5, 15));
    QVERIFY(!sw.isOn());
    QCOMPARE(toggled.count(), 0);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(60, 15));
    QVERIFY(sw.isOn());
    QCOMPARE(toggled.count(), 1);
}

/*! Space and Enter are the two keys a switch answers; Space answers when it
 *  is let go, which is why a key click - press and release - is one toggle. */
void tst_CupertinoSwitch::theKeyboardToggles()
{
    CupertinoSwitch sw;
    sw.resize(51, 31);
    sw.setFocus();

    QTest::keyClick(&sw, Qt::Key_Space);
    QVERIFY(sw.isOn());

    QTest::keyClick(&sw, Qt::Key_Enter);
    QVERIFY(!sw.isOn());
}

/*! What it is, and what is true of it, are answered once by the base. */
void tst_CupertinoSwitch::whatItIsAndWhetherItIsOn()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    CupertinoSwitch sw;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&sw);
    QVERIFY(iface);
    QVERIFY(iface->state().checkable);
    QVERIFY(!iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    sw.setOn(true);
    QAccessibleInterface *onIface = QAccessible::queryAccessibleInterface(&sw);
    QVERIFY(onIface);
    QVERIFY(onIface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(onIface));
}

TEST_ADD(tst_CupertinoSwitch)

#include "tst_cupertinoswitch.moc"
