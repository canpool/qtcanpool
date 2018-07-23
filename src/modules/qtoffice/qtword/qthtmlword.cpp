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
#include "qthtmlword.h"
#include <QFile>
#include <QTextStream>

static QString toHtmlFilter(QString text)
{
    text.replace("&","&amp;");
    text.replace(">","&gt;");
    text.replace("<","&lt;");
    text.replace("\"","&quot;");
    text.replace("\'","&#39;");
    text.replace(" ","&nbsp;");
    text.replace("\n","<br>");
    text.replace("\r","<br>");
    return text;
}

static QStringList paragraphFormat(HtmlText &text)
{
    QStringList info;
    info.append(QString("<p class=MsoNormal align=%1 style='text-align:%2'>").arg(text.align).arg(text.align));

    info.append(QString("<span style='font-size:%1pt;line-height:150%;font-family:%2'>%3</span>")
                .arg(text.font.pointSize()).arg(text.font.family()).arg(toHtmlFilter(text.text)));
    if(text.font.bold()){
        info.prepend("<b>");
        info.append("</b>");
    }
    if(text.isUnderline){
        info.prepend("<u>");
        info.append("</u>");
    }
    info.append("</p>\n");
    return info;
}

HtmlText::HtmlText()
{
    text = QString(" ");
}

TableCell::TableCell()
{
    width = 1;
    colspan = 1;
    rowspan = 1;
    valign = QString("top");
}

TableCell::~TableCell()
{

}

TableRow::TableRow()
{
    tableCells.clear();
}

TableRow::~TableRow()
{
    qDeleteAll(tableCells);
    tableCells.clear();
}

QtHtmlTable::QtHtmlTable(int rows)
{
    if(rows < 1){
        rows = 1;
    }
    m_tableRows.clear();
    for(int i = 0; i < rows; i++){
        TableRow *tableRow = new TableRow;
        m_tableRows.append(tableRow);
    }
    m_frontTexts.clear();
    m_backTexts.clear();

    m_textUnderline = false;
}

QtHtmlTable::~QtHtmlTable()
{
    qDeleteAll(m_tableRows);
    m_tableRows.clear();

    m_frontTexts.clear();
    m_backTexts.clear();
}

bool QtHtmlTable::insertCellText(int row, int colspan, const QString &text,
                                 ParagraphAlign align, CellVerticalAlign valign)
{
    return insertCellText(row, 1, colspan, text, align, valign);
}

bool QtHtmlTable::insertCellText(int row, int rowspan, int colspan, const QString &text, ParagraphAlign align, CellVerticalAlign valign)
{
    if(row < 0 || row >= m_tableRows.count()){
        return false;
    }
    TableCell *cell = new TableCell;
    cell->colspan = colspan;
    cell->rowspan = rowspan;
    if(!text.isEmpty()){
        cell->text.text = text;
    }
    cell->valign = cellVerticalAlignment(valign);
    cell->text.align = ParagraphAlignment(align);
    cell->text.font = m_font;
    cell->text.isUnderline = m_textUnderline;
    TableRow *tableRow = m_tableRows[row];
    tableRow->tableCells.append(cell);
    return true;
}

void QtHtmlTable::insertFrontText(const QString &text, ParagraphAlign align)
{
    HtmlText htmlText;
    htmlText.text = text;
    htmlText.font = m_font;
    htmlText.align = ParagraphAlignment(align);
    htmlText.isUnderline = m_textUnderline;
    m_frontTexts.append(htmlText);
}

void QtHtmlTable::insertBackText(const QString &text, ParagraphAlign align)
{
    HtmlText htmlText;
    htmlText.text = text;
    htmlText.font = m_font;
    htmlText.align = ParagraphAlignment(align);
    htmlText.isUnderline = m_textUnderline;
    m_backTexts.append(htmlText);
}

void QtHtmlTable::setTextFont(const QFont &font)
{
    m_font = font;
}

void QtHtmlTable::setTextFontSize(int size)
{
    if(size < 1) return;
    m_font.setPointSize(size);
}

