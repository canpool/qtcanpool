#include <QApplication>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    RibbonStyle* ribbonStyle = new RibbonStyle;
    ribbonStyle->setTheme(OfficeStyle::Office2013White);
    app.setStyle( ribbonStyle );

    app.setApplicationName("QtitanRibbon Contextual Tabs");
    app.setOrganizationName("Developer Machines");

    MainWindow w;
    w.setWindowTitle(QObject::tr("QtitanRibbon Contextual Tabs"));
    w.show();
    return app.exec();
}
