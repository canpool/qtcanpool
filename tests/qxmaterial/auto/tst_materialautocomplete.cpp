/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialautocomplete.h"

#include <QtWidgets/QLineEdit>

QX_MATERIAL_USE_NAMESPACE

class tst_MaterialAutocomplete : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theOptionsAreWhatWasGivenIt();
    void theWholeListIsOfferedOnDemand();
    void typingNarrowsTheAnswers();
    void theKeyboardWalksTheAnswers();
    void takingAnAnswerPutsItInForce();
    void escapePutsTheListAwayAndKeepsTheWriting();
    void typingTakesTheAnswerBack();
    void aFreeFieldTakesWritingThatMatchesNothing();
    void aClosedFieldDoesNot();
    void theListIsAsWideAsTheQuestion();
    void aFieldNobodyCanTypeInIsDrawnFainter();
};

namespace
{
/*! A field of five answers, laid out at a size a caller would give it. */
MaterialAutocomplete *makeField(QWidget *page)
{
    auto *field = new MaterialAutocomplete(page);
    field->setLabel(QStringLiteral("Fruit"));
    field->setOptions(QStringList() << QStringLiteral("Apple") << QStringLiteral("Apricot") << QStringLiteral("Banana")
                                    << QStringLiteral("Blueberry") << QStringLiteral("Cherry"));
    field->resize(240, 56);
    field->show();
    QTest::qWaitForWindowExposed(field);
    return field;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(400, 320);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}
}   // namespace

void tst_MaterialAutocomplete::initTestCase()
{
    qtcGiveFonts();
}

/*! The answers are the list it was handed, and reading them back gives the
 *  same list. */
void tst_MaterialAutocomplete::theOptionsAreWhatWasGivenIt()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);

    QCOMPARE(field->options().count(), 5);
    QCOMPARE(field->options().at(1), QStringLiteral("Apricot"));
    QVERIFY(!field->isListOpen());
    QVERIFY(!field->isFreeSolo());
    QVERIFY(field->opensOnFocus());
}

/*! Asking for the answers gives all of them, in a surface that is its own
 *  window standing beside the field. */
void tst_MaterialAutocomplete::theWholeListIsOfferedOnDemand()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);

    field->showAll();
    QVERIFY(field->isListOpen());
    QVERIFY(field->popup());
    QVERIFY(field->popup()->isVisible());
    // The keyboard starts on the first of them, which is what Enter would take.
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apple"));

    field->hideList();
    QVERIFY(!field->isListOpen());
}

/*! Writing narrows the answers to the ones that still fit it, and writing that
 *  fits none of them puts the list away: no answers is an answer. */
void tst_MaterialAutocomplete::typingNarrowsTheAnswers()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->lineEdit()->setFocus();

    QTest::keyClicks(field->lineEdit(), QStringLiteral("ap"));
    QCOMPARE(field->text(), QStringLiteral("ap"));
    QVERIFY(field->isListOpen());
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apple"));
    // Two of the five begin with "Ap"; the others are gone from the list.
    QCOMPARE(field->popup()->height() > 0, true);
    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apricot"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    // Two is the end of what fits "ap": the walk stops there.
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apricot"));

    QTest::keyClicks(field->lineEdit(), QStringLiteral("zz"));
    QVERIFY(!field->isListOpen());
}

/*! The arrows walk the answers, not the text: the cursor does not move down
 *  into a second line of a field that has one line. */
