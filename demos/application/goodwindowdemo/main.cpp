#include "goodmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("goodwindowdemo");
    a.setOrganizationName("qtcanpool");
    GoodMainWindow w;
    w.show();
    return a.exec();
}
