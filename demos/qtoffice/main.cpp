#include <QApplication>
#include <QTextCodec>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextDocumentWriter>

#include "qtword.h"
#include "qtexcel.h"
#include "qtxmlexcel.h"
#include "qthtmlword.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

    // excel test
#if 0
    // base on QtXmlExcel
    QString fileName = QFileDialog::getSaveFileName(0,QObject::tr("save"),QString("demo"),QObject::tr("Microsoft Excel(*.xls *.xlsx)"));
    QList<Worksheet> worksheets;
    for(int i = 0; i < 3; i++){
        Worksheet sheet;
        sheet.sheetName = QString("Sheet%1").arg(i+1);
        sheet.titleName = QString("Person Information");
        sheet.headers<<"no"<<"name"<<"age"<<"birthday";
        sheet.columnsWidth<<20<<100<<40<<150;
        sheet.contents.append("1;lilei;15;2002-11-12");
        sheet.contents.append("2;hanmeimei;14;2003-05-18");
        sheet.contents.append("3;tom;13;2004-08-08");
        sheet.contents.append("4;jack;18;1999-09-09");
        worksheets.append(sheet);
    }
    QtXmlExcel::instance()->toExcel(fileName,worksheets);
    QtXmlExcel::close();
    qDebug()<<"QtXmlExcel demo end!";

#endif

#if 1
    // base on QtExcel
    QString appDir = QApplication::applicationDirPath();
    QString sourceXls = QString("%1/../share/qtproject/template/source").arg(appDir);
    sourceXls = QDir::cleanPath(sourceXls);
    QString newXls = QFileDialog::getSaveFileName(0,QObject::tr("save"),QString("1"),QObject::tr("Microsoft Excel(*.xls *.xlsx)"));
    QString suffix = QFileInfo(newXls).suffix().toLower();
    sourceXls = QString("%1.%2").arg(sourceXls).arg(suffix);
    if(QFile::exists(newXls)){
        QFile::remove(newXls);
    }
    if(!QFile::copy(sourceXls, newXls)){
        qDebug()<<"copy error!";
        return -1;
    }
    QString fileName = newXls;
    QtExcel excel;
    if(!excel.open(fileName, false)){
        qDebug()<<"open file error!";
        return -1;
    }
    excel.prependSheet("demo1");
//    excel.selectSheet("demo1");
    excel.insertSheet("demo1", "demo2");
    excel.prependSheet("demo3");
    excel.appendSheet("demo4");
    excel.deleteSheet("Sheet1");
    qDebug()<<excel.currentSheetName();
    qDebug()<<"SheetCount:"<<excel.sheetsCount();

    excel.selectSheet(1);
    excel.setCellText(1,1, "haha");
    excel.setCellText("B2", "heihei");
    excel.mergeCells("A4:B5");
    excel.setCellTextColor(1,1,QColor(255,0,0));
    excel.setCellBackColor(1,1, QColor(255, 255,0));
    excel.setCellFontBold("B2", true);
    excel.setCellFontSize("B2", 15);
    excel.setCellTextHAlignment("B2", Excel::xlHAlignCenter);
    excel.setCellTextVAlignment("B2", Excel::xlVAlignCenter);
    excel.setCellBorderColor("A4:B5", QColor(255, 0, 255));

    excel.setRowHeight(8, 30);
    excel.setColumnWidth(8, 100);
    excel.setCellText(6,8,"autoFit");
    excel.setColumnAutoFit(8);

    excel.setOutsideLineColor("D5:F8", QColor(255, 0, 0));
    excel.setOutsideLineStyle("D5:F8", Excel::xlDouble);
    excel.setOutsideLineWeight("D5:F8", Excel::xlThick);
    excel.setInsideLineColor("D5:F8", QColor(0, 255, 0));
    excel.setInsideLineStyle("D5:F8", Excel::xlDash);
    excel.setInsideLineWeight("D5:F8", Excel::xlMedium);

    excel.setRowHeight(10, 40);
    excel.setCellPicture("B10", QDir::toNativeSeparators("D:/products/qtcanpool/qtcanpool/demos/qtoffice/resource/qrcode.png"), 32,32);
//    qDebug()<<QDir::toNativeSeparators(":/main/qrcode");

    excel.save();
    excel.close();
    qDebug()<<"excel demo end";

#endif

    // word test
