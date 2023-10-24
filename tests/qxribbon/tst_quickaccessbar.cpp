#include "tst_quickaccessbar.h"

#include "qxribbon/quickaccessbar.h"

QX_RIBBON_USE_NAMESPACE

void TestQuickAccessBar::visibleCount()
{
    QuickAccessBar qab;
    QVERIFY(qab.visibleCount() == 0);
}

void TestQuickAccessBar::addAction()
{
    QuickAccessBar qab;

    {
        QAction action(0);

        QCOMPARE(qab.actions().count(), 1); // actionCustomizeButton
        qab.addAction(&action);
        QCOMPARE(qab.actions().count(), 2);
        QCOMPARE(qab.actions()[0], &action);
        QCOMPARE(qab.visibleCount(), 1);

        qab.clear();
        QCOMPARE(qab.actions().count(), 1);
        QCOMPARE(qab.visibleCount(), 0);
    }
}
