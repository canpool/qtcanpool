/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonquickaccessbar.h"

#include <QAction>
#include <QMenu>
#include <QPainter>
#include <QPixmap>
#include <QToolButton>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonQuickAccessBar : public QObject
{
    Q_OBJECT
private slots:
    void visibleCount();
    void addAction();
    void customizeDialogEntry();
    void menuItemTakesNoMoreRoom();
};

void tst_RibbonQuickAccessBar::visibleCount()
{
    RibbonQuickAccessBar qab;
    QVERIFY(qab.visibleCount() == 0);
}

void tst_RibbonQuickAccessBar::addAction()
{
    RibbonQuickAccessBar qab;

    {
        QAction action(0);

        QCOMPARE(qab.actions().count(), 1);   // actionCustomizeButton
        qab.addAction(&action);
        QCOMPARE(qab.actions().count(), 2);
        QCOMPARE(qab.actions()[0], &action);
        QCOMPARE(qab.visibleCount(), 1);

        qab.clear();
        QCOMPARE(qab.actions().count(), 1);
        QCOMPARE(qab.visibleCount(), 0);
    }
}

void tst_RibbonQuickAccessBar::customizeDialogEntry()
{
    RibbonQuickAccessBar qab;
    QMenu *menu = qab.findChild<QMenu *>();
    QVERIFY(menu);

    QSignalSpy spy(&qab, &RibbonQuickAccessBar::customizeDialogRequested);

    // The menu is rebuilt on every show, and the customization entry is appended last,
    // behind a separator - the entry the application wires to its customize dialog.
    menu->popup(QPoint(0, 0));
    QAction *entry = nullptr;
    for (QAction *action : menu->actions()) {
        if (action->objectName() == QStringLiteral("qx_RibbonCustomizeQabAction")) {
            entry = action;
        }
    }
    QVERIFY(entry);
    QCOMPARE(menu->actions().last(), entry);
    QCOMPARE(entry->text(), QStringLiteral("Customize Quick Access Toolbar..."));

    entry->trigger();
    QCOMPARE(spy.count(), 1);
}

/*!
 * An item whose action has a menu takes no more room than one whose action has
 * none.
 *
 * Two things make that happen and both are needed. The bar opens the menu on the
 * click (InstantPopup) instead of leaving the button something of its own to
 * trigger, which is the condition the platform style asks for before it reserves a
 * twelve-pixel PM_MenuButtonIndicator beside a 16-pixel glyph. And the tables drop
 * the indicator where they describe the buttons themselves, which is what hands
 * the drawing to the style sheet - the rule below is that half, the one the shipped
 * themes carry (skeleton_light.css, skeleton_dark.css).
 *
 * The room is the check: with a triangle the menu item is twelve pixels wider, so
 * comparing the two size hints catches a bar that stopped asking for InstantPopup.
 */
void tst_RibbonQuickAccessBar::menuItemTakesNoMoreRoom()
{
    RibbonQuickAccessBar qab;

    QPixmap glyph(16, 16);
    glyph.fill(Qt::transparent);
    QPainter painter(&glyph);
    painter.setPen(QPen(Qt::black, 1.5));
    painter.drawRect(4, 2, 8, 12);
    painter.end();

    QAction plain(QIcon(glyph), QStringLiteral("plain"), &qab);
    QAction withMenu(QIcon(glyph), QStringLiteral("menu"), &qab);
    QMenu menu;
    menu.addAction(QStringLiteral("entry"));
    withMenu.setMenu(&menu);

    qab.addAction(&plain);
    qab.addAction(&withMenu);
    qab.show();
    QCoreApplication::processEvents();

    // The sheet lands after the bar is up, the way an application installs its theme.
    qApp->setStyleSheet(QStringLiteral("QxRibbon--RibbonQuickAccessBar QToolButton::menu-indicator {"
                                       " image: none; }"));
    QCoreApplication::processEvents();

    auto *plainButton = qobject_cast<QToolButton *>(qab.widgetForAction(&plain));
    auto *menuButton = qobject_cast<QToolButton *>(qab.widgetForAction(&withMenu));
    QVERIFY(plainButton != nullptr);
    QVERIFY(menuButton != nullptr);

    QCOMPARE(menuButton->popupMode(), QToolButton::InstantPopup);
    QCOMPARE(plainButton->popupMode(), QToolButton::DelayedPopup);
    QCOMPARE(menuButton->sizeHint(), plainButton->sizeHint());

    qApp->setStyleSheet(QString());
}

TEST_ADD(tst_RibbonQuickAccessBar)

#include "tst_ribbonquickaccessbar.moc"
