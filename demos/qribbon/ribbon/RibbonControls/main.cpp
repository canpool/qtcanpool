#include "mainwindow.h"
#include "qribbon/ribbonstyle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RibbonStyle *ribbonStyle = new RibbonStyle;
    ribbonStyle->setTheme(OfficeStyle::OfficeWhite);
    app.setStyle(ribbonStyle);

    app.setApplicationName("RibbonControls");
    app.setOrganizationName("qtcanpool");

    MainWindow w;
    w.show();

    return app.exec();
}
