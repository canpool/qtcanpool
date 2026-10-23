/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentnavigationview.h"

#include <QAccessible>
#include <QAction>
#include <QApplication>
#include <QIcon>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
using QxFluent::FluentNavigationView;

// The measures the pane lays out with: forty for the menu button, forty per
// destination, thirty-six for a heading over a group of them, sixteen in from
// the leading edge. Aiming at a destination means knowing where it stands.
constexpr int kMenuButtonHeight = 40;
constexpr int kItemHeight = 40;
constexpr int kHeaderHeight = 36;
constexpr int kItemPadding = 16;

QPoint menuButtonPoint()
{
    return QPoint(kItemPadding, kMenuButtonHeight / 2);
}

QPoint itemPoint(int index)
{
    return QPoint(kItemPadding, kMenuButtonHeight + index * kItemHeight + kItemHeight / 2);
}

/*! The middle of the \a index-th destination under one heading, which is a
 *  heading's height further down than the same destination in a flat column. */
QPoint itemPointBelowHeader(int index)
{
    return QPoint(kItemPadding, kMenuButtonHeight + kHeaderHeight + index * kItemHeight + kItemHeight / 2);
}

QPoint headerPoint()
{
    return QPoint(kItemPadding, kMenuButtonHeight + kHeaderHeight / 2);
}

// The measures of the panel a folded pane opens beside an icon: six of padding
// and the pane's own row height.
constexpr int kFlyoutPadding = 6;
constexpr int kFlyoutItemHeight = 40;

/*! The panel a folded pane has opened, if it has one: a popup belonging to the
 *  view that nothing else in these tests makes. */
QWidget *popupOf(QWidget *view)
{
    const QList<QWidget *> children = view->findChildren<QWidget *>();
    for (QWidget *child : children) {
        if (child->windowFlags().testFlag(Qt::Popup) && child->isVisible()) {
            return child;
        }
    }
    return nullptr;
}

/*! The middle of the \a index-th row of a panel. */
QPoint flyoutPoint(int index)
{
    return QPoint(kItemPadding, kFlyoutPadding + index * kFlyoutItemHeight + kFlyoutItemHeight / 2);
}

/*! Home, Mail with two destinations under it, and Notes - the shape a pane of
 *  three things that each hold six is made of. The destinations stand at the
 *  indexes they would have in a flat column, which is what folding a group
 *  does not change. */
struct Hierarchy {
    FluentNavigationView *view = nullptr;
    QAction *home = nullptr;
    QAction *mail = nullptr;
    QAction *inbox = nullptr;
    QAction *sent = nullptr;
    QAction *notes = nullptr;

    Hierarchy()
    {
        view = new FluentNavigationView;
        home = view->addItem(QStringLiteral("Home"));
        mail = view->addItem(QStringLiteral("Mail"));
        inbox = view->addItem(QStringLiteral("Inbox"), QIcon(), mail);
        sent = view->addItem(QStringLiteral("Sent"), QIcon(), mail);
        notes = view->addItem(QStringLiteral("Notes"));
        view->resize(view->sizeHint());
    }
};

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
    void initTestCase();
    void cleanup();

    void thePaneHoldsWhatItIsGiven();
    void aClickOnADestinationPutsItInForce();
    void theMenuButtonFoldsThePane();
    void theContentIsKeptBesideThePane();
    void theContentFollowsTheRoomItIsGiven();
    void aDestinationNobodyCanUseIsNotPutInForce();
    void theKeyboardWalksTheColumn();
    void aHeadingIsRoomOnThePaneAndNotADestination();
    void theKeyboardIsNotStoppedByAHeading();
    void aFoldedPaneKeepsTheHeadingInPlace();
    void aDestinationUnderAnotherStandsUnderIt();
    void closingAGroupHidesWhatsUnderIt();
    void pressingAParentTurnsItOverAndGoesThere();
    void theKeyboardWalksWhatIsShowing();
    void theKeyboardOpensAndClosesAGroup();
    void namingAFoldedDestinationOpensTheGroupsAboveIt();
    void aFoldedPaneOpensAGroupBesideItself();
    void pressingOutsideThePanelPutsItAway();
    void whatAnAssistiveToolReads();
};

void tst_FluentNavigationView::initTestCase()
{
    // A signal that carries a destination carries a pointer, and Qt 5 will not
    // put one in a QVariant until it is told the name of the type: a spy over
    // expandedChanged() would read null for every destination it saw. Qt 6
    // knows the type already, and asking twice costs nothing.
    qRegisterMetaType<QAction *>("QAction*");
}

