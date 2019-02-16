#include <QApplication>
#include "areacharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Area Charts Demo");
    app.setOrganizationName("Developer Machines");

    AreaCharts widget;
    widget.show();

    return app.exec();
}
