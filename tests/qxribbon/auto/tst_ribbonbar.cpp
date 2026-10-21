/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonstyleoption.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribbonmainwindow.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonquickaccessbar.h"
#include "qxribbon/windowbuttongroup.h"

#include <QAbstractButton>
#include <QAction>
#include <QColor>
#include <QCoreApplication>
#include <QImage>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPalette>
#include <QEventLoop>
#include <QMenu>
#include <QScopedPointer>
#include <QTabBar>
#include <QToolButton>
#include <QTextEdit>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

namespace
{

/// Exposes the protected context-menu hook so the entries can be built and inspected
/// without going through a modal popup.
class ContextMenuRibbonBar : public RibbonBar
{
public:
    QMenu *buildContextMenu()
    {
        QMenu *menu = new QMenu(this);
        addContextMenuActions(menu);
        return menu;
    }
};

}   // namespace

class tst_RibbonBar : public QObject
{
    Q_OBJECT
private slots:
    void applicationButton();
    void applicationButtonSharesTheTabRow();
    void page();
    void pageContext();
    void clear();
    void style();
    void objectNames();
    void windowButtonGroupIsPlacedBeforeTheWindowIsShown();
    void noSelectionCostsNoRoom();
    void tabBarFollowsTheTheme();
    void contextMenu();
    void minimumButtonFollowsThePalette();
    void cornerWidgetTakesItsRoom();
    void cornerWidgetArrivesAfterTheBarIsUp();
    void cornerWidgetInstalledThroughTheMenuBar();
    void maximizedMdiChildUsesTheCorners();

    void theFourStylesTakeTheHeightTheOptionStatesAndMoveTheTabRow();
};

void tst_RibbonBar::applicationButton()
{
    RibbonBar rb;

    // default button
    QAbstractButton *ab = rb.applicationButton();
    QCOMPARE("qx_RibbonApplicationButton", ab->objectName());
    QCOMPARE(tr("app"), ab->text());
    // the property the theme sheet dresses the button by, next to its name
    QCOMPARE(true, ab->property("qxRibbonApplicationButton").toBool());

    int i = 0;
    connect(&rb, &RibbonBar::applicationButtonClicked, this, [&i]() {
        ++i;
    });
    QTest::mouseClick(ab, Qt::LeftButton);
    QCOMPARE(1, i);

    // new button
    rb.setApplicationButton(new QPushButton(tr("newapp")));
    ab = rb.applicationButton();
    QCOMPARE("qx_RibbonApplicationButton", ab->objectName());
    QCOMPARE(tr("newapp"), ab->text());

    QTest::mouseClick(ab, Qt::LeftButton);
    QCOMPARE(2, i);

    // A button the caller named keeps that name - and since 4.6 it is dressed
    // all the same: the sheet answers to the property setApplicationButton()
    // maintains, not only to the ribbon's own name, so the dressing follows the
    // button rather than the name it happens to carry.
    QToolButton *named = new QToolButton();
    named->setObjectName(QStringLiteral("mine"));
    rb.setApplicationButton(named);
    QCOMPARE("mine", rb.applicationButton()->objectName());
    QCOMPARE(true, named->property("qxRibbonApplicationButton").toBool());

    // delete
    rb.setApplicationButton(nullptr);
    QCOMPARE(nullptr, rb.applicationButton());
}

void tst_RibbonBar::applicationButtonSharesTheTabRow()
{
    // A tab title is centred in its tab rect, and the application button usually carries a
    // label of its own on the same row. Giving the button a rect even one pixel shorter
    // centres that label half a pixel off every title beside it - visible as "the File
    // button does not sit on the tabs' line", which is how the mismatch was reported.
    for (RibbonBar::RibbonStyle style : {RibbonBar::TabBarBelowStyle, RibbonBar::TabBarInlineStyle}) {
        QWidget window;
        window.resize(900, 400);

        RibbonBar rb(&window);
        rb.addPage(QStringLiteral("page1"));
        rb.addPage(QStringLiteral("page2"));
        rb.setRibbonStyle(style);

        window.show();
        rb.resizeRibbon();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        QTabBar *bar = rb.findChild<QTabBar *>(QStringLiteral("qx_RibbonTabBar"));
        QVERIFY(bar);
        QAbstractButton *button = rb.applicationButton();
        QVERIFY(button);

        const QRect tab = bar->tabRect(0).translated(bar->pos());
        QVERIFY(tab.isValid());
        QCOMPARE(tab.top(), button->geometry().top());
        QCOMPARE(tab.height(), button->geometry().height());
    }
}

