#include "tst_global.h"

#include "qxribbon/ribbonquickaccessbar.h"

QX_RIBBON_USE_NAMESPACE

class tst_RibbonQuickAccessBar : public QObject
{
    Q_OBJECT
private slots:
    void visibleCount();
    void addAction();
};

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

        QCOMPARE(qab.actions().count(), 1);   // actionCustomizeButton
        qab.addAction(&action);
        QCOMPARE(qab.actions().count(), 2);
        QCOMPARE(qab.actions()[0], &action);
        QCOMPARE(qab.visibleCount(), 1);

        qab.clear();
        QCOMPARE(qab.actions().count(), 1);
        QCOMPARE(qab.visibleCount(), 0);
    }
}

TEST_ADD(tst_RibbonQuickAccessBar)

#include "tst_ribbonquickaccessbar.moc"
