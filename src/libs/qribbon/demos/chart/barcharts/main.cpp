#include <QApplication>
#include "barcharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Bar Charts Demo");
    app.setOrganizationName("Developer Machines");

    BarCharts widget;
    widget.show();

    return app.exec();
}
