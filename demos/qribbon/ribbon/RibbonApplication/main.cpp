#include "mainwindow.h"
#include "qribbon/ribbonstyle.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new RibbonStyle());
    app.setApplicationName("RibbonApplication");
    app.setOrganizationName("qtcanpool");
    MainWindow w;
    w.show();
    return app.exec();
}
