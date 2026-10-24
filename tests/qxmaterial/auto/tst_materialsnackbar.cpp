/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxtheme/qxthememotion.h"

#include "qxmaterial/materialsnackbar.h"

#include <QAccessible>
#include <QImage>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// Longer than the longest entrance Material asks for, so a test that looks at
// where a snackbar ends up is not looking at it on the way there.
const int kEntranceWait = 600;

int pixelsOf(const QColor &color, QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) == color) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_MaterialSnackbar : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void itStaysOutOfTheWayUntilItIsAsked();
    void itPutsItselfAtTheBottomOfWhatItIsOn();
    void itGoesAwayOnItsOwn();
    void itDoesNotArriveTwice();
    void theOneThingToDoAboutItCanBeAskedFor();
    void theKeyboardIsAWayOutOfIt();
    void whatAnAssistiveToolReads();
    void aBarNobodyCanAnswerIsDrawnFainter();
    void noMotionPutsItThereAtOnce();
private:
    bool m_wasActive = false;
};

void tst_MaterialSnackbar::cleanup()
{
    QAccessible::setActive(m_wasActive);
}

/*! A snackbar is not a place that is always there: it is not on the screen
 *  until there is something to say. */
void tst_MaterialSnackbar::itStaysOutOfTheWayUntilItIsAsked()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    QVERIFY(!snackbar.isVisible());

    snackbar.showMessage(QStringLiteral("Message sent"));
    QTest::qWait(kEntranceWait);
    QVERIFY(snackbar.isVisible());
    QCOMPARE(snackbar.message(), QStringLiteral("Message sent"));
}

/*! It belongs to the bottom of what it is on, and puts itself there. */
void tst_MaterialSnackbar::itPutsItselfAtTheBottomOfWhatItIsOn()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.showMessage(QStringLiteral("Message sent"), 0);
    QTest::qWait(kEntranceWait);

    QCOMPARE(snackbar.height(), 48);
    QCOMPARE(snackbar.y(), host.height() - 48 - 8);
    QCOMPARE(snackbar.geometry().center().x(), host.rect().center().x());

    // And it goes back there when what it is on changes size.
    host.resize(500, 240);
    QCOMPARE(snackbar.y(), host.height() - 48 - 8);
    QCOMPARE(snackbar.geometry().center().x(), host.rect().center().x());
}

/*! It is not a message box: it does not wait to be answered, it goes away when
 *  it has been read. */
void tst_MaterialSnackbar::itGoesAwayOnItsOwn()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.showMessage(QStringLiteral("Message sent"), 100);
    QVERIFY(snackbar.isVisible());

    // Not a message box: it does not wait to be answered.
    QTest::qWait(400);
    QVERIFY(!snackbar.isVisible());
}

/*! A second thing to say does not make it arrive again: it says the new thing
 *  where it is. */
void tst_MaterialSnackbar::itDoesNotArriveTwice()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.showMessage(QStringLiteral("Message sent"), 0);
    QTest::qWait(kEntranceWait);
    const QPoint where = snackbar.pos();

    snackbar.showMessage(QStringLiteral("Message archived"), 0);
    QCOMPARE(snackbar.pos(), where);
    QCOMPARE(snackbar.message(), QStringLiteral("Message archived"));
}

/*! It offers at most one thing to do about it, and doing it is the end of it. */
void tst_MaterialSnackbar::theOneThingToDoAboutItCanBeAskedFor()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.setActionText(QStringLiteral("Undo"));
    snackbar.showMessage(QStringLiteral("Message archived"), 0);
    QTest::qWait(kEntranceWait);

    QSignalSpy asked(&snackbar, &MaterialSnackbar::actionTriggered);
    QTest::mouseClick(&snackbar, Qt::LeftButton, Qt::NoModifier, QPoint(snackbar.width() - 12, snackbar.height() / 2));
    QCOMPARE(asked.count(), 1);
    QVERIFY(!snackbar.isVisible());
}

/*! It arrived without being asked for, so there is a way to send it away
 *  without waiting. */
void tst_MaterialSnackbar::theKeyboardIsAWayOutOfIt()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.setActionText(QStringLiteral("Undo"));
    snackbar.showMessage(QStringLiteral("Message archived"), 0);
    QTest::qWait(kEntranceWait);
    QVERIFY(snackbar.isVisible());

    QTest::keyClick(&snackbar, Qt::Key_Escape);
    QVERIFY(!snackbar.isVisible());
}

/*! What an assistive tool is told: a message that arrived, and the message is
 *  what it says. */
void tst_MaterialSnackbar::whatAnAssistiveToolReads()
{
    MaterialSnackbar snackbar;
    snackbar.setMessage(QStringLiteral("Message sent"));

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&snackbar);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::AlertMessage);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Message sent"));
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A snackbar nobody can answer is still read, but a step down: the message and
 *  the action together. Pinned first, so the difference measured is the bar's own.
 */
void tst_MaterialSnackbar::aBarNobodyCanAnswerIsDrawnFainter()
{
    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.showMessage(QStringLiteral("Message sent"), 0);
    QTest::qWait(kEntranceWait);
    qtcPinDisabledColours(&snackbar);

    const QImage live = snackbar.grab().toImage();
    snackbar.setEnabled(false);
    const QImage faded = snackbar.grab().toImage();
    QVERIFY(faded != live);
}

/*! A snackbar arrives by rising from under the edge, which is the whole of its
 *  entrance. Asked for less motion it is simply there - not a shorter rise, but
 *  no rise at all.
 */
void tst_MaterialSnackbar::noMotionPutsItThereAtOnce()
{
    const bool was = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    QWidget host;
    host.resize(400, 300);
    host.show();

    MaterialSnackbar snackbar(&host);
    snackbar.showMessage(QStringLiteral("Message sent"), 0);

    const QPoint arrived = snackbar.pos();
    QVERIFY(arrived.y() < host.height());
    QTest::qWait(100);
    QCOMPARE(snackbar.pos(), arrived);

    QxTheme::setMotionReduced(was);
}

TEST_ADD(tst_MaterialSnackbar)

#include "tst_materialsnackbar.moc"
