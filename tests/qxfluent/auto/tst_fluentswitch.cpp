/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentswitch.h"

#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// The knob's two resting places, in a switch sized to its track: four in from
// either end, twelve across, so its middle is at 10 and at 30.
constexpr int kOffCenter = 10;
constexpr int kOnCenter = 30;

// Longer than the longest motion tier, so a travel that is happening has
// finished by the time the pixels are read.
constexpr int kSettleWait = 500;

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}
}   // namespace

class tst_FluentSwitch : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void offIsThePageWithAKnobAtOneEnd();
    void onFillsTheTrackAndMovesTheKnob();
    void reducedMotionPutsTheKnobThereAtOnce();
    void aClickToggles();
    void aSwitchNobodyCanUseStaysPut();
    void theKeyboardToggles();
    void theLabelIsTheAccessibleName();
    void whatItIsAndWhetherItIsOn();
    void theLabelWidensTheHint();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentSwitch::cleanup()
{
    // The motion switch is process-wide and the accessible registry is global.
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! What off looks like: an outlined track the colour of the page, with the
 *  knob at the left end in the ink of the page. */
void tst_FluentSwitch::offIsThePageWithAKnobAtOneEnd()
{
    FluentSwitch sw;
    sw.resize(40, 20);

    QVERIFY(!sw.isOn());
    const QColor knob = pixel(&sw, kOffCenter, 10);
    const QColor empty = pixel(&sw, kOnCenter, 10);

    // The knob is the ink; what it has not reached yet is the page.
    QVERIFY(knob.red() < 128);
    QVERIFY(empty.red() > 200);
    QVERIFY(empty.green() > 200);
}

/*! Turning it on fills the track with the accent and carries the knob to the
 *  far end, over a Fluent duration. */
void tst_FluentSwitch::onFillsTheTrackAndMovesTheKnob()
{
    FluentSwitch sw;
    sw.resize(40, 20);
    const QColor wasEmpty = pixel(&sw, kOnCenter, 10);

    sw.setOn(true);
    QVERIFY(sw.isOn());
    QTest::qWait(kSettleWait);

    // Where the knob was not, it now is: the knob's ink is the accent's pair.
    const QColor knob = pixel(&sw, kOnCenter, 10);
    QVERIFY(knob.red() > 200);
    QVERIFY(knob.green() > 200);
    QVERIFY(knob.blue() > 200);

    // And where the knob was, the fill it left behind is not the page any more.
    QVERIFY(pixel(&sw, kOffCenter, 10) != wasEmpty);
}

/*! The reduce switch is honoured by the travel without the control asking
 *  about it: no duration means the knob is simply there. */
void tst_FluentSwitch::reducedMotionPutsTheKnobThereAtOnce()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    FluentSwitch sw;
    sw.resize(40, 20);
    sw.setOn(true);
    // No wait: a reduced travel is not a travel.
    const QColor knob = pixel(&sw, kOnCenter, 10);
    QVERIFY(knob.red() > 200);
    QVERIFY(knob.green() > 200);
    QVERIFY(knob.blue() > 200);
}

/*! A press and its release on the control is a click, and a click toggles. */
void tst_FluentSwitch::aClickToggles()
{
    FluentSwitch sw;
    sw.resize(40, 20);
    QSignalSpy toggled(&sw, &FluentSwitch::toggled);

    QTest::mousePress(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
    QTest::mouseRelease(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));

    QVERIFY(sw.isOn());
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.at(0).at(0).toBool(), true);
}

/*! Space and Enter are the two keys a switch answers; Space answers when it
 *  is let go, which is why a key click - press and release - is one toggle. */
void tst_FluentSwitch::theKeyboardToggles()
{
    FluentSwitch sw;
    sw.resize(40, 20);
    sw.setFocus();

    QTest::keyClick(&sw, Qt::Key_Space);
    QVERIFY(sw.isOn());

    QTest::keyClick(&sw, Qt::Key_Enter);
    QVERIFY(!sw.isOn());
}

/*! The sentence beside the track is what the control is called, for as long as
 *  the application has not named it something else. */
void tst_FluentSwitch::theLabelIsTheAccessibleName()
{
    FluentSwitch sw;
    sw.setText(QStringLiteral("Wi-Fi"));
    QCOMPARE(sw.text(), QStringLiteral("Wi-Fi"));
    QCOMPARE(sw.accessibleName(), QStringLiteral("Wi-Fi"));

    // A name the application gives wins, and survives the next label.
    sw.setAccessibleName(QStringLiteral("Wireless networking"));
    sw.setText(QStringLiteral("Wireless"));
    QCOMPARE(sw.accessibleName(), QStringLiteral("Wireless networking"));
}

/*! What it is, and what is true of it, are answered once by the base. */
void tst_FluentSwitch::whatItIsAndWhetherItIsOn()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentSwitch sw;
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

/*! A switch with a sentence asks for the room the sentence needs. */
void tst_FluentSwitch::theLabelWidensTheHint()
{
    FluentSwitch sw;
    const QSize bare = sw.sizeHint();
    sw.setText(QStringLiteral("Wi-Fi"));
    const QSize labelled = sw.sizeHint();

    QVERIFY(labelled.width() > bare.width());
    QCOMPARE(labelled.height(), bare.height());
}

/*! A switch nobody can use stays where it is: the click is still taken and
 *  is not an answer. */
void tst_FluentSwitch::aSwitchNobodyCanUseStaysPut()
{
    FluentSwitch sw;
    sw.resize(40, 20);
    const QImage live = sw.grab().toImage();

    // Qt fades a disabled widget through its palette; pinned, the only thing
    // left that can change is what the control itself paints.
    qtcPinDisabledColours(&sw);

    sw.setEnabled(false);
    QSignalSpy toggled(&sw, &FluentSwitch::toggled);
    QTest::mousePress(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
    QTest::mouseRelease(&sw, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
    QTest::keyClick(&sw, Qt::Key_Space);

    QVERIFY(!sw.isOn());
    QCOMPARE(toggled.count(), 0);
    QVERIFY(sw.grab().toImage() != live);
}

TEST_ADD(tst_FluentSwitch)

#include "tst_fluentswitch.moc"
