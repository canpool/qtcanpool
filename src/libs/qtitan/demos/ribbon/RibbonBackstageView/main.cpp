#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );

    RibbonStyle* style = new RibbonStyle;
    style->setTheme(OfficeStyle::Office2013White);
    app.setStyle(style);

    app.setApplicationName("QtitanRibbon Controls Sample");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.show();
    return app.exec();
}
