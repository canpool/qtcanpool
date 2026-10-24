/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcheckbox.h"
#include "qxfluent/fluentradiobutton.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QImage>
#include <QSignalSpy>
#include <QtWidgets/QApplication>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void sendEnter(QWidget *widget)
{
    QEnterEvent enter(QPointF(1, 1), QPointF(1, 1), QPointF(1, 1));
    QApplication::sendEvent(widget, &enter);
}
#else
void sendEnter(QWidget *widget)
{
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
}
#endif
}   // namespace

class tst_FluentCheckable : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theThreeAnswersWearThreeFaces();
    void theRowIsWhatAnswers();
    void theKeyboardAsksTheSameQuestion();
    void oneChoiceOfSeveralStandsAlone();
    void whatAnAssistiveToolReads();
    void aChoiceNobodyCanUseDoesNotChange();
    void theKeyboardLandsOnIt();
private:
    bool m_hadOverride = false;
};

void tst_FluentCheckable::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_FluentCheckable::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! The empty box is the page under a border, a box with an answer wears the
 *  accent, and the partial answer is the accent with the bar in it. */
void tst_FluentCheckable::theThreeAnswersWearThreeFaces()
{
    FluentCheckBox box;
    box.resize(box.sizeHint());

    // A point inside the box that neither the tick nor the bar crosses.
    const QPoint inside(7, 16);
    box.setCheckState(Qt::Unchecked);
    QCOMPARE(box.grab().toImage().pixelColor(inside), box.palette().color(QPalette::Window));

    box.setCheckState(Qt::Checked);
    QCOMPARE(box.grab().toImage().pixelColor(inside), kAccent);

    box.setCheckState(Qt::PartiallyChecked);
    QCOMPARE(box.grab().toImage().pixelColor(inside), kAccent);

    FluentRadioButton radio;
    radio.resize(radio.sizeHint());
    QVERIFY(!radio.isChecked());
    radio.setChecked(true);
    QVERIFY(radio.isChecked());
}

/*! The label is half of the same answer: a press on the words turns the box
 *  over, exactly as a press on the box does. */
void tst_FluentCheckable::theRowIsWhatAnswers()
{
    FluentCheckBox box;
    box.setText(QStringLiteral("Include archives"));
    box.resize(box.sizeHint());
    QSignalSpy changed(&box, &FluentCheckBox::stateChanged);

    // The words stand at the right of the box; the press lands there.
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QCOMPARE(box.checkState(), Qt::Checked);
    QCOMPARE(changed.count(), 1);

    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QCOMPARE(box.checkState(), Qt::Unchecked);
}

/*! The keyboard asks with the space bar, and the three-state box walks the
 *  three answers a list can give. */
void tst_FluentCheckable::theKeyboardAsksTheSameQuestion()
{
    FluentCheckBox box;
    box.setTristate(true);
    box.resize(box.sizeHint());

    QTest::keyClick(&box, Qt::Key_Space);
    QCOMPARE(box.checkState(), Qt::Checked);
    QTest::keyClick(&box, Qt::Key_Space);
    QCOMPARE(box.checkState(), Qt::PartiallyChecked);
    QTest::keyClick(&box, Qt::Key_Space);
    QCOMPARE(box.checkState(), Qt::Unchecked);
}

/*! The choice is one of several: the brothers that share its parent cannot
 *  be on while one of them is. */
void tst_FluentCheckable::oneChoiceOfSeveralStandsAlone()
{
    QWidget row;
    FluentRadioButton *first = new FluentRadioButton(&row);
    first->setText(QStringLiteral("Daily"));
    FluentRadioButton *second = new FluentRadioButton(&row);
    second->setText(QStringLiteral("Weekly"));
    first->resize(first->sizeHint());
    second->resize(second->sizeHint());

    first->setChecked(true);
    QVERIFY(first->isChecked());
    QVERIFY(!second->isChecked());

    second->setChecked(true);
    QVERIFY(second->isChecked());
    QVERIFY(!first->isChecked());

    // A button already in force stays so: a press is not a turn-over.
    second->setChecked(true);
    QVERIFY(second->isChecked());
}

/*! What it is, is a check box or a radio button, and what is true of it is
 *  that it is checkable and when it is on, checked. */
void tst_FluentCheckable::whatAnAssistiveToolReads()
{
    FluentCheckBox box;
    QAccessibleInterface *boxFace = QAccessible::queryAccessibleInterface(&box);
    QVERIFY(boxFace);
    QCOMPARE(boxFace->role(), QAccessible::CheckBox);
    QVERIFY(boxFace->state().checkable);
    QVERIFY(!boxFace->state().checked);
    box.setCheckState(Qt::Checked);
    QVERIFY(QAccessible::queryAccessibleInterface(&box)->state().checked);

    FluentRadioButton radio;
    QAccessibleInterface *radioFace = QAccessible::queryAccessibleInterface(&radio);
    QVERIFY(radioFace);
    QCOMPARE(radioFace->role(), QAccessible::RadioButton);
    QVERIFY(radioFace->state().checkable);
    radio.setChecked(true);
    QVERIFY(QAccessible::queryAccessibleInterface(&radio)->state().checked);
}

/*! A choice nobody can use does not change, however the question is put: the
 *  row still takes the click and the key, and answers neither. */
void tst_FluentCheckable::aChoiceNobodyCanUseDoesNotChange()
{
    FluentCheckBox box;
    box.setText(QStringLiteral("Include archives"));
    box.resize(box.sizeHint());
    const QImage live = box.grab().toImage();

    // Qt fades a disabled widget through its palette; pinned, the only thing
    // left that can change is what the control itself paints.
    qtcPinDisabledColours(&box);

    box.setEnabled(false);
    QSignalSpy changed(&box, &FluentCheckBox::stateChanged);
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QTest::keyClick(&box, Qt::Key_Space);

    QCOMPARE(box.checkState(), Qt::Unchecked);
    QCOMPARE(changed.count(), 0);
    QVERIFY(box.grab().toImage() != live);
}

/*! The keyboard finds it, and the ring says where the keyboard is: a control
 *  the keyboard cannot reach is a control half the users cannot use, and one it
 *  reaches without a mark is a control they cannot find.
 */
void tst_FluentCheckable::theKeyboardLandsOnIt()
{
    FluentCheckBox widget;
    widget.setText(QStringLiteral("Remember me"));
    widget.resize(widget.sizeHint());
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_FluentCheckable)

#include "tst_fluentcheckable.moc"
