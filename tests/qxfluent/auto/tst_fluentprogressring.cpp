/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentprogressring.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QImage>

QX_FLUENT_USE_NAMESPACE
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

class tst_FluentProgressRing : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aRingWithAnEndDrawsHowFarAlongItIs();
    void anEndlessRingTurns();
    void reduceMotionStopsTheTurning();
    void aRingNobodyCanUseHoldsStill();
    void howFarAlongIsThereForAnAssistiveTool();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentProgressRing::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasReduced = QxTheme::isMotionReduced();
    m_wasActive = QAccessible::isActive();
}

void tst_FluentProgressRing::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! Determinate is an arc with an end, and the end is the answer: half the
 *  range is half the ring. */
void tst_FluentProgressRing::aRingWithAnEndDrawsHowFarAlongItIs()
{
    FluentProgressRing ring;
    ring.resize(32, 32);
    ring.setIndeterminate(false);
    ring.setValue(0);
    QCOMPARE(accentPixels(&ring), 0);

    ring.setValue(25);
    const int quarter = accentPixels(&ring);
    QVERIFY(quarter > 0);

    ring.setValue(75);
    QVERIFY(accentPixels(&ring) > quarter);
}

/*! Indeterminate is an arc without an end: what it says is that the waiting
 *  has not stopped, and it says it by turning. */
void tst_FluentProgressRing::anEndlessRingTurns()
{
    FluentProgressRing ring;
    ring.resize(32, 32);
    QVERIFY(ring.isIndeterminate());

    QTest::qWait(100);
    const QImage before = ring.grab().toImage();
    QTest::qWait(400);
    const QImage after = ring.grab().toImage();

    QVERIFY(before != after);
}

/*! The turning is the one motion here that is not a transition, and it is what
 *  the reduce-motion switch is about: asked for less motion, a ring stops. */
void tst_FluentProgressRing::reduceMotionStopsTheTurning()
{
    QxTheme::setMotionReduced(true);

    FluentProgressRing ring;
    ring.resize(32, 32);
    ring.setIndeterminate(true);

    const QImage before = ring.grab().toImage();
    QTest::qWait(400);
    QCOMPARE(ring.grab().toImage(), before);
}

/*! How far along is a number, and an assistive tool reads it the way it reads
 *  a slider's - and sets it back the same way. */
void tst_FluentProgressRing::howFarAlongIsThereForAnAssistiveTool()
{
    QAccessible::setActive(true);
    FluentProgressRing ring;
    ring.setIndeterminate(false);

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&ring);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ProgressBar);

    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->minimumValue().toInt(), 0);
    QCOMPARE(value->maximumValue().toInt(), 100);

    value->setCurrentValue(40);
    QCOMPARE(ring.value(), 40);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A ring nobody can use holds still, for the reason the flat bar does: the
 *  turn is a claim, and a control that cannot be used makes none. */
void tst_FluentProgressRing::aRingNobodyCanUseHoldsStill()
{
    FluentProgressRing ring;
    ring.resize(32, 32);
    ring.setIndeterminate(true);
    ring.setEnabled(false);

    const QImage before = ring.grab().toImage();
    QTest::qWait(400);
    QCOMPARE(ring.grab().toImage(), before);
}

TEST_ADD(tst_FluentProgressRing)

#include "tst_fluentprogressring.moc"
