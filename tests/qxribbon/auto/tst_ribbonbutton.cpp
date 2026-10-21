/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxribbon/ribbonbutton.h"

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QHelpEvent>
#include <QImage>
#include <QKeySequence>
#include <QLabel>
#include <QMenu>
#include <QPalette>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <QTest>

QX_RIBBON_USE_NAMESPACE

namespace
{

/*! Puts the application palette back however the test leaves it. */
struct PaletteGuard {
    PaletteGuard()
        : saved(QApplication::palette())
    {
    }
    ~PaletteGuard()
    {
        QApplication::setPalette(saved);
    }
    QPalette saved;
};

/*! A button that has been stood up, which is what a picture of it needs: a
 *  widget that was never shown has nothing to grab. */
RibbonButton *standUp(RibbonButton *button, const QSize &size)
{
    button->resize(size);
    button->show();
    Probe::settle(button);
    return button;
}

/*! A picture of \a button as it stands now. */
QImage capture(RibbonButton *button)
{
    Probe::settle(button);
    return Probe::image(button);
}

/*! The pointer resting on \a pos, in widget coordinates.
 *
 *  A menu button does not believe the widget's own hover bookkeeping: it asks
 *  the global cursor whether it is inside, so a hover has to be a real move of
 *  the cursor and not an Enter event. */
void hover(RibbonButton *button, const QPoint &pos)
{
    QTest::mouseMove(button, pos);
    Probe::settle(button);
}

/*! The pointer somewhere that is not \a button.
 *
 *  The pointer is wherever the last thing that moved it left it, and a button
 *  that is 80 wide near the top left corner of the screen is very often under
 *  it - so a "resting" picture taken without this is a hovered one, and every
 *  comparison against it is a comparison between two hovered pictures. */
void parkThePointer(QWidget *away)
{
    QTest::mouseMove(away, QPoint(10, 10));
    Probe::settle(away);
}

/*! A window to park the pointer in, far enough from where a button stands.
 *  The caller owns it, so it goes away with the case that needed it. */
void standUpParking(QWidget *away)
{
    away->resize(40, 40);
    away->move(600, 600);
    away->show();
    Probe::settle(away);
}

/*! A solid square of one colour, which is all an icon has to be for the
 *  question "was this area painted". */
QPixmap squareIcon(const QColor &color)
{
    QPixmap pixmap(32, 32);
    pixmap.fill(color);
    return pixmap;
}

/*! The tip instance, which is a shared top-level tool window with no parent. */
QWidget *findScreenTip()
{
    const QWidgetList tops = QApplication::topLevelWidgets();
    for (QWidget *widget : tops) {
        if (widget->objectName() == QStringLiteral("RibbonScreenTip")) {
            return widget;
        }
    }
    return nullptr;
}

}   // namespace

class tst_RibbonButton : public QObject
{
    Q_OBJECT
private slots:
    void buttonType();
    void size();
    void screenTip();
    void screenTipFollowsThePalette();

    void aSplitButtonLightsTheHalfThePointerIsOn();
    void aButtonWithAMenuIsLitAsOnePiece();
    void theArrowIsDrawnForAMenuAndNotWithoutOne();
};

void tst_RibbonButton::buttonType()
{
    RibbonButton btn;

    RibbonButton::RibbonButtonType type = btn.buttonType();
    QCOMPARE(type, RibbonButton::SmallButton);
    QCOMPARE(btn.toolButtonStyle(), Qt::ToolButtonTextBesideIcon);

    QCOMPARE(btn.largeButtonType(), RibbonButton::Normal);

    btn.setButtonType(RibbonButton::LargeButton);
    QCOMPARE(btn.buttonType(), RibbonButton::LargeButton);
    QCOMPARE(btn.toolButtonStyle(), Qt::ToolButtonTextUnderIcon);

    QCOMPARE(btn.largeButtonType(), RibbonButton::Normal);

    btn.setLargeButtonType(RibbonButton::Lite);
    QCOMPARE(btn.largeButtonType(), RibbonButton::Lite);
}

void tst_RibbonButton::size()
{
    RibbonButton btn;
    QCOMPARE(btn.sizeHint(), btn.minimumSizeHint());
}

