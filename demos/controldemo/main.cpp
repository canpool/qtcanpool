#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFont font("Segoe UI", 10);
    QApplication::setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
