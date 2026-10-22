/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentnavigationview.h"

#include <QAccessible>
#include <QAction>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
using QxFluent::FluentNavigationView;

// The measures the pane lays out with: forty for the menu button, forty per
// destination, sixteen in from the leading edge. Aiming at a destination means
// knowing where it stands.
constexpr int kMenuButtonHeight = 40;
constexpr int kItemHeight = 40;
constexpr int kItemPadding = 16;

QPoint menuButtonPoint()
{
    return QPoint(kItemPadding, kMenuButtonHeight / 2);
}

QPoint itemPoint(int index)
{
    return QPoint(kItemPadding, kMenuButtonHeight + index * kItemHeight + kItemHeight / 2);
}

FluentNavigationView *viewWith(const QStringList &texts)
{
    FluentNavigationView *view = new FluentNavigationView;
    for (const QString &text : texts) {
        view->addItem(text);
    }
    view->resize(view->sizeHint());
    return view;
}
}   // namespace

class tst_FluentNavigationView : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void thePaneHoldsWhatItIsGiven();
    void aClickOnADestinationPutsItInForce();
    void theMenuButtonFoldsThePane();
    void theContentIsKeptBesideThePane();
    void theContentFollowsTheRoomItIsGiven();
    void aDestinationNobodyCanUseIsNotPutInForce();
    void theKeyboardWalksTheColumn();
    void whatAnAssistiveToolReads();
};

void tst_FluentNavigationView::cleanup()
{
    QAccessible::setActive(false);
}

/*! A destination is a QAction, so what the pane holds is whatever the
 *  application already knows how to use - and what it says of the one in force
 *  is said in the same language. */
void tst_FluentNavigationView::thePaneHoldsWhatItIsGiven()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    QCOMPARE(view->items().count(), 2);
    QCOMPARE(view->currentIndex(), -1);
    QVERIFY(!view->currentItem());

    view->removeItem(view->items().first());
    QCOMPARE(view->items().count(), 1);

    view->clear();
    QCOMPARE(view->items().count(), 0);
    QVERIFY(!view->currentItem());
}

/*! A click does two things and they are not the same thing: the destination is
 *  put in force, which the view says, and it is run, which the action says. */
void tst_FluentNavigationView::aClickOnADestinationPutsItInForce()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    QAction *mail = view->items().at(1);
    QSignalSpy changed(view.get(), &FluentNavigationView::currentIndexChanged);
    QSignalSpy ran(mail, &QAction::triggered);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));

    QCOMPARE(view->currentIndex(), 1);
    QCOMPARE(view->currentItem(), mail);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(ran.count(), 1);
}

/*! Folding is a state the view holds rather than a size the application sets,
 *  and the button that folds it is the pane's own. */
void tst_FluentNavigationView::theMenuButtonFoldsThePane()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home")}));
    QVERIFY(view->isPaneOpen());
    QSignalSpy folded(view.get(), &FluentNavigationView::paneOpenChanged);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, menuButtonPoint());
    QVERIFY(!view->isPaneOpen());
    QCOMPARE(folded.count(), 1);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, menuButtonPoint());
    QVERIFY(view->isPaneOpen());
    QCOMPARE(folded.count(), 2);
}

/*! The room beside the pane is the content's, and the line between them moves
 *  when the pane folds - which is the only place the fold can be read from
 *  outside, since the pane's own width is not something the view publishes. */
void tst_FluentNavigationView::theContentIsKeptBesideThePane()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home")}));
    QWidget *content = new QWidget;
    view->setContent(content);

    QCOMPARE(view->contentWidget(), content);
    QCOMPARE(content->parent(), view.get());
    const int openLeft = content->geometry().left();
    QVERIFY(openLeft > 0);

    view->setPaneOpen(false);
    const int foldedLeft = content->geometry().left();
    QVERIFY(foldedLeft > 0);
    QVERIFY(foldedLeft < openLeft);
    QCOMPARE(content->height(), view->height());
}

/*! An action that is disabled says so, and a pane that answered a click on it
 *  would be putting a destination in force that cannot be reached. */
/*! The room beside the pane is the room the view has, and it is worked out
 *  when it is asked for rather than when the room changed: a view that has
 *  never been shown is never told it was resized, so a layout made before that
 *  would go on giving the content the room the view used to have. */
void tst_FluentNavigationView::theContentFollowsTheRoomItIsGiven()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home")}));
    QWidget *content = new QWidget;
    view->setContent(content);
    QVERIFY(content->height() != 300);

    view->resize(600, 300);
    // What makes the view work out where things stand again is being asked
    // where they are: a press asks.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(0));

    QCOMPARE(content->height(), view->height());
    QCOMPARE(content->geometry().right() + 1, view->width());
}

void tst_FluentNavigationView::aDestinationNobodyCanUseIsNotPutInForce()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    view->items().at(1)->setEnabled(false);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QCOMPARE(view->currentIndex(), -1);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(0));
    QCOMPARE(view->currentIndex(), 0);
}

/*! The column is walked with the keyboard: the arrows say which destination is
 *  meant, and Enter is what says go. */
void tst_FluentNavigationView::theKeyboardWalksTheColumn()
{
    std::unique_ptr<FluentNavigationView> view(
        viewWith({QStringLiteral("Home"), QStringLiteral("Mail"), QStringLiteral("Notes")}));

    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 1);

    QTest::keyClick(view.get(), Qt::Key_Up);
    QTest::keyClick(view.get(), Qt::Key_Space);
    QCOMPARE(view->currentIndex(), 0);

    // The top is a wall: walking up from the first destination stays there.
    QTest::keyClick(view.get(), Qt::Key_Up);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 0);
}

void tst_FluentNavigationView::whatAnAssistiveToolReads()
{
    std::unique_ptr<FluentNavigationView> view(viewWith({QStringLiteral("Home")}));
    QAccessible::setActive(true);
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(view.get());
    QVERIFY(face);
    // A column of destinations is a list, and the room beside it is not what an
    // assistive tool is meant to walk.
    QCOMPARE(face->role(), QAccessible::List);
}

TEST_ADD(tst_FluentNavigationView)

#include "tst_fluentnavigationview.moc"
