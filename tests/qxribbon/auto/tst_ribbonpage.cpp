#include "tst_global.h"

#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpagecontext.h"

QX_RIBBON_USE_NAMESPACE

class tst_RibbonPage : public QObject
{
    Q_OBJECT
private slots:
    void page();
    void group();
    void clear();
};

void tst_RibbonPage::page()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    QCOMPARE(tr("page"), page->pageName());
    page->setPageName(tr("egap"));
    QCOMPARE(tr("egap"), page->pageName());

    QCOMPARE(true, page->isCanCustomize());
    page->setCanCustomize(false);
    QCOMPARE(false, page->isCanCustomize());

    QCOMPARE(false, page->isPageContext());
    RibbonPageContext *pc = rb.addPageContext(tr("context"));
    RibbonPage *page2 = pc->addPage(tr("page2"));
    QCOMPARE(true, page2->isPageContext());
}

void tst_RibbonPage::group()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    RibbonGroup *g1 = page->addGroup(tr("group1"));
    QCOMPARE(0, page->groupIndex(g1));
    RibbonGroup *g2 = page->addGroup(tr("group2"));
    QCOMPARE(1, page->groupIndex(g2));
    RibbonGroup *g3 = new RibbonGroup(tr("group3"), page);
    page->addGroup(g3);
    QCOMPARE(2, page->groupIndex(g3));
    RibbonGroup *g4 = new RibbonGroup(page);
    page->addGroup(g4);
    QCOMPARE(3, page->groupIndex(g4));
    RibbonGroup *g5 = page->insertGroup(2, tr("group5"));
    QCOMPARE(2, page->groupIndex(g5));
    QCOMPARE(3, page->groupIndex(g3));

    QCOMPARE(g2, page->group(1));
    QCOMPARE(nullptr, page->group(-1));
    QCOMPARE(nullptr, page->group(10));

    QCOMPARE(g1, page->groupByName(tr("group1")));
    QCOMPARE(nullptr, page->groupByName(tr("xxx")));
    QCOMPARE(g4, page->groupByName(tr("")));   // g4 name is empty and is first

    QCOMPARE(g1, page->groupByObjectName(tr("group1")));
    QCOMPARE(nullptr, page->groupByObjectName(tr("xxx")));
    QCOMPARE(g3, page->groupByObjectName(tr("")));   // g3 and g4 object name are empty, but g3 is first

    QCOMPARE(5, page->groupCount());
    QCOMPARE(page->groupCount(), page->groupList().count());

    QString titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group5,group3,,", titles);

    // move
    page->moveGroup(2, 3);
    QCOMPARE(2, page->groupIndex(g3));
    QCOMPARE(3, page->groupIndex(g5));
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,group5,,", titles);

    page->moveGroup(3, 2);
    QCOMPARE(2, page->groupIndex(g5));
    QCOMPARE(3, page->groupIndex(g3));
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group5,group3,,", titles);

    // take
    QCOMPARE(false, page->takeGroup(nullptr));
    QCOMPARE(true, page->takeGroup(g5));
    QCOMPARE(tr("group5"), g5->groupName());   // not deleted
    QCOMPARE(-1, page->groupIndex(g5));
    QCOMPARE(2, page->groupIndex(g3));
    QCOMPARE(4, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,,", titles);

    delete g5;

    // remove
    QCOMPARE(true, page->removeGroup(g4));   // deleted
    g4 = nullptr;
    QCOMPARE(3, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,", titles);

    QCOMPARE(true, page->removeGroup(0));   // g1 deleted
    g1 = nullptr;
    QCOMPARE(0, page->groupIndex(g2));
    QCOMPARE(2, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group2,group3,", titles);

    QCOMPARE(false, page->removeGroup(-1));
    QCOMPARE(false, page->removeGroup(10));
}

void tst_RibbonPage::clear()
{
    RibbonBar rb;

    RibbonPage *p1 = rb.addPage(tr("page1"));
    RibbonGroup *g1 = p1->addGroup(tr("group1"));
    RibbonGroup *g2 = p1->addGroup(tr("group2"));

    RibbonPage *p2 = rb.addPage(tr("page2"));
    RibbonGroup *g3 = new RibbonGroup(tr("group3"), p2);
    p2->addGroup(g3);
    RibbonGroup *g4 = new RibbonGroup(p2);
    p2->addGroup(g4);

    p2->clearGroups();

    QCOMPARE(g3->groupName(), tr("group3")); // g3 deleteLater
    QCOMPARE(g4->groupName(), QString());

    rb.clearPages();

    // QCOMPARE(g1->groupName(), tr("group1")); // g1 is deleted when p1 is deleted
    // QCOMPARE(g3->groupName(), tr("group3")); // g3 is deleted when p2 is deleted

    Q_UNUSED(g1);
    Q_UNUSED(g2);

    p1 = nullptr;
    g1 = nullptr;
    g2 = nullptr;
    p2 = nullptr;
    g3 = nullptr;
    g4 = nullptr;
}

TEST_ADD(tst_RibbonPage)

#include "tst_ribbonpage.moc"