void tst_MaterialAutocomplete::theKeyboardWalksTheAnswers()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->lineEdit()->setFocus();
    field->showAll();

    QCOMPARE(field->highlightedOption(), QStringLiteral("Apple"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apricot"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    QCOMPARE(field->highlightedOption(), QStringLiteral("Banana"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Up);
    QCOMPARE(field->highlightedOption(), QStringLiteral("Apricot"));
    // The walking is not the answering.
    QVERIFY(field->currentText().isEmpty());
}

/*! Taking an answer writes it into the field, closes the list, and says which
 *  answer it was - once. */
void tst_MaterialAutocomplete::takingAnAnswerPutsItInForce()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->lineEdit()->setFocus();
    field->showAll();

    int chosen = 0;
    QString reported;
    connect(field, &MaterialAutocomplete::optionChosen, this, [&](const QString &option) {
        ++chosen;
        reported = option;
    });

    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    QTest::keyClick(field->lineEdit(), Qt::Key_Return);

    QCOMPARE(field->text(), QStringLiteral("Apricot"));
    QCOMPARE(field->currentText(), QStringLiteral("Apricot"));
    QCOMPARE(chosen, 1);
    QCOMPARE(reported, QStringLiteral("Apricot"));
    QVERIFY(!field->isListOpen());
}

/*! Escape puts the list away and leaves the writing where it was: the way out
 *  of a list is not a way out of the field. */
void tst_MaterialAutocomplete::escapePutsTheListAwayAndKeepsTheWriting()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->setOpenOnFocus(false);
    field->lineEdit()->setFocus();
    QTest::keyClicks(field->lineEdit(), QStringLiteral("ban"));
    field->showAll();

    QTest::keyClick(field->lineEdit(), Qt::Key_Escape);
    QVERIFY(!field->isListOpen());
    QCOMPARE(field->text(), QStringLiteral("ban"));
}

/*! Writing again takes the answer back: a value that no longer matches what
 *  the field says is not a value. */
void tst_MaterialAutocomplete::typingTakesTheAnswerBack()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->setOpenOnFocus(false);
    field->lineEdit()->setFocus();
    field->showAll();
    QTest::keyClick(field->lineEdit(), Qt::Key_Down);
    QTest::keyClick(field->lineEdit(), Qt::Key_Return);
    QCOMPARE(field->currentText(), QStringLiteral("Apricot"));

    // One more letter and the answer is gone, while the writing stays.
    QTest::keyClicks(field->lineEdit(), QStringLiteral("x"));
    QVERIFY(field->currentText().isEmpty());
    QCOMPARE(field->text(), QStringLiteral("Apricotx"));
}

/*! A free field answers with writing that matches none of its answers, and the
 *  writing is the answer. */
void tst_MaterialAutocomplete::aFreeFieldTakesWritingThatMatchesNothing()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->setOpenOnFocus(false);
    field->setFreeSolo(true);
    field->lineEdit()->setFocus();

    QTest::keyClicks(field->lineEdit(), QStringLiteral("Zucchini"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Return);
    QCOMPARE(field->currentText(), QStringLiteral("Zucchini"));
}

/*! A closed field does not: it answers only with what is on its list. */
void tst_MaterialAutocomplete::aClosedFieldDoesNot()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->setOpenOnFocus(false);
    field->lineEdit()->setFocus();

    QTest::keyClicks(field->lineEdit(), QStringLiteral("Zucchini"));
    QTest::keyClick(field->lineEdit(), Qt::Key_Return);
    QVERIFY(field->currentText().isEmpty());
}

/*! The list reaches across the question: a list of answers narrower than the
 *  field looks like it is answering something else. */
void tst_MaterialAutocomplete::theListIsAsWideAsTheQuestion()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    field->showAll();

    QVERIFY2(field->popup()->width() >= field->width(),
             qPrintable(QStringLiteral("list %1, field %2").arg(field->popup()->width()).arg(field->width())));
}

/*! A suggest field nobody can type in is written a step down - the fade lives in
 *  the text field it is built on. Both palettes are pinned, so what differs
 *  between the two frames is the family's own hand.
 */
void tst_MaterialAutocomplete::aFieldNobodyCanTypeInIsDrawnFainter()
{
    QWidget *page = makePage();
    MaterialAutocomplete *field = makeField(page);
    qtcPinDisabledColours(field);
    qtcPinDisabledColours(field->lineEdit());

    const QImage live = field->grab().toImage();
    field->setEnabled(false);
    const QImage faded = field->grab().toImage();
    QVERIFY(faded != live);

    delete page;
}

TEST_ADD(tst_MaterialAutocomplete)

#include "tst_materialautocomplete.moc"
