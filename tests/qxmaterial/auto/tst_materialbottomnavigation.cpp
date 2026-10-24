/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialbottomnavigation.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QAccessible>
#include <QAction>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The bar's own measures, read the way a reader reads them.
constexpr int kBarHeight = 56;
constexpr int kIndicatorTop = 3;

/*! How many pixels of \a room are the accent itself, allowance made for the
 *  one or two of anti-aliasing either side of it. */
int accentPixels(const QImage &image, const QRect &room)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (qAbs(pixel.red() - accent.red()) <= 8 && qAbs(pixel.green() - accent.green()) <= 8 &&
                qAbs(pixel.blue() - accent.blue()) <= 8) {
                ++count;
            }
        }
    }
    return count;
}

/*! Whether \a pixel is \a expected, allowing for the anti-aliasing of whatever
 *  was drawn over it. */
bool alike(const QColor &pixel, const QColor &expected)
{
    return qAbs(pixel.red() - expected.red()) <= 3 && qAbs(pixel.green() - expected.green()) <= 3 &&
           qAbs(pixel.blue() - expected.blue()) <= 3;
}
}   // namespace

class tst_MaterialBottomNavigation : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theBarIsFiftySixTallAndDividesItsWidth();
    void theDestinationInForceWearsThePill();
    void pressingADestinationPutsItInForce();
    void aDestinationNobodyCanUseIsNotPutInForce();
    void theKeyboardWalksTheRow();
    void theBarAnswersAsAList();
};

namespace
{
/*! A bar of three destinations across three hundred pixels, so that each one
 *  stands a hundred wide and a probe can be aimed at one by arithmetic alone. */
MaterialBottomNavigation *makeBar(QWidget *page)
{
    auto *bar = new MaterialBottomNavigation(page);
    bar->addDestination(QStringLiteral("Home"));
    bar->addDestination(QStringLiteral("Search"));
    bar->addDestination(QStringLiteral("Settings"));
    bar->resize(300, kBarHeight);
    bar->show();
    QTest::qWaitForWindowExposed(bar);
    return bar;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(320, 200);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}
}   // namespace

void tst_MaterialBottomNavigation::initTestCase()
{
    // The labels are drawn with whatever font the machine has, and the tests
    // read pixels, so there has to be one.
    qtcGiveFonts();
}

/*! A bar is the height Material gives it, and the width it was handed is
 *  divided between the destinations - the last one taking the remainder. */
void tst_MaterialBottomNavigation::theBarIsFiftySixTallAndDividesItsWidth()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);

    QCOMPARE(bar->height(), kBarHeight);
    QCOMPARE(bar->sizeHint(), QSize(240, kBarHeight));
    QCOMPARE(bar->minimumSizeHint(), QSize(168, kBarHeight));

    // Each destination owns a third of the row: a press at two hundred and
    // fifty is a press on the third, and one at fifty on the first.
    bar->setCurrentIndex(0);
    QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, QPoint(250, kBarHeight / 2));
    QCOMPARE(bar->currentIndex(), 2);
    QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, QPoint(50, kBarHeight / 2));
    QCOMPARE(bar->currentIndex(), 0);
}

/*! The destination in force wears the pill - the accent's own shape - and the
 *  ones that are not do not. Its name turns to the accent with it. */
void tst_MaterialBottomNavigation::theDestinationInForceWearsThePill()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);
    bar->setCurrentIndex(0);

    const QImage frame = bar->grab().toImage();
    const QColor page_ = bar->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor pill = QxControls::QxStateLayer::layered(page_, accent, 0.12);

    // The top edge of the pill, over the icon and clear of it: at fifty it is
    // the one in force, at a hundred and fifty the next one.
    const QPoint inForce(50, kIndicatorTop + 2);
    const QPoint resting(150, kIndicatorTop + 2);
    QVERIFY2(alike(frame.pixelColor(inForce), pill),
             qPrintable(QStringLiteral("pill %1 vs %2").arg(frame.pixelColor(inForce).name()).arg(pill.name())));
    QVERIFY(alike(frame.pixelColor(resting), page_));

    // And the name under it is the accent, while the next one's is not.
    const QRect first(10, 38, 80, 16);
    const QRect second(110, 38, 80, 16);
    QVERIFY(accentPixels(frame, first) > 0);
    QCOMPARE(accentPixels(frame, second), 0);
}

/*! Pressing a destination puts it in force, and says so once. */
void tst_MaterialBottomNavigation::pressingADestinationPutsItInForce()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);

    int changes = 0;
    int reported = -1;
    connect(bar, &MaterialBottomNavigation::currentIndexChanged, this, [&](int index) {
        ++changes;
        reported = index;
    });

    QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, QPoint(150, kBarHeight / 2));
    QCOMPARE(bar->currentIndex(), 1);
    QCOMPARE(changes, 1);
    QCOMPARE(reported, 1);

    // Pressing the one already in force is not a change.
    QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, QPoint(150, kBarHeight / 2));
    QCOMPARE(changes, 1);
}

/*! A destination nobody can use answers no press: it is not put in force. */
void tst_MaterialBottomNavigation::aDestinationNobodyCanUseIsNotPutInForce()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);
    bar->setCurrentIndex(0);
    bar->destinations().at(1)->setEnabled(false);

    QTest::mouseClick(bar, Qt::LeftButton, Qt::NoModifier, QPoint(150, kBarHeight / 2));
    QCOMPARE(bar->currentIndex(), 0);
}

/*! The keyboard walks the row and puts what it walks to in force: along a bar
 *  the walking is choosing. */
void tst_MaterialBottomNavigation::theKeyboardWalksTheRow()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);
    bar->setCurrentIndex(0);
    bar->setFocus();

    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(bar->currentIndex(), 1);
    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(bar->currentIndex(), 2);
    // The end of the row is the end: it does not wrap round to the other one.
    QTest::keyClick(bar, Qt::Key_Right);
    QCOMPARE(bar->currentIndex(), 2);
    QTest::keyClick(bar, Qt::Key_Home);
    QCOMPARE(bar->currentIndex(), 0);
}

/*! To an assistive tool the bar is a list of destinations. */
void tst_MaterialBottomNavigation::theBarAnswersAsAList()
{
    QWidget *page = makePage();
    MaterialBottomNavigation *bar = makeBar(page);

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(bar);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::List);

    // The destinations are the bar's actions, and they are what a caller reads
    // back to find out where it is.
    QCOMPARE(bar->destinations().count(), 3);
    QCOMPARE(bar->currentDestination(), bar->destinations().at(0));
}

TEST_ADD(tst_MaterialBottomNavigation)

#include "tst_materialbottomnavigation.moc"
