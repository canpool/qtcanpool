#include <QApplication>
#include "piecharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Pie Charts Demo");
    app.setOrganizationName("Developer Machines");

    PieCharts widget;
    widget.show();

    return app.exec();
}
