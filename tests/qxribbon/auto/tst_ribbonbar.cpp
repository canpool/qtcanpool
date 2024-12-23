#include "tst_global.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonpagecontext.h"

#include <QAbstractButton>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonBar : public QObject
{
    Q_OBJECT
private slots:
    void applicationButton();
    void page();
    void pageContext();
    void clear();
    void style();
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
    QCOMPARE(-1, rb.pageIndex(page2)); // page2 unusable
    QCOMPARE(-1, rb.pageIndex(page3)); // page3 unusable
    QCOMPARE(-1, rb.pageIndex(page4)); // page4 unusable
    QCOMPARE(-1, rb.pageIndex(page5)); // page5 unusable

    rb.showPageContext(pc1);
    QCOMPARE(1, rb.pageIndex(page2)); // page2 usable
    QCOMPARE(true, rb.isPageContextVisible(pc1));

    rb.setPageContextVisible(pc2, true);
    QCOMPARE(3, rb.pageIndex(page4)); // page4 usable
    QCOMPARE(true, rb.isPageContextVisible(pc2));

    rb.hidePageContext(pc1);
    QCOMPARE(-1, rb.pageIndex(page2)); // page2 unusable
    QCOMPARE(1, rb.pageIndex(page4));  // move forward
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

    QCOMPARE(RibbonBar::OfficeStyle, rb.currentRibbonStyle());
    QCOMPARE(true, rb.isOfficeStyle());
    QCOMPARE(false, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::OfficeStyleTwoRow);

    QCOMPARE(RibbonBar::OfficeStyleTwoRow, rb.currentRibbonStyle());
    QCOMPARE(true, rb.isOfficeStyle());
    QCOMPARE(true, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::WpsLiteStyle);

    QCOMPARE(RibbonBar::WpsLiteStyle, rb.currentRibbonStyle());
    QCOMPARE(false, rb.isOfficeStyle());
    QCOMPARE(false, rb.isTwoRowStyle());

    rb.setRibbonStyle(RibbonBar::WpsLiteStyleTwoRow);

    QCOMPARE(RibbonBar::WpsLiteStyleTwoRow, rb.currentRibbonStyle());
    QCOMPARE(false, rb.isOfficeStyle());
    QCOMPARE(true, rb.isTwoRowStyle());
}

TEST_ADD(tst_RibbonBar)

#include "tst_ribbonbar.moc"
