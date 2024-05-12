#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanRibbon design-time creation");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.show();
    return app.exec();
}