void tst_RibbonBar::page()
{
    RibbonBar rb;

    // addPage and insertPage
    RibbonPage *page1 = rb.addPage(tr("page1"));   // change index
    QCOMPARE(0, rb.pageIndex(page1));
    QCOMPARE(0, rb.currentIndex());
    RibbonPage *page2 = rb.insertPage(0, tr("page2"));   // change index
    QCOMPARE(0, rb.pageIndex(page2));
    QCOMPARE(1, rb.pageIndex(page1));
    QCOMPARE(1, rb.currentIndex());
    RibbonPage *page3 = rb.insertPage(-1, tr("page3"));
    QCOMPARE(2, rb.pageIndex(page3));
    RibbonPage *page4 = rb.insertPage(10, tr("page4"));
    QCOMPARE(3, rb.pageIndex(page4));
    QCOMPARE(1, rb.currentIndex());

    RibbonPage *page5 = new RibbonPage(tr("page5"));
    rb.addPage(page5);
    QCOMPARE(4, rb.pageIndex(page5));
    RibbonPage *page6 = new RibbonPage(tr("page6"));
    rb.insertPage(4, page6);
    QCOMPARE(4, rb.pageIndex(page6));
    QCOMPARE(5, rb.pageIndex(page5));

    QString titles = "";
    for (RibbonPage *page : rb.pages()) {
        titles += page->pageName() + ",";
    }
    QCOMPARE("page2,page1,page3,page4,page6,page5,", titles);

    // page
    QCOMPARE(page4, rb.page(3));
    QCOMPARE(nullptr, rb.page(-1));
    QCOMPARE(nullptr, rb.page(10));

    // pageByname
    QCOMPARE(page4, rb.pageByName(tr("page4")));
    QCOMPARE(page5, rb.pageByName(tr("page5")));
    QCOMPARE(nullptr, rb.pageByName(tr("pagexxx")));

    // pageByObjectName
    QCOMPARE(page4, rb.pageByObjectName(page4->objectName()));
    QCOMPARE(nullptr, rb.pageByObjectName("xxx"));
    QCOMPARE(page6, rb.pageByObjectName(""));                    // page6 is the first of the empty object names
    QCOMPARE(page6, rb.pageByObjectName(page5->objectName()));   // page5 objectname is empty

    // move
    rb.movePage(4, 5);   // swap page6 and page5
    QCOMPARE(4, rb.pageIndex(page5));
    QCOMPARE(5, rb.pageIndex(page6));

    rb.movePage(5, 4);   // swap page6 and page5
    QCOMPARE(4, rb.pageIndex(page6));
    QCOMPARE(5, rb.pageIndex(page5));

    titles = "";
    for (RibbonPage *page : rb.pages()) {
        titles += page->pageName() + ",";
    }
    QCOMPARE("page2,page1,page3,page4,page6,page5,", titles);
    rb.movePage(4, 5);   // swap page6 and page5
    QCOMPARE(4, rb.pageIndex(page5));
    QCOMPARE(5, rb.pageIndex(page6));

    // show
    QCOMPARE(true, rb.isPageVisible(page5));
    rb.hidePage(page5);
    QCOMPARE(false, rb.isPageVisible(page5));
    rb.showPage(page5);   // change index
    QCOMPARE(true, rb.isPageVisible(page5));
    QCOMPARE(4, rb.currentIndex());

    // remove
    rb.removePage(page5);
    QCOMPARE(4, rb.pageIndex(page6));
    rb.insertPage(4, page5);   // readd page,  change index
    QCOMPARE(5, rb.currentIndex());
    QCOMPARE(4, rb.pageIndex(page5));
    QCOMPARE(5, rb.pageIndex(page6));

    // index
    QCOMPARE(5, rb.currentIndex());
    rb.setCurrentIndex(4);
    QCOMPARE(4, rb.currentIndex());
    rb.raisePage(page6);   // change index
    QCOMPARE(5, rb.currentIndex());
}

