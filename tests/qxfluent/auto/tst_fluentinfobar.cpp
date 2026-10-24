/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentinfobar.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QImage>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);
const QColor kError(0xC4, 0x2B, 0x1C);

// The icon disc is twenty-four and sits twelve in, so its middle is at 24.
constexpr int kDiscCenter = 24;
}   // namespace

class tst_FluentInfoBar : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void theSeverityIsWhatColoursIt();
    void closingTakesItAwayAndSaysSo();
    void theRestOfTheBarDoesNotCloseIt();
    void aMessageIsAnotherLine();
    void aBarNobodyCanUseIsDrawnFainter();
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

/*! A bar nobody can use is still read, but at a step down: the disc, the title,
 *  the message and the cross all fade together. The palette's disabled group is
 *  pinned first - Qt fades a turned-off widget by itself - so the only hand that
 *  can make these two frames differ is the bar's own.
 */
void tst_FluentInfoBar::aBarNobodyCanUseIsDrawnFainter()
{
    FluentInfoBar bar;
    bar.setTitle(QStringLiteral("Saved"));
    bar.setMessage(QStringLiteral("Your changes are on the disk."));
    bar.resize(320, bar.sizeHint().height());
    qtcPinDisabledColours(&bar);

    const QImage live = bar.grab().toImage();
    bar.setEnabled(false);
    const QImage faded = bar.grab().toImage();
    QVERIFY(faded != live);

    // Fainter, not gone, and no longer answering: the cross is a request the bar
    // cannot take any more.
    QVERIFY(bar.isOpen());
    QSignalSpy closed(&bar, &FluentInfoBar::closed);
    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, QPoint(bar.width() - 24, bar.height() / 2));
    QCOMPARE(closed.count(), 0);
    QVERIFY(bar.isOpen());
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

void tst_FluentInfoBar::theKeyboardLandsOnIt()
{
    FluentInfoBar widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentInfoBar)

#include "tst_fluentinfobar.moc"
