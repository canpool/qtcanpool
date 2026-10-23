/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxharmony/harmonyslider.h"
#include "qxharmony/harmonytokens.h"

#include <QAccessible>
#include <QApplication>
#include <QFontMetricsF>
#include <QImage>
#include <QMouseEvent>

QX_HARMONY_USE_NAMESPACE

namespace
{
// The handle is twenty-six across and the rail starts half a handle in, so the
// middle of a two-hundred wide slider is what a pointer at a hundred reaches.
// The rail sits fourteen up from the bottom of the widget, which is where the
// room for the bubble comes from.
constexpr int kHandleSize = 26;
constexpr int kRailFromBottom = 14;

int railRow(QWidget *widget)
{
    return widget->height() - kRailFromBottom;
}

void sendDrag(QWidget *widget, const QPoint &pos)
{
    // QTest::mouseMove carries no buttons, and a drag is a move with one down:
    // without it the move is a hover and the slider is right to ignore it.
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

/*! How much blue there is in a part of the widget. The bubble is the family's
 *  blue on white, and nothing else above the rail is blue - the handle's shadow
 *  is grey - so this is a geometric probe for the bubble rather than a
 *  screenshot comparison. */
int bluePixels(QWidget *widget, const QRect &area)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = area.top(); y < qMin(area.bottom(), image.height()); ++y) {
        for (int x = area.left(); x < qMin(area.right(), image.width()); ++x) {
            const QColor colour = image.pixelColor(x, y);
            if (colour.blue() > colour.red() + 40 && colour.blue() > colour.green() + 40) {
                ++count;
            }
        }
    }
    return count;
}

/*! The first row, counting from the top of the widget, that the bubble reaches;
 *  -1 while there is no bubble at all. */
int firstBlueRow(QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QColor colour = image.pixelColor(x, y);
            if (colour.blue() > colour.red() + 40 && colour.blue() > colour.green() + 40) {
                return y;
            }
        }
    }
    return -1;
}

/*! How much ink there is in a part of the widget: anything that is not the page
 *  it sits on. */
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

class tst_HarmonySlider : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void aPressOnTheRailMovesTheHandle();
    void aDragFollowsThePointer();
    void theBubbleFloatsOverTheHeldHandle();
    void theBubbleGoesOnceTheHandleIsLetGo();
    void theBubbleIsWholeAtTheHeightTheSliderAsksFor();
    void theKeyboardStepsIt();
    void itCarriesAValue();
    void thereAreNoMarksUnderTheRail();
};

void tst_HarmonySlider::cleanup()
{
    QAccessible::setActive(false);
}

/*! A press anywhere on the rail moves the handle there: waiting for a drag
 *  would be a slider that lies about where the pointer is. */
void tst_HarmonySlider::aPressOnTheRailMovesTheHandle()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());
    QCOMPARE(slider.value(), 0);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
    QCOMPARE(slider.value(), 50);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
}

/*! What the pointer does while it is down: the handle follows, at once, and
 *  stops at the ends of the rail rather than running off it. */
void tst_HarmonySlider::aDragFollowsThePointer()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kHandleSize / 2, railRow(&slider)));
    QCOMPARE(slider.value(), 0);

    sendDrag(&slider, QPoint(190, railRow(&slider)));
    QCOMPARE(slider.value(), 100);
    sendDrag(&slider, QPoint(400, railRow(&slider)));
    QCOMPARE(slider.value(), 100);
    sendDrag(&slider, QPoint(-40, railRow(&slider)));
    QCOMPARE(slider.value(), 0);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
}

/*! The half of the drawing that is this family's: a bubble over the handle with
 *  the number in it, there only while the pointer is down - and over the
 *  handle, so it moves with it. */
void tst_HarmonySlider::theBubbleFloatsOverTheHeldHandle()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    QTest::qWait(300);

    // Nothing above the rail while the handle is at rest.
    QCOMPARE(bluePixels(&slider, QRect(60, 0, 80, railRow(&slider) - kHandleSize / 2 - 2)), 0);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
    // Held at fifty, the bubble is over the middle of the slider.
    QVERIFY(bluePixels(&slider, QRect(78, 0, 44, 24)) > 0);

    // Dragged to the right, the bubble goes with the handle and leaves the
    // middle it was in empty.
    sendDrag(&slider, QPoint(190, railRow(&slider)));
    QVERIFY(bluePixels(&slider, QRect(156, 0, 44, 24)) > 0);
    QCOMPARE(bluePixels(&slider, QRect(0, 0, 60, 24)), 0);

    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(190, railRow(&slider)));
}

/*! Let go, the bubble fades out rather than vanishing: it is gone a moment
 *  later, and not before. */
