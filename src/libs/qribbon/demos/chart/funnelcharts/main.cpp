#include <QApplication>
#include "funnelcharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart Funnel Charts 2D/3D Demo");
    app.setOrganizationName("Developer Machines");

    FunnelCharts widget;
    widget.show();
    return app.exec();
}
