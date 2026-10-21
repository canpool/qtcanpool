/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentslider.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QMouseEvent>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A slider sized to its hint: a rail from 8 to 112, so the thumb's middle
// stands at 8, at 60 and at 112 for the ends and the middle of the range.
constexpr int kLeft = 8;
constexpr int kMiddle = 60;
constexpr int kRight = 112;

constexpr int kSettleWait = 500;

/*! The accent, pinned: a pixel probe that depends on the platform's accent
 *  depends on the machine it runs on. */
const QColor kAccent(0, 0, 255);

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! A move with the button still down, sent straight at the widget: QTest's own
 *  move carries no buttons, which is a pointer wandering over the control
 *  rather than a drag holding it. */
void sendDrag(QWidget *widget, const QPoint &pos)
{
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::NoButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}
}   // namespace

class tst_FluentSlider : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theValueFollowsThePointer();
    void aDragCarriesTheThumb();
    void theKeyboardWalksTheRange();
    void theRailFillsUpToTheThumb();
    void ticksAppearWhenTheRailIsDivided();
    void theValueIsThereForAnAssistiveTool();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentSlider::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasReduced = QxTheme::isMotionReduced();
    m_wasActive = QAccessible::isActive();
}

void tst_FluentSlider::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! A press on the rail is a request to move there: a slider that waited for a
 *  drag would be lying about where the pointer is. */
void tst_FluentSlider::theValueFollowsThePointer()
{
    FluentSlider slider;
    slider.resize(120, 24);
    QSignalSpy changed(&slider, &FluentSlider::valueChanged);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kMiddle, 12));

    QVERIFY(qAbs(slider.value() - 50) <= 2);
    QCOMPARE(changed.count(), 1);
}

/*! Holding and moving takes the thumb along, and letting go leaves it there. */
void tst_FluentSlider::aDragCarriesTheThumb()
{
    FluentSlider slider;
    slider.resize(120, 24);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kLeft + 12, 12));
    sendDrag(&slider, QPoint(kRight - 12, 12));
    const int dragged = slider.value();
    QVERIFY(dragged > 80);

    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kRight - 12, 12));
    QCOMPARE(slider.value(), dragged);
}

/*! The arrow keys step, PageUp and PageDown jump, and Home and End are the
 *  two ends - the keyboard vocabulary a slider is expected to answer. */
void tst_FluentSlider::theKeyboardWalksTheRange()
{
    FluentSlider slider;
    slider.resize(120, 24);
    slider.setValue(50);

    QTest::keyClick(&slider, Qt::Key_Left);
    QCOMPARE(slider.value(), 49);
    QTest::keyClick(&slider, Qt::Key_Right);
    QCOMPARE(slider.value(), 50);
    QTest::keyClick(&slider, Qt::Key_PageUp);
    QCOMPARE(slider.value(), 60);
    QTest::keyClick(&slider, Qt::Key_PageDown);
    QCOMPARE(slider.value(), 50);
    QTest::keyClick(&slider, Qt::Key_Home);
    QCOMPARE(slider.value(), slider.minimum());
    QTest::keyClick(&slider, Qt::Key_End);
    QCOMPARE(slider.value(), slider.maximum());
}

/*! On is not a state a slider has: what it has is how far along the rail the
 *  accent reaches, and the thumb at the end of it. */
void tst_FluentSlider::theRailFillsUpToTheThumb()
{
    FluentSlider slider;
    slider.resize(120, 24);

    // At the near end the far end of the rail is bare, and nothing is there.
    const QColor wasBare = pixel(&slider, kRight, 6);
    QVERIFY(wasBare.red() > 200);

    slider.setValue(slider.maximum());
    QTest::qWait(kSettleWait);

    // At the far end the thumb is there, in the accent.
    const QColor thumb = pixel(&slider, kRight, 6);
    QCOMPARE(thumb, kAccent);
}

/*! The marks are Fluent's way of saying the rail is divided: without an
 *  interval there is nothing under it, with one there is a mark at every
 *  step of it. */
void tst_FluentSlider::ticksAppearWhenTheRailIsDivided()
{
    FluentSlider slider;
    slider.resize(120, 24);

    const QImage plain = slider.grab().toImage();
    QCOMPARE(plain.pixelColor(kMiddle, 18), plain.pixelColor(kMiddle + 6, 18));

    slider.setTickInterval(25);
    const QImage ticked = slider.grab().toImage();
    QVERIFY(ticked.pixelColor(kMiddle, 18) != ticked.pixelColor(kMiddle + 6, 18));
}

/*! A slider's answer is a number, and an assistive tool wants to read it and
 *  to set it back - which a role and a state cannot carry. */
void tst_FluentSlider::theValueIsThereForAnAssistiveTool()
{
    QAccessible::setActive(true);
    FluentSlider slider;

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&slider);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Slider);

    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->currentValue().toInt(), 0);
    QCOMPARE(value->minimumValue().toInt(), slider.minimum());
    QCOMPARE(value->maximumValue().toInt(), slider.maximum());
    QCOMPARE(value->minimumStepSize().toInt(), slider.singleStep());

    value->setCurrentValue(42);
    QCOMPARE(slider.value(), 42);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_FluentSlider)

#include "tst_fluentslider.moc"
