/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxtheme/qxthememotion.h"

#include "qxmaterial/materialdialog.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QKeyEvent>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The card, as the dialog draws it: three hundred and twenty wide with
// twenty-four in from its edge at every side, and a bar across the top of the
// full-screen shape that is fifty-six tall with the cross at its right end.
// These are the measurements the language is drawn by, not numbers asked of
// the dialog - a card that quietly changed them would be a card that no longer
// matches the pictures.
constexpr int kCardWidth = 320;
constexpr int kCardPadding = 24;
constexpr int kBarHeight = 56;
constexpr int kBarInset = 16;
constexpr int kCrossSize = 48;

/*! A page to open the dialog over: a dialog dims or takes the window it was
 *  opened over, so a test of either needs one. */
struct Page {
    QWidget window;

    Page()
    {
        window.resize(800, 600);
        window.show();
    }
};

/*! The dialog as it was painted, at one device pixel per logical pixel.
 *  QWidget::grab() answers device pixels, and every number here is a layout
 *  number. */
QImage capture(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(1.0);
    image.fill(Qt::black);
    widget->render(&image);
    return image;
}

/*! Where the cross of a full-screen dialog is: a touch target at the right end
 *  of the bar, so it is asked for by the layout rather than by a getter. */
QPoint crossCentre(const QWidget *dialog)
{
    return QPoint(dialog->width() - kBarInset - kCrossSize / 2, kBarHeight / 2);
}

/*! Sends the pointer to \a at and says whether what is drawn at \a sample
 *  changed. A hover is a wash under what the pointer is over, so it is a pixel
 *  that was not that colour before - and the move is aimed at whatever child
 *  widget is there, because that is what a pointer hits first; the widget is
 *  expected to ignore it, and an ignored move travels up to the dialog.
 *
 *  The move is sent rather than driven through the platform because Qt 5's
 *  QTest::mouseMove() puts the real cursor somewhere, and the offscreen screen a
 *  test runs on has nowhere to put one. What that leaves untested is the
 *  delivery itself - that is what the answers test asks the widget for. */
bool hoverChanges(QWidget *dialog, const QPoint &at, const QPoint &sample)
{
    const QColor before = capture(dialog).pixelColor(sample);
    QWidget *under = dialog->childAt(at);
    QWidget *target = under ? under : dialog;
    const QPoint local = target->mapFrom(dialog, at);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMouseEvent move(QEvent::MouseMove, QPointF(local), QPointF(), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
#else
    QMouseEvent move(QEvent::MouseMove, local, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
#endif
    QApplication::sendEvent(target, &move);
    return capture(dialog).pixelColor(sample) != before;
}
}   // namespace

class tst_MaterialDialog : public QObject
{
    Q_OBJECT
private slots:

    void theKeyboardLandsOnIt();
    void cleanup();

    void theCardSitsInTheMiddleOfADimmedPage();
    void theFullScreenOneIsThePage();
    void theAnswersRunToTheRightWithTheQuestionLast();
    void anAnswerWithNothingToSayHasNoPlace();
    void theCrossIsTheWayOutOfAFullScreenDialog();
    void theAnswersAskAndTheDialogDoesNotDecide();
    void theAnswersAreWhereThePointerFindsThem();
    void escapeIsTheWayOut();
    void theContentIsWhatTheDialogHolds();
    void whatAnAssistiveToolReads();
    void noMotionArrivesWithoutAFade();
private:
    bool m_wasActive = false;
};

void tst_MaterialDialog::cleanup()
{
    // The accessible registry is global.
    QAccessible::setActive(m_wasActive);
    m_wasActive = false;
}

/*! A basic dialog is a card on a dimmed page, and the scrim is what separates
 *  them: the card is a lifted version of the page's own colour, so without the
 *  veil around it, it would not read as being above anything. */
void tst_MaterialDialog::theCardSitsInTheMiddleOfADimmedPage()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.setMessage(QStringLiteral("The draft cannot be brought back."));
    dialog.setButtonText(MaterialDialog::Role::Primary, QStringLiteral("Discard"));
    dialog.show();

    // The dialog is the page, so the veil lands on the page rather than on a
    // rectangle of its own.
    QCOMPARE(dialog.size(), page.window.size());

    const QImage image = capture(&dialog);
    const QRect card = dialog.cardRect();
    QCOMPARE(card.width(), kCardWidth);
    const QColor corner = image.pixelColor(4, 4);
    // Halfway up the card and four in from its edge: the corner is turned by a
    // radius of twenty-eight, so the top-left corner of the rectangle is not
    // part of the card at all.
    const QColor inside = image.pixelColor(card.left() + 4, card.center().y());
    QVERIFY2(corner.lightness() < inside.lightness(),
             qPrintable(QStringLiteral("the veil is not darker than the card: %1 against %2")
                            .arg(corner.lightness())
                            .arg(inside.lightness())));
}

