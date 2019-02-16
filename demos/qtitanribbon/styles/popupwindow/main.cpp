#include <QtGui>

#include "popupwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PopupDemoDialog dialog;
    return dialog.exec();
}
