/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "qtxmlexcel.h"
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QColor>

static QString colorToArgb(const QColor &color)
{
    QString argb =  QString("%1%2%3%4")
            .arg(color.alpha(), 2, 16, QLatin1Char('0'))
            .arg(color.red(), 2, 16, QLatin1Char('0'))
            .arg(color.green(), 2, 16, QLatin1Char('0'))
            .arg(color.blue(), 2, 16, QLatin1Char('0'));
    return argb;
}

Worksheet::Worksheet()
{
    sheetName = QString("Sheet");
    titleName = QString("Title");
    headers.clear();
    columnsWidth.clear();
    contents.clear();
}

Worksheet::~Worksheet()
{

}

QtXmlExcel* QtXmlExcel::m_instance = 0;

QtXmlExcel::QtXmlExcel(QObject *parent)
    : QObject(parent)
{
    m_instance = this;
}

QtXmlExcel *QtXmlExcel::instance()
{
    if (!m_instance) {
        m_instance = new QtXmlExcel;
    }
    return m_instance;
}

void QtXmlExcel::close()
{
    if(m_instance){
        delete m_instance;
        m_instance = 0;
    }
}

void QtXmlExcel::toExcel(QString fileName, QList<Worksheet> worksheets)
{
    //逐个拼接xml字符,再保存为xls文件
    //清空原有数据,确保每次都是新的数据
    m_xml.clear();

    //固定头部信息
    m_xml.append("<?xml version=\"1.0\"?>\n");
    m_xml.append("<?mso-application progid=\"Excel.Sheet\"?>\n");
    m_xml.append("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    m_xml.append(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"\n");
    m_xml.append(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"\n");
    m_xml.append(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"\n");
    m_xml.append(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n");

    //文档信息
    m_xml.append(" <DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    m_xml.append(QString("  <LastAuthor>%1</LastAuthor>\n").arg("canpool"));
    m_xml.append(QString("  <Created>%1</Created>\n").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    m_xml.append("  <Version>11.6360</Version>\n");
    m_xml.append(" </DocumentProperties>\n");

    m_xml.append(" <OfficeDocumentSettings xmlns=\"urn:schemas-microsoft-com:office:office\">\n");
    m_xml.append("  <AllowPNG/>\n");
    m_xml.append(" </OfficeDocumentSettings>\n");

    m_xml.append(" <ExcelWorkbook xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
    m_xml.append("  <WindowHeight>9795</WindowHeight>\n");
    m_xml.append("  <WindowWidth>21435</WindowWidth>\n");
    m_xml.append("  <WindowTopX>120</WindowTopX>\n");
    m_xml.append("  <WindowTopY>75</WindowTopY>\n");
    m_xml.append("  <WindowTopY>75</WindowTopY>\n");
    m_xml.append("  <ProtectWindows>False</ProtectWindows>\n");
    m_xml.append(" </ExcelWorkbook>\n");

    //样式表
    m_xml.append(" <Styles>\n");
    m_xml.append("  <Style ss:ID=\"Default\" ss:Name=\"Normal\">\n");
    m_xml.append("   <Alignment ss:Vertical=\"Center\"/>\n");
    m_xml.append("   <Borders/>\n");
    m_xml.append("   <Font ss:FontName=\"Microsoft YaHei\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\"/>\n");
    m_xml.append("   <Interior/>\n");
    m_xml.append("   <NumberFormat/>\n");
    m_xml.append("   <Protection/>\n");
    m_xml.append("  </Style>\n");
    //标题样式
    m_xml.append("  <Style ss:ID=\"Title\">\n");
    m_xml.append("   <Alignment ss:Horizontal=\"Center\" ss:Vertical=\"Center\"/>\n");
    m_xml.append("   <Font ss:FontName=\"Microsoft YaHei\" x:CharSet=\"134\" ss:Size=\"14\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n");
    m_xml.append("  </Style>\n");
    //带边框样式
    m_xml.append("  <Style ss:ID=\"Header\">\n");
    m_xml.append("   <Font ss:FontName=\"Microsoft YaHei\" x:CharSet=\"134\" ss:Size=\"12\" ss:Color=\"#000000\" ss:Bold=\"1\"/>\n");
    m_xml.append("   <Interior ss:Color=\"#FCE4D6\" ss:Pattern=\"Solid\"/>\n");
    m_xml.append("  <Borders>\n");
    m_xml.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("  </Borders>\n");
    m_xml.append(" </Style>\n");

    m_xml.append("  <Style ss:ID=\"Body\">\n");
    m_xml.append("   <Font ss:FontName=\"Microsoft YaHei\" x:CharSet=\"134\" ss:Size=\"11\" ss:Color=\"#000000\"/>\n");
    m_xml.append("   <Interior/>\n");
    m_xml.append("  <Borders>\n");
    m_xml.append("   <Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Left\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Right\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("   <Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"1\"/>\n");
    m_xml.append("  </Borders>\n");
    m_xml.append(" </Style>\n");

    m_xml.append(" </Styles>\n");

    foreach (Worksheet sheet, worksheets) {
        //计算行数列数
        int columnCount = sheet.headers.count();
        int rowCount = sheet.contents.count();

        //工作表名称
        m_xml.append(QString(" <Worksheet ss:Name=\"%1\">\n").arg(sheet.sheetName));

        //表格开始
        m_xml.append(QString("  <Table ss:ExpandedColumnCount=\"%1\" ss:ExpandedRowCount=\"%2\" x:FullColumns=\"1\"\n")
                    .arg(columnCount).arg(rowCount + 2));
        m_xml.append("   x:FullRows=\"1\" ss:DefaultColumnWidth=\"54\" ss:DefaultRowHeight=\"18\">\n");

        //设置字段宽度
        for (int i = 0; i < columnCount; i++) {
            m_xml.append(QString("   <Column ss:AutoFitWidth=\"0\" ss:Width=\"%1\"/>\n")
                        .arg(QString::number(sheet.columnsWidth.at(i))));
        }

        //表格标题
        m_xml.append("   <Row ss:AutoFitHeight=\"0\" ss:Height=\"26.25\">\n");
        m_xml.append(QString("    <Cell ss:MergeAcross=\"%1\" ss:StyleID=\"Title\"><Data ss:Type=\"String\">%2</Data></Cell>\n")
                    .arg(columnCount - 1).arg(sheet.titleName));
        m_xml.append("   </Row>");

        //逐个添加字段名称
        m_xml.append("   <Row ss:AutoFitHeight=\"0\">\n");
        for (int i = 0; i < columnCount; i++) {
            m_xml.append(QString("    <Cell ss:StyleID=\"Header\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                        .arg(sheet.headers.at(i)));
        }
        m_xml.append("   </Row>\n");

        //逐个添加数据
        for (int i = 0; i < rowCount; i++) {
            m_xml.append("   <Row ss:AutoFitHeight=\"0\">\n");
            QString temp = sheet.contents.at(i);
            QStringList value = temp.split(";");
            foreach (QString str, value) {
                m_xml.append(QString("    <Cell ss:StyleID=\"Body\"><Data ss:Type=\"String\">%1</Data></Cell>\n")
                            .arg(str));
            }
            m_xml.append("   </Row>\n");
        }
        m_xml.append("  </Table>\n");
        //固定结尾格式
        m_xml.append("  <WorksheetOptions xmlns=\"urn:schemas-microsoft-com:office:excel\">\n");
        m_xml.append("   <PageSetup>\n");
        m_xml.append("    <Header x:Margin=\"0.3\"/>\n");
        m_xml.append("    <Footer x:Margin=\"0.3\"/>\n");
        m_xml.append("    <PageMargins x:Bottom=\"0.75\" x:Left=\"0.7\" x:Right=\"0.7\" x:Top=\"0.75\"/>\n");
        m_xml.append("   </PageSetup>\n");
        m_xml.append("   <Unsynced/>\n");
        m_xml.append("   <Selected/>\n");
        m_xml.append("   <ProtectObjects>False</ProtectObjects>\n");
        m_xml.append("   <ProtectScenarios>False</ProtectScenarios>\n");
        m_xml.append("  </WorksheetOptions>\n");
        m_xml.append(" </Worksheet>\n");
    }

    m_xml.append("</Workbook>\n");

    //写入文件
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text.setCodec("UTF-8");
        text << m_xml.join("");
    }
}


