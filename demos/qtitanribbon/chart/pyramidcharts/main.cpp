#include <QApplication>

#include "pyramidcharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart Pyramid Charts 2D/3D Demo");
    app.setOrganizationName("Developer Machines");

    PyramidCharts widget;
    widget.show();
    return app.exec();
}
