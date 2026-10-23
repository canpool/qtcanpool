/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinoalertdialog.h"

QX_CUPERTINO_USE_NAMESPACE

namespace
{
/*! A dialog with the alerts' own question in it, shown so that its card has a
 *  place to be: a dialog that was never shown has no room and no rectangles. */
void showWith(CupertinoAlertDialog *dialog)
{
    dialog->setTitle(QStringLiteral("Delete this?"));
    dialog->setMessage(QStringLiteral("The item goes and cannot come back."));
    dialog->show();
    QCoreApplication::processEvents();
}

/*! A point on the veil: outside the card, inside the dialog. */
QPoint veilPoint(const CupertinoAlertDialog *dialog)
{
    const QRect card = dialog->cardRect();
    return QPoint(card.left() / 2, qMax(2, card.top() / 2));
}
/*! The room the card keeps inside itself, above and below the words: the test
 *  reads it as the smallest a question can be and still be a card. */
constexpr int kCardPadding = 20;
}   // namespace

class tst_CupertinoAlertDialog : public QObject
{
    Q_OBJECT
private slots:
    void aFourthAnswerIsRefusedAndTheOrderIsKept();
    void twoAnswersShareTheRowAndThreeStack();
    void pressingAnAnswerSaysWhich();
    void escapeLeavesTheAlert();
    void aPressOnTheVeilLeavesTheAlert();
    void anAnswerAddedAfterShowingGrowsTheCard();
};

/*! An alert takes three answers and refuses a fourth, and keeps the order they
 *  were given in - the order is the meaning, so nothing sorts them. */
void tst_CupertinoAlertDialog::aFourthAnswerIsRefusedAndTheOrderIsKept()
{
    CupertinoAlertDialog dialog;
    QVERIFY(dialog.addAction(QStringLiteral("Delete"), CupertinoAlertDialog::ActionRole::Destructive));
    QVERIFY(dialog.addAction(QStringLiteral("Cancel"), CupertinoAlertDialog::ActionRole::Cancel));
    QVERIFY(dialog.addAction(QStringLiteral("Later")));
    QCOMPARE(dialog.actionCount(), 3);
    QCOMPARE(dialog.actionCount(), CupertinoAlertDialog::maximumActionCount());

    // A fourth has nowhere to go: the card would be taller than the question.
    QVERIFY(!dialog.addAction(QStringLiteral("Never")));
    QCOMPARE(dialog.actionCount(), 3);

    QCOMPARE(dialog.actionText(0), QStringLiteral("Delete"));
    QCOMPARE(dialog.actionRole(0), CupertinoAlertDialog::ActionRole::Destructive);
    QCOMPARE(dialog.actionText(1), QStringLiteral("Cancel"));
    QCOMPARE(dialog.actionRole(1), CupertinoAlertDialog::ActionRole::Cancel);
    QCOMPARE(dialog.actionText(2), QStringLiteral("Later"));
    QCOMPARE(dialog.actionRole(2), CupertinoAlertDialog::ActionRole::Default);

    // And emptying it leaves nothing behind: no card, no rectangles.
    dialog.clearActions();
    QCOMPARE(dialog.actionCount(), 0);
    QVERIFY(dialog.actionRect(0).isEmpty());
}

/*! One answer is the card, two are halves of it, and three are three cells
 *  down it - which is the platform's rule and the reason the fourth is
 *  refused. */
void tst_CupertinoAlertDialog::twoAnswersShareTheRowAndThreeStack()
{
    CupertinoAlertDialog two;
    showWith(&two);
    QVERIFY(two.addAction(QStringLiteral("Delete")));
    QVERIFY(two.addAction(QStringLiteral("Cancel")));
    const QRect first = two.actionRect(0);
    const QRect second = two.actionRect(1);
    QVERIFY(!first.isEmpty() && !second.isEmpty());
    // Side by side: the same line, each half of the card, no gap between them.
    QCOMPARE(first.top(), second.top());
    QCOMPARE(first.height(), second.height());
    QCOMPARE(first.right() + 1, second.left());
    QCOMPARE(first.left(), two.cardRect().left());
    QCOMPARE(second.right(), two.cardRect().right());
    // Under the question, on the card's bottom edge.
    QCOMPARE(first.bottom(), two.cardRect().bottom());

    CupertinoAlertDialog three;
    showWith(&three);
    QVERIFY(three.addAction(QStringLiteral("One")));
    QVERIFY(three.addAction(QStringLiteral("Two")));
    QVERIFY(three.addAction(QStringLiteral("Three")));
    const QRect top = three.actionRect(0);
    const QRect middle = three.actionRect(1);
    const QRect bottom = three.actionRect(2);
    // Stacked: each cell starts where the one above it ended.
    QCOMPARE(middle.top(), top.bottom() + 1);
    QCOMPARE(bottom.top(), middle.bottom() + 1);
    QCOMPARE(top.left(), three.cardRect().left());
    QCOMPARE(bottom.bottom(), three.cardRect().bottom());
}

