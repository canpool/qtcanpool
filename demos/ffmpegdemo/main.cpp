#include <QApplication>
#include <QDebug>

extern "C"
{
    #include <libavcodec/avcodec.h>
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug()<< avcodec_configuration();
    unsigned int version = avcodec_version();
    qDebug()<<"version:"<<version;

    return a.exec();
}
