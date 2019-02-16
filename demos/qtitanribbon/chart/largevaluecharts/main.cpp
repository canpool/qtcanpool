#include <QApplication>
#include "largevaluecharts.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setApplicationName("QtitanChart - Large Value Charts Demo");
    app.setOrganizationName("Developer Machines");

    LargeValueCharts widget;
    widget.show();

    return app.exec();
}
