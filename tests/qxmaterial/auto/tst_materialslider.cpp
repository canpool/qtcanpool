/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialslider.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QMouseEvent>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The handle is twenty across and the track starts half a handle in, so the
// middle of the track is what a pointer at the middle of the widget reaches.
constexpr int kHandleSize = 20;

void sendDrag(QWidget *widget, const QPoint &pos)
{
    // QTest::mouseMove carries no buttons, and a drag is a move with one down:
    // without it the move is a hover and the slider is right to ignore it.
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

/*! How much ink there is in a part of the widget: anything that is not the
 *  page it sits on. A slider that drew marks under its track would show them
 *  here. */
int inkedPixels(QWidget *widget, const QRect &area)
{
    const QColor page = widget->palette().color(QPalette::Window);
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = area.top(); y < qMin(area.bottom(), image.height()); ++y) {
        for (int x = area.left(); x < qMin(area.right(), image.width()); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_MaterialSlider : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void aPressOnTheTrackMovesTheHandle();
    void aDragFollowsThePointer();
    void theKeyboardStepsIt();
    void itCarriesAValue();
    void thereAreNoMarksUnderTheTrack();
};

void tst_MaterialSlider::cleanup()
{
    QAccessible::setActive(false);
}

/*! A press anywhere on the track moves the handle there: waiting for a drag
 *  would be a slider that lies about where the pointer is. */
void tst_MaterialSlider::aPressOnTheTrackMovesTheHandle()
{
    MaterialSlider slider;
    slider.resize(200, slider.sizeHint().height());
    QCOMPARE(slider.value(), 0);

    // The track starts half a handle in and ends half a handle short, so the
    // middle of a two-hundred wide slider is at a hundred.
    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
    QCOMPARE(slider.value(), 50);
}

/*! What the pointer does while it is down: the handle follows, at once. */
void tst_MaterialSlider::aDragFollowsThePointer()
{
    MaterialSlider slider;
    slider.resize(200, slider.sizeHint().height());

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kHandleSize / 2, slider.height() / 2));
    QCOMPARE(slider.value(), 0);

    sendDrag(&slider, QPoint(190, slider.height() / 2));
    QCOMPARE(slider.value(), 100);
}

/*! A slider is a value in a range, so the keyboard walks it one step at a time
 *  and knows both ends. */
void tst_MaterialSlider::theKeyboardStepsIt()
{
    MaterialSlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);

    QTest::keyClick(&slider, Qt::Key_Right);
    QCOMPARE(slider.value(), 51);
    QTest::keyClick(&slider, Qt::Key_Left);
    QCOMPARE(slider.value(), 50);
    QTest::keyClick(&slider, Qt::Key_PageUp);
    QCOMPARE(slider.value(), 60);
    QTest::keyClick(&slider, Qt::Key_Home);
    QCOMPARE(slider.value(), slider.minimum());
    QTest::keyClick(&slider, Qt::Key_End);
    QCOMPARE(slider.value(), slider.maximum());
}

/*! A role and a state cannot carry a number, so a slider brings its own face:
 *  the value can be read back and set back. */
void tst_MaterialSlider::itCarriesAValue()
{
    MaterialSlider slider;
    slider.setRange(0, 10);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&slider);
    QVERIFY(iface);
    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->maximumValue().toInt(), 10);
    QCOMPARE(value->minimumValue().toInt(), 0);

    value->setCurrentValue(7);
    QCOMPARE(slider.value(), 7);
    QCOMPARE(value->currentValue().toInt(), 7);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! The one thing Material's slider does not say: where the track is divided.
 *  Fluent's slider draws a mark at every interval; this one draws nothing
 *  under the track at all, because the handle's position is the answer. */
void tst_MaterialSlider::thereAreNoMarksUnderTheTrack()
{
    MaterialSlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    // The handle travels to a value it was given rather than appearing there,
    // so the probe has to wait for it to arrive - what is under the track is
    // only readable once the handle is somewhere else.
    QTest::qWait(300);

    const int middle = slider.height() / 2;
    const int trackTop = middle - 6;
    const int trackBottom = middle + 6;
    // Where the handle is not: the handle sits in the middle at fifty, so the
    // left end of the track has nothing over it but the track itself.
    const QRect leftEnd(0, trackTop, 60, trackBottom);
    const QRect underLeftEnd(0, trackBottom, 60, slider.height());

    // The probe can see ink where there is ink, so its answer lower down means
    // something.
    QVERIFY(inkedPixels(&slider, leftEnd) > 0);
    QCOMPARE(inkedPixels(&slider, underLeftEnd), 0);
}

TEST_ADD(tst_MaterialSlider)

#include "tst_materialslider.moc"
