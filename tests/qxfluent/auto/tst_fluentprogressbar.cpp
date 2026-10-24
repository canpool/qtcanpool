/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentprogressbar.h"

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

/*! The fill is one row tall in the middle of the widget; how much of that
 *  row is accent tells how far the fill reaches. */
int accentSpan(QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    const int row = image.height() / 2;
    int count = 0;
    for (int x = 0; x < image.width(); ++x) {
        if (image.pixelColor(x, row) == kAccent) {
            ++count;
        }
    }
    return count;
}
}   // namespace

class tst_FluentProgressBar : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aBarWithAnEndDrawsHowFarAlongItIs();
    void theEndIsTheFarEdge();
    void anEndlessBarSlides();
    void reduceMotionStopsTheSlideWithoutErasingIt();
    void howFarAlongIsThereForAnAssistiveTool();
    void aBarNobodyCanUseHoldsStill();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentProgressBar::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasReduced = QxTheme::isMotionReduced();
    m_wasActive = QAccessible::isActive();
}

void tst_FluentProgressBar::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! Determinate is a fill with an end, and the end is the answer: a quarter of
 *  the range is a quarter of the track. */
void tst_FluentProgressBar::aBarWithAnEndDrawsHowFarAlongItIs()
{
    FluentProgressBar bar;
    bar.resize(120, 4);
    bar.setIndeterminate(false);
    bar.setValue(0);
    QCOMPARE(accentPixels(&bar), 0);

    bar.setValue(25);
    const int quarter = accentPixels(&bar);
    QVERIFY(quarter > 0);

    bar.setValue(75);
    QVERIFY(accentPixels(&bar) > quarter);
}

/*! The end of the range is the far edge: filled to it, the fill reaches the
 *  whole track, whatever width the layout gave. */
void tst_FluentProgressBar::theEndIsTheFarEdge()
{
    FluentProgressBar bar;
    bar.resize(120, 4);
    bar.setIndeterminate(false);
    bar.setMinimum(-100);
    bar.setMaximum(100);
    bar.setValue(0);
    QVERIFY(accentSpan(&bar) > 0);
    QVERIFY(accentSpan(&bar) < bar.width());

    bar.setValue(100);
    // The pill's antialiased ends blend a pixel each; the fill is otherwise
    // the whole row.
    QVERIFY(accentSpan(&bar) >= bar.width() - 2);
}

/*! Indeterminate is a block without an end: what it says is that the waiting
 *  has not stopped, and it says it by sliding. */
void tst_FluentProgressBar::anEndlessBarSlides()
{
    FluentProgressBar bar;
    bar.resize(120, 4);
    QVERIFY(!bar.isIndeterminate());

    bar.setIndeterminate(true);
    QVERIFY(bar.isIndeterminate());

    QTest::qWait(100);
    const QImage before = bar.grab().toImage();
    QTest::qWait(200);
    const QImage after = bar.grab().toImage();

    QVERIFY(before != after);
    // The block is a third of the track or so, never the whole of it.
    QVERIFY(accentPixels(&bar) > 0);
    QVERIFY(accentSpan(&bar) < bar.width());
}

/*! The slide is the one motion here that is not a transition, and it is what
 *  the reduce-motion switch is about: asked for less motion, the block holds
 *  still - but it holds still in view, because an empty track is not
 *  progress, it is nothing. */
void tst_FluentProgressBar::reduceMotionStopsTheSlideWithoutErasingIt()
{
    QxTheme::setMotionReduced(true);

    FluentProgressBar bar;
    bar.resize(120, 4);
    bar.setIndeterminate(true);

    const QImage before = bar.grab().toImage();
    QTest::qWait(200);
    QCOMPARE(bar.grab().toImage(), before);

    // And what holds still is the block, not an empty track.
    QVERIFY(accentPixels(&bar) > 0);
    QCOMPARE(before.pixelColor(2, 2), kAccent);
}

/*! How far along is a number, and an assistive tool reads it the way it reads
 *  a slider's - over whatever range the bar was given - and sets it back the
 *  same way. */
void tst_FluentProgressBar::howFarAlongIsThereForAnAssistiveTool()
{
    QAccessible::setActive(true);
    FluentProgressBar bar;
    bar.setIndeterminate(false);
    bar.setMinimum(10);
    bar.setMaximum(90);

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&bar);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ProgressBar);

    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->minimumValue().toInt(), 10);
    QCOMPARE(value->maximumValue().toInt(), 90);

    value->setCurrentValue(60);
    QCOMPARE(bar.value(), 60);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A bar nobody can use holds still: the slide is the bar saying that
 *  something is still happening, and a control that cannot be used makes no
 *  claim about anything happening. What holds still is the block, not an empty
 *  track. */
void tst_FluentProgressBar::aBarNobodyCanUseHoldsStill()
{
    FluentProgressBar bar;
    bar.resize(120, 4);
    bar.setIndeterminate(true);
    bar.setEnabled(false);

    const QImage before = bar.grab().toImage();
    QTest::qWait(200);
    QCOMPARE(bar.grab().toImage(), before);
    QVERIFY(accentPixels(&bar) > 0);
}

TEST_ADD(tst_FluentProgressBar)

#include "tst_fluentprogressbar.moc"
