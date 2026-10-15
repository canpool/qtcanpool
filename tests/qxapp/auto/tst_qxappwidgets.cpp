/*!
 * \file
 *
 * The qxapp widgets that had no test at all before the 4.4 B group audit:
 * QxTabBar, QxTabWidget, QxNavTabBar, QxToolButton and the three small buttons
 * QxMenuButton, QxMenuAccessButton and QxExtensionButton.
 *
 * They are the reason the audit was worth running. Every one of them is an
 * exported class with a documented surface, and none of them is referenced by
 * anything else in the repository - not by the other qxapp sources, not by the
 * demos. That is what a widget a library offers its users looks like from the
 * inside of that library, and it is also what an abandoned class looks like, so
 * the two cannot be told apart by counting callers. What tells them apart is
 * whether the behaviour is worth anything to somebody outside: a tab bar that
 * keeps its current tab across an insertion is a contract, and a class with no
 * contract is not.
 *
 * Everything here runs offscreen with no window shown. Only what can be
 * observed without a screen is asserted; paint() output is not.
 */
#include "tst_global.h"

#include "qxapp/qxextensionbutton.h"
#include "qxapp/qxmenuaccessbutton.h"
#include "qxapp/qxmenubutton.h"
#include "qxapp/qxnavtabbar.h"
#include "qxapp/qxtabbar.h"
#include "qxapp/qxtabwidget.h"
#include "qxapp/qxtoolbutton.h"

#include <QtCore/QPointer>
#include <QtTest/QSignalSpy>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QX_APP_USE_NAMESPACE

class tst_QxAppWidgets : public QObject
{
    Q_OBJECT
private slots:
    void tabBarHoldsTabs();
    void tabBarSignalsFollowTheSelection();
    void tabBarInsertAndRemoveKeepTheCurrentTab();
    void tabBarIsSafeOutOfRange();
    void tabWidgetPagesAndCurrentPage();
    void tabWidgetPositionsMoveTheTabBar();
    void tabWidgetButtons();
    void navBarCountsTheVisibleTabs();
    void toolButtonMenuAreaAndArrowRoundTrip();
    void smallButtonsSizeHintAndOrientation();
};

/*!
 * The tab bar is a QToolBar whose entries are checkable actions, and the index
 * it hands back is what a caller stores to reach the tab again. The first tab
 * becomes current on its own - a bar with tabs and no selection would leave the
 * caller with nothing to show.
 */
void tst_QxAppWidgets::tabBarHoldsTabs()
{
    QxTabBar bar;
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.currentIndex(), -1);

    QCOMPARE(bar.addTab(QStringLiteral("one")), 0);
    QCOMPARE(bar.count(), 1);
    QCOMPARE(bar.currentIndex(), 0);
    QCOMPARE(bar.isTabChecked(0), true);

    QCOMPARE(bar.addTab(QIcon(), QStringLiteral("two")), 1);
    QCOMPARE(bar.count(), 2);
    // Adding a tab does not move the selection.
    QCOMPARE(bar.currentIndex(), 0);

    QCOMPARE(bar.isTogglable(), true);

    bar.setTabToolTip(0, QStringLiteral("tip"));
    QCOMPARE(bar.tabToolTip(0), QStringLiteral("tip"));

    bar.setTabEnabled(1, false);
    QCOMPARE(bar.isTabEnabled(1), false);
    QCOMPARE(bar.isTabEnabled(0), true);

    bar.setTabVisible(1, false);
    QCOMPARE(bar.isTabVisible(1), false);
    QCOMPARE(bar.isTabVisible(0), true);
}

/*!
 * currentChanged() says which tab is now current; currentToggled() says a tab
 * was toggled and with what state. Turning togglable off drops the second one
 * - the tabs become radio buttons rather than checkable ones - without touching
 * the first.
 */