void tst_RibbonButton::screenTip()
{
    RibbonButton btn;
    QAction action(QStringLiteral("&Paste"), &btn);
    action.setStatusTip(QStringLiteral("Insert the clipboard contents"));
    action.setShortcut(QKeySequence(QStringLiteral("Ctrl+V")));
    btn.setDefaultAction(&action);

    QHelpEvent tipEvent(QEvent::ToolTip, QPoint(2, 2), btn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&btn, &tipEvent);

    // ScreenTip (B6): the tip is a shared top-level tool window, found by object name.
    QWidget *tip = nullptr;
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (widget->objectName() == QStringLiteral("RibbonScreenTip")) {
            tip = widget;
        }
    }
    QVERIFY(tip);
    QVERIFY(tip->isVisible());
    // The '&' mnemonic is stripped, and the description / shortcut are shown verbatim.
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipTitle"))->text(), QStringLiteral("Paste"));
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipDescription"))->text(),
             QStringLiteral("Insert the clipboard contents"));
    QCOMPARE(tip->findChild<QLabel *>(QStringLiteral("RibbonScreenTipShortcut"))->text(),
             QKeySequence(QStringLiteral("Ctrl+V")).toString(QKeySequence::NativeText));

    // Leaving the button hides it again.
    QEvent leaveEvent(QEvent::Leave);
    QApplication::sendEvent(&btn, &leaveEvent);
    QVERIFY(!tip->isVisible());

    // An action with neither a description nor a shortcut keeps the plain tooltip:
    // the ScreenTip must stay out of the way, so no tip is shown for it.
    RibbonButton plainBtn;
    QAction plainAction(QStringLiteral("Copy"), &plainBtn);
    plainBtn.setDefaultAction(&plainAction);
    QHelpEvent plainEvent(QEvent::ToolTip, QPoint(2, 2), plainBtn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&plainBtn, &plainEvent);
    QVERIFY(!tip->isVisible());
}

void tst_RibbonButton::screenTipFollowsThePalette()
{
    // The tip is a top-level window that lives for the whole run, so it is not re-polished
    // when the application palette changes. A style sheet naming palette(tooltiptext)
    // therefore froze the colour of whichever theme was up the first time a tip appeared,
    // and a later dark theme showed black text on a dark box. Every show resolves the
    // colours again; this asserts that the second show follows the second palette.
    PaletteGuard guard;

    RibbonButton btn;
    QAction action(QStringLiteral("Paste"), &btn);
    action.setStatusTip(QStringLiteral("Insert the clipboard contents"));
    btn.setDefaultAction(&action);

    QPalette dark = QApplication::palette();
    dark.setColor(QPalette::ToolTipBase, QColor(0x23, 0x28, 0x2c));
    dark.setColor(QPalette::ToolTipText, QColor(0xf0, 0xf0, 0xf0));
    QApplication::setPalette(dark);
    QHelpEvent darkEvent(QEvent::ToolTip, QPoint(2, 2), btn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&btn, &darkEvent);

    QWidget *tip = findScreenTip();
    QVERIFY(tip);
    QVERIFY(tip->styleSheet().contains(QStringLiteral("#f0f0f0")));

    const QPalette light = QApplication::style()->standardPalette();
    QApplication::setPalette(light);
    QHelpEvent lightEvent(QEvent::ToolTip, QPoint(2, 2), btn.mapToGlobal(QPoint(2, 2)));
    QApplication::sendEvent(&btn, &lightEvent);

    QVERIFY(tip->styleSheet().contains(light.color(QPalette::ToolTipText).name()));
    QVERIFY(!tip->styleSheet().contains(QStringLiteral("#f0f0f0")));
}

/*!
 * A button with a split menu is two buttons that share one rectangle: the icon
 * answers on its own and the text with the arrow answers on its own, and the
 * pointer decides which of them is lit.
 *
 * This is the branch a picture is the only witness to. A single-step test can
 * say the menu was set and the mode was set; it cannot say that hovering the
 * icon left the text alone, because that is a claim about a place on the
 * button and not about a value the button holds. So the three cases below are
 * read as rectangles: where the two pictures disagree is which half answered.
 */
void tst_RibbonButton::aSplitButtonLightsTheHalfThePointerIsOn()
{
    RibbonButton button;
    button.setButtonType(RibbonButton::LargeButton);
    QAction action(QStringLiteral("Paste"), &button);
    action.setIcon(QIcon(squareIcon(QColor(Qt::darkBlue))));
    button.setDefaultAction(&action);
    QMenu menu;
    menu.addAction(QStringLiteral("Keep formatting"));
    button.setMenu(&menu);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    standUp(&button, QSize(80, 80));

    QWidget parking;
    standUpParking(&parking);
    parkThePointer(&parking);
    const QImage rest = capture(&button);

    hover(&button, QPoint(button.width() / 2, 4));
    const QImage onIcon = capture(&button);
    hover(&button, QPoint(button.width() / 2, button.height() - 4));
    const QImage onText = capture(&button);

    const QRect upperHalf(0, 0, button.width(), button.height() / 2);
    const QRect lowerHalf(0, button.height() / 2, button.width(), button.height() - button.height() / 2);

    QVERIFY2(!Probe::differingRect(rest, onIcon).isEmpty(),
             "hovering the icon half painted nothing the resting picture did not have");
    QVERIFY2(!Probe::differingRect(rest, onText).isEmpty(),
             "hovering the text half painted nothing the resting picture did not have");

    // And the two hovers are not the same picture, because the highlight is on
    // one part in one and on the other part in the other: both parts are
    // involved in the difference, and neither is left out of it.
    QVERIFY2(Probe::differingPixels(onIcon, onText, upperHalf) > 0,
             "the two halves of a split button look the same whichever of them the pointer is on");
    QVERIFY2(Probe::differingPixels(onIcon, onText, lowerHalf) > 0,
             "the two halves of a split button look the same whichever of them the pointer is on");
}