#if 0
    // base on QtHtmlWord
    QString fileName = QFileDialog::getSaveFileName(0,QObject::tr("save"),QString("test"),QObject::tr("Microsoft word(*.doc *.html)"));
    QtHtmlWord word;
    QtHtmlTable *table = word.insertTable(11);
    table->setTextFontSize(9);
    table->insertCellText(0, 3, QString::fromUtf8("检验设备及保护名称"));
    table->insertCellText(0, 6, QString::fromUtf8("220kV北永一线保护智能组件柜(No.26)220kV北永一线合并单元A "));

    table->insertCellText(1, 2, QString::fromUtf8("工作负责人"));
    table->insertCellText(1, 1, QString::fromUtf8(""));
    table->insertCellText(1, 2, QString::fromUtf8("工作时间"));
    table->insertCellText(1, 1, QString::fromUtf8("   年   月   日"));
    table->insertCellText(1, 1, QString::fromUtf8("签发人"));
    table->insertCellText(1, 2, QString::fromUtf8(""));

    table->insertCellText(2, 9, QString::fromUtf8("工作内容: 220kV北永一线合并单元A(不停电) 消缺"));

    table->insertCellText(3, 2, QString::fromUtf8("工作条件"));
    table->insertCellText(3, 7, QString::fromUtf8("1. 一次设备运行情况：\n"
                                                  "2.检验保护作用的断路器：\n"
                                                  "3.检验保护屏上的运行设备:\n"
                                                  "4.检验保护屏、端子箱与其他保护联接线："), alignParagraphLeft);

    table->insertCellText(4, 9, QString::fromUtf8("技术安全措施：包括应打开及恢复压板、直流线、交流线、信号线、连锁线和联锁开关等，按下列顺序做安全措施。"
                                                  "已执行，在执行栏打“√”按相反的顺序恢复安全措施，以恢复的，在恢复栏打“√”。"));

    table->insertCellText(5, 1, QString::fromUtf8("序号"));
    table->insertCellText(5, 1, QString::fromUtf8("执行"));
    table->insertCellText(5, 2, QString::fromUtf8("回路类别"));
    table->insertCellText(5, 4, QString::fromUtf8("安  全  措  施  内  容"));
    table->insertCellText(5, 1, QString::fromUtf8("恢复"));

    table->insertCellText(6, 1, QString::fromUtf8("1"));
    table->insertCellText(6, 1, QString::fromUtf8(""));
    table->insertCellText(6, 2, QString::fromUtf8("记录交接状态"));
    table->insertCellText(6, 4, QString::fromUtf8("在220kV北永一线路保护屏A(No.7)220kV北永一线路保护A“处查看确认闭重GOOSE发送软压板”已退出"));
    table->insertCellText(6, 1, QString::fromUtf8(""));

//    for(int i = 7; i <30; i++){
//    table->insertCellText(i, 1, QString::fromUtf8("1"));
//    table->insertCellText(i, 1, QString::fromUtf8(""));
//    table->insertCellText(i, 2, QString::fromUtf8("记录交接状态"));
//    table->insertCellText(i, 4, QString::fromUtf8("在220kV北永一线路保护屏A(No.7)220kV北永一线路保护A“处查看确认闭重GOOSE发送软压板”已退出"));
//    table->insertCellText(i, 1, QString::fromUtf8(""));
//    }

    table->insertCellText(7, 3, 2, QString::fromUtf8("测试行合并"));
    table->insertCellText(7, 7, QString::fromUtf8("第一行"));
    table->insertCellText(8, 7, QString::fromUtf8("第二行"));
    table->insertCellText(9, 7, QString::fromUtf8("第三行"));
    table->insertCellText(10, 2, QString::fromUtf8("测试行合并结束"));
    table->insertCellText(10, 7, QString::fromUtf8("第四行"));

    table->setTextFontSize(20);
    table->setTextFontBold(true);
    table->insertFrontText(QString::fromUtf8("现场工作安全技术措施票"));
    table->setTextFontSize(12);
    table->setTextFontBold(false);
    table->setTextUnderline(true);
    table->insertFrontText(QString::fromUtf8("南京南瑞继保电气有限公司"), alignParagraphLeft);
    table->setTextUnderline(false);
    table->setTextFontBold(true);
    table->insertBackText(QString::fromUtf8("执行人_______ 监护人_______ 恢复人_______ 监护人_______ "));
    word.saveAs(fileName);
    qDebug()<<"QtHtmlWord demo end!";
#endif


#if 0
    // base on QtWord
    QtWord word;
    QString fileName = QFileDialog::getSaveFileName(0,QObject::tr("save"),QString("1"),QObject::tr("Microsoft Word(*.doc *.docx)"));
    if(!word.open()){
        return -1;
    }
    qDebug()<<word.version();
//        word.setPageOrientation(Word::wdOrientLandscape);
    word.setTextFontSize(25);
    word.setTextFontBold(true);
    word.setTextColor(QColor(255,0,0));
//        word.setTextBackColor(Word::wdBrightGreen);
    word.setTextBackColor(QColor(0,0,255));
    word.setTextAlignment(Word::wdAlignParagraphCenter);
    word.insertText(QObject::tr("word demo application"));
    word.insertNewline();
    word.setTextColor(QColor(0,255,0));
    word.setTextBackColor(Word::wdAuto);
//        word.setTextColor(Word::wdColorBlue);
    word.setTextAlignment(Word::wdAlignParagraphRight);
    word.setTextFontSize(14);
    word.insertText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh::mm::ss"));
    word.insertNewline();
    word.setTextColor(Word::wdColorBlack);
    word.setTextAlignment(Word::wdAlignParagraphLeft);
    QtWordTable wordtable = word.insertTable(5,5);