/*! An answer says which one it is and nothing else: whether the alert was
 *  answered, and which way that counts, is the application's. */
void tst_CupertinoAlertDialog::pressingAnAnswerSaysWhich()
{
    CupertinoAlertDialog dialog;
    showWith(&dialog);
    QVERIFY(dialog.addAction(QStringLiteral("Delete")));
    QVERIFY(dialog.addAction(QStringLiteral("Cancel")));

    QSignalSpy chosen(&dialog, &CupertinoAlertDialog::actionChosen);
    QTest::mouseClick(&dialog, Qt::LeftButton, Qt::NoModifier, dialog.actionRect(1).center());
    QCOMPARE(chosen.count(), 1);
    QCOMPARE(chosen.first().first().toInt(), 1);
    // The dialog is still up: closing it is the application's decision, not
    // this control's.
    QVERIFY(dialog.isVisible());

    // A press that began on an answer and ended on the veil has answered
    // nothing, and has not put the alert away either.
    const QPoint answer = dialog.actionRect(0).center();
    const QPoint veil = veilPoint(&dialog);
    QTest::mousePress(&dialog, Qt::LeftButton, Qt::NoModifier, answer);
    QTest::mouseRelease(&dialog, Qt::LeftButton, Qt::NoModifier, veil);
    QCOMPARE(chosen.count(), 1);
    QVERIFY(dialog.isVisible());
}

/*! Escape is the way out of an alert, and it is the alert being put away rather
 *  than answered - the result says so. */
void tst_CupertinoAlertDialog::escapeLeavesTheAlert()
{
    CupertinoAlertDialog dialog;
    showWith(&dialog);
    QVERIFY(dialog.addAction(QStringLiteral("Cancel")));

    QSignalSpy chosen(&dialog, &CupertinoAlertDialog::actionChosen);
    QTest::keyClick(&dialog, Qt::Key_Escape);
    QCOMPARE(chosen.count(), 0);
    QCOMPARE(dialog.result(), int(QDialog::Rejected));
    QVERIFY(!dialog.isVisible());
}

/*! A press on the veil and its release on the veil is the alert being put away
 *  as well: the card is the question, and what is outside it is "not now". */
void tst_CupertinoAlertDialog::aPressOnTheVeilLeavesTheAlert()
{
    CupertinoAlertDialog dialog;
    showWith(&dialog);
    QVERIFY(dialog.addAction(QStringLiteral("Cancel")));

    QSignalSpy chosen(&dialog, &CupertinoAlertDialog::actionChosen);
    const QPoint veil = veilPoint(&dialog);
    QVERIFY(!dialog.cardRect().contains(veil));
    QTest::mouseClick(&dialog, Qt::LeftButton, Qt::NoModifier, veil);
    QCOMPARE(chosen.count(), 0);
    QCOMPARE(dialog.result(), int(QDialog::Rejected));
    QVERIFY(!dialog.isVisible());
}

/*! A dialog with no page under it is the size of its card, and the answers are
 *  part of the card - so adding one after the dialog is up has to make it
 *  taller. Sized once and never again, the card was clamped to the room a
 *  dialog sized before the answers existed had, and the sentence under the
 *  title was the first thing to go. */
void tst_CupertinoAlertDialog::anAnswerAddedAfterShowingGrowsTheCard()
{
    CupertinoAlertDialog dialog;
    showWith(&dialog);
    QCOMPARE(dialog.height(), dialog.sizeHint().height());
    const int before = dialog.cardRect().height();

    QVERIFY(dialog.addAction(QStringLiteral("Cancel"), CupertinoAlertDialog::ActionRole::Cancel));
    QCOMPARE(dialog.height(), dialog.sizeHint().height());
    QVERIFY(dialog.cardRect().height() > before);

    // Everything the card says still fits inside it: a cell for the answer, and
    // the room above and below the words.
    QVERIFY(dialog.cardRect().height() >= dialog.actionRect(0).height() + 2 * kCardPadding);
    QCOMPARE(dialog.actionRect(0).bottom(), dialog.cardRect().bottom());
}

TEST_ADD(tst_CupertinoAlertDialog)

#include "tst_cupertinoalertdialog.moc"
