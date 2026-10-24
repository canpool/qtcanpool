/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcontentdialog.h"

#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QPushButton>
#include <QSignalSpy>
#include <QTest>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// The card, as the dialog draws it: four hundred wide with twenty-four in from
// its edge at every side. Neither number is asked of the dialog - these are the
// measurements the language is drawn by, and a card that quietly changed them
// would be a card that no longer matches the pictures.
constexpr int kCardWidth = 400;
constexpr int kCardPadding = 24;

/*! A page to open the dialog over. A dialog dims the window it was opened over,
 *  so a test of the veil needs one. */
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
 *  number, so the picture is rendered into an image of the widget's own size
 *  instead. */
QImage capture(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(1.0);
    image.fill(Qt::black);
    widget->render(&image);
    return image;
}

QRect inDialog(QWidget *widget, const QWidget *dialog)
{
    return QRect(widget->mapTo(dialog, QPoint(0, 0)), widget->size());
}
}   // namespace

class tst_FluentContentDialog : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void cleanup();

    void thePageUnderItIsDimmedAndTheCardIsNot();
    void theCardIsWhereTheButtonsAreMeasuredFrom();
    void theButtonsLineUpInTheLanguagesOrder();
    void aButtonExistsOnlyOnceItIsAskedFor();
    void theRowMakesRoomForItself();
    void theDialogDoesNotAnswerForTheApplication();
    void escapeIsTheWayOut();
    void theContentIsWhatTheDialogHolds();
    void whatAnAssistiveToolReads();
    void theEntranceIsSkippedWhenMotionIsReduced();
private:
    bool m_wasActive = false;
};

void tst_FluentContentDialog::cleanup()
{
    // Both are process-wide: the reduce-motion switch and the accessible
    // registry outlive the dialog that read them.
    QxTheme::setMotionReduced(false);
    QAccessible::setActive(m_wasActive);
    m_wasActive = false;
}

/*! A dialog is a card on a dimmed page, and the veil is the whole of what lifts
 *  it: the card is painted in the page's own colour, so a card with no veil
 *  around it would not be a card. */
void tst_FluentContentDialog::thePageUnderItIsDimmedAndTheCardIsNot()
{
    Page page;
    FluentContentDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Delete this file?"));
    dialog.setMessage(QStringLiteral("It cannot be brought back."));
    dialog.primaryButton()->setText(QStringLiteral("Delete"));
    dialog.show();

    // The dialog is the page: a veil over part of a page would say the page is
    // out of reach in part.
    QCOMPARE(dialog.size(), page.window.size());

    const QImage image = capture(&dialog);
    const QRect card = dialog.cardRect();
    const QColor corner = image.pixelColor(4, 4);
    const QColor inside = image.pixelColor(card.left() + 4, card.top() + 4);
    QVERIFY2(corner.lightness() < inside.lightness(),
             qPrintable(QStringLiteral("the veil is not darker than the card: %1 against %2")
                            .arg(corner.lightness())
                            .arg(inside.lightness())));
}

/*! The card is the rectangle a caller measures against, so it is also the one
 *  the dialog has to be true about: it is centred, it is four hundred wide, and
 *  it keeps the reach of its own shadow off the edge of the page. */
void tst_FluentContentDialog::theCardIsWhereTheButtonsAreMeasuredFrom()
{
    Page page;
    FluentContentDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("A question"));
    dialog.show();

    const QRect card = dialog.cardRect();
    QCOMPARE(card.width(), kCardWidth);
    QVERIFY(card.left() > 0);
    QVERIFY(card.right() < dialog.width() - 1);
    // Centred, give or take the odd pixel a rectangle cannot split.
    QVERIFY(qAbs((card.left() + card.right()) / 2 - dialog.width() / 2) <= 1);
    QVERIFY(qAbs((card.top() + card.bottom()) / 2 - dialog.height() / 2) <= 1);
}

/*! Fluent reads the row from the affirmative - what to do, what else could be
 *  done, and the way out of being asked - and the row ends where the card's
 *  padding does. */
void tst_FluentContentDialog::theButtonsLineUpInTheLanguagesOrder()
{
    Page page;
    FluentContentDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Save before closing?"));
    QPushButton *primary = dialog.primaryButton();
    primary->setText(QStringLiteral("Save"));
    QPushButton *secondary = dialog.secondaryButton();
    secondary->setText(QStringLiteral("Do not save"));
    QPushButton *close = dialog.closeButton();
    close->setText(QStringLiteral("Cancel"));
    dialog.show();

    const QRect first = inDialog(primary, &dialog);
    const QRect second = inDialog(secondary, &dialog);
    const QRect last = inDialog(close, &dialog);
    QVERIFY(first.left() < second.left());
    QVERIFY(second.left() < last.left());

    // One row, one height: three ways of answering the same question.
    QCOMPARE(first.top(), last.top());
    QCOMPARE(first.height(), last.height());

    const QRect card = dialog.cardRect();
    QCOMPARE(last.right() + 1, card.right() + 1 - kCardPadding);
    QCOMPARE(last.bottom() + 1, card.bottom() + 1 - kCardPadding);
}