void tst_FluentNavigationView::cleanup()
{
    QAccessible::setActive(false);
    // A panel a folded pane opened outlives the case that opened it, and a
    // popup left up would swallow the presses of the next one.
    if (QWidget *panel = QApplication::activePopupWidget()) {
        panel->close();
    }
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

/*! A heading is room on the pane and not a place in the column: it says what
 *  the destinations under it are for, and it cannot be landed on. */
void tst_FluentNavigationView::aHeadingIsRoomOnThePaneAndNotADestination()
{
    std::unique_ptr<FluentNavigationView> view(new FluentNavigationView);
    view->addHeader(QStringLiteral("All controls"));
    QAction *home = view->addItem(QStringLiteral("Home"));
    QAction *mail = view->addItem(QStringLiteral("Mail"));
    view->resize(view->sizeHint());

    // What the view holds is the two destinations. The heading is neither of
    // them: it is not counted, so the count is what it was without it.
    QCOMPARE(view->items().count(), 2);
    view->setCurrentIndex(0);
    QCOMPARE(view->currentItem(), home);

    // A heading takes room, so the first destination stands a heading's height
    // further down than it would in a flat column - and the second one is
    // asked for by its own place, not by a place that counts the heading.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPointBelowHeader(1));
    QCOMPARE(view->currentIndex(), 1);
    QCOMPARE(view->currentItem(), mail);

    // Pressing the heading is pressing the pane: nothing is put in force.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, headerPoint());
    QCOMPARE(view->currentIndex(), 1);
}

/*! The column walked by the keyboard is the same column the pointer walks: the
 *  headings are passed over rather than landed on, which is what keeps the
 *  last press of Down on the last destination instead of one past it. */
void tst_FluentNavigationView::theKeyboardIsNotStoppedByAHeading()
{
    std::unique_ptr<FluentNavigationView> view(new FluentNavigationView);
    view->addHeader(QStringLiteral("All controls"));
    view->addItem(QStringLiteral("Home"));
    view->addItem(QStringLiteral("Mail"));
    view->addItem(QStringLiteral("Notes"));
    view->resize(view->sizeHint());

    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 1);

    // Down is pressed until there is nothing further down. If a heading were a
    // step of its own, one of these presses would have gone to it and the last
    // one would be past the end - where Enter does nothing at all.
    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 2);

    // And up from there is the second destination, not the heading above it.
    QTest::keyClick(view.get(), Qt::Key_Up);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 1);
}

/*! Folded, a heading has no room for words and is a rule instead - and it is
 *  the same height either way, so the destinations under it do not move when
 *  the pane folds. */
void tst_FluentNavigationView::aFoldedPaneKeepsTheHeadingInPlace()
{
    std::unique_ptr<FluentNavigationView> view(new FluentNavigationView);
    view->addHeader(QStringLiteral("All controls"));
    view->addItem(QStringLiteral("Home"));
    view->addItem(QStringLiteral("Mail"));
    view->resize(view->sizeHint());

    view->setPaneOpen(false);
    QCOMPARE(view->isPaneOpen(), false);

    // The press that reached the second destination reaches it folded too:
    // that is the same place on the pane, whatever the pane is drawn as.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPointBelowHeader(1));
    QCOMPARE(view->currentIndex(), 1);

    // And the heading is still drawn, as a rule across the middle of its row.
    const int width = view->sizeHint().width();
    const int height = kMenuButtonHeight + kHeaderHeight + 2 * kItemHeight;
    QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    view->render(&image);
    // The pane's own colour is asked of it where nothing is drawn - above the
    // heading, at the foot of the menu button.
    const QColor paneColor = image.pixelColor(2, kMenuButtonHeight - 2);
    int ink = 0;
    for (int x = kItemPadding; x < kItemPadding + 16; ++x) {
        if (image.pixelColor(x, kMenuButtonHeight + kHeaderHeight / 2) != paneColor) {
            ++ink;
        }
    }
    QVERIFY2(ink >= 8, qPrintable(QStringLiteral("%1 of 16 pixels are the rule").arg(ink)));
}

/*! A destination under another one stands under it: after its parent in the
 *  order the pane draws, at the index it would have in a flat column, and
 *  reachable by the pointer like any other. */
void tst_FluentNavigationView::aDestinationUnderAnotherStandsUnderIt()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);

    QCOMPARE(view->items().count(), 5);
    QCOMPARE(view->parentItem(h.inbox), h.mail);
    QCOMPARE(view->parentItem(h.sent), h.mail);
    QVERIFY(!view->parentItem(h.mail));

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(2));
    QCOMPARE(view->currentIndex(), 2);
    QCOMPARE(view->currentItem(), h.inbox);
}

/*! Folding a group takes it off the pane without taking it out of the view: the
 *  destinations keep the indexes they had, and what followed the group moves
 *  up into the room it gave back. */
void tst_FluentNavigationView::closingAGroupHidesWhatsUnderIt()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->setExpanded(h.mail, false);
    QVERIFY(!view->isExpanded(h.mail));

    // The room under the last row is the pane, and a press there puts nothing
    // in force.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, QPoint(kItemPadding, 180));
    QCOMPARE(view->currentIndex(), -1);

    // They are still destinations, at the indexes they always had: an index is
    // a name, and one that moved when a group closed would be a different
    // destination.
    QCOMPARE(view->items().count(), 5);
    QCOMPARE(view->items().at(2), h.inbox);

    // What followed the group moved up into the room it gave back - the third
    // row now, the fifth destination still.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(2));
    QCOMPARE(view->currentItem(), h.notes);
    QCOMPARE(view->currentIndex(), 4);
}

