#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setStyle(new RibbonStyle);
    app.setApplicationName("QtitanRibbon Group Scrolling");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.show();

    return app.exec();
}
