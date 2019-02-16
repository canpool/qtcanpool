#include <QApplication>

#include <QtnRibbonStyle.h>

#include "widgets.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Widgets");
    app.setOrganizationName("Qtitan");
    Widgets *mainWin = new Widgets;
    mainWin->show();
    return app.exec();
}
