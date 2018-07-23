#include <QtGui>
#include <QTextCodec>
#include "cliccreatedlg.h"
#include <QTranslator>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
	QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
#endif
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

	QTranslator* appTranslator=new QTranslator();
	QTranslator* basicTranslator=new QTranslator();
	QString transdir = QCoreApplication::applicationDirPath();
	QString language = QString(QLocale::system().name());

	if ( language.length() < 2 )
		language = "en";
	
	if (appTranslator->load(QString("liccreator_")+language,transdir))
    {
		qApp->installTranslator(appTranslator);
    }
	if (basicTranslator->load(QString("qt_")+language,transdir))
    {
		qApp->installTranslator(basicTranslator);
    }
	
//	CLicCreateDlg dlg(0,Qt::Dialog|Qt::CustomizeWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint);
    CLicCreateDlg dlg(0);
	dlg.show();

    return app.exec();
}
 
