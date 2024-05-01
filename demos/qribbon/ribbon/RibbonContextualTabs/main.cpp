#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RibbonStyle *ribbonStyle = new RibbonStyle;
    ribbonStyle->setTheme(OfficeStyle::OfficeWhite);
    app.setStyle(ribbonStyle);

    app.setApplicationName("RibbonContextualTabs");
    app.setOrganizationName("qtcanpool");

    MainWindow w;
    w.setWindowTitle(QObject::tr("Ribbon Contextual Tabs"));
    w.show();
    return app.exec();
}
