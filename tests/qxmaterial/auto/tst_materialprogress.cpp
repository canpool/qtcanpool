/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialprogress.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QImage>
#include <QPalette>

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

class tst_MaterialProgress : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aTrackWithAnEndDrawsHowFarAlongItIs();
    void theEndIsTheFarEdge();
    void theColoursComeFromTheTokens();
    void anEndlessTrackSlides();
    void reduceMotionStopsTheSlideWithoutErasingIt();
    void howFarAlongIsThereForAnAssistiveTool();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_MaterialProgress::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasReduced = QxTheme::isMotionReduced();
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialProgress::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! Determinate is a fill with an end, and the end is the answer: a quarter of
 *  the range is a quarter of the track. */
void tst_MaterialProgress::aTrackWithAnEndDrawsHowFarAlongItIs()
{
    MaterialProgress progress;
    progress.resize(120, 4);
    progress.setIndeterminate(false);
    progress.setValue(0);
    QCOMPARE(accentPixels(&progress), 0);

    progress.setValue(25);
    const int quarter = accentPixels(&progress);
    QVERIFY(quarter > 0);

    progress.setValue(75);
    QVERIFY(accentPixels(&progress) > quarter);
}

/*! The end of the range is the far edge: filled to it, the fill reaches the
 *  whole track, whatever width the layout gave. */
void tst_MaterialProgress::theEndIsTheFarEdge()
{
    MaterialProgress progress;
    progress.resize(120, 4);
    progress.setIndeterminate(false);
    progress.setMinimum(-100);
    progress.setMaximum(100);
    progress.setValue(0);
    QVERIFY(accentSpan(&progress) > 0);
    QVERIFY(accentSpan(&progress) < progress.width());

    progress.setValue(100);
    // The pill's antialiased ends blend a pixel each; the fill is otherwise
    // the whole row.
    QVERIFY(accentSpan(&progress) >= progress.width() - 2);
}

/*! The track is a surface one step from the page and the fill the primary -
 *  both read from the palette, not spelled into the code: mute the ink the
 *  page writes with and the track follows it; the fill stays the accent. */
void tst_MaterialProgress::theColoursComeFromTheTokens()
{
    MaterialProgress progress;
    progress.resize(120, 4);
    progress.setIndeterminate(false);
    progress.setValue(50);

    QPalette pal = progress.palette();
    pal.setColor(QPalette::Window, Qt::white);
    pal.setColor(QPalette::WindowText, Qt::red);
    progress.setPalette(pal);
    const QImage withRed = progress.grab().toImage();
    // Half the track is fill; the unfilled half is the page ink thinned:
    // red washed to a quarter of its strength over the white page.
    const QColor track = withRed.pixelColor(progress.width() - 5, 2);
    QCOMPARE(track.red(), 255);
    QVERIFY(qAbs(track.green() - 194) <= 2);
    QVERIFY(qAbs(track.blue() - 194) <= 2);
    // And the filled half is the accent, whatever the ink does.
    QCOMPARE(withRed.pixelColor(2, 2), kAccent);

    pal.setColor(QPalette::WindowText, Qt::blue);
    progress.setPalette(pal);
    const QImage withBlue = progress.grab().toImage();
    QVERIFY(withRed.pixelColor(progress.width() - 5, 2) != withBlue.pixelColor(progress.width() - 5, 2));
}

/*! Indeterminate is a block without an end: what it says is that the waiting
 *  has not stopped, and it says it by sliding. */
void tst_MaterialProgress::anEndlessTrackSlides()
{
    MaterialProgress progress;
    progress.resize(120, 4);
    QVERIFY(!progress.isIndeterminate());

    progress.setIndeterminate(true);
    QVERIFY(progress.isIndeterminate());

    QTest::qWait(100);
    const QImage before = progress.grab().toImage();
    QTest::qWait(200);
    const QImage after = progress.grab().toImage();

    QVERIFY(before != after);
    // The block is a third of the track or so, never the whole of it.
    QVERIFY(accentPixels(&progress) > 0);
    QVERIFY(accentSpan(&progress) < progress.width());
}

/*! The slide is the one motion here that is not a transition, and it is what
 *  the reduce-motion switch is about: asked for less motion, the block holds
 *  still - but it holds still in view, because an empty track is not
 *  progress, it is nothing. */
void tst_MaterialProgress::reduceMotionStopsTheSlideWithoutErasingIt()
{
    QxTheme::setMotionReduced(true);

    MaterialProgress progress;
    progress.resize(120, 4);
    progress.setIndeterminate(true);

    const QImage before = progress.grab().toImage();
    QTest::qWait(200);
    QCOMPARE(progress.grab().toImage(), before);

    // And what holds still is the block, not an empty track.
    QVERIFY(accentPixels(&progress) > 0);
    QCOMPARE(before.pixelColor(2, 2), kAccent);
}

/*! How far along is a number, and an assistive tool reads it the way it reads
 *  a slider's - over whatever range the track was given - and sets it back
 *  the same way. */
void tst_MaterialProgress::howFarAlongIsThereForAnAssistiveTool()
{
    QAccessible::setActive(true);
    MaterialProgress progress;
    progress.setIndeterminate(false);
    progress.setMinimum(10);
    progress.setMaximum(90);

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&progress);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ProgressBar);

    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->minimumValue().toInt(), 10);
    QCOMPARE(value->maximumValue().toInt(), 90);

    value->setCurrentValue(60);
    QCOMPARE(progress.value(), 60);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialProgress)

#include "tst_materialprogress.moc"
