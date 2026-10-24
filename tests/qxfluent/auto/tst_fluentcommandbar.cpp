/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluenttokens.h"

#include <QAccessible>
#include <QAction>
#include <QFontMetrics>
#include <QMenu>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE

namespace
{
using QxFluent::FluentCommandBar;

// The same measures the bar lays out with: a command is its name with eight
// either side, two between commands, and a chevron of twenty for the two
// roles that carry a menu. Aiming at a command means knowing where it is.
constexpr int kPadding = 8;
constexpr int kSpacing = 2;
constexpr int kChevronWidth = 20;
constexpr int kBarHeight = 40;

bool hasChevron(FluentCommandBar::ItemRole role)
{
    return role == FluentCommandBar::ItemRole::DropDown || role == FluentCommandBar::ItemRole::Split;
}

int itemWidth(const QString &text, bool chevron)
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    return qMax(kPadding + metrics.horizontalAdvance(text) + kPadding + (chevron ? kChevronWidth : 0), 32);
}

QPoint itemPoint(const QStringList &texts, const QList<FluentCommandBar::ItemRole> &roles, int index,
                 bool chevronPart = false)
{
    int left = kPadding;
    for (int i = 0; i < index; ++i) {
        left += itemWidth(texts.at(i), hasChevron(roles.at(i))) + kSpacing;
    }
    const int width = itemWidth(texts.at(index), hasChevron(roles.at(index)));
    const int x = chevronPart ? left + width - kChevronWidth / 2 : left + 4;
    return QPoint(x, kBarHeight / 2);
}

FluentCommandBar *barWith(const QStringList &texts, const QList<FluentCommandBar::ItemRole> &roles)
{
    FluentCommandBar *bar = new FluentCommandBar;
    for (int i = 0; i < texts.count(); ++i) {
        bar->addCommand(roles.at(i), texts.at(i));
    }
    bar->resize(bar->sizeHint());
    return bar;
}
}   // namespace

class tst_FluentCommandBar : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void cleanup();

    void aClickRunsTheCommand();
    void aToggleStaysInForce();
    void theChevronIsNotTheCommand();
    void whatDoesNotFitIsKept();
    void theKeyboardWalksTheRow();
    void aCommandNobodyCanUseAnswersNothing();
    void theRowIsItsCommands();
    void whatAnAssistiveToolReads();
};

void tst_FluentCommandBar::cleanup()
{
    QAccessible::setActive(false);
}

/*! A command is a QAction with a shape: a click runs it, and the bar has said
 *  nothing about how - the application hears it from the action. */
void tst_FluentCommandBar::aClickRunsTheCommand()
{
    std::unique_ptr<FluentCommandBar> bar(barWith({QStringLiteral("Copy")}, {FluentCommandBar::ItemRole::Button}));
    QAction *copy = bar->commands().first();
    QSignalSpy ran(copy, &QAction::triggered);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier,
                      itemPoint({QStringLiteral("Copy")}, {FluentCommandBar::ItemRole::Button}, 0));

    QCOMPARE(ran.count(), 1);
}

/*! A command that stays in force is a different answer to a click: it is not
 *  run, it is on - and it says so in the action's own language. */
void tst_FluentCommandBar::aToggleStaysInForce()
{
    std::unique_ptr<FluentCommandBar> bar(barWith({QStringLiteral("Bold")}, {FluentCommandBar::ItemRole::Toggle}));
    QAction *bold = bar->commands().first();
    QVERIFY(bold->isCheckable());
    QSignalSpy flipped(bold, &QAction::toggled);

    const QPoint at = itemPoint({QStringLiteral("Bold")}, {FluentCommandBar::ItemRole::Toggle}, 0);
    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, at);
    QVERIFY(bold->isChecked());
    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, at);
    QVERIFY(!bold->isChecked());
    QCOMPARE(flipped.count(), 2);
}

/*! A split command is two things in one: the command runs, and its chevron
 *  opens the menu. A click that lands on the chevron is not a click on the
 *  command. */
void tst_FluentCommandBar::theChevronIsNotTheCommand()
{
    std::unique_ptr<FluentCommandBar> bar(barWith({QStringLiteral("Share")}, {FluentCommandBar::ItemRole::Split}));
    QAction *share = bar->commands().first();
    QVERIFY(share->menu());
    share->menu()->addAction(QStringLiteral("Mail"));
    QSignalSpy ran(share, &QAction::triggered);

    const QStringList texts{QStringLiteral("Share")};
    const QList<FluentCommandBar::ItemRole> roles{FluentCommandBar::ItemRole::Split};
    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(texts, roles, 0));
    QCOMPARE(ran.count(), 1);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(texts, roles, 0, true));
    QCOMPARE(ran.count(), 1);
    QCOMPARE(share->menu()->actions().count(), 1);
}

