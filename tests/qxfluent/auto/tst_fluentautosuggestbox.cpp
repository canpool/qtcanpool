/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentautosuggestbox.h"

#include "qxfluent/fluentmenuflyout.h"
#include <QLineEdit>

#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE

namespace
{
void giveSuggestions(FluentAutoSuggestBox *box)
{
    box->setSuggestions(QStringList() << QStringLiteral("Apples") << QStringLiteral("Apricots")
                                      << QStringLiteral("Bananas") << QStringLiteral("Cherries"));
}
}   // namespace

class tst_FluentAutoSuggestBox : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();

    void theWritingDrivesTheList();
    void enterTakesWhatIsInHand();
    void aClickTakesWhatWasClicked();
    void escapePutsTheListAwayWithoutTouchingTheWriting();
    void aListWithNothingToOfferDoesNotOpen();
    void aBoxNobodyCanTypeInIsDrawnFainter();
};

void tst_FluentAutoSuggestBox::init()
{
    qtcGiveFonts();
}

/*! What still matches is in the list, and what matches better comes higher;
 *  the list is rebuilt as the writing goes. */
void tst_FluentAutoSuggestBox::theWritingDrivesTheList()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();

    box.lineEdit()->setText(QStringLiteral("a"));
    QVERIFY(box.flyout()->isVisible());
    QCOMPARE(box.flyout()->actions().count(), 3);   // Apples, Apricots, Bananas

    box.lineEdit()->setText(QStringLiteral("ap"));
    QVERIFY(box.flyout()->isVisible());
    QCOMPARE(box.flyout()->actions().count(), 2);   // Apples, Apricots
    QCOMPARE(box.flyout()->actions().first()->text(), QStringLiteral("Apples"));

    box.lineEdit()->setText(QStringLiteral("che"));
    QCOMPARE(box.flyout()->actions().count(), 1);   // Cherries
}

/*! Enter works the command in hand: the word comes into the box. */
void tst_FluentAutoSuggestBox::enterTakesWhatIsInHand()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();
    box.lineEdit()->setText(QStringLiteral("a"));
    QVERIFY(box.flyout()->isVisible());

    QSignalSpy accepted(&box, &FluentAutoSuggestBox::suggestionAccepted);

    QTest::keyClick(box.lineEdit(), Qt::Key_Down);
    QCOMPARE(box.flyout()->currentAction()->text(), QStringLiteral("Apricots"));
    QTest::keyClick(box.lineEdit(), Qt::Key_Down);
    QCOMPARE(box.flyout()->currentAction()->text(), QStringLiteral("Bananas"));

    QTest::keyClick(box.lineEdit(), Qt::Key_Return);
    QCOMPARE(box.text(), QStringLiteral("Bananas"));
    QCOMPARE(accepted.count(), 1);
    QCOMPARE(accepted.first().first().toString(), QStringLiteral("Bananas"));
    QVERIFY(!box.flyout()->isVisible());
}

/*! A click on a suggestion takes it the same way. */
void tst_FluentAutoSuggestBox::aClickTakesWhatWasClicked()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();
    box.lineEdit()->setText(QStringLiteral("a"));
    QVERIFY(box.flyout()->isVisible());

    QSignalSpy accepted(&box, &FluentAutoSuggestBox::suggestionAccepted);
    // The second row's middle: rows of thirty-two from the content's top.
    const int rowY = int(box.flyout()->contentRect().top()) + 4 + 32 + 16;
    QTest::mouseClick(box.flyout(), Qt::LeftButton, Qt::NoModifier,
                      QPoint(int(box.flyout()->contentRect().left()) + 40, rowY));

    QCOMPARE(box.text(), QStringLiteral("Apricots"));
    QCOMPARE(accepted.count(), 1);
    QVERIFY(!box.flyout()->isVisible());
}

/*! Escape puts the list away and touches nothing: what was written stays. */
void tst_FluentAutoSuggestBox::escapePutsTheListAwayWithoutTouchingTheWriting()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();
    box.lineEdit()->setText(QStringLiteral("a"));
    QVERIFY(box.flyout()->isVisible());

    QTest::keyClick(box.lineEdit(), Qt::Key_Escape);
    QVERIFY(!box.flyout()->isVisible());
    QCOMPARE(box.text(), QStringLiteral("a"));
}

/*! Nothing matches, nothing opens - and a list already open closes rather
 *  than stand there offering nothing. */
void tst_FluentAutoSuggestBox::aListWithNothingToOfferDoesNotOpen()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();

    box.lineEdit()->setText(QStringLiteral("zzz"));
    QVERIFY(!box.flyout()->isVisible());

    box.lineEdit()->setText(QStringLiteral("a"));
    QVERIFY(box.flyout()->isVisible());
    box.lineEdit()->setText(QStringLiteral("zz"));
    QVERIFY(!box.flyout()->isVisible());
}

/*! A suggest box nobody can type in is drawn a step down: the box, the prompt
 *  and the icon together. The palette's disabled group is pinned first - Qt fades
 *  a turned-off widget by itself - so the difference that is measured is the
 *  face's own.
 *
 *  The interaction half is not asserted here: the list opens on the writing,
 *  and the writing is driven by the inner line edit, so a case would be reading
 *  QLineEdit's answer rather than this face's.
 */
void tst_FluentAutoSuggestBox::aBoxNobodyCanTypeInIsDrawnFainter()
{
    FluentAutoSuggestBox box;
    giveSuggestions(&box);
    box.resize(160, 32);
    box.show();
    qtcPinDisabledColours(&box);

    const QImage live = box.grab().toImage();
    box.setEnabled(false);
    const QImage faded = box.grab().toImage();
    QVERIFY(faded != live);
}

void tst_FluentAutoSuggestBox::theKeyboardLandsOnIt()
{
    FluentAutoSuggestBox widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentAutoSuggestBox)

#include "tst_fluentautosuggestbox.moc"
