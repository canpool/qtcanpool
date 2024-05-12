#include <QApplication>
#include <QtnRibbonStyle.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setStyle(new Qtitan::RibbonStyle());
    app.setApplicationName("QtitanRibbon Galleries Sample");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.show();
    return app.exec();
}
