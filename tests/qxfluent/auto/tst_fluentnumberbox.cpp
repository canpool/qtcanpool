/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentnumberbox.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QApplication>
#include <QFocusEvent>
#include <QImage>
#include <QLineEdit>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A box sized to its hint: the editor runs from 8 to 96, so the stepper column
// is 96 to 120 and its two halves are 0-16 and 16-32.
constexpr int kBoxWidth = 120;
constexpr int kBoxHeight = 32;
constexpr int kUpStep = 8;
constexpr int kDownStep = 24;
constexpr int kStepX = 108;

/*! The accent, pinned: a pixel probe that depends on the platform's accent
 *  depends on the machine it runs on. */
const QColor kAccent(0, 0, 255);

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! The editor is not part of the API: a number box is the field, and the field
 *  is an implementation detail it owns. The test reaches in the way a user's
 *  keyboard does, which is the only way that matters. */
QLineEdit *editorOf(FluentNumberBox *box)
{
    return box->findChild<QLineEdit *>();
}

/*! Take the keyboard away from the editor, whatever the window manager thinks
 *  about focus: leaving the field is what commits it. */
void leaveField(QLineEdit *editor)
{
    QFocusEvent out(QEvent::FocusOut, Qt::OtherFocusReason);
    QApplication::sendEvent(editor, &out);
}
}   // namespace

class tst_FluentNumberBox : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theValueStaysInsideTheRange();
    void theStepperStepsOneAtATime();
    void theKeyboardWalksTheRange();
    void typingNeverReachesTheValue();
    void aTypedNumberBecomesTheValue();
    void theValueIsThereForAnAssistiveTool();
    void theFieldSaysItHasTheKeyboard();
    void aBoxNobodyCanUseHoldsItsValue();
private:
    bool m_wasActive = false;
};

void tst_FluentNumberBox::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_FluentNumberBox::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    QAccessible::setActive(m_wasActive);
}

/*! A number box has a range, and the range is the truth: a value asked for
 *  outside it arrives at the end, and shrinking the range pulls the value in
 *  with it rather than leaving a number the box would not accept. */
void tst_FluentNumberBox::theValueStaysInsideTheRange()
{
    FluentNumberBox box;
    box.setRange(0, 100);
    QSignalSpy changed(&box, &FluentNumberBox::valueChanged);

    box.setValue(500);
    QCOMPARE(box.value(), 100);
    QCOMPARE(changed.count(), 1);

    // Asking for the value it already has is not a change.
    box.setValue(500);
    QCOMPARE(changed.count(), 1);

    box.setValue(-20);
    QCOMPARE(box.value(), 0);

    box.setMaximum(50);
    QCOMPARE(box.value(), 0);
    box.setValue(80);
    QCOMPARE(box.value(), 50);

    // And the other way round: a minimum above the value lifts it.
    box.setMinimum(60);
    QCOMPARE(box.value(), 60);
    QCOMPARE(box.minimum(), 60);
    QCOMPARE(box.maximum(), 60);
}

/*! The two chevrons are two commands: the upper one adds a step, the lower one
 *  takes one away, and each press moves the value exactly once. */
void tst_FluentNumberBox::theStepperStepsOneAtATime()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    box.setRange(0, 10);
    box.setValue(5);
    QSignalSpy changed(&box, &FluentNumberBox::valueChanged);

    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, QPoint(kStepX, kUpStep));
    QCOMPARE(box.value(), 6);
    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, QPoint(kStepX, kDownStep));
    QCOMPARE(box.value(), 5);
    QCOMPARE(changed.count(), 2);

    // The ends of the range are ends: a press there does nothing and says
    // nothing.
    box.setValue(10);
    changed.clear();
    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, QPoint(kStepX, kUpStep));
    QCOMPARE(box.value(), 10);
    QCOMPARE(changed.count(), 0);
}

/*! The arrow keys step and PageUp and PageDown jump, wherever the keyboard
 *  arrives: on the field, which is where it normally lands, and on the control
 *  itself, which is what an application that focused the box directly gets. */
void tst_FluentNumberBox::theKeyboardWalksTheRange()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    box.setRange(0, 100);
    box.setSingleStep(2);
    box.setPageStep(20);
    box.setValue(50);

    QLineEdit *editor = editorOf(&box);
    QVERIFY(editor);

    QTest::keyClick(editor, Qt::Key_Up);
    QCOMPARE(box.value(), 52);
    QCOMPARE(editor->text(), QStringLiteral("52"));
    QTest::keyClick(editor, Qt::Key_Down);
    QCOMPARE(box.value(), 50);
    QTest::keyClick(editor, Qt::Key_PageUp);
    QCOMPARE(box.value(), 70);
    QTest::keyClick(editor, Qt::Key_PageDown);
    QCOMPARE(box.value(), 50);

    QTest::keyClick(&box, Qt::Key_Up);
    QCOMPARE(box.value(), 52);

    // Typed text does not block a step: the control is the one talking.
    editor->setText(QStringLiteral("999"));
    QTest::keyClick(editor, Qt::Key_Down);
    QCOMPARE(box.value(), 50);
}

