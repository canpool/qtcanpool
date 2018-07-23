#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(500,400);
    w.setMinimumSize(300,200);
    QPushButton button(&w);
    button.setGeometry(200,200,60,20);
    button.setText("Click Me");
    a.connect(&button,SIGNAL(clicked()),&a,SLOT(quit()));
    w.show();

    return a.exec();
}