/*! A row that runs out of room does not drop what it cannot show: it keeps it
 *  behind the overflow, and hands it back when there is room again. */
void tst_FluentCommandBar::whatDoesNotFitIsKept()
{
    FluentCommandBar bar;
    const QStringList texts{QStringLiteral("One"), QStringLiteral("Two"), QStringLiteral("Three"),
                            QStringLiteral("Four")};
    for (const QString &text : texts) {
        bar.addCommand(FluentCommandBar::ItemRole::Button, text);
    }
    bar.resize(bar.sizeHint());
    QCOMPARE(bar.visibleCommands().count(), 4);
    QVERIFY(bar.overflowMenu()->actions().isEmpty());

    bar.resize(bar.sizeHint().width() / 2, bar.sizeHint().height());
    QVERIFY(bar.visibleCommands().count() < 4);
    QVERIFY(!bar.overflowMenu()->actions().isEmpty());
    // Nothing was lost, and nothing was counted twice.
    QCOMPARE(bar.visibleCommands().count() + bar.overflowMenu()->actions().count(), 4);
    QCOMPARE(bar.overflowMenu()->actions().first()->text(), texts.at(bar.visibleCommands().count()));

    bar.resize(bar.sizeHint());
    QCOMPARE(bar.visibleCommands().count(), 4);
    QVERIFY(bar.overflowMenu()->actions().isEmpty());
}

/*! The keyboard walks the row - one command at a time, stopping at both ends -
 *  and what it presses is the command it reached. */
void tst_FluentCommandBar::theKeyboardWalksTheRow()
{
    std::unique_ptr<FluentCommandBar> bar(
        barWith({QStringLiteral("Back"), QStringLiteral("Forward")},
                {FluentCommandBar::ItemRole::Button, FluentCommandBar::ItemRole::Button}));
    QSignalSpy back(bar->commands().at(0), &QAction::triggered);
    QSignalSpy forward(bar->commands().at(1), &QAction::triggered);

    QTest::keyClick(bar.get(), Qt::Key_Right);
    QTest::keyClick(bar.get(), Qt::Key_Enter);
    QCOMPARE(back.count(), 1);
    QCOMPARE(forward.count(), 0);

    QTest::keyClick(bar.get(), Qt::Key_Right);
    QTest::keyClick(bar.get(), Qt::Key_Enter);
    QCOMPARE(forward.count(), 1);
}

/*! A command nobody can use is drawn thinned and answers nothing: it is not
 *  hidden, because what it would do is still part of what the row says. */
void tst_FluentCommandBar::aCommandNobodyCanUseAnswersNothing()
{
    std::unique_ptr<FluentCommandBar> bar(barWith({QStringLiteral("Paste")}, {FluentCommandBar::ItemRole::Button}));
    QAction *paste = bar->commands().first();
    paste->setEnabled(false);
    QSignalSpy ran(paste, &QAction::triggered);

    QTest::mouseClick(bar.get(), Qt::LeftButton, Qt::NoModifier,
                      itemPoint({QStringLiteral("Paste")}, {FluentCommandBar::ItemRole::Button}, 0));

    QCOMPARE(ran.count(), 0);
}

/*! The row is as wide as its commands need and no wider, and it gives the room
 *  back when they go. */
void tst_FluentCommandBar::theRowIsItsCommands()
{
    FluentCommandBar bar;
    const QSize empty = bar.sizeHint();
    bar.addCommand(FluentCommandBar::ItemRole::Button, QStringLiteral("Copy"));
    const QSize one = bar.sizeHint();
    QVERIFY(one.width() > empty.width());

    bar.addCommand(FluentCommandBar::ItemRole::Button, QStringLiteral("Paste"));
    QVERIFY(bar.sizeHint().width() > one.width());
    QCOMPARE(bar.commands().count(), 2);

    bar.clear();
    QVERIFY(bar.commands().isEmpty());
    QVERIFY(bar.sizeHint().width() < one.width());
}

/*! What an assistive tool is told: a bar of commands, not a widget that
 *  happens to be a row. */
void tst_FluentCommandBar::whatAnAssistiveToolReads()
{
    FluentCommandBar bar;
    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&bar);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ToolBar);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

void tst_FluentCommandBar::theKeyboardLandsOnIt()
{
    FluentCommandBar widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentCommandBar)

#include "tst_fluentcommandbar.moc"
