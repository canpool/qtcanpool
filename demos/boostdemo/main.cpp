#include <QApplication>
#include <QDebug>
#include <boost/timer.hpp>

using namespace boost;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    timer t;
    qDebug() << "max timespan:" << t.elapsed_max()/3600 ;
    qDebug() << "min timespan:" << t.elapsed_min();
    qDebug() << "now time elapsed:" << t.elapsed_min();

    return a.exec();
}
