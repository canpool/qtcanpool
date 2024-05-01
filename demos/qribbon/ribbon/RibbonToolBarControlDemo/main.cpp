#include "mainwindow.h"
#include <QApplication>
#include <qribbon/ribbonstyle.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new QRibbon::RibbonStyle());
    app.setApplicationName("RibbonToolBarControlDemo");
    app.setOrganizationName("qtcanpool");

    MainWindow w;
    w.show();
    return app.exec();
}
