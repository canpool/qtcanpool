#include <QtWidgets/QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#ifdef QX_MATERIAL_LIBRARY_STATIC
    Q_INIT_RESOURCE(qxmaterial);
#endif // QX_MATERIAL_LIBRARY_STATIC

    MainWindow window;
    window.show();

    return a.exec();
}
