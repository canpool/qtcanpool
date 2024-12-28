#include "tst_global.h"

#include "qxdock/docktab.h"
#include "qxdock/dockwidget.h"

QX_DOCK_USE_NAMESPACE

class tst_DockTab : public QObject
{
    Q_OBJECT
private slots:
    void tab();
    void active();
};

void tst_DockTab::tab()
{
    DockWidget dw("dw");
    DockTab tab(&dw);

    QCOMPARE(tab.dockWidget(), &dw);
    QCOMPARE(tab.dockPanel(), nullptr);
}

void tst_DockTab::active()
{
    DockWidget dw("dw");
    DockTab tab(&dw);

    int i = 0;
    connect(&tab, &DockTab::activeTabChanged, this, [&i](){
        ++i;
    });

    QCOMPARE(tab.isActive(), false);

    tab.setActive(true);
    QCOMPARE(tab.isActive(), true);
    QCOMPARE(i, 1);
}

TEST_ADD(tst_DockTab)

#include "tst_docktab.moc"
