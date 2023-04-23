#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("fancydemo");
    a.setOrganizationName("qtcanpool");
    MainWindow w;
    w.show();
    return a.exec();
}
