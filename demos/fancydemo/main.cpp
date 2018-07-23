#include "mainwindow.h"
#include "cmainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QSplashScreen>
#include <QFile>
#include <QStyleFactory>
//#include <QLocale>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
     QTextCodec::setCodecForCStrings(codec);
     QTextCodec::setCodecForTr(codec);
#endif

//    QString language = QString(":/qm/qt_zh_CN");
//    QTranslator translator;
//    translator.load(language);
//    qApp->installTranslator(&translator);

//    language = QString(":/qm/qt_zh_CN_%1").arg(QT_VERSION_STR);
//    QTranslator translator1;
//    translator1.load(language);
//    qApp->installTranslator(&translator1);


//    QStringList languages = Global::init();
//    QTranslator translator;
//    QTranslator qtTranslator;
//    if(languages.count() == 2){
//        qtTranslator.load(languages.at(0));
//        qApp->installTranslator(&qtTranslator);
//        translator.load(languages.at(1));
//        qApp->installTranslator(&translator);
//    }else{
//        translator.load(languages.at(0));
//        qApp->installTranslator(&translator);
//    }


#if 0
    QString theme = QString(":/qss/classic");
    QFile qssFile(theme);
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen()){
        QString qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }
#endif

    QApplication::processEvents(QEventLoop::AllEvents);

#if USE_LOG
//    qInstallMessageHandler(Global::customMessageHandler);
#endif

    QFont font("Segoe UI",10);
    QApplication::setFont(font);

//    QPalette palette;
//    palette.setColor(QPalette::WindowText,Qt::white);
//    a.setPalette(palette);

//    QFont font = QApplication::font();
//    font.setPointSize(12);
//    QApplication::setFont(font);
//    a.setStyleSheet("QCheckBox,QRadioButton{color:black;}");

//    qDebug()<<QLocale::system().name(); // zh_CN

    CMainWindow w;
//    MainWindow w;
    w.show();
//    w.checkLicense();

    return a.exec();
}
