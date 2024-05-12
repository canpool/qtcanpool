#include "qsseditor/qsswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("QssEditor");
    QCoreApplication::setOrganizationName("QssEditor");

    QssWindow w;
    w.show();

    return app.exec();
}