#include "tst_global.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockcontainer.h"

#include <QLabel>
#include <QDebug>

QX_DOCK_USE_NAMESPACE

class tst_DockWindow : public QObject
{
    Q_OBJECT
private slots:
    void addDockWidget();
};

void tst_DockWindow::addDockWidget()
{
    DockWindow window;

    DockWidget *w = new DockWidget("Label 1");
    QLabel *l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("I am happy to join with you today in what will go down in history as the greatest demonstration "
               "for freedom in the history of our nation.");
    w->setWidget(l);
    DockPanel *centerPanel = window.addDockWidget(Qx::CenterDockWidgetArea, w);

    QCOMPARE(centerPanel, w->dockPanel());
    QCOMPARE(centerPanel->dockWidgetsCount(), 1);
    QCOMPARE(centerPanel->openDockWidgetsCount(), 1);
    QCOMPARE(centerPanel->currentDockWidget(), w);
    QCOMPARE(centerPanel->currentDockWidget(), centerPanel->dockWidget(centerPanel->currentIndex()));

    DockContainer *container = centerPanel->dockContainer();
    QCOMPARE(container, w->dockContainer());
    QCOMPARE(container->dockPanelCount(), 1);

    DockPanel *centerPanel_2 = window.addDockWidget(Qx::CenterDockWidgetArea, w);
    QVERIFY(centerPanel_2 != centerPanel);
    QCOMPARE(centerPanel->dockWidgetsCount(), 0);   // w was removed from centerPanel
    QVERIFY(centerPanel != w->dockPanel());
    QCOMPARE(centerPanel->dockContainer(), nullptr);   // centerPanel was removed from it's container
    QCOMPARE(centerPanel_2, w->dockPanel());
    QCOMPARE(centerPanel_2->currentDockWidget(), w);

    // WARNING: centerPanel called deleteLater but not deleted.
    // The centerPanel can no longer be used and may become a wild pointer

    DockWidget *w2 = new DockWidget("empty");
    DockPanel *centerPanel_3 = window.addDockWidget(Qx::CenterDockWidgetArea, w2, centerPanel);
    QVERIFY(centerPanel_3 != centerPanel);
    QCOMPARE(centerPanel->dockContainer(), nullptr);
    QCOMPARE(window.dockWidgetsMap().count(), 2);
    QCOMPARE(window.dockWidgetsMap().value("Label 1"), w);

    DockPanel *topPanel = window.addDockWidget(Qx::TopDockWidgetArea, w2, centerPanel);
    QCOMPARE(centerPanel_3->dockWidgetsCount(), 0); // w2 was removed from centerPanel_3
    QVERIFY(topPanel != centerPanel);
    QVERIFY(topPanel != centerPanel_3);
    QCOMPARE(topPanel->currentDockWidget(), w2);
}

TEST_ADD(tst_DockWindow)

#include "tst_dockwindow.moc"
