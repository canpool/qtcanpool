#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    RibbonStyle* ribbonStyle = new RibbonStyle;
    ribbonStyle->setTheme(OfficeStyle::Office2013White);
    app.setStyle(ribbonStyle);

    app.setApplicationName("QtitanRibbon Controls");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.show();

    return app.exec();
}
