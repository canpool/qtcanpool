#include "tst_global.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonquickaccessbar.h"
#include "qxribbon/windowbuttongroup.h"

#include <QAbstractButton>
#include <QAction>
#include <QCoreApplication>
#include <QEventLoop>
#include <QMenu>
#include <QScopedPointer>
#include <QToolButton>
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
    void page();
    void pageContext();
    void clear();
    void style();
    void objectNames();
    void windowButtonGroupIsPlacedBeforeTheWindowIsShown();
    void contextMenu();
};

void tst_RibbonBar::applicationButton()
{
    RibbonBar rb;

    // default button
    QAbstractButton *ab = rb.applicationButton();
    QCOMPARE("qx_RibbonApplicationButton", ab->objectName());
    QCOMPARE(tr("app"), ab->text());

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

    // A button the caller named keeps that name. The theme stylesheets target
    // the ribbon's own name (QAbstractButton#qx_RibbonApplicationButton), so
    // keeping it is what leaves a caller's button on the platform style instead
    // of the theme - see doc/pages/limitations.md, "custom QToolButton
    // application button sinks on press".
    QToolButton *named = new QToolButton();
    named->setObjectName(QStringLiteral("mine"));
    rb.setApplicationButton(named);
    QCOMPARE("mine", rb.applicationButton()->objectName());

    // delete
    rb.setApplicationButton(nullptr);
    QCOMPARE(nullptr, rb.applicationButton());
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

TEST_ADD(tst_RibbonBar)

#include "tst_ribbonbar.moc"