void tst_RibbonBar::pageContext()
{
    RibbonBar rb;

    // addPageContext and addPage
    RibbonPage *page1 = rb.addPage(tr("page1"));
    RibbonPageContext *pc1 = rb.addPageContext(tr("context1"));
    RibbonPage *page2 = pc1->addPage(tr("page2"));
    RibbonPage *page3 = pc1->addPage(tr("page3"));
    RibbonPageContext *pc2 = new RibbonPageContext();
    pc2->setContextTitle(tr("context2"));
    rb.addPageContext(pc2);
    RibbonPage *page4 = pc2->addPage(tr("page4"));
    RibbonPage *page5 = pc2->addPage(tr("page5"));

    QCOMPARE(false, rb.isPageContextVisible(pc1));
    QCOMPARE(false, rb.isPageContextVisible(pc2));

    QCOMPARE(5, rb.pages().count());
    QCOMPARE(1, rb.pages(false).count());
    QCOMPARE(2, rb.pageContextList().count());

    QCOMPARE(0, rb.currentIndex());
    QCOMPARE(0, rb.pageIndex(page1));
    QCOMPARE(-1, rb.pageIndex(page2));   // page2 unusable
    QCOMPARE(-1, rb.pageIndex(page3));   // page3 unusable
    QCOMPARE(-1, rb.pageIndex(page4));   // page4 unusable
    QCOMPARE(-1, rb.pageIndex(page5));   // page5 unusable

    rb.showPageContext(pc1);
    QCOMPARE(1, rb.pageIndex(page2));   // page2 usable
    QCOMPARE(true, rb.isPageContextVisible(pc1));

    rb.setPageContextVisible(pc2, true);
    QCOMPARE(3, rb.pageIndex(page4));   // page4 usable
    QCOMPARE(true, rb.isPageContextVisible(pc2));

    rb.hidePageContext(pc1);
    QCOMPARE(-1, rb.pageIndex(page2));   // page2 unusable
    QCOMPARE(1, rb.pageIndex(page4));    // move forward
    QCOMPARE(false, rb.isPageContextVisible(pc1));

    rb.destroyPageContext(pc2);
    QCOMPARE(1, rb.pageContextList().count());
    page4 = nullptr;
    page5 = nullptr;
    pc2 = nullptr;
}

void tst_RibbonBar::clear()
{
    RibbonBar rb;

    RibbonPage *page1 = rb.addPage(tr("page1"));
    RibbonPage *page2 = rb.insertPage(0, tr("page2"));
    RibbonPage *page3 = new RibbonPage(tr("page3"));
    rb.addPage(page3);
    RibbonPage *page4 = new RibbonPage(tr("page4"));
    rb.insertPage(1, page4);

    rb.clearPages();

    // QCOMPARE(page1->pageName(), tr("page1"));
    // QCOMPARE(page2->pageName(), tr("page2"));
    // QCOMPARE(page3->pageName(), tr("page3"));
    // QCOMPARE(page4->pageName(), tr("page4"));

    Q_UNUSED(page1);
    Q_UNUSED(page2);

    page1 = nullptr;
    page2 = nullptr;
    page3 = nullptr;
    page4 = nullptr;
}

void tst_RibbonBar::style()
{
    RibbonBar rb;

    QCOMPARE(RibbonBar::TabBarBelowStyle, rb.currentRibbonStyle());
    QCOMPARE(true, rb.hasOwnTabRow());
    QCOMPARE(false, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::TabBarBelowTwoRowStyle);

    QCOMPARE(RibbonBar::TabBarBelowTwoRowStyle, rb.currentRibbonStyle());
    QCOMPARE(true, rb.hasOwnTabRow());
    QCOMPARE(true, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::TabBarInlineStyle);

    QCOMPARE(RibbonBar::TabBarInlineStyle, rb.currentRibbonStyle());
    QCOMPARE(false, rb.hasOwnTabRow());
    QCOMPARE(false, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::TabBarInlineTwoRowStyle);

    QCOMPARE(RibbonBar::TabBarInlineTwoRowStyle, rb.currentRibbonStyle());
    QCOMPARE(false, rb.hasOwnTabRow());
    QCOMPARE(true, rb.isTwoRowStyle());
}

/*!
 * A style is not four flags: it is four arrangements of the same window, and
 * the option is what states how tall each of them is. The arithmetic of those
 * heights is covered where the option lives; what was never covered is that the
 * bar standing there takes the height the option states for the style it was
 * given, and that moving between the styles moves the tab row with it.
 *
 * So the four values are walked in one window and the two numbers that say
 * where everything ended up are read at every step: how tall the bar is, and
 * where its tab row begins.
 */
