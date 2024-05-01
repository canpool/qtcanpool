#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new RibbonStyle);
    app.setApplicationName("RibbonGroupScrolling");
    app.setOrganizationName("qtcanpool");

    MainWindow w;
    w.show();

    return app.exec();
}