void tst_QxAppWidgets::tabBarSignalsFollowTheSelection()
{
    QxTabBar bar;
    QSignalSpy changed(&bar, &QxTabBar::currentChanged);
    QSignalSpy toggled(&bar, &QxTabBar::currentToggled);

    bar.addTab(QStringLiteral("one"));
    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.first().at(0).toInt(), 0);
    QCOMPARE(toggled.count(), 1);
    QCOMPARE(toggled.first().at(0).toInt(), 0);
    QCOMPARE(toggled.first().at(1).toBool(), true);

    changed.clear();
    toggled.clear();
    bar.addTab(QStringLiteral("two"));
    bar.setCurrentIndex(1);

    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.first().at(0).toInt(), 1);
    QCOMPARE(toggled.count(), 1);

    // Selecting the tab that is already current changes nothing.
    changed.clear();
    toggled.clear();
    bar.setCurrentIndex(1);
    QCOMPARE(changed.count(), 0);
    QCOMPARE(toggled.count(), 0);

    // A disabled tab cannot be selected.
    bar.setTabEnabled(0, false);
    bar.setCurrentIndex(0);
    QCOMPARE(bar.currentIndex(), 1);
    QCOMPARE(changed.count(), 0);

    // With togglable off the checkable half is gone but the selection remains.
    bar.setTogglable(false);
    QCOMPARE(bar.isTogglable(), false);
    changed.clear();
    toggled.clear();
    bar.setCurrentIndex(0);
    QCOMPARE(changed.count(), 0);   // still disabled, still refused
    bar.setTabEnabled(0, true);
    bar.setCurrentIndex(0);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(toggled.count(), 0);
}

/*!
 * The current index is an index, so it has to keep pointing at the same tab
 * when the list moves underneath it. That is the whole reason insertion and
 * removal are not just container operations here.
 */
void tst_QxAppWidgets::tabBarInsertAndRemoveKeepTheCurrentTab()
{
    QxTabBar bar;
    bar.addTab(QStringLiteral("a"));
    bar.addTab(QStringLiteral("b"));
    bar.addTab(QStringLiteral("c"));
    bar.setCurrentIndex(2);
    QCOMPARE(bar.currentIndex(), 2);

    // Inserting before the current tab pushes it along.
    bar.insertTab(0, QStringLiteral("z"));
    QCOMPARE(bar.count(), 4);
    QCOMPARE(bar.currentIndex(), 3);

    // Removing a tab before it pulls it back.
    bar.removeTab(0);
    QCOMPARE(bar.count(), 3);
    QCOMPARE(bar.currentIndex(), 2);

    // Removing the current tab falls back to the one that took its place; for
    // the last tab that is the one before it.
    bar.removeTab(2);
    QCOMPARE(bar.count(), 2);
    QCOMPARE(bar.currentIndex(), 1);

    // Removing everything leaves no selection rather than a stale index.
    bar.removeTab(1);
    bar.removeTab(0);
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.currentIndex(), -1);
}

/*!
 * Every accessor takes an index and every one of them is reachable with an
 * index the caller made up, so out of range has to be answered rather than
 * acted on.
 */
void tst_QxAppWidgets::tabBarIsSafeOutOfRange()
{
    QxTabBar bar;
    bar.addTab(QStringLiteral("a"));

    QCOMPARE(bar.isTabEnabled(9), false);
    QCOMPARE(bar.isTabVisible(9), false);
    QCOMPARE(bar.isTabChecked(9), false);
    QCOMPARE(bar.tabToolTip(9), QString());
    QCOMPARE(bar.isTabEnabled(-1), false);

    // The mutators are no-ops, and the bar is unchanged afterwards.
    bar.setTabEnabled(9, false);
    bar.setTabVisible(9, false);
    bar.setTabToolTip(9, QStringLiteral("nowhere"));
    bar.removeTab(9);
    bar.removeTab(-1);
    QCOMPARE(bar.count(), 1);
    QCOMPARE(bar.currentIndex(), 0);

    // setCurrentIndex() on a nonexistent tab must not dereference it either.
    bar.setCurrentIndex(9);
    QCOMPARE(bar.currentIndex(), 0);
}

/*!
 * QxTabWidget is the page container on top of the bar: the stack holds the
 * pages, the bar holds the labels, and the two indices are kept in step by a
 * connection rather than by hand.
 */
