/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinoslider.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QMouseEvent>

QX_CUPERTINO_USE_NAMESPACE

namespace
{
// The handle is twenty-eight across and the rail starts half a handle in, so
// the middle of a two-hundred wide slider is what a pointer at a hundred
// reaches.
constexpr int kHandleSize = 28;

void sendDrag(QWidget *widget, const QPoint &pos)
{
    // QTest::mouseMove carries no buttons, and a drag is a move with one down:
    // without it the move is a hover and the slider is right to ignore it.
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

/*! How much ink there is in a part of the widget: anything that is not the page
 *  it sits on. A slider that drew marks under its rail would show them here. */
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

/*! How many pixels of the handle's middle row are the handle: it is white and
 *  everything else on that row - the blue filled rail, the grey of the part not
 *  reached, the shadow under the handle - is several steps below it, on either
 *  face of the palette. Reading the row against its own brightest pixel is what
 *  makes the same code work on a light page and a dark one. */
int handleWidth(QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    const int row = widget->height() / 2;
    // The rail, and nothing beside it: the page outside the rail is as white as
    // the handle, so counting the whole row would count the page too.
    const int left = kHandleSize / 2;
    const int right = image.width() - kHandleSize / 2;
    int brightest = 0;
    for (int x = left; x < right; ++x) {
        brightest = qMax(brightest, image.pixelColor(x, row).lightness());
    }
    int count = 0;
    for (int x = left; x < right; ++x) {
        if (image.pixelColor(x, row).lightness() >= brightest - 12) {
            ++count;
        }
    }
    return count;
}
}   // namespace

class tst_CupertinoSlider : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void aPressOnTheRailMovesTheHandle();
    void aDragFollowsThePointer();
    void theHeldHandleGrows();
    void theKeyboardStepsIt();
    void itCarriesAValue();
    void thereAreNoMarksUnderTheRail();
};

void tst_CupertinoSlider::cleanup()
{
    QAccessible::setActive(false);
}

/*! A press anywhere on the rail moves the handle there: waiting for a drag
 *  would be a slider that lies about where the pointer is. */
void tst_CupertinoSlider::aPressOnTheRailMovesTheHandle()
{
    CupertinoSlider slider;
    slider.resize(200, slider.sizeHint().height());
    QCOMPARE(slider.value(), 0);

    // The rail starts half a handle in and ends half a handle short, so the
    // middle of a two-hundred wide slider is at a hundred.
    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
    QCOMPARE(slider.value(), 50);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
}

/*! What the pointer does while it is down: the handle follows, at once. */
void tst_CupertinoSlider::aDragFollowsThePointer()
{
    CupertinoSlider slider;
    slider.resize(200, slider.sizeHint().height());

    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(kHandleSize / 2, slider.height() / 2));
    QCOMPARE(slider.value(), 0);

    sendDrag(&slider, QPoint(190, slider.height() / 2));
    QCOMPARE(slider.value(), 100);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(190, slider.height() / 2));

    // The rail is the whole of what the pointer can reach, so a drag past its
    // end is the last value and not a crash.
    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
    sendDrag(&slider, QPoint(400, slider.height() / 2));
    QCOMPARE(slider.value(), 100);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
}

/*! The handle says it is being held: it grows by four while the pointer is
 *  down, which is the one thing here that moves without a value changing. */
void tst_CupertinoSlider::theHeldHandleGrows()
{
    CupertinoSlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    QTest::qWait(300);

    const int resting = handleWidth(&slider);
    QTest::mousePress(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));
    const int held = handleWidth(&slider);
    QTest::mouseRelease(&slider, Qt::LeftButton, Qt::NoModifier, QPoint(100, slider.height() / 2));

    QVERIFY(held > resting);
    QCOMPARE(held - resting, 4);
}

/*! A slider is a value in a range, so the keyboard walks it one step at a time
 *  and knows both ends - the same keys as the other families, which is one
 *  answer for the whole project rather than a set per family. */
void tst_CupertinoSlider::theKeyboardStepsIt()
{
    CupertinoSlider slider;
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
void tst_CupertinoSlider::itCarriesAValue()
{
    CupertinoSlider slider;
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
void tst_CupertinoSlider::thereAreNoMarksUnderTheRail()
{
    CupertinoSlider slider;
    slider.resize(200, slider.sizeHint().height());
    slider.setValue(50);
    // The handle travels to a value it was given rather than appearing there,
    // so the probe has to wait for it to arrive - what is under the rail is only
    // readable once the handle is somewhere else.
    QTest::qWait(300);

    const int middle = slider.height() / 2;
    const int railTop = middle - 2;
    const int railBottom = middle + 2;
    // Where the handle is not: it sits in the middle at fifty, so the left end
    // of the rail has nothing over it but the rail itself.
    const QRect leftEnd(0, railTop, 60, railBottom);
    const QRect underLeftEnd(0, railBottom, 60, slider.height());

    // The probe can see ink where there is ink, so its answer lower down means
    // something.
    QVERIFY(inkedPixels(&slider, leftEnd) > 0);
    QCOMPARE(inkedPixels(&slider, underLeftEnd), 0);
}

TEST_ADD(tst_CupertinoSlider)

#include "tst_cupertinoslider.moc"