/*! A full-screen dialog is not a bigger card: it is the page, from the top
 *  down, and there is nothing left beside it for a veil to say anything
 *  about. */
void tst_MaterialDialog::theFullScreenOneIsThePage()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setVariant(MaterialDialog::Variant::FullScreen);
    dialog.setTitle(QStringLiteral("New message"));
    dialog.show();

    QCOMPARE(dialog.cardRect(), dialog.rect());
    QCOMPARE(dialog.size(), page.window.size());

    // No veil anywhere: the bar at the top and the body under it are the same
    // surface as the card of a basic dialog, and nothing is drawn over them.
    const QImage image = capture(&dialog);
    QCOMPARE(image.pixelColor(4, 4), image.pixelColor(4, dialog.height() - 4));
}

/*! M3 reads the row to the right, and the answer to the question is the last
 *  thing in it - the row ends where the card's padding does. */
void tst_MaterialDialog::theAnswersRunToTheRightWithTheQuestionLast()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.setButtonText(MaterialDialog::Role::Primary, QStringLiteral("Discard"));
    dialog.setButtonText(MaterialDialog::Role::Secondary, QStringLiteral("Cancel"));
    dialog.show();

    const QRect cancel = dialog.buttonRect(MaterialDialog::Role::Secondary);
    const QRect discard = dialog.buttonRect(MaterialDialog::Role::Primary);
    QVERIFY(cancel.left() < discard.left());
    QCOMPARE(cancel.top(), discard.top());
    QCOMPARE(cancel.height(), discard.height());

    const QRect card = dialog.cardRect();
    QCOMPARE(discard.right() + 1, card.right() + 1 - kCardPadding);
    QCOMPARE(discard.bottom() + 1, card.bottom() + 1 - kCardPadding);
}

void tst_MaterialDialog::anAnswerWithNothingToSayHasNoPlace()
{
    MaterialDialog dialog;
    QVERIFY(dialog.buttonRect(MaterialDialog::Role::Primary).isEmpty());
    QVERIFY(dialog.buttonRect(MaterialDialog::Role::Secondary).isEmpty());

    dialog.setButtonText(MaterialDialog::Role::Primary, QStringLiteral("Delete"));
    QVERIFY(!dialog.buttonRect(MaterialDialog::Role::Primary).isEmpty());
    QVERIFY(dialog.buttonRect(MaterialDialog::Role::Secondary).isEmpty());
    // The words are the answer: taking them back takes the answer away.
    QCOMPARE(dialog.buttonText(MaterialDialog::Role::Primary), QStringLiteral("Delete"));
    dialog.setButtonText(MaterialDialog::Role::Primary, QString());
    QVERIFY(dialog.buttonRect(MaterialDialog::Role::Primary).isEmpty());
}

/*! The cross is the dialog being put away, not an answer to it - and it is the
 *  only way out that a frameless window does not otherwise have. */
void tst_MaterialDialog::theCrossIsTheWayOutOfAFullScreenDialog()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setVariant(MaterialDialog::Variant::FullScreen);
    dialog.setTitle(QStringLiteral("New message"));
    dialog.show();

    QTest::mouseClick(&dialog, Qt::LeftButton, Qt::NoModifier, crossCentre(&dialog));
    QCOMPARE(dialog.result(), int(QDialog::Rejected));
    QVERIFY(!dialog.isVisible());

    // A basic dialog has no bar and so no cross: the same press asks nothing
    // and answers nothing.
    MaterialDialog card(&page.window);
    card.setTitle(QStringLiteral("Discard draft?"));
    card.show();
    QTest::mouseClick(&card, Qt::LeftButton, Qt::NoModifier, crossCentre(&card));
    QVERIFY(card.isVisible());
}

/*! Pressing an answer asks, it does not answer: whether the question was
 *  answered, and which way that counts, is the application's. */
