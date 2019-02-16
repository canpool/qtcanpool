#include <QApplication>
#include "scattercharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Scatter Charts Demo");
    app.setOrganizationName("Developer Machines");

    ScatterCharts widget;
    widget.show();
    return app.exec();
}