void QtHtmlTable::setTextFontName(const QString &name)
{
    m_font.setFamily(name);
}

void QtHtmlTable::setTextFontBold(bool bold)
{
    m_font.setBold(bold);
}

void QtHtmlTable::setTextUnderline(bool underline)
{
    m_textUnderline = underline;
}

QString QtHtmlTable::ParagraphAlignment(ParagraphAlign align)
{
    QString alignment;
    switch (align) {
    case alignParagraphLeft:
        alignment = QString("left");
        break;
    case alignParagraphRight:
        alignment = QString("right");
        break;
    case alignParagraphCenter:
    default:
        alignment = QString("center");
        break;
    }
    return alignment;
}

QString QtHtmlTable::cellVerticalAlignment(CellVerticalAlign valign)
{
    QString alignment;
    switch (valign) {
    case cellAlignVerticalTop:
        alignment = QString("top");
        break;
    case cellAlignVerticalBottom:
        alignment = QString("bottom");
        break;
    case cellAlignVerticalCenter:
    default:
        alignment = QString("center");
        break;
    }
    return alignment;
}

QtHtmlWord::QtHtmlWord(QObject *parent)
    : QObject(parent)
{
    m_tables.clear();
    m_tableWidth = 639;
}

QtHtmlWord::~QtHtmlWord()
{
    qDeleteAll(m_tables);
    m_tables.clear();
}

void QtHtmlWord::saveAs(const QString &fileName)
{
    QStringList html;
    html.append("<html>\n");

    html.append("<head>\n");
    html.append(" <meta http-equiv=Content-Type content=\"text/html; charset=gb2312\">\n");
    html.append(" <meta name=Generator content=\"Microsoft Word 12 (filtered)\">\n");
    html.append("<style>\n");
    html.append("</style>\n");
    html.append("</head>\n");

    html.append("<body lang=ZH-CN style='text-justify-trim:punctuation'>\n");
    html.append("<div class=Section1 style='layout-grid:15.6pt'>\n");
    for(int i = 0; i < m_tables.count(); i++){
        QtHtmlTable *table = m_tables.at(i);
        // front text
        for(int m = 0; m < table->m_frontTexts.count(); m++){
            HtmlText &text = table->m_frontTexts[m];
            html.append(paragraphFormat(text));
        }
        // table
        html.append(QString("<table class=MsoNormalTable border=1 cellspacing=0 cellpadding=0 width=%1 \
                    style='width:479.0pt;border-collapse:collapse;border:none'>").arg(m_tableWidth));
        for(int j = 0; j < table->m_tableRows.count(); j++){
            TableRow *row = table->m_tableRows.at(j);
            html.append("<tr>\n");
            for(int k = 0; k < row->tableCells.count(); k++){
                TableCell *cell = row->tableCells.at(k);
                html.append(QString("<td rowspan=%1 colspan=%2 valign=%3>\n").arg(cell->rowspan).arg(cell->colspan).arg(cell->valign));
                html.append(paragraphFormat(cell->text));
                html.append("</td>\n");
            }
            html.append("</tr>\n");
        }
        html.append("</table>\n");
        // back text
        for(int m = 0; m < table->m_backTexts.count(); m++){
            HtmlText &text = table->m_backTexts[m];
            html.append(paragraphFormat(text));
        }
    }
    html.append("</div>\n");
    html.append("</body>\n");
    html.append("</html>\n");

    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QIODevice::Text)) {
        QTextStream text(&file);
        text.setCodec("gb2312");
        text << html.join("");
    }

}

QtHtmlTable *QtHtmlWord::insertTable(int rows)
{
    QtHtmlTable *table = new QtHtmlTable(rows);
    m_tables.append(table);
    return table;
}

QtHtmlTable *QtHtmlWord::table(int index)
{
    if(index < 0 || index >= m_tables.count())
        return NULL;
    QtHtmlTable *table = m_tables.at(index);
    return table;
}

void QtHtmlWord::setTableWidth(int width)
{
    if(width < 1){
        m_tableWidth = 639;
    }else{
        m_tableWidth = width;
    }
}

