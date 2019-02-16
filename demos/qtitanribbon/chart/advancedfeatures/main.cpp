#include <QApplication>
#include "advancedfeatures.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChat - Line Charts Demo");
    app.setOrganizationName("Developer Machines");

    AdvancedFeatures widget;
    widget.show();
    return app.exec();
}
