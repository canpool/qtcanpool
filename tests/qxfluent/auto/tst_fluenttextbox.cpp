/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttextbox.h"

#include "qxtheme/qxthememanager.h"

#include <QFontDatabase>
#include <QLineEdit>
#include <QSignalSpy>
#include <QStyle>
#include <QtTest>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

QImage grabOf(QWidget *widget)
{
    return widget->grab().toImage();
}

}   // namespace

class tst_FluentTextBox : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void thePromptStandsWhereTheWordsWillAndGoes();
    void theClearEmptiesAndSaysSo();
    void readOnlyKeepsTheWordsAndLosesTheClear();
    void disabledKeepsTheWordsAndTakesNoMore();
    void theKeyboardArrivalIsTheAccentUnderline();
    void anIconMakesRoomBeforeTheWords();
};

void tst_FluentTextBox::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    qtcGiveFonts();
}

void tst_FluentTextBox::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
}

/*! The prompt stands where the words will while there are none, and goes the
 *  moment there are words: filled, the box shows the words and nothing else
 *  - the same drawing a box without a prompt gives. */
void tst_FluentTextBox::thePromptStandsWhereTheWordsWillAndGoes()
{
    FluentTextBox bare;
    bare.resize(160, 32);
    bare.show();
    const QImage bareResting = grabOf(&bare);

    FluentTextBox box;
    box.resize(160, 32);
    box.setPlaceholderText("Search the widgets");
    box.show();
    const QImage prompted = grabOf(&box);
    // The prompt is the ink, held back: something is written where the words
    // will go.
    QVERIFY(prompted != bareResting);

    box.setText("Go");
    bare.setText("Go");
    // Whichever box the activation handed the keyboard to, hand it back: the
    // caret and the underline are the keyboard's answers, not the words'.
    box.lineEdit()->clearFocus();
    bare.lineEdit()->clearFocus();
    // The words and nothing else: the prompt did not stay under them.
    QCOMPARE(grabOf(&box), grabOf(&bare));
}

/*! The clear answers a click by emptying the box and saying so - and only
 *  when there was something to erase. */
void tst_FluentTextBox::theClearEmptiesAndSaysSo()
{
    FluentTextBox box;
    box.resize(160, 32);
    box.show();
    box.setText("Widget");
    QCOMPARE(box.text(), QStringLiteral("Widget"));

    QSignalSpy clearedSpy(&box, &FluentTextBox::cleared);
    QSignalSpy changedSpy(&box, &FluentTextBox::textChanged);
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 20, 16));
    QCOMPARE(box.text(), QString());
    QCOMPARE(clearedSpy.count(), 1);
    QCOMPARE(changedSpy.count(), 1);

    // An empty box has nothing to erase: the click is nobody's.
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 20, 16));
    QCOMPARE(clearedSpy.count(), 1);
}

/*! Read-only, the words stay and nothing more comes in - and the clear is
 *  gone, because a box that cannot be worked cannot be emptied either. */
void tst_FluentTextBox::readOnlyKeepsTheWordsAndLosesTheClear()
{
    FluentTextBox box;
    box.resize(160, 32);
    box.show();
    box.setText("Widget");
    box.lineEdit()->setReadOnly(true);

    QTest::keyClicks(box.lineEdit(), "s");
    QCOMPARE(box.text(), QStringLiteral("Widget"));

    QSignalSpy clearedSpy(&box, &FluentTextBox::cleared);
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 20, 16));
    QCOMPARE(box.text(), QStringLiteral("Widget"));
    QCOMPARE(clearedSpy.count(), 0);
}

/*! Disabled, the box stops being worked: the editor with it, and the clear
 *  does not answer. */
void tst_FluentTextBox::disabledKeepsTheWordsAndTakesNoMore()
{
    FluentTextBox box;
    box.resize(160, 32);
    box.show();
    box.setText("Widget");
    box.setEnabled(false);

    QVERIFY(!box.lineEdit()->isEnabled());

    QSignalSpy clearedSpy(&box, &FluentTextBox::cleared);
    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 20, 16));
    QCOMPARE(box.text(), QStringLiteral("Widget"));
    QCOMPARE(clearedSpy.count(), 0);
}

/*! The keyboard's arrival is the accent along the foot, and its leaving is
 *  the accent going back in. */
void tst_FluentTextBox::theKeyboardArrivalIsTheAccentUnderline()
{
    FluentTextBox box;
    box.resize(160, 32);
    box.show();

    const QImage resting = grabOf(&box);
    QVERIFY(resting.pixelColor(box.width() / 2, box.height() - 1) != kAccent);

    QEvent focusIn(QEvent::FocusIn);
    QApplication::sendEvent(box.lineEdit(), &focusIn);
    const QImage focused = grabOf(&box);
    QCOMPARE(focused.pixelColor(box.width() / 2, box.height() - 1), kAccent);
    QVERIFY(resting != focused);

    QEvent focusOut(QEvent::FocusOut);
    QApplication::sendEvent(box.lineEdit(), &focusOut);
    QCOMPARE(grabOf(&box).pixelColor(box.width() / 2, box.height() - 1),
             resting.pixelColor(box.width() / 2, box.height() - 1));
}

/*! An icon before the words makes room before the words: the box asks for
 *  the wider hint. */
void tst_FluentTextBox::anIconMakesRoomBeforeTheWords()
{
    FluentTextBox box;
    box.setPlaceholderText("Search");
    const QSize bare = box.sizeHint();

    box.setLeadingIcon(box.style()->standardIcon(QStyle::SP_FileDialogContentsView));
    QVERIFY(box.sizeHint().width() > bare.width());
}

TEST_ADD(tst_FluentTextBox)

#include "tst_fluenttextbox.moc"
