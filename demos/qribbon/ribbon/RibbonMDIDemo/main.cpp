#include "mainwindow.h"
#include "qribbon/ribbonstyle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new QRibbon::RibbonStyle());
    app.setApplicationName("RibbonMDIDemo");
    app.setOrganizationName("qtcanpool");

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