/*! Text that is not a number never reaches the value, and leaving the field
 *  puts the text back to what the value is - the two cannot drift apart. */
void tst_FluentNumberBox::typingNeverReachesTheValue()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    box.setValue(7);

    QLineEdit *editor = editorOf(&box);
    QVERIFY(editor);
    QSignalSpy changed(&box, &FluentNumberBox::valueChanged);

    editor->setText(QStringLiteral("twelve"));
    QCOMPARE(box.value(), 7);
    QCOMPARE(changed.count(), 0);

    leaveField(editor);
    QCOMPARE(box.value(), 7);
    QCOMPARE(editor->text(), QStringLiteral("7"));

    // A number outside the range is still a number: it is taken, clamped and
    // shown clamped, rather than thrown away.
    editor->setText(QStringLiteral("500"));
    leaveField(editor);
    QCOMPARE(box.value(), 100);
    QCOMPARE(editor->text(), QStringLiteral("100"));
}

/*! Enter commits the field where it stands: the number the user typed becomes
 *  the value and the text is rewritten the way the control spells it. */
void tst_FluentNumberBox::aTypedNumberBecomesTheValue()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    box.setRange(0, 100);
    box.setValue(1);

    QLineEdit *editor = editorOf(&box);
    QVERIFY(editor);

    editor->setText(QStringLiteral("042"));
    QTest::keyClick(editor, Qt::Key_Return);
    QCOMPARE(box.value(), 42);
    QCOMPARE(editor->text(), QStringLiteral("42"));

    // Escape is the other answer: the text is dropped and the field goes back
    // to the value.
    editor->setText(QStringLiteral("nonsense"));
    QTest::keyClick(editor, Qt::Key_Escape);
    QCOMPARE(box.value(), 42);
    QCOMPARE(editor->text(), QStringLiteral("42"));
}

/*! A spin box is one of the controls whose answer is a number, so the face it
 *  hands an assistive tool carries the number, the ends of the range and the
 *  size of a step - and the tool can set it back. */
void tst_FluentNumberBox::theValueIsThereForAnAssistiveTool()
{
    QAccessible::setActive(true);
    FluentNumberBox box;
    box.setRange(0, 100);
    box.setSingleStep(3);
    box.setValue(12);

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&box);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::SpinBox);

    auto *value = static_cast<QAccessibleValueInterface *>(iface->interface_cast(QAccessible::ValueInterface));
    QVERIFY(value);
    QCOMPARE(value->currentValue().toInt(), 12);
    QCOMPARE(value->minimumValue().toInt(), 0);
    QCOMPARE(value->maximumValue().toInt(), 100);
    QCOMPARE(value->minimumStepSize().toInt(), 3);

    value->setCurrentValue(30);
    QCOMPARE(box.value(), 30);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! Fluent says "this has the keyboard" with the accent under the field, so a
 *  field that has it has to look different from one that does not. */
void tst_FluentNumberBox::theFieldSaysItHasTheKeyboard()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    // The keyboard only lands on a widget that is on a screen, and the frame
    // under the field is what changes when it does.
    box.show();
    QVERIFY(QTest::qWaitForWindowExposed(&box));

    box.setFocus();
    QVERIFY(box.hasFocus());
    const QColor focused = pixel(&box, kBoxWidth / 2, kBoxHeight - 2);

    box.clearFocus();
    QVERIFY(!box.hasFocus());
    const QColor resting = pixel(&box, kBoxWidth / 2, kBoxHeight - 2);

    QVERIFY(focused != resting);
    QCOMPARE(focused.blue(), kAccent.blue());
}

/*! A box nobody can use keeps what it holds: the arrows and the keys are the
 *  question, and a control that cannot take it does not answer. */
void tst_FluentNumberBox::aBoxNobodyCanUseHoldsItsValue()
{
    FluentNumberBox box;
    box.resize(kBoxWidth, kBoxHeight);
    box.setRange(0, 100);
    box.setValue(50);
    const QImage live = box.grab().toImage();

    box.setEnabled(false);
    QLineEdit *editor = editorOf(&box);
    QVERIFY(editor);
    QTest::keyClick(editor, Qt::Key_Up);
    QTest::keyClick(editor, Qt::Key_PageDown);

    QCOMPARE(box.value(), 50);
    QVERIFY(box.grab().toImage() != live);
}

TEST_ADD(tst_FluentNumberBox)

#include "tst_fluentnumberbox.moc"