void tst_HarmonySlider::theBubbleGoesOnceTheHandleIsLetGo()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    QTest::qWait(300);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
    QVERIFY(bluePixels(&slider, QRect(78, 0, 44, 24)) > 0);

    // A release, then far longer than the quick duration: the bubble is gone.
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, railRow(&slider)));
    QTest::qWait(400);
    QCOMPARE(bluePixels(&slider, QRect(0, 0, slider.width(), 24)), 0);
}

/*! The bubble is drawn above the rail, so the room for it is part of what the
 *  control has to ask for. Asking for less is not a shorter slider: a widget
 *  cannot paint outside itself, so the bubble would arrive with its top sliced
 *  off - a slice, not a rounded box. */
void tst_HarmonySlider::theBubbleIsWholeAtTheHeightTheSliderAsksFor()
{
    HarmonySlider slider;
    slider.setValue(70);
    slider.resize(200, slider.sizeHint().height());
    QTest::qWait(300);

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(140, railRow(&slider)));

    // A whole bubble is the caption's line in its padding. The count is exact
    // and not a floor: the top edge is a rounded corner, but its first row is
    // blue across the middle, so a bubble one row short is a bubble with its top
    // sliced off by the widget's own edge.
    const qreal line = QFontMetricsF(HarmonyType::font(HarmonyType::Ramp::Caption)).height();
    const int wanted = qRound(line) + 6;
    const int bottomOfBubble = railRow(&slider) - kHandleSize / 2 - 2;
    const int first = firstBlueRow(&slider);
    QVERIFY(first >= 0);
    QVERIFY2(bottomOfBubble - first >= wanted,
             qPrintable(QStringLiteral("the bubble is %1 rows tall where it wants %2: the control asks for %3 "
                                       "and the drawing needs %4")
                            .arg(bottomOfBubble - first)
                            .arg(wanted)
                            .arg(slider.sizeHint().height())
                            .arg(slider.minimumSizeHint().height())));

    // And a layout that squeezes it gets the same answer: a control too short for
    // its own bubble is a control that lies about what it is.
    QCOMPARE(slider.minimumSizeHint().height(), slider.sizeHint().height());

    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(140, railRow(&slider)));
}

/*! A slider is a value in a range, so the keyboard walks it one step at a time
 *  and knows both ends - the same keys as the other families, which is one
 *  answer for the whole project rather than a set per family. */
void tst_HarmonySlider::theKeyboardStepsIt()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);

    QTest::keyClick(&slider, Qt::Key_Right);
    QCOMPARE(slider.value(), 51);
    QTest::keyClick(&slider, Qt::Key_Left);
    QCOMPARE(slider.value(), 50);
    QTest::keyClick(&slider, Qt::Key_PageUp);
    QCOMPARE(slider.value(), 60);
    QTest::keyClick(&slider, Qt::Key_PageDown);
    QCOMPARE(slider.value(), 50);
    QTest::keyClick(&slider, Qt::Key_Home);
    QCOMPARE(slider.value(), slider.minimum());
    QTest::keyClick(&slider, Qt::Key_End);
    QCOMPARE(slider.value(), slider.maximum());

    // And the range is a range: a value outside it is the nearest end of it.
    slider.setValue(-40);
    QCOMPARE(slider.value(), 0);
    slider.setValue(400);
    QCOMPARE(slider.value(), 100);
}

/*! A role and a state cannot carry a number, so a slider brings its own face:
 *  the value can be read back and set back. */
void tst_HarmonySlider::itCarriesAValue()
{
    HarmonySlider slider;
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

/*! The one thing this family's slider does not say: where the rail is divided.
 *  Fluent's slider draws a mark at every interval; this one draws nothing under
 *  the rail at all, because the handle's position is the answer. */
void tst_HarmonySlider::thereAreNoMarksUnderTheRail()
{
    HarmonySlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    // The handle travels to a value it was given rather than appearing there,
    // so the probe has to wait for it to arrive - what is under the rail is only
    // readable once the handle is somewhere else.
    QTest::qWait(300);

    const int row = railRow(&slider);
    // Where the handle is not: it sits in the middle at fifty, so the left end
    // of the rail has nothing over it but the rail itself.
    const QRect leftEnd(0, row - 2, 60, row + 4);
    const QRect underLeftEnd(0, row + 4, 60, slider.height());

    // The probe can see ink where there is ink, so its answer lower down means
    // something.
    QVERIFY(inkedPixels(&slider, leftEnd) > 0);
    QCOMPARE(inkedPixels(&slider, underLeftEnd), 0);
}

TEST_ADD(tst_HarmonySlider)

#include "tst_harmonyslider.moc"