/*! A menu that is not split is not two halves: the whole button answers, and it
 *  answers the same wherever the pointer is. That is the branch next to the one
 *  above, and the way to tell them apart is that this one has no halves. */
void tst_RibbonButton::aButtonWithAMenuIsLitAsOnePiece()
{
    RibbonButton button;
    button.setButtonType(RibbonButton::LargeButton);
    QAction action(QStringLiteral("Paste"), &button);
    action.setIcon(QIcon(squareIcon(QColor(Qt::darkBlue))));
    button.setDefaultAction(&action);
    QMenu menu;
    menu.addAction(QStringLiteral("Keep formatting"));
    button.setMenu(&menu);
    button.setPopupMode(QToolButton::InstantPopup);
    standUp(&button, QSize(80, 80));

    QWidget parking;
    standUpParking(&parking);
    parkThePointer(&parking);
    const QImage rest = capture(&button);

    hover(&button, QPoint(button.width() / 2, 4));
    const QImage onIcon = capture(&button);
    hover(&button, QPoint(button.width() / 2, button.height() - 4));
    const QImage onText = capture(&button);

    // One piece: the two pointer positions produced the same picture, because
    // there is no second part that could have answered on its own.
    QVERIFY2(Probe::differingRect(onIcon, onText).isEmpty(),
             qPrintable(QStringLiteral("the halves answered separately: %1,%2,%3,%4")
                            .arg(Probe::differingRect(onIcon, onText).left())
                            .arg(Probe::differingRect(onIcon, onText).top())
                            .arg(Probe::differingRect(onIcon, onText).width())
                            .arg(Probe::differingRect(onIcon, onText).height())));
    // And the whole of it is lit, not a strip of it.
    const QRect lit = Probe::differingRect(rest, onText);
    QVERIFY2(lit.height() > button.height() / 2,
             qPrintable(QStringLiteral("only %1 of %2 rows lit").arg(lit.height()).arg(button.height())));
}

/*! The third branch is the one with no menu at all, and what it lacks is a
 *  place on the button: the arrow that says a menu is there is not drawn, and
 *  neither is the room it takes. */
void tst_RibbonButton::theArrowIsDrawnForAMenuAndNotWithoutOne()
{
    RibbonButton plain;
    plain.setButtonType(RibbonButton::LargeButton);
    QAction plainAction(QStringLiteral("Paste"), &plain);
    plainAction.setIcon(QIcon(squareIcon(QColor(Qt::darkBlue))));
    plain.setDefaultAction(&plainAction);
    standUp(&plain, QSize(80, 80));

    RibbonButton withMenu;
    withMenu.setButtonType(RibbonButton::LargeButton);
    QAction menuAction(QStringLiteral("Paste"), &withMenu);
    menuAction.setIcon(QIcon(squareIcon(QColor(Qt::darkBlue))));
    withMenu.setDefaultAction(&menuAction);
    QMenu menu;
    menu.addAction(QStringLiteral("Keep formatting"));
    withMenu.setMenu(&menu);
    withMenu.setPopupMode(QToolButton::InstantPopup);
    standUp(&withMenu, QSize(80, 80));

    QWidget parking;
    standUpParking(&parking);
    parkThePointer(&parking);

    const QImage plainImage = capture(&plain);
    const QImage menuImage = capture(&withMenu);
    QVERIFY2(!Probe::differingRect(plainImage, menuImage).isEmpty(),
             "a button with a menu is drawn exactly like one without");

    // Where the difference lands is what says what the menu added: the arrow
    // hangs below the text in a large button and takes its room from there, so
    // the lower half is where the two pictures come apart the most.
    const QRect upperHalf(0, 0, plain.width(), plain.height() / 2);
    const QRect lowerHalf(0, plain.height() / 2, plain.width(), plain.height() - plain.height() / 2);
    const int above = Probe::differingPixels(plainImage, menuImage, upperHalf);
    const int below = Probe::differingPixels(plainImage, menuImage, lowerHalf);
    QVERIFY2(below > above, qPrintable(QStringLiteral("the menu changed %1 pixels above and %2 below, so what it "
                                                      "added is not under the text")
                                           .arg(above)
                                           .arg(below)));
}

TEST_ADD(tst_RibbonButton)

#include "tst_ribbonbutton.moc"
