/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonpage.h"

#include <QScopedPointer>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonPageContext : public QObject
{
    Q_OBJECT
private slots:
    void page();
    void builtPage();
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
    QCOMPARE(true, pc.takePage(page2));   // not deleted
    QCOMPARE(tr("page2"), page2->pageName());
    QCOMPARE(page3, pc.page(1));
    QCOMPARE(nullptr, pc.page(2));
    titles = "";
    for (RibbonPage *page : pc.pageList()) {
        titles += page->pageName() + ",";
    }
    QCOMPARE("page1,page3,", titles);
}

/*! addPage(RibbonPage *) is what lets a caller build the page - a subclass of its
 * own, say - and still have the context show it. */
void tst_RibbonPageContext::builtPage()
{
    RibbonPageContext pc;

    // The caller keeps owning the page, so this is the caller deleting it.
    QScopedPointer<RibbonPage> owned(new RibbonPage());
    owned->setPageName(tr("built"));
    pc.addPage(owned.data());
    QCOMPARE(1, pc.pageCount());
    QCOMPARE(owned.data(), pc.page(0));
    QCOMPARE(true, owned->isPageContext());
    QCOMPARE(tr("built"), pc.page(0)->pageName());

    // A null page is ignored rather than counted, as elsewhere in the ribbon.
    pc.addPage(nullptr);
    QCOMPARE(1, pc.pageCount());

    // The ribbon learns about pages through pageAdded, so the overload has to
    // announce them exactly as addPage(title) does.
    int added = 0;
    connect(&pc, &RibbonPageContext::pageAdded, this, [&added, &owned](RibbonPage *page) {
        ++added;
        QCOMPARE(owned.data(), page);
    });
    pc.addPage(owned.data());
    QCOMPARE(1, added);
    QCOMPARE(2, pc.pageCount());
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