void tst_RibbonBar::theFourStylesTakeTheHeightTheOptionStatesAndMoveTheTabRow()
{
    RibbonBar bar;
    RibbonPage *page = bar.addPage(QStringLiteral("page1"));
    RibbonGroup *group = page->addGroup(QStringLiteral("group1"));
    QAction *action = new QAction(QStringLiteral("action1"), &bar);
    action->setObjectName(QStringLiteral("action1"));
    group->addAction(action, RibbonGroup::Small);
    bar.resize(800, 400);
    bar.show();

    const RibbonStyleOption option;
    const QList<RibbonBar::RibbonStyle> styles{RibbonBar::TabBarBelowStyle, RibbonBar::TabBarBelowTwoRowStyle,
                                               RibbonBar::TabBarInlineStyle, RibbonBar::TabBarInlineTwoRowStyle};

    QHash<int, int> heights;
    QHash<int, int> tabRows;
    for (RibbonBar::RibbonStyle style : styles) {
        bar.setRibbonStyle(style);
        Probe::settle(&bar);
        QTabBar *tabBar = bar.findChild<QTabBar *>();
        QVERIFY(tabBar);
        heights.insert(style, bar.height());
        tabRows.insert(style, tabBar->y());
    }

    // What the option states is what the bar takes, style for style.
    for (RibbonBar::RibbonStyle style : styles) {
        QCOMPARE(heights.value(style), option.ribbonBarHeight(style));
    }
    // And the two families are different families: a tab row of its own starts
    // below the title, an inlined one starts beside it.
    QVERIFY(tabRows.value(RibbonBar::TabBarBelowStyle) > tabRows.value(RibbonBar::TabBarInlineStyle));
    QVERIFY(tabRows.value(RibbonBar::TabBarBelowTwoRowStyle) > tabRows.value(RibbonBar::TabBarInlineTwoRowStyle));
}

/*!
 * The bar names its children with the qx_ prefix so a stylesheet can target
 * them. The stacked widget used to be left out: the tab bar was named twice
 * and the second name clobbered the first (see ribbonbar.cpp init()), so the
 * tab bar also lost the name it had just been given.
 */
void tst_RibbonBar::objectNames()
{
    RibbonBar rb;

    const auto has = [&rb](const char *name) {
        return rb.findChild<QWidget *>(QString::fromLatin1(name)) != Q_NULLPTR;
    };

    QCOMPARE(has("qx_RibbonTabBar"), true);
    QCOMPARE(has("qx_RibbonStackedWidget"), true);
    QCOMPARE(has("qx_RibbonTabBarBaseLine"), true);
    QCOMPARE(has("qx_RibbonQuickAccessBar"), true);
    QCOMPARE(has("qx_RibbonApplicationButton"), true);
}

/*!
 * The ribbon lays itself out through resizeRibbon(), which queues a resize
 * event. An application window is normally still hidden when that event is
 * delivered - a start-up splash drives the event loop while the window is
 * being built - and isVisible() is false for the whole window then. A layout
 * that asks "visible?" instead of "hidden?" therefore skips the window button
 * group and leaves it in the top-left corner, over the tab bar, until the
 * window happens to be resized.
 */
