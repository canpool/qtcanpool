#include "tst_global.h"

#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

class tst_RibbonPageContext : public QObject
{
    Q_OBJECT
private slots:
    void page();
    void context();
    void signal();
};

void tst_RibbonPageContext::page()
{
    RibbonPageContext pc;

    RibbonPage *page1 = pc.addPage(tr("page1"));
    QCOMPARE(page1, pc.page(0));
    RibbonPage *page2 = pc.addPage(tr("page2"));
    QCOMPARE(page2, pc.page(1));
    RibbonPage *page3 = pc.addPage(tr("page3"));
    QCOMPARE(page3, pc.page(2));

    QCOMPARE(nullptr, pc.page(-1));
    QCOMPARE(nullptr, pc.page(10));

    QCOMPARE(3, pc.pageCount());
    QCOMPARE(pc.pageCount(), pc.pageList().count());

    QString titles = "";
    for (RibbonPage *page : pc.pageList()) {
        titles += page->pageName() + ",";
    }
    QCOMPARE("page1,page2,page3,", titles);

    QCOMPARE(false, pc.takePage(nullptr));
    QCOMPARE(true, pc.takePage(page2)); // not deleted
    QCOMPARE(tr("page2"), page2->pageName());
    QCOMPARE(page3, pc.page(1));
    QCOMPARE(nullptr, pc.page(2));
    titles = "";
    for (RibbonPage *page : pc.pageList()) {
        titles += page->pageName() + ",";
    }
    QCOMPARE("page1,page3,", titles);
}

void tst_RibbonPageContext::context()
{
    RibbonPageContext pc;
    QVERIFY(pc.contextTitle().isEmpty());
    pc.setContextTitle(tr("context"));
    QCOMPARE(tr("context"), pc.contextTitle());

    QVERIFY(!pc.contextColor().isValid());
    pc.setContextColor(Qt::red);
    QVERIFY(pc.contextColor().isValid());
    QCOMPARE(QColor(Qt::red), pc.contextColor());
}

void tst_RibbonPageContext::signal()
{
    RibbonPageContext pc;

    int i = 0;
    connect(&pc, &RibbonPageContext::pageAdded, this, [&i](RibbonPage *page) {
        ++i;
        QCOMPARE(tr("page1"), page->pageName());
    });
    RibbonPage *page1 = pc.addPage(tr("page1"));
    QCOMPARE(i, 1);

    int j = 0;
    connect(&pc, &RibbonPageContext::pageRemoved, this, [&j](RibbonPage *page) {
        ++j;
        QCOMPARE(tr("page1"), page->pageName());
    });
    pc.takePage(page1);
    QCOMPARE(j, 1);
}

TEST_ADD(tst_RibbonPageContext)

#include "tst_ribbonpagecontext.moc"
