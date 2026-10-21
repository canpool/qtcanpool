/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentexpander.h"
#include "qxfluent/fluentinfobar.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QImage>
#include <QLabel>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);
const QColor kError(0xC4, 0x2B, 0x1C);
constexpr int kSettleWait = 500;

// The header is forty tall, the icon disc is twenty-four and sits twelve in.
constexpr int kHeaderHeight = 40;
constexpr int kDiscCenter = 24;
}   // namespace

class tst_FluentExpander : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theHeaderIsWhatThePointerCanReach();
    void openingGivesTheBodyRoom();
    void reduceMotionOpensItAtOnce();
    void itIsAButtonThatOpensSomething();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentExpander::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! A click below the header is not a request to open: the header is the
 *  control, and what it keeps is not. */
void tst_FluentExpander::theHeaderIsWhatThePointerCanReach()
{
    FluentExpander expander;
    expander.resize(200, 120);

    QTest::mousePress(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, kHeaderHeight + 20));
    QTest::mouseRelease(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, kHeaderHeight + 20));
    QVERIFY(!expander.isExpanded());

    QTest::mousePress(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QTest::mouseRelease(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QVERIFY(expander.isExpanded());
}

/*! Open is not a flag: it is room, and the room is the body's. */
void tst_FluentExpander::openingGivesTheBodyRoom()
{
    FluentExpander expander;
    QLabel body(QStringLiteral("Body"));
    expander.setContent(&body);
    const int bodyHeight = body.sizeHint().height();

    QCOMPARE(expander.sizeHint().height(), kHeaderHeight);

    expander.setExpanded(true);
    QTest::qWait(kSettleWait);

    QCOMPARE(expander.sizeHint().height(), kHeaderHeight + bodyHeight);
    // isVisible() answers for the whole ancestry, and nothing here is shown;
    // isHidden() is the question about the body itself.
    QVERIFY(!body.isHidden());
}

/*! Asked for less motion, an expander is simply open - the movement was never
 *  the point. */
void tst_FluentExpander::reduceMotionOpensItAtOnce()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    FluentExpander expander;
    QLabel body(QStringLiteral("Body"));
    expander.setContent(&body);

    expander.setExpanded(true);
    // No wait: under reduce-motion the room is there at once.
    QCOMPARE(expander.sizeHint().height(), kHeaderHeight + body.sizeHint().height());
}

/*! What it is, and what is true of it: a button that opens something, and
 *  whether what it keeps is out. */
void tst_FluentExpander::itIsAButtonThatOpensSomething()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentExpander expander;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&expander);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Button);
    QVERIFY(iface->state().expandable);
    QVERIFY(!iface->state().expanded);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    expander.setExpanded(true);
    QAccessibleInterface *openIface = QAccessible::queryAccessibleInterface(&expander);
    QVERIFY(openIface);
    QVERIFY(openIface->state().expanded);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(openIface));
}

class tst_FluentInfoBar : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theSeverityIsWhatColoursIt();
    void closingTakesItAwayAndSaysSo();
    void theRestOfTheBarDoesNotCloseIt();
    void aMessageIsAnotherLine();
    void itIsAnAlert();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_FluentInfoBar::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_FluentInfoBar::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! The severity is what the bar is about, so the severity is the colour: the
 *  accent when it is this application speaking, its own red when it is not. */
void tst_FluentInfoBar::theSeverityIsWhatColoursIt()
{
    FluentInfoBar informational;
    informational.resize(informational.sizeHint());
    QCOMPARE(informational.grab().toImage().pixelColor(16, kDiscCenter), kAccent);

    FluentInfoBar error;
    error.setSeverity(FluentInfoBar::Severity::Error);
    error.resize(error.sizeHint());
    QCOMPARE(error.grab().toImage().pixelColor(16, kDiscCenter), kError);
}

/*! Closing is not drawing the bar differently: it is gone, and it says it
 *  went. */
void tst_FluentInfoBar::closingTakesItAwayAndSaysSo()
{
    FluentInfoBar bar;
    bar.resize(bar.sizeHint());
    QSignalSpy closed(&bar, &FluentInfoBar::closed);

    const QSize size = bar.sizeHint();
    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, QPoint(size.width() - 24, size.height() / 2));

    QVERIFY(!bar.isOpen());
    QVERIFY(!bar.isVisible());
    QCOMPARE(closed.count(), 1);
}

/*! The cross is small and the bar is not: a click that missed the cross is
 *  not a request to close. */
void tst_FluentInfoBar::theRestOfTheBarDoesNotCloseIt()
{
    FluentInfoBar bar;
    bar.setTitle(QStringLiteral("Saved"));
    bar.setMessage(QStringLiteral("Your changes are on the disk."));
    bar.resize(320, bar.sizeHint().height());

    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, QPoint(100, bar.height() / 2));

    QVERIFY(bar.isOpen());
}

/*! A title and a message are two lines, and the bar is as tall as both. */
void tst_FluentInfoBar::aMessageIsAnotherLine()
{
    FluentInfoBar titled;
    titled.setTitle(QStringLiteral("Saved"));
    const int oneLine = titled.sizeHint().height();

    FluentInfoBar withMessage;
    withMessage.setTitle(QStringLiteral("Saved"));
    withMessage.setMessage(QStringLiteral("Your changes are on the disk."));

    QVERIFY(withMessage.sizeHint().height() > oneLine);
}

/*! A bar is a message that stays, which is what an assistive tool calls an
 *  alert. */
void tst_FluentInfoBar::itIsAnAlert()
{
    QAccessible::setActive(true);
    FluentInfoBar bar;

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&bar);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::AlertMessage);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_FluentExpander)
TEST_ADD(tst_FluentInfoBar)

#include "tst_fluentexpander.moc"