void tst_QxAppWidgets::tabWidgetPagesAndCurrentPage()
{
    QxTabWidget tabs;
    QCOMPARE(tabs.count(), 0);
    QCOMPARE(tabs.currentIndex(), -1);
    QVERIFY(tabs.currentWidget() == Q_NULLPTR);
    QVERIFY(tabs.tabBar() != Q_NULLPTR);

    QWidget first;
    QWidget second;
    QCOMPARE(tabs.addTab(&first, QStringLiteral("First")), 0);
    QCOMPARE(tabs.addTab(&second, QStringLiteral("Second")), 1);

    QCOMPARE(tabs.count(), 2);
    QCOMPARE(tabs.currentIndex(), 0);
    QVERIFY(tabs.currentWidget() == &first);
    QVERIFY(tabs.widget(1) == &second);
    QCOMPARE(tabs.indexOf(&second), 1);

    // A page that is not in the tab widget is reported as absent.
    QWidget stranger;
    QCOMPARE(tabs.indexOf(&stranger), -1);

    // A null page is refused: it would desynchronize the stack from the bar.
    QCOMPARE(tabs.insertTab(0, Q_NULLPTR, QStringLiteral("null")), -1);
    QCOMPARE(tabs.count(), 2);

    tabs.setCurrentIndex(1);
    QCOMPARE(tabs.currentIndex(), 1);
    QVERIFY(tabs.currentWidget() == &second);

    tabs.setCurrentWidget(&first);
    QCOMPARE(tabs.currentIndex(), 0);

    tabs.removeTab(0);
    QCOMPARE(tabs.count(), 1);
    QCOMPARE(tabs.indexOf(&first), -1);
    QVERIFY(tabs.widget(0) == &second);
}

/*!
 * The tab position exists so the labels can sit on any of the four sides. What
 * it changes observably is the bar's orientation: the two that run along the
 * top and bottom are horizontal, the two that run along the sides are vertical.
 */
void tst_QxAppWidgets::tabWidgetPositionsMoveTheTabBar()
{
    QxTabWidget tabs;
    QCOMPARE(tabs.tabPosition(), QxTabWidget::North);
    QCOMPARE(tabs.tabBar()->orientation(), Qt::Horizontal);

    tabs.setTabPosition(QxTabWidget::West);
    QCOMPARE(tabs.tabPosition(), QxTabWidget::West);
    QCOMPARE(tabs.tabBar()->orientation(), Qt::Vertical);

    tabs.setTabPosition(QxTabWidget::East);
    QCOMPARE(tabs.tabBar()->orientation(), Qt::Vertical);

    tabs.setTabPosition(QxTabWidget::South);
    QCOMPARE(tabs.tabPosition(), QxTabWidget::South);
    QCOMPARE(tabs.tabBar()->orientation(), Qt::Horizontal);
}

/*!
 * The strip next to the tabs holds buttons the caller adds - a close button, a
 * settings button. addButton() hands one back, removeButton() takes it away and
 * destroys it, and a null is tolerated so a caller that kept no pointer can
 * still run its cleanup path.
 */
void tst_QxAppWidgets::tabWidgetButtons()
{
    QxTabWidget tabs;
    QToolButton *button = tabs.addButton(QStringLiteral("Action"));
    QVERIFY(button != Q_NULLPTR);
    QCOMPARE(button->text(), QStringLiteral("Action"));
    QCOMPARE(button->toolTip(), QStringLiteral("Action"));

    QPointer<QToolButton> guard(button);
    tabs.removeButton(button);
    QVERIFY(guard.isNull());

    // Passing nothing is a no-op rather than a crash.
    tabs.removeButton(Q_NULLPTR);
}

/*!
 * QxNavTabBar adds a customize menu to the tab bar: every tab gets a checkable
 * entry in it, and unchecking one hides the tab. visibleCount() is the number
 * of tabs that are on, and it has to follow setTabVisible() - that sync is the
 * only reason the two lists exist side by side.
 */
void tst_QxAppWidgets::navBarCountsTheVisibleTabs()
{
    QxNavTabBar bar;
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.visibleCount(), 0);
    QVERIFY(bar.actionCustomizeButton() != Q_NULLPTR);

    bar.addTab(QStringLiteral("a"));
    bar.addTab(QStringLiteral("b"));
    QCOMPARE(bar.count(), 2);
    QCOMPARE(bar.visibleCount(), 2);

    bar.setTabVisible(1, false);
    QCOMPARE(bar.count(), 2);
    QCOMPARE(bar.visibleCount(), 1);

    bar.setTabVisible(1, true);
    QCOMPARE(bar.visibleCount(), 2);

    // The customize entry goes away with its tab.
    bar.removeTab(0);
    QCOMPARE(bar.count(), 1);
    QCOMPARE(bar.visibleCount(), 1);
}

