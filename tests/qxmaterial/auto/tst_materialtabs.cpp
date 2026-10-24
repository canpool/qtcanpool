/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtabs.h"
#include "qxmaterial/materialtokens.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! The room the indicator takes: the last two rows of the row, which is where
 *  it stands and nowhere else. */
QRect indicatorRoom(const QImage &image)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int left = image.width();
    int right = -1;
    for (int y = image.height() - 2; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) == accent) {
                left = qMin(left, x);
                right = qMax(right, x);
            }
        }
    }
    return right < 0 ? QRect() : QRect(left, image.height() - 2, right - left + 1, 2);
}

/*! How long the indicator takes to travel, from the family's own answer. */
int travelDuration()
{
    return MaterialMotion::forDuration(MaterialMotion::Duration::Short).duration;
}

/*! The indicator once it has got there.
 *
 *  The travel runs on the event loop's own clock, so a fixed wait is a guess
 *  about the machine: a runner under load delivers the last frame late, and the
 *  row is then read one pixel short of the tab it is under - which is what a
 *  narrow row on CI reported. Asking until the indicator stands where the tab
 *  is costs a fast machine nothing and says what the test means.
 */
QRect arrivedIndicator(MaterialTabs *tabs)
{
    const QRect target = tabs->tabRect(tabs->currentIndex());
    QRect room = indicatorRoom(tabs->grab().toImage());
    for (int waited = 0; waited < travelDuration() * 4 + 400; waited += 20) {
        if (room.left() == target.left() && room.width() == target.width()) {
            break;
        }
        QTest::qWait(20);
        room = indicatorRoom(tabs->grab().toImage());
    }
    return room;
}
}   // namespace

class tst_MaterialTabs : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theTabsStandInARow();
    void theIndicatorStandsUnderTheTabInForce();
    void theIndicatorTravelsThere();
    void noMotionPutsItThereAtOnce();
    void aClickMovesTheTab();
    void theKeyboardWalksTheRow();
    void aNarrowRowSlidesToTheTabInForce();
private:
    bool m_hadOverride = false;
    bool m_wasReduced = false;
    MaterialTabs *m_tabs = nullptr;
};

void tst_MaterialTabs::init()
{
    qtcGiveFonts();
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor(0, 0, 255));
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(false);

    m_tabs = new MaterialTabs;
    m_tabs->addTab(QStringLiteral("Overview"));
    m_tabs->addTab(QStringLiteral("Details"));
    m_tabs->addTab(QStringLiteral("History"));
    m_tabs->resize(600, m_tabs->sizeHint().height());
    m_tabs->show();
}

