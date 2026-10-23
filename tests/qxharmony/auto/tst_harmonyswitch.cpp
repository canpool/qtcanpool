/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxharmony/harmonyswitch.h"
#include "qxharmony/harmonytokens.h"

#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QElapsedTimer>
#include <QImage>
#include <QSignalSpy>

QX_HARMONY_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// The switch this suite drives: a 46 x 26 track with a 22 knob two in from
// either end, so the knob rests at 2 and at 22 along the track and its middle
// row is thirteen down.
constexpr int kOffCenterX = 13;
constexpr int kOnCenterX = 33;
constexpr int kMiddleRow = 13;

// Longer than the longest motion tier, so a travel that is happening has
// finished by the time the pixels are read.
constexpr int kSettleWait = 600;

QImage shot(QWidget *widget)
{
    return widget->grab().toImage();
}

/*! A point on the track the knob never covers, in either resting place or
 *  anywhere between: the capsule's top edge is straight over its middle, and
 *  the knob is a circle that stops two in from it. */
QColor fillPixel(QWidget *widget)
{
    return shot(widget).pixelColor(23, 1);
}

/*! How bright the brightest patch on the control's middle row is. The knob is
 *  white and everything else there - the grey or blue track, the shadow under
 *  the knob, the wash a held control adds over both - is several steps below
 *  it, on either face of the palette. Reading the row against its own brightest
 *  pixel is what makes the same code work on a light page and a dark one. */
int rowBrightest(const QImage &image)
{
    int brightest = 0;
    for (int x = 0; x < image.width(); ++x) {
        brightest = qMax(brightest, image.pixelColor(x, kMiddleRow).lightness());
    }
    return brightest;
}

bool isKnobAt(QWidget *widget, int x)
{
    const QImage image = shot(widget);
    return image.pixelColor(x, kMiddleRow).lightness() >= rowBrightest(image) - 12;
}

/*! Where the knob's left edge is on the middle row, or -1 if it is not there.
 *  The travel is read this way rather than at a fixed probe because the spring
 *  takes the knob past the room it rests in, which is the point of the test. */
int knobLeft(QWidget *widget)
{
    const QImage image = shot(widget);
    const int brightest = rowBrightest(image);
    for (int x = 0; x < image.width(); ++x) {
        if (image.pixelColor(x, kMiddleRow).lightness() >= brightest - 12) {
            return x;
        }
    }
    return -1;
}

/*! How many pixels of the middle row the knob covers - its width in place. */
int knobWidth(QWidget *widget)
{
    const QImage image = shot(widget);
    const int brightest = rowBrightest(image);
    int count = 0;
    for (int x = 0; x < image.width(); ++x) {
        if (image.pixelColor(x, kMiddleRow).lightness() >= brightest - 12) {
            ++count;
        }
    }
    return count;
}

bool isBlue(const QColor &colour)
{
    return colour.blue() > colour.red() + 40 && colour.blue() > colour.green() + 40;
}

bool isGrey(const QColor &colour)
{
    // The off colour is the family's grey thinned over the page, so it has no
    // great claim to being one value; what matters is that it is not the blue.
    const int spread = qMax(qMax(colour.red(), colour.green()), colour.blue()) -
                       qMin(qMin(colour.red(), colour.green()), colour.blue());
    return spread <= 12;
}
}   // namespace

class tst_HarmonySwitch : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void offIsAPaleTrackWithTheKnobAtOneEnd();
    void onTurnsTheTrackBlueAndMovesTheKnob();
    void theKnobPassesItsEndAndSettlesBack();
    void theHeldKnobStretchesIntoTheMediumCorner();
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

void tst_HarmonySwitch::cleanup()
{
    // The motion switch, the scale and the accessible registry are all
    // process-wide.
    QxTheme::setMotionReduced(m_wasReduced);
    QxTheme::setMotionScale(m_wasScale);
    QAccessible::setActive(m_wasActive);
}

/*! What off looks like: a pale capsule with the knob at the left end and the
 *  off colour showing where the knob has not been. */
void tst_HarmonySwitch::offIsAPaleTrackWithTheKnobAtOneEnd()
{
    HarmonySwitch sw;
    QCOMPARE(sw.sizeHint(), QSize(46, 26));
    sw.resize(46, 26);

    QVERIFY(!sw.isOn());
    const QColor track = fillPixel(&sw);

    QVERIFY(isKnobAt(&sw, kOffCenterX));
    QVERIFY(isGrey(track));
    QVERIFY(!isBlue(track));
}

/*! Turning it on turns the whole track the brand's blue and carries the knob to
 *  the far end over this family's normal duration. */
void tst_HarmonySwitch::onTurnsTheTrackBlueAndMovesTheKnob()
{
    HarmonySwitch sw;
    sw.resize(46, 26);
    sw.setOn(true);
    QVERIFY(sw.isOn());
    QTest::qWait(kSettleWait);

    QVERIFY(isKnobAt(&sw, kOnCenterX));
    QVERIFY(isBlue(fillPixel(&sw)));
    QVERIFY(isBlue(shot(&sw).pixelColor(5, kMiddleRow)));

    // And it goes back the same way.
    sw.setOn(false);
    QTest::qWait(kSettleWait);
    QVERIFY(isGrey(fillPixel(&sw)));
    QVERIFY(isKnobAt(&sw, kOffCenterX));
}

