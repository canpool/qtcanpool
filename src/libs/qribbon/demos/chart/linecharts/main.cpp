#include <QApplication>
#include "linecharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Line Charts Demo");
    app.setOrganizationName("Developer Machines");

    LineCharts widget;
    widget.show();
    return app.exec();
}