void tst_MaterialTabs::cleanup()
{
    delete m_tabs;
    m_tabs = nullptr;
    QxTheme::setMotionReduced(m_wasReduced);
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! The row keeps its tabs in the order given, side by side and touching -
 *  a row of names, not a set of gaps. */
void tst_MaterialTabs::theTabsStandInARow()
{
    QCOMPARE(m_tabs->count(), 3);
    QCOMPARE(m_tabs->tabLabel(0), QStringLiteral("Overview"));
    QCOMPARE(m_tabs->tabLabel(3), QString());
    QCOMPARE(m_tabs->currentIndex(), 0);

    QCOMPARE(m_tabs->tabRect(0).left(), 0);
    QCOMPARE(m_tabs->tabRect(1).left(), m_tabs->tabRect(0).left() + m_tabs->tabRect(0).width());
    QCOMPARE(m_tabs->tabRect(2).left(), m_tabs->tabRect(1).left() + m_tabs->tabRect(1).width());
    // A tab is at least the family's ninety wide, whatever it says.
    QVERIFY(m_tabs->tabRect(0).width() >= 90);
    QCOMPARE(m_tabs->tabRect(3), QRect());
}

/*! The indicator stands under the tab in force, as wide as the tab is. */
void tst_MaterialTabs::theIndicatorStandsUnderTheTabInForce()
{
    QRect room = indicatorRoom(m_tabs->grab().toImage());
    QVERIFY(!room.isNull());
    QCOMPARE(room.left(), m_tabs->tabRect(0).left());
    QCOMPARE(room.width(), m_tabs->tabRect(0).width());

    m_tabs->setCurrentIndex(1);
    room = arrivedIndicator(m_tabs);
    QCOMPARE(room.left(), m_tabs->tabRect(1).left());
    QCOMPARE(room.width(), m_tabs->tabRect(1).width());
}

/*! With motion on, the indicator is not under the new tab the moment the tab
 *  changes: it gets there, and then it stays. */
void tst_MaterialTabs::theIndicatorTravelsThere()
{
    const QRect from = indicatorRoom(m_tabs->grab().toImage());
    m_tabs->setCurrentIndex(2);

    // Straight away it is still on its way - it started where it stood.
    const QRect starting = indicatorRoom(m_tabs->grab().toImage());
    QVERIFY(starting.left() < m_tabs->tabRect(2).left());

    const QRect arrived = arrivedIndicator(m_tabs);
    QCOMPARE(arrived.left(), m_tabs->tabRect(2).left());
    QVERIFY(from.left() < arrived.left());
}

/*! Asked for less motion, there is nothing to travel: the indicator is under
 *  the new tab at once. */
void tst_MaterialTabs::noMotionPutsItThereAtOnce()
{
    QxTheme::setMotionReduced(true);
    m_tabs->setCurrentIndex(2);

    const QRect room = indicatorRoom(m_tabs->grab().toImage());
    QVERIFY(!room.isNull());
    QCOMPARE(room.left(), m_tabs->tabRect(2).left());
    QCOMPARE(room.width(), m_tabs->tabRect(2).width());
}

/*! A click puts the tab under the pointer in front, and says so once - a
 *  click on the tab already in front says nothing. */
void tst_MaterialTabs::aClickMovesTheTab()
{
    QSignalSpy changedSpy(m_tabs, &MaterialTabs::currentChanged);

    const QRect second = m_tabs->tabRect(1);
    QTest::mouseClick(m_tabs, Qt::LeftButton, Qt::NoModifier, second.center());
    QCOMPARE(m_tabs->currentIndex(), 1);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.at(0).at(0).toInt(), 1);

    QTest::mouseClick(m_tabs, Qt::LeftButton, Qt::NoModifier, second.center());
    QCOMPARE(changedSpy.count(), 1);

    // A tab that is not there is no answer, and neither is one past the end.
    m_tabs->setCurrentIndex(3);
    m_tabs->setCurrentIndex(-1);
    QCOMPARE(m_tabs->currentIndex(), 1);
    QCOMPARE(changedSpy.count(), 1);
}

/*! The arrow keys walk the row, and stop at its ends. */
void tst_MaterialTabs::theKeyboardWalksTheRow()
{
    m_tabs->setFocus();
    QTest::keyClick(m_tabs, Qt::Key_Right);
    QCOMPARE(m_tabs->currentIndex(), 1);
    QTest::keyClick(m_tabs, Qt::Key_Right);
    QCOMPARE(m_tabs->currentIndex(), 2);
    // The last tab is the end of the row.
    QTest::keyClick(m_tabs, Qt::Key_Right);
    QCOMPARE(m_tabs->currentIndex(), 2);

    QTest::keyClick(m_tabs, Qt::Key_Left);
    QCOMPARE(m_tabs->currentIndex(), 1);
    QTest::keyClick(m_tabs, Qt::Key_Left);
    QCOMPARE(m_tabs->currentIndex(), 0);
    QTest::keyClick(m_tabs, Qt::Key_Left);
    QCOMPARE(m_tabs->currentIndex(), 0);
}

/*! A row too narrow for its tabs slides to the tab in force: the tab that is
 *  chosen is whole, and the row says so with the indicator inside it. */
void tst_MaterialTabs::aNarrowRowSlidesToTheTabInForce()
{
    MaterialTabs narrow;
    for (int i = 0; i < 6; ++i) {
        narrow.addTab(QStringLiteral("Step %1").arg(i + 1));
    }
    narrow.resize(200, narrow.sizeHint().height());
    narrow.show();

    QVERIFY(narrow.tabRect(0).left() >= 0);
    narrow.setCurrentIndex(5);

    // The last tab is pulled fully into the row.
    const QRect last = narrow.tabRect(5);
    QVERIFY(last.left() >= 0);
    QVERIFY(last.right() < narrow.width());

    const QRect room = arrivedIndicator(&narrow);
    QVERIFY(!room.isNull());
    QCOMPARE(room.left(), last.left());
    QCOMPARE(room.width(), last.width());
}

TEST_ADD(tst_MaterialTabs)

#include "tst_materialtabs.moc"
