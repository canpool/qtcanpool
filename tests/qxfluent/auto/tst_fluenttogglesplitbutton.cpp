/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttogglesplitbutton.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QApplication>
#include <QMenu>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A button given a size of its own, so the tests can press the two halves by
// place rather than by asking a layout where they are. In a face one hundred
// and sixty wide the chevron owns x 128 to 148 - twenty pixels, with twelve of
// inset after them - and everything before that is the command.
constexpr int kWidth = 160;
constexpr int kHeight = 32;
constexpr int kBodyX = 12;
constexpr int kChevronX = kWidth - 22;

void giveSize(FluentToggleSplitButton *button)
{
    button->resize(kWidth, kHeight);
}

QColor pixel(QWidget *widget, int x, int y)
{
    const QImage image = widget->grab().toImage();
    const qreal dpr = image.width() / qreal(widget->width());
    return image.pixelColor(qRound(x * dpr), qRound(y * dpr));
}
}   // namespace

class tst_FluentToggleSplitButton : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theBodyTogglesTheCommand();
    void theChevronOpensTheMenu();
    void aPressIsAnsweredByTheHalfItStartedOn();
    void theKeyboardTogglesAndOpens();
    void beingOnIsWashedInTheAccent();
    void aCommandNobodyCanPressIsDrawnFainter();
    void anEmptyMenuIsNotOpened();
    void theMenuIsTheButtonsOwn();
    void whatAnAssistiveToolReads();
private:
    bool m_wasActive = false;
};

void tst_FluentToggleSplitButton::cleanup()
{
    // The accessible registry is global.
    QAccessible::setActive(m_wasActive);
    m_wasActive = false;
}

/*! The left half is the command: it goes down and stays down, and a second
 *  press brings it back up. */
void tst_FluentToggleSplitButton::theBodyTogglesTheCommand()
{
    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    QVERIFY(!button.isChecked());

    QSignalSpy toggled(&button, &FluentToggleSplitButton::toggled);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kBodyX, kHeight / 2));
    QVERIFY(button.isChecked());
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.first().first().toBool(), true);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kBodyX, kHeight / 2));
    QVERIFY(!button.isChecked());
    QCOMPARE(toggled.count(), 2);
    QCOMPARE(toggled.last().first().toBool(), false);
}

/*! The right half is the menu, and it is not the command: opening it leaves
 *  the command exactly as it was. */
void tst_FluentToggleSplitButton::theChevronOpensTheMenu()
{
    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    button.menu()->addAction(QStringLiteral("Pin to the left"));

    QSignalSpy about(&button, &FluentToggleSplitButton::aboutToShowMenu);
    QSignalSpy toggled(&button, &FluentToggleSplitButton::toggled);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kChevronX, kHeight / 2));
    QCOMPARE(about.count(), 1);
    QVERIFY(button.menu()->isVisible());
    QCOMPARE(toggled.count(), 0);
    QVERIFY(!button.isChecked());
    button.menu()->hide();
}

/*! A press is answered by the half it started on, and by nothing else: the
 *  release confirms rather than decides. */
void tst_FluentToggleSplitButton::aPressIsAnsweredByTheHalfItStartedOn()
{
    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    button.menu()->addAction(QStringLiteral("Pin to the left"));

    QSignalSpy about(&button, &FluentToggleSplitButton::aboutToShowMenu);
    QSignalSpy toggled(&button, &FluentToggleSplitButton::toggled);

    // Pressed on the body, released over the chevron: neither thing happened.
    QTest::mousePress(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kBodyX, kHeight / 2));
    QTest::mouseRelease(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kChevronX, kHeight / 2));
    QCOMPARE(about.count(), 0);
    QCOMPARE(toggled.count(), 0);
    QVERIFY(!button.isChecked());

    // Pressed on the chevron, released over the body: the same answer.
    QTest::mousePress(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kChevronX, kHeight / 2));
    QTest::mouseRelease(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kBodyX, kHeight / 2));
    QCOMPARE(about.count(), 0);
    QCOMPARE(toggled.count(), 0);
    QVERIFY(!button.isChecked());
}

/*! There is no keyboard half to aim at, so the keys are told apart: the keys
 *  of a button toggle, the keys of a menu open it. */
