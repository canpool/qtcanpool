
#include <QApplication>
#include "ribbondockwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    RibbonDockWindow w;
    w.show();
    return app.exec();
}
