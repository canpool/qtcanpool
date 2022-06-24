
#include <QApplication>
#include "ribbonwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    RibbonWindow w;
    w.show();
    return app.exec();
}