/*! A parent is a destination too: pressing it goes there, and turns it over in
 *  the same press - which is WinUI's SelectsOnInvoked, and what a group that
 *  was only a group would say by not connecting to its action. */
void tst_FluentNavigationView::pressingAParentTurnsItOverAndGoesThere()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    QSignalSpy ran(h.mail, &QAction::triggered);
    QSignalSpy turned(view.get(), &FluentNavigationView::expandedChanged);
    QVERIFY(view->isExpanded(h.mail));

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QCOMPARE(view->currentIndex(), 1);
    QCOMPARE(view->currentItem(), h.mail);
    QCOMPARE(ran.count(), 1);
    QVERIFY(!view->isExpanded(h.mail));
    QCOMPARE(turned.count(), 1);
    QCOMPARE(turned.at(0).at(0).value<QAction *>(), h.mail);
    QCOMPARE(turned.at(0).at(1).toBool(), false);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QVERIFY(view->isExpanded(h.mail));
    QCOMPARE(ran.count(), 2);
}

/*! The arrows walk what is on the pane. A group that is closed is one step over,
 *  not four: the destinations folded away are not places the eye can go. */
void tst_FluentNavigationView::theKeyboardWalksWhatIsShowing()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->setExpanded(h.mail, false);

    // The first Down is the parent itself; it is only walked over here,
    // because pressing it would turn it over rather than go anywhere new.
    QTest::keyClick(view.get(), Qt::Key_Down);
    // The second Down is the destination after the closed group, not the first
    // of the destinations folded away under it.
    QTest::keyClick(view.get(), Qt::Key_Down);
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentIndex(), 4);
    QCOMPARE(view->currentItem(), h.notes);
}

/*! Right opens a group and then walks into it; Left walks out of one and then
 *  closes it. The same press means "further in" or "further out" whether the
 *  way is an arrow or a step. */
void tst_FluentNavigationView::theKeyboardOpensAndClosesAGroup()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->setExpanded(h.mail, false);

    QTest::keyClick(view.get(), Qt::Key_Down);    // onto the parent
    QTest::keyClick(view.get(), Qt::Key_Right);   // and open it
    QVERIFY(view->isExpanded(h.mail));

    QTest::keyClick(view.get(), Qt::Key_Right);   // the second Right walks in
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentItem(), h.inbox);

    QTest::keyClick(view.get(), Qt::Key_Left);   // out to the parent
    QTest::keyClick(view.get(), Qt::Key_Enter);
    QCOMPARE(view->currentItem(), h.mail);

    QTest::keyClick(view.get(), Qt::Key_Left);   // and closed
    QVERIFY(!view->isExpanded(h.mail));
}

/*! Naming a destination that is folded away opens the groups above it rather
 *  than leaving the destination out of sight while currentIndex() says it is in
 *  force. */
void tst_FluentNavigationView::namingAFoldedDestinationOpensTheGroupsAboveIt()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->setExpanded(h.mail, false);

    view->setCurrentIndex(3);
    QVERIFY(view->isExpanded(h.mail));
    QCOMPARE(view->currentItem(), h.sent);
}

/*! Folded, what stands under an icon has no room beside it and steps out into a
 *  panel instead: the fold is not allowed to lose a destination, and a parent
 *  with nowhere to open is a way in rather than a journey. */
void tst_FluentNavigationView::aFoldedPaneOpensAGroupBesideItself()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->setPaneOpen(false);

    // What stood under the icon is not on the pane any more: the destination
    // that followed the group is the third row now.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(2));
    QCOMPARE(view->currentItem(), h.notes);
    QCOMPARE(view->currentIndex(), 4);

    // Pressing the icon opens the group beside it, and does not go anywhere.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QCOMPARE(view->currentIndex(), 4);
    QWidget *panel = popupOf(view.get());
    QVERIFY2(panel, "a folded pane opens a panel beside the icon it belongs to");
    QCOMPARE(panel->width(), 240);

    // The second row of the panel is the second destination under the icon.
    QTest::mouseClick(panel, Qt::LeftButton, Qt::NoModifier, flyoutPoint(1));
    QCOMPARE(view->currentItem(), h.sent);
    QCOMPARE(view->currentIndex(), 3);
}

void tst_FluentNavigationView::pressingOutsideThePanelPutsItAway()
{
    Hierarchy h;
    std::unique_ptr<FluentNavigationView> view(h.view);
    view->show();
    QVERIFY(QTest::qWaitForWindowExposed(view.get()));
    view->setPaneOpen(false);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QWidget *panel = popupOf(view.get());
    QVERIFY2(panel, "a folded pane opens a panel beside the icon it belongs to");

    // The panel is a way in, not a mode: a press anywhere else puts it away,
    // and the press itself still counts as the press it was.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(0));
    QVERIFY2(!popupOf(view.get()), "a press outside the panel puts it away");
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
