#include "tst_global.h"

#include "qxribbon/ribbonmdiarea.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"

#include <QApplication>
#include <QEventLoop>
#include <QMdiSubWindow>
#include <QPair>
#include <QTabBar>
#include <QTextEdit>
#include <QVector>

QX_RIBBON_USE_NAMESPACE

namespace
{

void flushEvents()
{
    // The activation changes are delivered synchronously; this only settles
    // anything the area posted on the way (tab geometry, deletion).
    QApplication::processEvents(QEventLoop::AllEvents);
}

RibbonPage *addPage(RibbonBar *bar, const QString &title)
{
    RibbonPage *page = bar->addPage(title);
    page->addGroup(QStringLiteral("group"));
    return page;
}

/*!
 * The invariant the area promises: of the pages it manages, exactly the
 * active document's is up - and none at all when nothing is active. Which
 * document QMdiArea hands the activation to after a close is Qt's call and
 * is deliberately not pinned here.
 */
void verifyPageInvariant(RibbonMdiArea *area, RibbonBar *bar, const QVector<QPair<QMdiSubWindow *, RibbonPage *>> &docs)
{
    QMdiSubWindow *active = area->activeSubWindow();
    for (const auto &doc : docs) {
        if (doc.first == active) {
            QVERIFY2(bar->isPageVisible(doc.second), "the active document's page must be up");
        } else {
            QVERIFY2(!bar->isPageVisible(doc.second), "a page of a document that is not active must be down");
        }
    }
}

}   // namespace

class tst_RibbonMdiArea : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void defaultsToTabbedDocuments();
    void setTabbedDocumentsTogglesViewMode();
    void documentTabBarBelongsToTheArea();
    void documentsCarryTheirPages();
    void pagesStartDownUntilADocumentIsActive();
    void switchingActiveDocumentSwapsPages();
    void closingActiveDocumentHidesItsPage();
    void closingLastDocumentHidesEveryPage();
    void documentWithoutPageShowsNoPage();
    void pageOnNoBarIsLeftAlone();
};

void tst_RibbonMdiArea::defaultsToTabbedDocuments()
{
    RibbonMdiArea area;
    QCOMPARE(area.viewMode(), QMdiArea::TabbedView);
    QVERIFY(area.documentMode());
    QVERIFY(area.tabsClosable());
    QVERIFY(area.tabsMovable());
    QVERIFY(area.isTabbedDocuments());
}

void tst_RibbonMdiArea::setTabbedDocumentsTogglesViewMode()
{
    RibbonMdiArea area;
    area.setTabbedDocuments(false);
    QCOMPARE(area.viewMode(), QMdiArea::SubWindowView);
    QVERIFY(!area.documentMode());
    QVERIFY(!area.isTabbedDocuments());

    area.setTabbedDocuments(true);
    QCOMPARE(area.viewMode(), QMdiArea::TabbedView);
    QVERIFY(area.documentMode());
    QVERIFY(area.isTabbedDocuments());
}

void tst_RibbonMdiArea::documentTabBarBelongsToTheArea()
{
    RibbonMdiArea area;
    QTabBar *tabBar = area.documentTabBar();
    QVERIFY(tabBar != Q_NULLPTR);
    // It is the bar QMdiArea drives, not a copy of it.
    QVERIFY(area.isAncestorOf(tabBar));

    area.addDocument(new QTextEdit, QStringLiteral("one"));
    flushEvents();
    QCOMPARE(tabBar->count(), 1);
}

void tst_RibbonMdiArea::documentsCarryTheirPages()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    QMdiSubWindow *docA = area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);

    QCOMPARE(area.documentPage(docA), pageA);
    QCOMPARE(area.documentPage(docB), pageB);
    QCOMPARE(area.subWindowList().size(), 2);
    QCOMPARE(docA->windowTitle(), QString("A"));
    QCOMPARE(docB->windowTitle(), QString("B"));
    // The sub-window owns the content, as QMdiArea promises.
    QVERIFY(docA->widget() != Q_NULLPTR);
}

void tst_RibbonMdiArea::pagesStartDownUntilADocumentIsActive()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    QMdiSubWindow *docA = area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);
    const QVector<QPair<QMdiSubWindow *, RibbonPage *>> docs = {{docA, pageA}, {docB, pageB}};

    // Pages begin their life visible - they are tabs of the bar - so a fresh
    // document's page is brought down here to establish the invariant: of the
    // pages the area manages, only the active document's is up, and none at
    // all while nothing is active.
    QVERIFY(area.activeSubWindow() == Q_NULLPTR);
    QVERIFY(!bar.isPageVisible(pageA));
    QVERIFY(!bar.isPageVisible(pageB));

    // Note: Qt's tabbed view selects the first document's tab but leaves the
    // area's active window null, and setActiveSubWindow() is then a no-op on
    // the tab that is already current - so the first activation here goes
    // through the second document, the same path a click on its tab takes.
    area.setActiveSubWindow(docB);
    flushEvents();
    verifyPageInvariant(&area, &bar, docs);
}