/*! The distinctive half of the drawing, and the opposite assertion to the other
 *  family's switch: the knob does not stop at its end, it passes it and comes
 *  back.
 *
 *  A slow travel is used so the samples land well inside it - the twentyfold
 *  scale turns the normal tier into seconds - and the knob's left edge is read
 *  every frame, so the spring is measured rather than asserted about. */
void tst_HarmonySwitch::theKnobPassesItsEndAndSettlesBack()
{
    m_wasScale = QxTheme::motionScale();
    QxTheme::setMotionScale(20.0);

    // Where the knob rests once it has arrived, taken from a switch that was
    // never in motion.
    QxTheme::setMotionReduced(true);
    HarmonySwitch settled;
    settled.resize(46, 26);
    settled.setOn(true);
    const int atRest = knobLeft(&settled);
    QxTheme::setMotionReduced(m_wasReduced);
    QVERIFY(atRest > 0);

    const int travel = HarmonyMotion::forDuration(HarmonyMotion::Duration::Normal).duration;
    QVERIFY(travel > 4000);

    HarmonySwitch sw;
    sw.resize(46, 26);

    QElapsedTimer clock;
    clock.start();
    sw.setOn(true);

    int furthest = atRest;
    while (clock.elapsed() < travel + 200) {
        QTest::qWait(20);
        const int left = knobLeft(&sw);
        QVERIFY(left > 0);
        furthest = qMax(furthest, left);
    }

    // The spring took the knob past the place it rests...
    QVERIFY(furthest >= atRest + 1);
    // ...and brought it back there.
    QCOMPARE(knobLeft(&sw), atRest);
}

/*! Held, the knob is a rounded rectangle of the family's medium corner rather
 *  than a circle - one state, not a shape for every distance travelled. */
void tst_HarmonySwitch::theHeldKnobStretchesIntoTheMediumCorner()
{
    HarmonySwitch sw;
    sw.resize(46, 26);
    const int round = knobWidth(&sw);

    QTest::mousePress(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOffCenterX, kMiddleRow));
    const int held = knobWidth(&sw);
    QTest::mouseRelease(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOffCenterX, kMiddleRow));

    QVERIFY(held > round);
    // The knob grows by six and stays inside the capsule while it does.
    QCOMPARE(held - round, 6);

    // Let go, it is a circle again.
    QTest::qWait(kSettleWait);
    QCOMPARE(knobWidth(&sw), round);
}

/*! The reduce switch is honoured by the travel without the control asking
 *  about it: no duration means the knob is simply there - and no spring, since
 *  a spring is a journey. */
void tst_HarmonySwitch::reducedMotionPutsTheKnobThereAtOnce()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    HarmonySwitch sw;
    sw.resize(46, 26);
    sw.setOn(true);
    // No wait: a reduced travel is not a travel.
    QVERIFY(isKnobAt(&sw, kOnCenterX));
    QVERIFY(isBlue(shot(&sw).pixelColor(5, kMiddleRow)));

    sw.setOn(false);
    QVERIFY(isKnobAt(&sw, kOffCenterX));
    QVERIFY(isGrey(fillPixel(&sw)));
}

/*! A press and its release on the track is a click, and a click toggles. */
void tst_HarmonySwitch::aClickToggles()
{
    HarmonySwitch sw;
    sw.resize(46, 26);
    QSignalSpy toggled(&sw, &HarmonySwitch::toggled);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOffCenterX, kMiddleRow));
    QVERIFY(sw.isOn());
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.at(0).at(0).toBool(), true);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(kOnCenterX, kMiddleRow));
    QVERIFY(!sw.isOn());
    QCOMPARE(toggled.count(), 2);
}

/*! A switch given a wider cell than it needs is still the capsule and only the
 *  capsule: the room beside the track does not toggle it. */
void tst_HarmonySwitch::theEmptyRoomBesideTheTrackIsNotTheControl()
{
    HarmonySwitch sw;
    sw.resize(120, 26);
    QSignalSpy toggled(&sw, &HarmonySwitch::toggled);

    // The track is centred, so it spans 37 to 83; five is the page.
    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(5, kMiddleRow));
    QVERIFY(!sw.isOn());
    QCOMPARE(toggled.count(), 0);

    QTest::mouseClick(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(60, kMiddleRow));
    QVERIFY(sw.isOn());
    QCOMPARE(toggled.count(), 1);
}

/*! Space and Enter are the two keys a switch answers; Space answers when it
 *  is let go, which is why a key click - press and release - is one toggle. */
void tst_HarmonySwitch::theKeyboardToggles()
{
    HarmonySwitch sw;
    sw.resize(46, 26);
    sw.setFocus();

    QTest::keyClick(&sw, Qt::Key_Space);
    QVERIFY(sw.isOn());

    QTest::keyClick(&sw, Qt::Key_Enter);
    QVERIFY(!sw.isOn());
}

/*! What it is, and what is true of it, are answered once by the base. */
void tst_HarmonySwitch::whatItIsAndWhetherItIsOn()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    HarmonySwitch sw;
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

TEST_ADD(tst_HarmonySwitch)

#include "tst_harmonyswitch.moc"
