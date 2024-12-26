#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    DockWindow *wd = new DockWindow(this);
    setCentralWidget(wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");
    DockWidget *dw5 = new DockWidget("dw5");
    DockWidget *dw6 = new DockWidget("dw6");
    DockWidget *dw7 = new DockWidget("dw7");
    DockWidget *dw8 = new DockWidget("dw8");
    DockWidget *dw9 = new DockWidget("dw9");
    DockWidget *dw10 = new DockWidget("dw10");
    DockWidget *dw11 = new DockWidget("dw11");
    DockWidget *dw12 = new DockWidget("dw12");
    DockWidget *dw13 = new DockWidget("dw13");
    DockWidget *dw14 = new DockWidget("dw14");
    DockWidget *dw15 = new DockWidget("dw15");
    DockWidget *dw16 = new DockWidget("dw16");

    /**
     *      -----------------------------
     *      |          p1(dw7)          |
     *      -----------------------------
     *      |          p4(dw4)          |
     *      -----------------------------
     *      |p2(dw2)|  p1(dw1)  |p3(dw3)|
     *      -----------------------------
     *      |          p5(dw5)          |
     *      -----------------------------
     *      |          p6(dw6)          |
     *      -----------------------------
     */
    DockPanel *p1 = wd->addDockWidget(Qx::NoDockWidgetArea, dw1);       // same as TopDockWidgetArea
    DockPanel *p2 = wd->addDockWidget(Qx::LeftDockWidgetArea, dw2);
    DockPanel *p3 = wd->addDockWidget(Qx::RightDockWidgetArea, dw3);
    DockPanel *p4 = wd->addDockWidget(Qx::TopDockWidgetArea, dw4);
    DockPanel *p5 = wd->addDockWidget(Qx::BottomDockWidgetArea, dw5);
    DockPanel *p6 = wd->addDockWidget(Qx::CenterDockWidgetArea, dw6);   // same as BottomDockWidgetArea
    DockPanel *p7 = wd->addDockWidget(Qx::LeftAutoHideArea, dw7);       // same as TopDockWidgetArea

    /**
     * p1:
     *      --------------------------------------
     *      |   |   |    |       dw12       |    |
     *      |   |   |    --------------------    |
     *      |dw8|dw9|dw10|dw16|dw1|dw14|dw15|dw11|
     *      |   |   |    --------------------    |
     *      |   |   |    |       dw13       |    |
     *      --------------------------------------
     */
    DockPanel *p8 = wd->addDockWidget(Qx::LeftDockWidgetArea, dw8, p1);
    DockPanel *p9 = wd->addDockWidget(Qx::LeftDockWidgetArea, dw9, p1);
    DockPanel *p10 = wd->addDockWidget(Qx::LeftDockWidgetArea, dw10, p1, 0);
    DockPanel *p11 = wd->addDockWidget(Qx::RightDockWidgetArea, dw11, p1);
    DockPanel *p12 = wd->addDockWidget(Qx::TopDockWidgetArea, dw12, p1);
    DockPanel *p13 = wd->addDockWidget(Qx::BottomDockWidgetArea, dw13, p1);
    DockPanel *p14 = wd->addDockWidget(Qx::CenterDockWidgetArea, dw14, p1);
    DockPanel *p15 = wd->addDockWidget(Qx::CenterDockWidgetArea, dw15, p1);
    DockPanel *p16 = wd->addDockWidget(Qx::CenterDockWidgetArea, dw16, p1, 0);

    Q_UNUSED(p2);
    Q_UNUSED(p3);
    Q_UNUSED(p4);
    Q_UNUSED(p5);
    Q_UNUSED(p6);
    Q_UNUSED(p7);
    Q_UNUSED(p8);
    Q_UNUSED(p9);
    Q_UNUSED(p10);
    Q_UNUSED(p11);
    Q_UNUSED(p12);
    Q_UNUSED(p13);
    Q_UNUSED(p14);
    Q_UNUSED(p15);
    Q_UNUSED(p16);
}

MainWindow::~MainWindow()
{
}
