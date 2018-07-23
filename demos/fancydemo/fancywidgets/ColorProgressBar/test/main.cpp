#include "colorprogressbartest.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ColorProgressBarTest w;
    w.show();

    return a.exec();
}
