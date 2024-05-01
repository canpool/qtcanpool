#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RibbonStyle *style = new RibbonStyle;
    style->setTheme(OfficeStyle::OfficeWhite);
    app.setStyle(style);

    app.setApplicationName("RibbonBackstageView");
    app.setOrganizationName("qtcanpool");

    MainWindow w;
    w.show();
    return app.exec();
}