/*!
 * QxToolButton splits its face when the text goes under the icon, putting the
 * menu arrow on the right or at the bottom. Both settings are plain state with
 * a documented default, and the default is what an existing caller that never
 * touches them already shows.
 *
 * The menu area is the interesting half: it only means anything for the
 * TextUnderIcon style, and the two setters enforce that from opposite sides.
 * setMenuArea() ignores the call for every other style, and setToolButtonStyle()
 * moves the area to the right when the style is not TextUnderIcon - the right
 * edge being where the other styles have room for an arrow. A button that was
 * never told anything therefore reports BottomMenuArea while showing the icon
 * only style, which looks odd in isolation and is exactly what the other setter
 * cleans up.
 */
void tst_QxAppWidgets::toolButtonMenuAreaAndArrowRoundTrip()
{
    QxToolButton button(QStringLiteral("btn"));
    QCOMPARE(button.text(), QStringLiteral("btn"));
    QCOMPARE(button.menuArea(), QxToolButton::BottomMenuArea);
    QCOMPARE(button.menuArrowType(), Qt::DownArrow);

    // Ignored: the default style is not TextUnderIcon.
    button.setMenuArea(QxToolButton::RightMenuArea);
    QCOMPARE(button.menuArea(), QxToolButton::BottomMenuArea);

    // Honoured once the style makes the area mean something.
    button.setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button.setMenuArea(QxToolButton::RightMenuArea);
    QCOMPARE(button.menuArea(), QxToolButton::RightMenuArea);

    // Switching away from TextUnderIcon restores the right edge, and the
    // menu area survives a round trip through that style.
    button.setToolButtonStyle(Qt::ToolButtonIconOnly);
    QCOMPARE(button.menuArea(), QxToolButton::RightMenuArea);

    QxToolButton plain(QStringLiteral("plain"));
    QCOMPARE(plain.menuArea(), QxToolButton::BottomMenuArea);
    plain.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    QCOMPARE(plain.menuArea(), QxToolButton::RightMenuArea);

    button.setMenuArrowType(Qt::UpArrow);
    QCOMPARE(button.menuArrowType(), Qt::UpArrow);

    // The remaining setters are switches with no getter; they must not crash
    // and must leave the button usable.
    button.setForceAlignCenter(true);
    button.setForceAlignCenter(false);
    button.setForceDefaultShowMenu(true);
    button.setForceDefaultShowMenu(false);
    button.setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QVERIFY(button.sizeHint().isValid());

    // The icon+text constructor is the one the ribbon uses.
    QxToolButton labelled(QIcon(), QStringLiteral("labelled"));
    QCOMPARE(labelled.text(), QStringLiteral("labelled"));
}

/*!
 * The three small buttons are drawn, not driven: their whole observable surface
 * offscreen is that they can be built, that their size hint is a real size, and
 * that switching their orientation is safe. What the painting does with the
 * orientation is not asserted here - there is no screen to check it against.
 */
void tst_QxAppWidgets::smallButtonsSizeHintAndOrientation()
{
    QxMenuButton menuButton(QStringLiteral("menu"));
    QCOMPARE(menuButton.text(), QStringLiteral("menu"));
    QVERIFY(menuButton.sizeHint().isValid());

    QxMenuAccessButton access;
    QVERIFY(access.sizeHint().isValid());
    access.setOrientation(Qt::Vertical);
    QVERIFY(access.sizeHint().isValid());
    access.setOrientation(Qt::Horizontal);
    access.setToolButtonStyle(Qt::ToolButtonTextOnly);

    QxExtensionButton extension;
    // The extension button is square, which is the one thing its size hint has
    // to say: it is the arrow at the end of a toolbar.
    QCOMPARE(extension.sizeHint().width(), extension.sizeHint().height());
    QVERIFY(!extension.icon().isNull());
    extension.setOrientation(Qt::Vertical);
    QVERIFY(!extension.icon().isNull());
    extension.setOrientation(Qt::Horizontal);
}

TEST_ADD(tst_QxAppWidgets)

#include "tst_qxappwidgets.moc"