void tst_MaterialDialog::theAnswersAskAndTheDialogDoesNotDecide()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.setButtonText(MaterialDialog::Role::Primary, QStringLiteral("Discard"));
    QList<int> pressed;
    connect(&dialog, &MaterialDialog::buttonClicked, this, [&pressed](MaterialDialog::Role role) {
        pressed.append(int(role));
    });
    dialog.show();

    // A press that began on the answer and ended somewhere else has not
    // answered anything.
    const QPoint onIt = dialog.buttonRect(MaterialDialog::Role::Primary).center();
    QTest::mousePress(&dialog, Qt::LeftButton, Qt::NoModifier, onIt);
    QTest::mouseRelease(&dialog, Qt::LeftButton, Qt::NoModifier, QPoint(dialog.width() / 2, 4));
    QCOMPARE(pressed.size(), 0);

    QTest::mouseClick(&dialog, Qt::LeftButton, Qt::NoModifier, onIt);
    QCOMPARE(pressed.size(), 1);
    QCOMPARE(pressed.first(), int(MaterialDialog::Role::Primary));
    QVERIFY(dialog.isVisible());
    QCOMPARE(dialog.result(), int(QDialog::Rejected));

    // The application answers, and then it is answered.
    dialog.accept();
    QCOMPARE(dialog.result(), int(QDialog::Accepted));
    QVERIFY(!dialog.isVisible());
}

/*! The answers are painted on the card, and the card is a widget: a pointer
 *  that arrives over an answer hits the card first. It is still a pointer on
 *  the dialog, and the wash under the words is what says so - an answer that
 *  never lights up is one the user has to guess at, and a press on it has to
 *  reach the dialog the same way. */
void tst_MaterialDialog::theAnswersAreWhereThePointerFindsThem()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.setButtonText(MaterialDialog::Role::Primary, QStringLiteral("Discard"));
    dialog.setButtonText(MaterialDialog::Role::Secondary, QStringLiteral("Keep"));
    QList<int> pressed;
    connect(&dialog, &MaterialDialog::buttonClicked, this, [&pressed](MaterialDialog::Role role) {
        pressed.append(int(role));
    });
    dialog.show();

    const QRect keep = dialog.buttonRect(MaterialDialog::Role::Secondary);
    const QPoint onIt = keep.center();

    // Whatever is under the pointer there is what a pointer aims at, and it is
    // a widget that has to ask to be told where the pointer is: a widget
    // without mouse tracking is not sent moves while no button is down, so it
    // would be the only thing a pointer over the answers ever reached.
    QWidget *under = dialog.childAt(onIt);
    QVERIFY(under);
    QVERIFY2(under->hasMouseTracking(), "the widget over the answers is never told where the pointer is");

    // A spot inside the answer and clear of the words drawn on it.
    const QPoint clear(keep.left() + 6, keep.center().y());
    QVERIFY2(hoverChanges(&dialog, onIt, clear), "the pointer over an answer left no wash under it");

    QTest::mouseClick(under, Qt::LeftButton, Qt::NoModifier, under->mapFrom(&dialog, onIt));
    QCOMPARE(pressed.size(), 1);
    QCOMPARE(pressed.first(), int(MaterialDialog::Role::Secondary));
}

void tst_MaterialDialog::escapeIsTheWayOut()
{
    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.show();

    QTest::keyClick(&dialog, Qt::Key_Escape);
    QCOMPARE(dialog.result(), int(QDialog::Rejected));
    QVERIFY(!dialog.isVisible());
}

void tst_MaterialDialog::theContentIsWhatTheDialogHolds()
{
    MaterialDialog dialog;
    dialog.setTitle(QStringLiteral("Discard draft?"));
    const int withoutContent = dialog.sizeHint().height();

    auto *first = new QWidget;
    first->setMinimumHeight(120);
    dialog.setContent(first);
    QCOMPARE(dialog.contentWidget(), first);
    QVERIFY(first->parentWidget());
    QVERIFY(dialog.sizeHint().height() > withoutContent);

    // Handing another one replaces it, and the one that was there is orphaned
    // rather than deleted: the dialog was handed it and never owned it.
    auto *second = new QWidget;
    second->setMinimumHeight(40);
    dialog.setContent(second);
    QCOMPARE(dialog.contentWidget(), second);
    QVERIFY(!first->parentWidget());
    delete first;

    const int withSecond = dialog.sizeHint().height();
    dialog.setContent(second);
    QCOMPARE(dialog.sizeHint().height(), withSecond);
}

void tst_MaterialDialog::whatAnAssistiveToolReads()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    MaterialDialog dialog;
    dialog.setTitle(QStringLiteral("Discard draft?"));
    QCOMPARE(dialog.accessibleName(), QStringLiteral("Discard draft?"));

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&dialog);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Dialog);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! Reduced motion is not a shorter arrival, it is no arrival: the card is there
 *  at full weight in the frame it was opened in.
 */
void tst_MaterialDialog::noMotionArrivesWithoutAFade()
{
    const bool was = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    Page page;
    MaterialDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Discard draft?"));
    dialog.show();

    QCOMPARE(dialog.windowOpacity(), 1.0);

    QxTheme::setMotionReduced(was);
}

void tst_MaterialDialog::theKeyboardLandsOnIt()
{
    MaterialDialog widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_MaterialDialog)

#include "tst_materialdialog.moc"
