#include <QApplication>

#include <QtnRibbonStyle.h>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("OfficeStyle Widgets");
    app.setOrganizationName("Developer Machines");
    Widgets *mainWin = new Widgets;
    mainWin->show();
    return app.exec();
}
