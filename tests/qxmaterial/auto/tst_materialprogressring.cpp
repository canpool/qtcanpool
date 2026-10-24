/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialprogressring.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QImage>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

int accentPixels(QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) == kAccent) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_MaterialProgressRing : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void itSaysSomethingIsHappeningUntilItIsToldHowFarAlong();
    void howFarAlongIsDrawnAsAnArcOverATrack();
    void theArcGrowsAndShrinksAsItGoesRound();
    void whatAnAssistiveToolReads();
    void aRingNobodyCanUseIsDrawnFainter();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_MaterialProgressRing::init()
{
    m_hadOverride = QxThemeManager::instance()->hasAccentOverride();
    QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialProgressRing::cleanup()
{
    if (!m_hadOverride) {
        QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! Until it knows how far along something is, all it can say is that something
 *  is happening - which is the reading it starts in. */
void tst_MaterialProgressRing::itSaysSomethingIsHappeningUntilItIsToldHowFarAlong()
{
    MaterialProgressRing ring;
    ring.resize(ring.sizeHint());
    QVERIFY(ring.isIndeterminate());
    QCOMPARE(ring.value(), 0);

    // The two modes are told apart by being told: how far along something is
    // does not stop the ring from being the kind that is waiting.
    ring.setIndeterminate(false);
    QVERIFY(!ring.isIndeterminate());
    ring.setValue(40);
    QCOMPARE(ring.value(), 40);

    // How far along is a fraction, so there is no "further than finished".
    ring.setValue(140);
    QCOMPARE(ring.value(), 100);
    ring.setValue(-5);
    QCOMPARE(ring.value(), 0);

    ring.setIndeterminate(true);
    QVERIFY(ring.isIndeterminate());
}

/*! How far along is an arc with an end, over the track it has not reached: the
 *  two are the same ink, and the track is the thinner one. */
void tst_MaterialProgressRing::howFarAlongIsDrawnAsAnArcOverATrack()
{
    MaterialProgressRing ring;
    ring.resize(ring.sizeHint());
    ring.setIndeterminate(false);
    const int empty = accentPixels(&ring);

    ring.setValue(50);
    const int half = accentPixels(&ring);
    QVERIFY(half > empty);

    ring.setValue(100);
    QVERIFY(accentPixels(&ring) > half);
}

/*! An arc that only says something is happening has no end, and its length is
 *  not a length: it grows and shrinks as it goes round, which is one movement
 *  rather than a fixed arc turning. */
void tst_MaterialProgressRing::theArcGrowsAndShrinksAsItGoesRound()
{
    MaterialProgressRing ring;
    ring.resize(ring.sizeHint());
    QVERIFY(ring.isIndeterminate());

    // A sweep is not a transition, so it does not run on the ladder's clock:
    // it runs until it is told to stop, and what is measured is that the arc
    // it draws is not the same one twice.
    const int first = accentPixels(&ring);
    QTest::qWait(400);
    const int second = accentPixels(&ring);
    QTest::qWait(400);
    const int third = accentPixels(&ring);
    QVERIFY(second != first || third != second);
}

/*! What an assistive tool is told: how far along is a number, and it can be
 *  read back and set back the way a slider's can. */
void tst_MaterialProgressRing::whatAnAssistiveToolReads()
{
    MaterialProgressRing ring;
    ring.setValue(40);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&ring);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ProgressBar);

    QAccessibleValueInterface *value = iface->valueInterface();
    QVERIFY(value);
    QCOMPARE(value->currentValue().toInt(), 40);
    QCOMPARE(value->minimumValue().toInt(), 0);
    QCOMPARE(value->maximumValue().toInt(), 100);

    QSignalSpy changed(&ring, &MaterialProgressRing::valueChanged);
    value->setCurrentValue(70);
    QCOMPARE(ring.value(), 70);
    QCOMPARE(changed.count(), 1);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A ring nobody can use is written a step down, for the reason the bar's case
 *  gives: the track and the arc that says how far along it is.
 */
void tst_MaterialProgressRing::aRingNobodyCanUseIsDrawnFainter()
{
    MaterialProgressRing ring;
    ring.resize(ring.sizeHint());
    ring.setIndeterminate(false);
    ring.setValue(40);
    ring.show();
    qtcPinDisabledColours(&ring);

    const QImage live = ring.grab().toImage();
    ring.setEnabled(false);
    const QImage faded = ring.grab().toImage();
    QVERIFY(faded != live);
}

TEST_ADD(tst_MaterialProgressRing)

#include "tst_materialprogressring.moc"
