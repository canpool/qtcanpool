#include <QApplication>

#include "QtnRibbonStyle.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(new Qtitan::RibbonStyle());
    app.setApplicationName("QtitanRibbon MDI Sample");
    app.setOrganizationName("Developer Machines");

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