void tst_FluentContentDialog::aButtonExistsOnlyOnceItIsAskedFor()
{
    FluentContentDialog dialog;
    QCOMPARE(dialog.findChildren<QPushButton *>().size(), 0);

    QPushButton *primary = dialog.primaryButton();
    QCOMPARE(dialog.findChildren<QPushButton *>().size(), 1);
    // Asking again is the same button, not a second one.
    QCOMPARE(dialog.primaryButton(), primary);
    // Return reaches the affirmative and nothing else: a stray Return that
    // landed on the way out would answer the question by closing it.
    QVERIFY(primary->isDefault());
    QVERIFY(!dialog.secondaryButton()->isDefault());
    QVERIFY(!dialog.closeButton()->isDefault());
    QVERIFY(!dialog.closeButton()->autoDefault());
}

/*! A bar with no buttons in it is not a row, and the card it sat in would be
 *  taller for nothing. */
void tst_FluentContentDialog::theRowMakesRoomForItself()
{
    FluentContentDialog dialog;
    dialog.setTitle(QStringLiteral("A question"));
    const int withoutButtons = dialog.sizeHint().height();

    dialog.primaryButton()->setText(QStringLiteral("Delete"));
    const int withOne = dialog.sizeHint().height();
    QVERIFY(withOne > withoutButtons);

    // Three answers, one row: the card grows once and stays that size.
    dialog.secondaryButton()->setText(QStringLiteral("Keep both"));
    dialog.closeButton()->setText(QStringLiteral("Cancel"));
    QCOMPARE(dialog.sizeHint().height(), withOne);
}

/*! Pressing a button asks, it does not answer: whether the question was
 *  answered, and which way that counts, is the application's - a dialog sits on
 *  top of the write it is asking about. */
void tst_FluentContentDialog::theDialogDoesNotAnswerForTheApplication()
{
    Page page;
    FluentContentDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Delete this file?"));
    QPushButton *primary = dialog.primaryButton();
    primary->setText(QStringLiteral("Delete"));
    QSignalSpy clicked(primary, &QPushButton::clicked);
    dialog.show();

    QTest::mouseClick(primary, Qt::LeftButton);
    QCOMPARE(clicked.count(), 1);

    QVERIFY(dialog.isVisible());
    QCOMPARE(dialog.result(), int(QDialog::Rejected));

    // The application answers, and then it is answered.
    dialog.accept();
    QCOMPARE(dialog.result(), int(QDialog::Accepted));
    QVERIFY(!dialog.isVisible());
}

void tst_FluentContentDialog::escapeIsTheWayOut()
{
    Page page;
    FluentContentDialog dialog(&page.window);
    dialog.setTitle(QStringLiteral("Delete this file?"));
    dialog.show();

    QTest::keyClick(&dialog, Qt::Key_Escape);
    QCOMPARE(dialog.result(), int(QDialog::Rejected));
    QVERIFY(!dialog.isVisible());
}

void tst_FluentContentDialog::theContentIsWhatTheDialogHolds()
{
    FluentContentDialog dialog;
    dialog.setTitle(QStringLiteral("A question"));
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

    // Handing back the one that is already there changes nothing.
    const int withSecond = dialog.sizeHint().height();
    dialog.setContent(second);
    QCOMPARE(dialog.contentWidget(), second);
    QCOMPARE(dialog.sizeHint().height(), withSecond);
}

void tst_FluentContentDialog::whatAnAssistiveToolReads()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentContentDialog dialog;
    dialog.setTitle(QStringLiteral("Delete this file?"));
    // The title is what the dialog is called: a reader that arrives at the
    // window before it reads anything inside it asks this.
    QCOMPARE(dialog.accessibleName(), QStringLiteral("Delete this file?"));

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&dialog);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Dialog);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! Reduced motion is not a shorter arrival, it is no arrival: the dialog is
 *  simply there. Asking the duration scale is what makes that true - the switch
 *  is read there, not here. */
void tst_FluentContentDialog::theEntranceIsSkippedWhenMotionIsReduced()
{
    Page page;

    FluentContentDialog fading(&page.window);
    fading.setTitle(QStringLiteral("A question"));
    fading.show();
    QVERIFY(fading.windowOpacity() < 1.0);
    fading.hide();

    QxTheme::setMotionReduced(true);
    FluentContentDialog arriving(&page.window);
    arriving.setTitle(QStringLiteral("A question"));
    arriving.show();
    QCOMPARE(arriving.windowOpacity(), 1.0);
}

void tst_FluentContentDialog::theKeyboardLandsOnIt()
{
    FluentContentDialog widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentContentDialog)

#include "tst_fluentcontentdialog.moc"