void tst_FluentToggleSplitButton::theKeyboardTogglesAndOpens()
{
    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    button.menu()->addAction(QStringLiteral("Pin to the left"));

    QSignalSpy about(&button, &FluentToggleSplitButton::aboutToShowMenu);
    QSignalSpy toggled(&button, &FluentToggleSplitButton::toggled);

    QTest::keyClick(&button, Qt::Key_Space);
    QVERIFY(button.isChecked());
    QCOMPARE(toggled.count(), 1);

    QTest::keyClick(&button, Qt::Key_Return);
    QVERIFY(!button.isChecked());
    QCOMPARE(toggled.count(), 2);

    QTest::keyClick(&button, Qt::Key_Down);
    QCOMPARE(about.count(), 1);
    QVERIFY(button.menu()->isVisible());
    button.menu()->hide();

    QTest::keyClick(&button, Qt::Key_F4);
    QCOMPARE(about.count(), 2);
    QVERIFY(button.menu()->isVisible());
    button.menu()->hide();

    // A menu key is not a toggle key: the command is where it was.
    QCOMPARE(toggled.count(), 2);
    QVERIFY(!button.isChecked());
}

/*! A command in force is washed in the accent, over both halves, and the wash
 *  goes away when it is not in force. */
void tst_FluentToggleSplitButton::beingOnIsWashedInTheAccent()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor(0, 0, 255));

    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));

    // Inside the chevron's half and clear of both edges, where the focus ring
    // runs: the wash is the only thing at that point that can change.
    const int x = kWidth - 24;
    const int y = 8;
    const QColor off = pixel(&button, x, y);

    button.setChecked(true);
    const QColor on = pixel(&button, x, y);
    QVERIFY(on != off);
    QVERIFY(on.blue() > off.blue());

    button.setChecked(false);
    QCOMPARE(pixel(&button, x, y), off);

    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
}

/*! A menu with nothing in it is not a menu, whichever half asked. */
void tst_FluentToggleSplitButton::anEmptyMenuIsNotOpened()
{
    FluentToggleSplitButton button;
    giveSize(&button);

    QSignalSpy about(&button, &FluentToggleSplitButton::aboutToShowMenu);
    QVERIFY(button.menu()->isEmpty());

    button.showMenu();
    QCOMPARE(about.count(), 1);
    QVERIFY(!button.menu()->isVisible());
}

void tst_FluentToggleSplitButton::theMenuIsTheButtonsOwn()
{
    FluentToggleSplitButton button;
    QVERIFY(button.menu());
    QCOMPARE(button.menu()->parent(), static_cast<QObject *>(&button));
    QCOMPARE(button.menu(), button.menu());
}

/*! For an assistive tool it is a check box, because the question it answers is
 *  whether the command is in force. */
void tst_FluentToggleSplitButton::whatAnAssistiveToolReads()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentToggleSplitButton button;
    button.setText(QStringLiteral("Pin"));

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&button);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::CheckBox);
    QVERIFY(iface->state().checkable);
    QVERIFY(!iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    button.setChecked(true);
    QAccessibleInterface *onIface = QAccessible::queryAccessibleInterface(&button);
    QVERIFY(onIface);
    QVERIFY(onIface->state().checked);
    QCOMPARE(onIface->text(QAccessible::Name), QStringLiteral("Pin"));
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(onIface));
}

/*! A command nobody can press is written a step down - the words, the icon and
 *  the chevron - and neither half answers. Pinned to the active colours first, so
 *  the frame that differs is the button's own drawing.
 */
void tst_FluentToggleSplitButton::aCommandNobodyCanPressIsDrawnFainter()
{
    FluentToggleSplitButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Pin"));
    button.show();
    qtcPinDisabledColours(&button);

    const QImage live = button.grab().toImage();
    button.setEnabled(false);
    const QImage faded = button.grab().toImage();
    QVERIFY(faded != live);

    QSignalSpy toggled(&button, &FluentToggleSplitButton::toggled);
    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kBodyX, kHeight / 2));
    QVERIFY(!button.isChecked());
    QCOMPARE(toggled.count(), 0);
}

TEST_ADD(tst_FluentToggleSplitButton)

#include "tst_fluenttogglesplitbutton.moc"