void tst_RibbonMdiArea::switchingActiveDocumentSwapsPages()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    QMdiSubWindow *docA = area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);

    // Make B the active document: only its page may be up.
    area.setActiveSubWindow(docB);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docB);
    QVERIFY(!bar.isPageVisible(pageA));
    QVERIFY(bar.isPageVisible(pageB));

    // Switch to A: the pages follow.
    area.setActiveSubWindow(docA);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docA);
    QVERIFY(bar.isPageVisible(pageA));
    QVERIFY(!bar.isPageVisible(pageB));

    // And back.
    area.setActiveSubWindow(docB);
    flushEvents();
    QVERIFY(!bar.isPageVisible(pageA));
    QVERIFY(bar.isPageVisible(pageB));
}

void tst_RibbonMdiArea::closingActiveDocumentHidesItsPage()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    QMdiSubWindow *docA = area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);
    const QVector<QPair<QMdiSubWindow *, RibbonPage *>> docs = {{docA, pageA}, {docB, pageB}};

    area.setActiveSubWindow(docB);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docB);
    QVERIFY(bar.isPageVisible(pageB));

    // Closing the active document deletes it; its page goes with it, and the
    // invariant holds whichever document (if any) takes the activation over.
    docB->close();
    flushEvents();
    QVERIFY(!bar.isPageVisible(pageB));
    verifyPageInvariant(&area, &bar, docs);
}

void tst_RibbonMdiArea::closingLastDocumentHidesEveryPage()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    QMdiSubWindow *docA = area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);
    const QVector<QPair<QMdiSubWindow *, RibbonPage *>> docs = {{docA, pageA}, {docB, pageB}};

    // The first activation goes through B: Qt's tabbed view selects the
    // first document's tab but leaves the area's active window null, and
    // setActiveSubWindow() is a no-op on the tab that is already current.
    area.setActiveSubWindow(docB);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docB);
    QVERIFY(bar.isPageVisible(pageB));

    area.setActiveSubWindow(docA);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docA);
    QVERIFY(bar.isPageVisible(pageA));

    docA->close();
    flushEvents();
    // A is gone with its page; whichever document (if any) takes the
    // activation over, the invariant holds.
    QVERIFY(!bar.isPageVisible(pageA));
    verifyPageInvariant(&area, &bar, docs);

    docB->close();
    flushEvents();
    // Nothing is active any more, so no registered page may stay up.
    QVERIFY(area.activeSubWindow() == Q_NULLPTR);
    QVERIFY(!bar.isPageVisible(pageA));
    QVERIFY(!bar.isPageVisible(pageB));
}

void tst_RibbonMdiArea::documentWithoutPageShowsNoPage()
{
    RibbonBar bar;
    RibbonPage *pageA = addPage(&bar, QStringLiteral("PageA"));
    RibbonPage *pageB = addPage(&bar, QStringLiteral("PageB"));

    RibbonMdiArea area;
    area.addDocument(new QTextEdit, QStringLiteral("A"), pageA);
    QMdiSubWindow *docB = area.addDocument(new QTextEdit, QStringLiteral("B"), pageB);
    QMdiSubWindow *docC = area.addDocument(new QTextEdit, QStringLiteral("C"));

    QCOMPARE(area.documentPage(docC), Q_NULLPTR);

    // B is up, then C - which carries no page - takes over: B's page goes
    // down and nothing comes up in its place.
    area.setActiveSubWindow(docB);
    flushEvents();
    QVERIFY(bar.isPageVisible(pageB));

    area.setActiveSubWindow(docC);
    flushEvents();
    QCOMPARE(area.activeSubWindow(), docC);
    QVERIFY(!bar.isPageVisible(pageA));
    QVERIFY(!bar.isPageVisible(pageB));
}

void tst_RibbonMdiArea::pageOnNoBarIsLeftAlone()
{
    // A page that was never added to a RibbonBar has no bar to talk to; the
    // area must leave it alone instead of dereferencing one.
    RibbonPage orphan;
    RibbonMdiArea area;
    QMdiSubWindow *doc = area.addDocument(new QTextEdit, QStringLiteral("A"), &orphan);

    QCOMPARE(area.documentPage(doc), &orphan);
    area.setActiveSubWindow(doc);
    flushEvents();

    // Closing must not crash either - the destroyed() handler looks the bar
    // up the same guarded way.
    doc->close();
    flushEvents();
    QVERIFY(area.activeSubWindow() == Q_NULLPTR);
}

TEST_ADD(tst_RibbonMdiArea)

#include "tst_ribbonmdiarea.moc"
