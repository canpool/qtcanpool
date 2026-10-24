/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentrating.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QMouseEvent>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

// A star is twenty across with four between the stars, so the middle of the
// first star is at 10 and the step is 24.
constexpr int kStarStep = 24;
constexpr int kFirstStar = 10;

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

void sendHover(QWidget *widget, const QPoint &pos)
{
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}
}   // namespace

class tst_FluentRating : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aClickRatesIt();
    void theHoverSaysWhatAClickWouldDo();
    void theKeyboardWalksIt();
    void theStarsAreTheRange();
    void aRatingNobodyCanUseIsDrawnFaded();
    void theKeyboardLandsOnIt();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_FluentRating::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_FluentRating::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! A click on the third star is a rating of three, and nothing more is
 *  filled than was asked for. */
void tst_FluentRating::aClickRatesIt()
{
    FluentRating rating;
    rating.resize(rating.sizeHint());

    QTest::mousePress(&rating, Qt::LeftButton, Qt::NoModifier, QPoint(kFirstStar + 2 * kStarStep, 10));
    QCOMPARE(rating.value(), 3);

    const int three = accentPixels(&rating);
    QTest::mousePress(&rating, Qt::LeftButton, Qt::NoModifier, QPoint(kFirstStar, 10));
    QCOMPARE(rating.value(), 1);
    QVERIFY(accentPixels(&rating) < three);
}

/*! What a rating has to answer that a row of buttons does not: what a click
 *  here would do. The hover says it and the value does not move. */
void tst_FluentRating::theHoverSaysWhatAClickWouldDo()
{
    FluentRating rating;
    rating.resize(rating.sizeHint());
    const QImage resting = rating.grab().toImage();
    QCOMPARE(rating.value(), 0);

    sendHover(&rating, QPoint(kFirstStar + 3 * kStarStep, 10));

    QVERIFY(rating.grab().toImage() != resting);
    QCOMPARE(rating.value(), 0);
}

/*! A rating is a value in a range, so the keyboard walks it: one star at a
 *  time, and the two ends. */
void tst_FluentRating::theKeyboardWalksIt()
{
    FluentRating rating;
    rating.resize(rating.sizeHint());
    rating.setValue(2);

    QTest::keyClick(&rating, Qt::Key_Right);
    QCOMPARE(rating.value(), 3);
    QTest::keyClick(&rating, Qt::Key_Left);
    QCOMPARE(rating.value(), 2);
    QTest::keyClick(&rating, Qt::Key_Home);
    QCOMPARE(rating.value(), 0);
    QTest::keyClick(&rating, Qt::Key_End);
    QCOMPARE(rating.value(), rating.count());
}

/*! The number of stars is the range, both to look at and to an assistive
 *  tool that wants to know what the ends are. */
void tst_FluentRating::theStarsAreTheRange()
{
    FluentRating rating;
    const QSize five = rating.sizeHint();
    rating.setCount(3);
    QVERIFY(rating.sizeHint().width() < five.width());
    QVERIFY(rating.value() <= 3);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&rating);
    QVERIFY(iface);
    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->maximumValue().toInt(), rating.count());
    QCOMPARE(value->minimumValue().toInt(), 0);

    value->setCurrentValue(2);
    QCOMPARE(rating.value(), 2);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A rating nobody can use is drawn faded. It already refuses the click; a
 *  row of stars that still looks ready to answer is the part that lies. */
void tst_FluentRating::aRatingNobodyCanUseIsDrawnFaded()
{
    FluentRating ready;
    ready.resize(ready.sizeHint());
    ready.setValue(3);
    QVERIFY(accentPixels(&ready) > 0);

    FluentRating faded;
    faded.resize(faded.sizeHint());
    faded.setValue(3);
    faded.setEnabled(false);

    QCOMPARE(accentPixels(&faded), 0);
}

/*! The keyboard finds it, and the ring says where the keyboard is: a control
 *  the keyboard cannot reach is a control half the users cannot use, and one it
 *  reaches without a mark is a control they cannot find.
 */
void tst_FluentRating::theKeyboardLandsOnIt()
{
    FluentRating widget;
    widget.setCount(5);
    widget.resize(widget.sizeHint());
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_FluentRating)

#include "tst_fluentrating.moc"
