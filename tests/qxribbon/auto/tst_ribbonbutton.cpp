/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonbutton.h"

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QHelpEvent>
#include <QKeySequence>
#include <QLabel>
#include <QPalette>
#include <QStyle>

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

TEST_ADD(tst_RibbonButton)

#include "tst_ribbonbutton.moc"