void tst_RibbonBar::windowButtonGroupIsPlacedBeforeTheWindowIsShown()
{
    const int windowWidth = 800;

    QWidget window;
    window.resize(windowWidth, 400);

    RibbonBar rb(&window);
    rb.addPage(QStringLiteral("page"));
    rb.resize(windowWidth, rb.height());

    WindowButtonGroup group(&window,
                            Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    rb.setWindowButtonGroup(&group);

    // The deferred layout, delivered while the window is still hidden.
    rb.resizeRibbon();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    const int groupWidth = group.sizeHint().width();
    QVERIFY(groupWidth > 0);
    QCOMPARE(group.geometry(), QRect(windowWidth - groupWidth, 0, groupWidth, rb.titleBarHeight()));
}

/*!
 * A minimized ribbon shows no selected tab, and QTabBar has no state for that:
 * setCurrentIndex(-1) does nothing, because Qt stores an index only when it
 * resolves to a tab. The bar therefore keeps a placeholder tab - empty,
 * disabled, always last - and selects that one instead.
 *
 * The placeholder must cost nothing. Leaving its width to the style made it 16
 * to 28 pixels wide in the shipped themes, because they give ::tab its padding,
 * and it was counted into the strip's minimum width. The padding below is what
 * makes this test say so: it is wide enough that a placeholder sized by the
 * theme could not pass.
 */
void tst_RibbonBar::noSelectionCostsNoRoom()
{
    QWidget window;
    window.resize(800, 400);

    RibbonBar rb(&window);
    rb.addPage(QStringLiteral("page1"));
    rb.addPage(QStringLiteral("page2"));
    rb.setStyleSheet(QStringLiteral("QTabBar::tab { padding: 0 24px; }"));

    QTabBar *bar = rb.findChild<QTabBar *>(QStringLiteral("qx_RibbonTabBar"));
    QVERIFY(bar);
    // The placeholder is a tab as far as QTabBar is concerned, so the base class
    // count is one more than the pages the ribbon holds.
    QCOMPARE(3, bar->QTabBar::count());

    // Lay the strip out: before that every tab rect is empty, and the width
    // asserted below would hold whatever the placeholder did.
    window.show();
    rb.resizeRibbon();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QVERIFY(bar->tabRect(0).width() > 48);   // the padding is in effect

    QCOMPARE(0, bar->tabRect(bar->QTabBar::count() - 1).width());

    // "No page is selected" is what the ribbon reports, while QTabBar itself
    // holds the placeholder.
    rb.setCurrentIndex(-1);
    QCOMPARE(-1, rb.currentIndex());
    QCOMPARE(bar->QTabBar::count() - 1, bar->QTabBar::currentIndex());

    // Selecting a page again leaves the placeholder where it was.
    rb.setCurrentIndex(1);
    QCOMPARE(1, rb.currentIndex());
}

/*!
 * A theme decides how wide a tab is: the shipped sheets pad ::tab by 8 to 12
 * pixels, and the tab bar's own width comes from adding those tabs up (see
 * RibbonBarPrivate::calcMinTabBarWidth and layoutTabBar). A stylesheet change
 * therefore has to reach the layout as well as the style, or the bar keeps the
 * width the previous theme asked for and the wider tabs no longer fit - QTabBar
 * answers that by scrolling, so the strip grows a pair of arrows on its right.
 */
void tst_RibbonBar::tabBarFollowsTheTheme()
{
    QWidget window;
    window.resize(1200, 400);

    RibbonBar rb(&window);
    for (int i = 0; i < 4; ++i) {
        rb.addPage(QStringLiteral("page%1").arg(i));
    }
    rb.setStyleSheet(QStringLiteral("QTabBar::tab { padding: 0 4px; }"));

    QTabBar *bar = rb.findChild<QTabBar *>(QStringLiteral("qx_RibbonTabBar"));
    QVERIFY(bar);

    window.show();
    rb.resizeRibbon();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    const int narrowWidth = bar->width();
    QVERIFY(narrowWidth > 0);

    // The same thing a theme switch does, only narrower in scope: a new sheet
    // with wider tabs. Nothing calls resizeRibbon() here on purpose - an
    // application that switches a theme calls setStyleSheet and nothing else,
    // so the bar has to notice the change itself.
    rb.setStyleSheet(QStringLiteral("QTabBar::tab { padding: 0 24px; }"));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QVERIFY(bar->width() > narrowWidth);

    // The tabs fit: the last one ends inside the strip, so QTabBar has no reason
    // to put its scroll buttons up. Both widths are checked because a bar that
    // grew by less than the tabs did would still scroll.
    const int last = bar->QTabBar::count() - 1;
    QVERIFY(bar->tabRect(last - 1).right() < bar->width());

    // And back: the width is recomputed from the tabs each time, not grown
    // monotonically, so a narrower theme has to give the space up again.
    rb.setStyleSheet(QStringLiteral("QTabBar::tab { padding: 0 4px; }"));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QCOMPARE(narrowWidth, bar->width());
}

void tst_RibbonBar::contextMenu()
{
    ContextMenuRibbonBar rb;
    rb.addPage(QStringLiteral("page"));

    QScopedPointer<QMenu> menu(rb.buildContextMenu());
    QVERIFY(!menu->isEmpty());

    QAction *minimizeAction = nullptr;
    QAction *customizeAction = nullptr;
    for (QAction *action : menu->actions()) {
        if (action->objectName() == QStringLiteral("qx_RibbonMinimizeAction")) {
            minimizeAction = action;
        } else if (action->objectName() == QStringLiteral("qx_RibbonCustomizeQabAction")) {
            customizeAction = action;
        }
    }
    QVERIFY(minimizeAction);
    QVERIFY(customizeAction);

    // The minimize entry mirrors the ribbon state and toggles it.
    QCOMPARE(minimizeAction->isCheckable(), true);
    QCOMPARE(minimizeAction->isChecked(), rb.isMinimized());
    minimizeAction->trigger();
    QVERIFY(rb.isMinimized());

    // The customize entry reaches the quick access bar's own entry, so the application
    // receives the same signal it receives from the bar's customize menu.
    QSignalSpy spy(rb.quickAccessBar(), &RibbonQuickAccessBar::customizeDialogRequested);
    customizeAction->trigger();
    QCOMPARE(spy.count(), 1);
}

/*!
 * The button that folds the ribbon away.
 *
 * It is an icon-only button, so its glyph is the palette's: the stock style icon
 * this used to ask for is black whatever the palette says, which made it the one
 * glyph that disappeared on a dark sheet. The tooltip is asserted with it because
 * an icon-only button has nothing else to name it.
 */
void tst_RibbonBar::minimumButtonFollowsThePalette()
{
    RibbonBar rb;
    QVERIFY(!rb.haveShowMinimumButton());

    rb.showMinimumButton(true);
    QVERIFY(rb.haveShowMinimumButton());

    QToolButton *button = rb.cornerButtonGroup(Qt::BottomRightCorner)->findChild<QToolButton *>();
    QVERIFY(button);
    QAction *action = button->defaultAction();
    QVERIFY(action);

    // Both states name what a click does, and the two names differ.
    QVERIFY(!action->toolTip().isEmpty());
    const QString expandedTip = action->toolTip();
    const QImage expandedGlyph = action->icon().pixmap(16, 16).toImage();
    QVERIFY(!expandedGlyph.isNull());

    rb.setMinimized(true);
    QVERIFY(!action->toolTip().isEmpty());
    QVERIFY(action->toolTip() != expandedTip);
    const QImage foldedGlyph = action->icon().pixmap(16, 16).toImage();
    QVERIFY(!foldedGlyph.isNull());
    // Folded and unfolded point opposite ways, so they cannot be the same raster.
    QVERIFY(foldedGlyph != expandedGlyph);

    // The same state painted from a light and from a dark palette: only the tint
    // differs, which is the whole point of drawing the glyph from the palette.
    QPalette dark = rb.palette();
    dark.setColor(QPalette::ButtonText, QColor(Qt::white));
    rb.setPalette(dark);
    const QImage onDark = action->icon().pixmap(16, 16).toImage();
    QVERIFY(onDark != foldedGlyph);

    QPalette light = rb.palette();
    light.setColor(QPalette::ButtonText, QColor(Qt::black));
    rb.setPalette(light);
    QVERIFY(action->icon().pixmap(16, 16).toImage() != onDark);

    // Hide it again: the corner group goes back to having no minimum button.
    rb.showMinimumButton(false);
    QVERIFY(!rb.haveShowMinimumButton());
}

/*!
 * The corner widgets are QMenuBar's, and both layout functions place them: the
 * left one on the title row, the right one on the tab row in TabBarBelowStyle (the
 * ribbon shares nothing with the title there) and on the title row in
 * TabBarInlineStyle.
 *
 * Nothing in this tree sets one - what sets them in practice is Qt: while an MDI
 * child is maximized, QMainWindow moves the child's icon and its window buttons
 * into the menu bar, which for a ribbon window is this bar. The geometry below is
 * the contract those widgets get: the width they ask for, inside the bar, on the
 * row the style puts them on, and clear of the tab strip.
 */
void tst_RibbonBar::cornerWidgetTakesItsRoom()
{
    for (RibbonBar::RibbonStyle style : {RibbonBar::TabBarBelowStyle, RibbonBar::TabBarInlineStyle}) {
        QWidget window;
        window.resize(900, 400);

        RibbonBar rb(&window);
        rb.addPage(QStringLiteral("page1"));
        rb.addPage(QStringLiteral("page2"));
        rb.setRibbonStyle(style);

        // Created without a parent, the way a caller writes it: the setter has to
        // show them, or a hidden widget takes no part in the layout.
        QToolButton *left = new QToolButton();
        left->setText(QStringLiteral("L"));
        QToolButton *right = new QToolButton();
        right->setText(QStringLiteral("R"));
        rb.setCornerWidget(left, Qt::TopLeftCorner);
        rb.setCornerWidget(right, Qt::TopRightCorner);

        window.show();
        rb.resizeRibbonNow();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

        QVERIFY(left->isVisible());
        QVERIFY(right->isVisible());
        QCOMPARE(left->width(), left->sizeHint().width());
        QCOMPARE(right->width(), right->sizeHint().width());

        QVERIFY(rb.rect().contains(left->geometry()));
        QVERIFY(rb.rect().contains(right->geometry()));
        QVERIFY(left->geometry().right() < right->geometry().left());

        QTabBar *bar = rb.findChild<QTabBar *>(QStringLiteral("qx_RibbonTabBar"));
        QVERIFY(bar);
        QVERIFY(!left->geometry().intersects(bar->geometry()));
        QVERIFY(!right->geometry().intersects(bar->geometry()));

        // The right corner ends where the bar's margin does, on both styles.
        QCOMPARE(right->geometry().right(), rb.width() - 1 - rb.contentsMargins().right());

        if (style == RibbonBar::TabBarBelowStyle) {
            // Two rows: left shares the title row with the icon and the quick access
            // bar, right shares the tab row with the application button.
            QVERIFY(left->geometry().top() < rb.titleBarHeight());
            QVERIFY(right->geometry().top() >= rb.titleBarHeight());

            // The quick access bar follows the left corner widget in that row.
            QWidget *quickAccess = rb.quickAccessBar()->parentWidget();
            if (quickAccess && !quickAccess->isHidden()) {
                QVERIFY(left->geometry().right() < quickAccess->geometry().left());
            }
        } else {
            // One row: both sit in it, and the left one is put right of the tabs.
            QVERIFY(left->geometry().top() < rb.titleBarHeight());
            QVERIFY(right->geometry().top() < rb.titleBarHeight());
            QVERIFY(left->geometry().left() > bar->geometry().right());
        }
    }
}

/*!
 * A corner widget installed after the bar is up, and one that changes its size
 * later on.
 *
 * setCornerWidget() is QMenuBar's and not virtual, so there are two things it
 * cannot do for this bar: a widget created without a parent arrives hidden (a
 * hidden widget takes no part in the layout), and nothing asks the bar to lay
 * itself out again. Both leave the widget at the position and size it was born
 * with until something else resizes the bar.
 *
 * A size change is the same defect one step later: the ribbon gives a corner
 * widget the width it asks for, so a new size hint makes that placement stale
 * without any resize of the bar at all.
 */
void tst_RibbonBar::cornerWidgetArrivesAfterTheBarIsUp()
{
    RibbonBar rb;
    rb.addPage(QStringLiteral("page"));
    rb.resize(700, rb.height());
    rb.show();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QToolButton *corner = new QToolButton();
    corner->setText(QStringLiteral("A"));
    rb.setCornerWidget(corner, Qt::TopLeftCorner);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QVERIFY(corner->isVisible());
    QVERIFY(corner->x() < 100);   // at the left end, not left where a new child sits
    QCOMPARE(corner->width(), corner->sizeHint().width());

    const int narrow = corner->sizeHint().width();
    corner->setText(QStringLiteral("a much longer label"));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QVERIFY(corner->sizeHint().width() > narrow);
    QCOMPARE(corner->width(), corner->sizeHint().width());
}

/*!
 * A corner widget installed the way Qt installs one - through QMenuBar.
 *
 * setCornerWidget() is not virtual, so QMdiArea, and anything else that treats a
 * ribbon window as a QMainWindow, reaches QMenuBar's implementation. That one lays
 * the widget out itself: across the whole height of the menu bar, centred on the
 * left and top-aligned on the right (Qt 5 and 6, QMenuBarPrivate::updateGeometries).
 * A menu bar can use both placements - its corners own its whole height - and a
 * ribbon cannot, because its rows are shorter than the bar. The widget keeps that
 * placement too: it equals the size the widget asks for, so a check on the size
 * alone says nothing is wrong.
 *
 * What this checks is that the bar takes the placement back, on the strength of the
 * LayoutRequest Qt posts after placing them: the widget has to end up on the title
 * row instead of centred in the middle of the ribbon.
 */
void tst_RibbonBar::cornerWidgetInstalledThroughTheMenuBar()
{
    QWidget window;
    window.resize(900, 400);

    RibbonBar rb(&window);
    rb.addPage(QStringLiteral("page"));
    rb.resizeRibbonNow();
    window.show();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QToolButton *corner = new QToolButton();
    corner->setText(QStringLiteral("L"));
    static_cast<QMenuBar *>(&rb)->setCornerWidget(corner, Qt::TopLeftCorner);
    // Reparenting hides a widget, and QMenuBar never shows a corner widget it is given.
    corner->show();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QVERIFY(corner->isVisible());
    QCOMPARE(corner->width(), corner->sizeHint().width());
    QVERIFY(corner->geometry().bottom() < rb.titleBarHeight());
}

/*!
 * A maximized MDI child is what exercises the corner widgets in a running
 * application: QMdiArea installs the child's icon (TopLeftCorner) and its
 * minimize / restore / close buttons (TopRightCorner) on the host window's menu
 * bar, and for a ribbon window that is this bar. They are Qt's widgets, drawn by
 * the platform style, so what is checked here is the part the ribbon owns: they
 * get the room they ask for, on the row the style puts them on, clear of the strip.
 *
 * The row is the half of that which QMenuBar gets wrong when it places them itself,
 * so the assertions below are also what keeps the bar from leaving them there.
 *
 * On Qt 5 the glyphs inside those buttons are clipped even when the room is
 * right - see doc/pages/limitations.md, "the MDI child's window buttons on Qt 5".
 * That is why this asserts geometry and not pixels.
 */
void tst_RibbonBar::maximizedMdiChildUsesTheCorners()
{
    RibbonMainWindow window;
    RibbonBar *ribbon = window.ribbonBar();
    QVERIFY(ribbon);
    ribbon->addPage(QStringLiteral("page"));

    QMdiArea *area = new QMdiArea(&window);
    window.setCentralWidget(area);
    QMdiSubWindow *child = area->addSubWindow(new QTextEdit(QStringLiteral("child")));
    child->resize(300, 200);
    window.resize(900, 500);
    window.show();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    // A child that is a window of its own puts nothing on the bar.
    QCOMPARE(nullptr, ribbon->cornerWidget(Qt::TopLeftCorner));
    QCOMPARE(nullptr, ribbon->cornerWidget(Qt::TopRightCorner));

    child->showMaximized();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    QWidget *icon = ribbon->cornerWidget(Qt::TopLeftCorner);
    QWidget *buttons = ribbon->cornerWidget(Qt::TopRightCorner);
    QVERIFY(icon);
    QVERIFY(buttons);
    QVERIFY(icon->isVisible());
    QVERIFY(buttons->isVisible());

    QCOMPARE(icon->width(), icon->sizeHint().width());
    QCOMPARE(buttons->width(), buttons->sizeHint().width());
    QVERIFY(ribbon->rect().contains(icon->geometry()));
    QVERIFY(ribbon->rect().contains(buttons->geometry()));
    QCOMPARE(buttons->geometry().right(), ribbon->width() - 1 - ribbon->contentsMargins().right());

    // The icon on the title row, the buttons on the tab row when the style has one.
    QVERIFY(icon->geometry().bottom() < ribbon->titleBarHeight());
    if (ribbon->hasOwnTabRow()) {
        QVERIFY(buttons->geometry().top() >= ribbon->titleBarHeight());
    } else {
        QVERIFY(buttons->geometry().bottom() < ribbon->titleBarHeight());
    }

    QTabBar *bar = ribbon->findChild<QTabBar *>(QStringLiteral("qx_RibbonTabBar"));
    QVERIFY(bar);
    QVERIFY(!icon->geometry().intersects(bar->geometry()));
    QVERIFY(!buttons->geometry().intersects(bar->geometry()));

    // Restoring the child takes them away again; Qt removes them from the bar.
    child->showNormal();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    QCOMPARE(nullptr, ribbon->cornerWidget(Qt::TopLeftCorner));
    QCOMPARE(nullptr, ribbon->cornerWidget(Qt::TopRightCorner));
}

TEST_ADD(tst_RibbonBar)

#include "tst_ribbonbar.moc"
