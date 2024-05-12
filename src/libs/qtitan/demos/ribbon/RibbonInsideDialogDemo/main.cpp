#include <QApplication>
#include <QtnRibbonStyle.h>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setStyle(new RibbonStyle);

    app.setApplicationName("QtitanRibbon Inside Dialog Sample");
    app.setOrganizationName("Developer Machines");

    MainDialog dialog;
    return dialog.exec();
}
