/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxtheme/qxthememotion.h"

#include "qxmaterial/materialswitch.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QImage>

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

class tst_MaterialSwitch : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aClickTurnsItOn();
    void theTrackFillsWhenItIsOn();
    void theKeyboardTurnsIt();
    void theSentenceIsItsName();
    void whatAnAssistiveToolReads();
    void aSwitchNobodyCanFlipIsDrawnFainter();
    void noMotionCrossesAtOnce();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_MaterialSwitch::init()
{
    m_hadOverride = QxThemeManager::instance()->hasAccentOverride();
    QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialSwitch::cleanup()
{
    if (!m_hadOverride) {
        QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! A switch is not a check box: a click puts what it stands for in force now,
 *  and says so. */
void tst_MaterialSwitch::aClickTurnsItOn()
{
    MaterialSwitch materialSwitch;
    materialSwitch.resize(materialSwitch.sizeHint());
    QVERIFY(!materialSwitch.isOn());
    QSignalSpy flipped(&materialSwitch, &MaterialSwitch::toggled);

    QTest::mouseClick(&materialSwitch, Qt::LeftButton, Qt::NoModifier, QPoint(10, 16));
    QVERIFY(materialSwitch.isOn());
    QCOMPARE(flipped.count(), 1);

    QTest::mouseClick(&materialSwitch, Qt::LeftButton, Qt::NoModifier, QPoint(10, 16));
    QVERIFY(!materialSwitch.isOn());
    QCOMPARE(flipped.count(), 2);
}

/*! The track fills with the primary colour once it is on, which is the
 *  difference Material makes: the state is read off the track, not off how far
 *  along it the handle has got. */
void tst_MaterialSwitch::theTrackFillsWhenItIsOn()
{
    MaterialSwitch materialSwitch;
    materialSwitch.resize(materialSwitch.sizeHint());
    QCOMPARE(accentPixels(&materialSwitch), 0);

    materialSwitch.setOn(true);
    QVERIFY(accentPixels(&materialSwitch) > 0);
}

/*! Enter answers on the press the way a button does; Space answers on the
 *  release, so holding it down does not flip the switch over and over. */
void tst_MaterialSwitch::theKeyboardTurnsIt()
{
    MaterialSwitch materialSwitch;
    materialSwitch.resize(materialSwitch.sizeHint());

    QTest::keyClick(&materialSwitch, Qt::Key_Enter);
    QVERIFY(materialSwitch.isOn());
    QTest::keyClick(&materialSwitch, Qt::Key_Space);
    QVERIFY(!materialSwitch.isOn());
}

/*! The sentence beside a switch is its name for as long as the application
 *  has not given it a better one. */
void tst_MaterialSwitch::theSentenceIsItsName()
{
    MaterialSwitch materialSwitch;
    materialSwitch.setText(QStringLiteral("Use dark mode"));
    QCOMPARE(materialSwitch.accessibleName(), QStringLiteral("Use dark mode"));

    materialSwitch.setAccessibleName(QStringLiteral("Dark mode"));
    materialSwitch.setText(QStringLiteral("Use dark mode"));
    QCOMPARE(materialSwitch.accessibleName(), QStringLiteral("Dark mode"));
}

/*! What an assistive tool is told: a control that is on or off, and which of
 *  the two it is. */
void tst_MaterialSwitch::whatAnAssistiveToolReads()
{
    MaterialSwitch materialSwitch;
    materialSwitch.setOn(true);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&materialSwitch);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::CheckBox);
    QVERIFY(iface->state().checkable);
    QVERIFY(iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A switch nobody can flip is written a step down - the track and the handle,
 *  together. Pinned first, so the difference measured is the switch's own.
 */
void tst_MaterialSwitch::aSwitchNobodyCanFlipIsDrawnFainter()
{
    MaterialSwitch materialSwitch;
    materialSwitch.resize(materialSwitch.sizeHint());
    materialSwitch.show();
    qtcPinDisabledColours(&materialSwitch);

    const QImage live = materialSwitch.grab().toImage();
    materialSwitch.setEnabled(false);
    const QImage faded = materialSwitch.grab().toImage();
    QVERIFY(faded != live);

    // And it stays where it was: fading is not flipping.
    QVERIFY(!materialSwitch.isOn());
}

/*! The handle crosses on a Material duration, and no duration is a crossing that
 *  has already happened: the switch stands in its new state at once.
 */
void tst_MaterialSwitch::noMotionCrossesAtOnce()
{
    const bool was = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    MaterialSwitch materialSwitch;
    materialSwitch.resize(materialSwitch.sizeHint());
    materialSwitch.show();
    QTest::qWait(120);
    const QImage off = materialSwitch.grab().toImage();
    materialSwitch.setOn(true);
    const QImage justAfter = materialSwitch.grab().toImage();
    QVERIFY(justAfter != off);
    QTest::qWait(200);
    QVERIFY(materialSwitch.grab().toImage() == justAfter);

    QxTheme::setMotionReduced(was);
}

TEST_ADD(tst_MaterialSwitch)

#include "tst_materialswitch.moc"
