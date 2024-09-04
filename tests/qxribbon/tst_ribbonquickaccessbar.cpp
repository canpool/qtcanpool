#include "tst_ribbonquickaccessbar.h"

#include "qxribbon/ribbonquickaccessbar.h"

QX_RIBBON_USE_NAMESPACE

void tst_RibbonQuickAccessBar::visibleCount()
{
    RibbonQuickAccessBar qab;
    QVERIFY(qab.visibleCount() == 0);
}

void tst_RibbonQuickAccessBar::addAction()
{
    RibbonQuickAccessBar qab;

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