//        wordtable.mergeCell(1,1,1,2);
    word.moveForEnd();
    word.insertNewline();
    wordtable = word.insertTable(4,4);

    // table 1
    wordtable = word.table(1);
    wordtable.mergeCell(1,1,1,2);
    wordtable.insertCellText(1, 1, "hahahaha");
    wordtable.insertCellText(1, 3, "hahahaha");
    wordtable.insertCellText(1, 4, "hahahaha");
    wordtable.insertCellText(2, 1, "hahahaha");
    wordtable.insertCellText(2, 2, "hahahaha");
    wordtable.insertCellText(2, 3, "hahahaha");
    wordtable.insertCellText(3, 1, "hahahaha");
    wordtable.insertCellText(4, 1, "hahahaha");
    wordtable.insertCellText(5, 1, "hahahaha");
    wordtable.insertCellText(5, 2, "hahahaha");
    wordtable.insertCellPicture(5, 3, "D:/products/fancydemo/doc/1.png");

    wordtable.setCellTextColor(1, 1, QColor(255, 0,0));
    wordtable.setCellTextColor(2, 1, QColor(255, 255,0));
    wordtable.setCellTextColor(3, 1, QColor(255, 0,255));

    wordtable.setCellTextColor(2, 2, QColor(255, 0, 0));
    wordtable.setCellTextBackColor(2, 2, QColor(0, 255, 0));
    wordtable.setCellBackColor(2, 2, QColor(255, 201, 14));

    wordtable.setRowTextColor(3, QColor(255, 128,0));
    wordtable.setRowTextBackColor(4, QColor(255, 0, 0));
    wordtable.setRowBackColor(3, QColor(0, 255, 255));

    // table 2
    wordtable = word.table(2);
    wordtable.setTableFont("Times");

    wordtable.insertCellText(1, 1, "heiheihei");
    wordtable.insertCellText(1, 3, "heiheihei");
    wordtable.insertCellText(1, 4, "heiheihei");
    wordtable.insertCellText(2, 1, "heiheihei");
    wordtable.insertCellText(2, 2, "heiheihei");
    wordtable.insertCellText(2, 3, "heiheihei");
    wordtable.insertCellText(3, 1, "heiheihei");
    wordtable.insertCellText(4, 1, "heiheihei");

    wordtable.setColTextColor(1, QColor(255, 0, 0));
    wordtable.setColTextBackColor(1, QColor(0, 255, 0));
    wordtable.setColBackColor(1, QColor(128, 0, 128));

//    wordtable.splitCell(3, 3, 2, 2);
    wordtable.splitCell(3, 2, 4, 4, 1, 2);

//    wordtable.setTableTextColor(QColor(255, 0, 0));
//    wordtable.setRowAlignment(2, Word::wdAlignRowCenter);
    wordtable.setTableAlignment(Word::wdAlignParagraphCenter);

//    wordtable.setRowAlignment(3, Word::wdAlignParagraphCenter);
//    wordtable.setCellParagraphAlignment(2,2,Word::wdAlignParagraphCenter);
//    wordtable.setCellVerticalAlignment(2,2, Word::wdCellAlignVerticalCenter);

//    wordtable.addRow(4, 2);
//    wordtable.addColumn(4, 1);
//    wordtable.appendRow(2);
//    wordtable.setAutoFitBehavior(Word::wdAutoFitContent);

    word.moveForEnd();
    word.insertNewline();

    // table 3
    wordtable = word.insertTable(3,3);
    wordtable.setOutsideLineStyle(Word::wdLineStyleDouble);
    wordtable.setOutsideColor(QColor(255, 0, 0));
    wordtable.setOutsideLineWidth(2);

    wordtable.setInsideLineStyle(Word::wdLineStyleDashDot);
    wordtable.setInsideColor(QColor(0, 0, 0));

    wordtable.insertCellText(1,1,"border");
    wordtable.insertCellText(1,2,"outside");
    wordtable.insertCellText(1,3,"inside");

    word.moveForEnd();
    word.insertNewline();

    // style
    word.setTextStyle(Word::wdStyleHeading1);
    word.insertText("Heading 1");
    word.insertNewline();
    word.setTextStyle(Word::wdStyleHeading2);
    word.insertText("Heading 2");
    word.insertNewline();
    word.setTextStyle(Word::wdStyleBodyText);
    word.insertText("Body text");

    // under line
    word.insertNewline();
    word.setTextUnderline(Word::wdUnderlineDouble);
    word.setTextUnderlineColor(QColor(255,0,0));
    word.insertText("Text Underline");
    word.insertNewline();
    word.setTextUnderline(Word::wdUnderlineNone);
    word.insertText("normal text");

    word.saveAs(fileName);
    word.close();
    qDebug()<<"word operation end!";
#endif

//    return a.exec();
}
