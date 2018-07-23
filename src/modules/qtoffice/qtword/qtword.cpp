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
#include "qtword.h"
#include <QDir>
#include "qt_windows.h"
#include <QDebug>

QtWordTable::QtWordTable(QAxObject *word, QAxObject *table)
{
    m_word = word;
    m_table = table;
}

QtWordTable::~QtWordTable()
{

}

QtWordTable &QtWordTable::operator =(const QtWordTable &other)
{
    if(this == &other)
        return *this;
    m_word = other.m_word;
    m_table = other.m_table;

    return *this;
}

bool QtWordTable::isValid()
{
    if(m_word == 0 || m_table == 0)
        return false;
    return true;
}

bool QtWordTable::mergeCell(int startRow, int startCol, int endRow, int endCol)
{
    QAxObject* startCell = m_table->querySubObject("Cell(int, int)",startRow,startCol);
    if(!startCell) return false;
    QAxObject* endCell = m_table->querySubObject("Cell(int, int)",endRow,endCol);
    if(!endCell) return false;
    startCell->dynamicCall("Merge(QAxObject *)",endCell->asVariant());
    return true;
}

bool QtWordTable::splitCell(int row, int col, int rows, int cols)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)", row, col);
    if(!cellObj) return false;
    cellObj->dynamicCall("Split(int, int)", rows, cols);
    return true;
}

bool QtWordTable::splitCell(int startRow, int startCol, int endRow, int endCol, int rows, int cols)
{
    if(!mergeCell(startRow, startCol, endRow, endCol)){
        return false;
    }
    return splitCell(startRow, startCol, rows, cols);
}

bool QtWordTable::setTableTextColor(const QColor &color)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowTextColor(i+1, color);
    }
    return true;
}

bool QtWordTable::setTableTextBackColor(Word::WdColorIndex colorIndex)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowTextBackColor(i+1, colorIndex);
    }
    return true;
}

bool QtWordTable::setTableBackColor(Word::WdColor color)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowBackColor(i+1, color);
    }
    return true;
}

bool QtWordTable::setTableBackColor(const QColor &color)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowBackColor(i+1, color);
    }
    return true;
}

bool QtWordTable::setRowTextColor(int row, const QColor &color)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    QAxObject *font = rowObj->querySubObject("Range")->querySubObject("Font");
    int value = color.blue()*65536+color.green()*256+color.red();
    font->setProperty("Color", value);
    return true;
}

bool QtWordTable::setRowTextBackColor(int row, const QColor &color)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    QAxObject *shade = rowObj->querySubObject("Range")->querySubObject("Font")->querySubObject("Shading");
    int value = color.blue()*65536+color.green()*256+color.red();
    shade->setProperty("BackgroundPatternColor", value);
    return true;
}

bool QtWordTable::setRowTextBackColor(int row, Word::WdColorIndex colorIndex)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    QAxObject *shade = rowObj->querySubObject("Range")->querySubObject("Font")->querySubObject("Shading");
    shade->setProperty("BackgroundPatternColorIndex", colorIndex);
    return true;
}

bool QtWordTable::setRowBackColor(int row, Word::WdColor color)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    rowObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", color);
    return true;
}

bool QtWordTable::setRowBackColor(int row, const QColor &color)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    int value = color.blue()*65536+color.green()*256+color.red();
    rowObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", value);
    return true;
}

bool QtWordTable::setColTextColor(int col, const QColor &color)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    int value = color.blue()*65536+color.green()*256+color.red();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Color", value);
    }
    return true;
}

bool QtWordTable::setColTextBackColor(int col, const QColor &color)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    int value = color.blue()*65536+color.green()*256+color.red();
    for(int i = 0; i < count; i++){
        QAxObject *shade = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")
                ->querySubObject("Font")->querySubObject("Shading");
        shade->setProperty("BackgroundPatternColor", value);
    }
    return true;
}

bool QtWordTable::setColTextBackColor(int col, Word::WdColorIndex colorIndex)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *shade = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")
                ->querySubObject("Font")->querySubObject("Shading");
        shade->setProperty("BackgroundPatternColorIndex", colorIndex);
    }
    return true;
}

bool QtWordTable::setColBackColor(int col, Word::WdColor color)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    colObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", color);
    return true;
}

bool QtWordTable::setColBackColor(int col, const QColor &color)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int value = color.blue()*65536+color.green()*256+color.red();
    colObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", value);
    return true;
}

bool QtWordTable::setCellTextColor(int row, int col, const QColor &color)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *font = cellObj->querySubObject("Range")->querySubObject("Font");
    int value = color.blue()*65536+color.green()*256+color.red();
    font->setProperty("Color", value);
    return true;
}

bool QtWordTable::setCellTextBackColor(int row, int col, const QColor &color)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *shade = cellObj->querySubObject("Range")->querySubObject("Font")->querySubObject("Shading");
    int value = color.blue()*65536+color.green()*256+color.red();
    shade->setProperty("BackgroundPatternColor", value);
    return true;
}

bool QtWordTable::setCellTextBackColor(int row, int col, Word::WdColorIndex colorIndex)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *shade = cellObj->querySubObject("Range")->querySubObject("Font")->querySubObject("Shading");
    shade->setProperty("BackgroundPatternColorIndex ", colorIndex);
    return true;
}

bool QtWordTable::setCellBackColor(int row, int col, Word::WdColor color)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    cellObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", color);
    return true;
}

bool QtWordTable::setCellBackColor(int row, int col, const QColor &color)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    int value = color.blue()*65536+color.green()*256+color.red();
    cellObj->querySubObject("Shading")->setProperty("BackgroundPatternColor", value);
    return true;
}

bool QtWordTable::setCellBackColor(int row, int col, Word::WdColorIndex colorIndex)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    cellObj->querySubObject("Shading")->setProperty("BackgroundPatternColorIndex ", colorIndex);
    return true;
}

bool QtWordTable::setTableFont(const QFont &font)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowFont(i+1, font);
    }
    return true;
}

bool QtWordTable::setTableFont(const QString &family, int pointSize, bool bold, bool italic)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowFont(i+1, family, pointSize, bold, italic);
    }
    return true;
}

bool QtWordTable::setTableFontSize(int size)
{
    int rowCount = m_table->querySubObject("Rows")->property("Count").toInt();
    for(int i = 0; i < rowCount; i++){
        setRowFontSize(i+1, size);
    }
    return true;
}

bool QtWordTable::setRowFont(int row, const QFont &font)
{
    return setRowFont(row, font.family(), font.pointSize(), font.bold(), font.italic());
}

bool QtWordTable::setRowFont(int row, const QString &family, int pointSize, bool bold, bool italic)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    int count = rowObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(itn, int)", row, i+1)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Name", family);
        if(pointSize > 0){
            font->setProperty("Size", pointSize);
        }
        font->setProperty("Bold", bold);
        font->setProperty("Italic", italic);
    }
    return true;
}

bool QtWordTable::setRowFontBold(int row, bool bold)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    int count = rowObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int, int)", row, i+1)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Bold", bold);
    }
    return true;
}

bool QtWordTable::setRowFontItalic(int row, bool italic)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    int count = rowObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int, int)", row, i+1)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Italic", italic);
    }
    return true;
}

bool QtWordTable::setRowFontSize(int row, int size)
{
    if(size <= 0) return false;
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    int count = rowObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int, int)", row, i+1)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Size", size);
    }
    return true;
}

bool QtWordTable::setColFont(int col, const QFont &font)
{
    return setColFont(col, font.family(), font.pointSize(), font.bold(), font.italic());
}

bool QtWordTable::setColFont(int col, const QString &family, int pointSize, bool bold, bool italic)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Name", family);
        if(pointSize > 0){
            font->setProperty("Size", pointSize);
        }
        font->setProperty("Bold", bold);
        font->setProperty("Italic", italic);
    }
    return true;
}

bool QtWordTable::setColFontBold(int col, bool bold)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Bold", bold);
    }
    return true;
}

bool QtWordTable::setColFontItalic(int col, bool italic)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Italic", italic);
    }
    return true;
}

bool QtWordTable::setColFontSize(int col, int size)
{
    if(size <= 0) return false;
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        QAxObject *font = m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")->querySubObject("Font");
        font->setProperty("Size", size);
    }
    return true;
}

bool QtWordTable::setCellFont(int row, int col, const QFont &font)
{
    return setCellFont(row,col,font.family(), font.pointSize(), font.bold(), font.italic());
}

bool QtWordTable::setCellFont(int row, int col, const QString &family, int pointSize, bool bold, bool italic)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *font = cellObj->querySubObject("Range")->querySubObject("Font");
    font->setProperty("Name", family);
    if(pointSize > 0){
        font->setProperty("Size", pointSize);
    }
    font->setProperty("Bold", bold);
    font->setProperty("Italic", italic);
    return true;
}

bool QtWordTable::setCellFontBold(int row, int col, bool bold)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *font = cellObj->querySubObject("Range")->querySubObject("Font");
    font->setProperty("Bold", bold);
    return true;
}

bool QtWordTable::setCellFontItalic(int row, int col, bool italic)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *font = cellObj->querySubObject("Range")->querySubObject("Font");
    font->setProperty("Italic", italic);
    return true;
}

bool QtWordTable::setCellFontSize(int row, int col, int size)
{
    if(size <= 0) return false;
    QAxObject *cellObj = m_table->querySubObject("Cell(int,int)", row, col);
    if(!cellObj) return false;
    QAxObject *font = cellObj->querySubObject("Range")->querySubObject("Font");
    font->setProperty("Size", size);
    return true;
}

bool QtWordTable::setTableAlignment(Word::WdParagraphAlignment align)
{
    m_table->querySubObject("Range")->querySubObject("ParagraphFormat")->setProperty("Alignment", align);
    return true;
}

bool QtWordTable::setRowAlignment(int row, Word::WdParagraphAlignment align)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return false;
    rowObj->querySubObject("Range")->querySubObject("ParagraphFormat")->setProperty("Alignment", align);
    return true;
}

bool QtWordTable::setColAlignment(int col, Word::WdParagraphAlignment align)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col)->querySubObject("Cells");
    if(!colObj) return false;
    int count = colObj->querySubObject("Cells")->property("Count").toInt();
    for(int i = 0; i < count; i++){
        m_table->querySubObject("Cell(int,int)", i+1, col)->querySubObject("Range")
                ->querySubObject("ParagraphFormat")->setProperty("Alignment", align);
    }
    return true;
}

bool QtWordTable::setCellParagraphAlignment(int row, int col, Word::WdParagraphAlignment align)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)",row,col);
    if(!cellObj) return false;
    cellObj->querySubObject("Range")->querySubObject("ParagraphFormat")->setProperty("Alignment", align);
    return true;
}

bool QtWordTable::setCellVerticalAlignment(int row, int col, Word::WdCellVerticalAlignment align)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)",row,col);
    if(!cellObj) return false;
    cellObj->setProperty("VerticalAlignment", align);
    return true;
}

bool QtWordTable::insertCellText(int row, int col, const QString &text)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)",row,col);
    if(!cellObj) return false;
    cellObj->querySubObject("Range")->dynamicCall("SetText(QString)", text);
    return true;
}

bool QtWordTable::insertCellPicture(int row, int col, const QString &name)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)",row,col);
    if(!cellObj) return false;
    QAxObject* range = cellObj->querySubObject("Range");
    range->dynamicCall("Select()"); // added on 2018-3-3 16:32:55
    range->querySubObject("InlineShapes")->dynamicCall("AddPicture(const QString&)",name);
    return true;
}

void QtWordTable::setColumnWidth(int col, int width)
{
    if(width < 1) return;
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return;
    colObj->setProperty("Width", width);
}

void QtWordTable::setRowHeight(int row, int height)
{
    if(height < 1) return;
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return;
    rowObj->setProperty("Height", height);
}

void QtWordTable::setInsideLineStyle(Word::WdLineStyle style)
{
    m_table->querySubObject("Borders")->setProperty("InsideLineStyle", style);
}

void QtWordTable::setInsideLineWidth(int width)
{
    if(width < 0) return;
    m_table->querySubObject("Borders")->setProperty("InsideLineWidth", width);
}

void QtWordTable::setInsideColor(const QColor &color)
{
    int value = color.blue()*65536+color.green()*256+color.red();
    m_table->querySubObject("Borders")->setProperty("InsideColor", value);
}

void QtWordTable::setInsideColorIndex(Word::WdColorIndex colorIndex)
{
    m_table->querySubObject("Borders")->setProperty("InsideColorIndex", colorIndex);
}

void QtWordTable::setOutsideLineStyle(Word::WdLineStyle style)
{
    m_table->querySubObject("Borders")->setProperty("OutsideLineStyle", style);
}

void QtWordTable::setOutsideLineWidth(int width)
{
    if(width < 0) return;
    m_table->querySubObject("Borders")->setProperty("OutsideLineWidth", width);
}

void QtWordTable::setOutsideColor(const QColor &color)
{
    int value = color.blue()*65536+color.green()*256+color.red();
    m_table->querySubObject("Borders")->setProperty("OutsideColor", value);
}

void QtWordTable::setOutsideColorIndex(Word::WdColorIndex colorIndex)
{
    m_table->querySubObject("Borders")->setProperty("OutsideColorIndex", colorIndex);
}

void QtWordTable::setAutoFitBehavior(Word::WdAutoFitBehavior behavior)
{
    m_table->dynamicCall("AutoFitBehavior(WdAutoFitBehavior)", behavior);
}

QString QtWordTable::cellText(int row, int col)
{
    return cellValue(row, col).toString();
}

QVariant QtWordTable::cellValue(int row, int col)
{
    QAxObject *cellObj = m_table->querySubObject("Cell(int, int)",row,col);
    if(cellObj){
        return cellObj->querySubObject("Range")->property("Text");
    }else{
        return QVariant();
    }
}

void QtWordTable::deleteColumn(int col)
{
    QAxObject *colObj = m_table->querySubObject("Columns(int)", col);
    if(!colObj) return;
    colObj->dynamicCall("Delete()");
}

void QtWordTable::deleteRow(int row)
{
    QAxObject *rowObj = m_table->querySubObject("Rows(int)", row);
    if(!rowObj) return;
    rowObj->dynamicCall("Delete()");
}

void QtWordTable::addRow(int referRow, int count)
{
    QAxObject *rowsObj = m_table->querySubObject("Rows");
    if(!rowsObj) return;
    int rowCount = rowsObj->property("Count").toInt();
    if(referRow < 1 || referRow > rowCount){
        return;
    }
    for(int i = 0; i < count; i++){
        QAxObject *referRowObj = rowsObj->querySubObject("Item(int)", referRow+i);
        rowsObj->dynamicCall("Add(QAxObject *)", referRowObj->asVariant());
    }
}

void QtWordTable::addColumn(int referColumn, int count)
{
    QAxObject *columnsObj = m_table->querySubObject("Columns");
    if(!columnsObj) return;
    int columnCount = columnsObj->property("Count").toInt();
    if(referColumn < 1 || referColumn > columnCount){
        return;
    }
    for(int i = 0; i < count; i++){
        QAxObject *referColumnObj = columnsObj->querySubObject("Item(int)", referColumn+i);
        columnsObj->dynamicCall("Add(QAxObject *)", referColumnObj->asVariant());
    }
}

void QtWordTable::appendRow(int count)
{
    QAxObject *rowsObj = m_table->querySubObject("Rows");
    if(!rowsObj) return;
    for(int i = 0; i < count; i++){
        rowsObj->dynamicCall("Add()");
    }
}

void QtWordTable::appendColumn(int count)
{
    QAxObject *columnsObj = m_table->querySubObject("Columns");
    if(!columnsObj) return;
    for(int i = 0; i < count; i++){
        columnsObj->dynamicCall("Add()");
    }
}

QtWord::QtWord(QObject *parent)
    : QObject(parent)
{
    m_word = new QAxObject();
    m_document = NULL;
    m_bOpened = false;
    m_fileName = QString("");
    OleInitialize(0);
}

QtWord::QtWord(const QString &name, QObject *parent)
    : QObject(parent)
{
    m_word = new QAxObject();
    m_document = NULL;
    m_bOpened = false;
    m_fileName = name;
    OleInitialize(0);
}

QtWord::~QtWord()
{
    close();
    OleUninitialize();
}

bool QtWord::open(bool visible)
{
    if(!m_word){
        m_word = new QAxObject();
    }
    bool flag = m_word->setControl("Word.Application");
    if(!flag){
        flag = m_word->setControl("kwps.Application");
        if(!flag){
            return false;
        }
    }
    m_word->setProperty("Visible", visible);
    m_word->setProperty("DisplayAlerts", false);
    m_documents = m_word->querySubObject("Documents");
    if(!m_documents){
        return false;
    }
    if(m_fileName.isEmpty() || !QFile::exists(m_fileName)){
        m_documents->dynamicCall("Add (void)");
    }else{
        m_documents->dynamicCall("Add (QString)", m_fileName);
    }
    m_document = m_word->querySubObject("ActiveDocument");
    m_bOpened = true;
    return true;
}

bool QtWord::open(const QString &name, bool visible)
{
//    close();
    m_fileName = name;
    return open(visible);
}

bool QtWord::close()
{
//    if(m_bOpened){
//        saveAs(m_fileName);
//        m_bOpened = false;
//    }
    if(m_document){
        m_document->dynamicCall("Close(Boolean)", false);
    }
    if(m_word){
        m_word->dynamicCall("Quit()");
    }
    if(m_documents){
        delete m_documents;
    }
    if(m_word){
        delete m_word;
    }
    m_word = NULL;
    m_document = NULL;
    m_documents = NULL;
    m_bOpened = false;
    return true;
}

bool QtWord::isOpen()
{
    return m_bOpened;
}

void QtWord::save()
{
    if(m_document){
        m_document->dynamicCall("Save()");
    }
}

void QtWord::saveAs(const QString &name)
{
    if(name.isEmpty()){
        return;
    }
    if(m_document){
        m_document->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(name));
    }
}

QString QtWord::version()
{
    return m_word->property("Version").toString();
}

void QtWord::insertText(const QString &text)
{
    QAxObject* selection  = m_word->querySubObject("Selection");
    selection->dynamicCall("TypeText(const QString&)",text);
}

void QtWord::setTextFont(const QFont &font)
{
    setTextFont(font.family(), font.pointSize(), font.bold(), font.italic());
}

void QtWord::setTextFont(const QString &family, int pointSize, bool bold, bool italic)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    QAxObject* font = selection->querySubObject("Font");
    font->setProperty("Name", family);
    if(pointSize > 0){
        font->setProperty("Size", pointSize);
    }
    font->setProperty("Bold", bold);
    font->setProperty("Italic", italic);
}

void QtWord::setTextFontSize(int size)
{
    if(size <= 0) return;
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Size",size);
}

void QtWord::setTextFontName(const QString &name)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Name",name);
}

void QtWord::setTextFontBold(bool bold)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Bold",bold);
}

void QtWord::setTextFontItalic(bool italic)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Italic",italic);
}

void QtWord::setTextColor(Word::WdColor color)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Color",color);
}

void QtWord::setTextColor(const QColor &color)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    int value = color.blue()*65536+color.green()*256+color.red();
    selection->querySubObject("Font")->setProperty("Color",value);
}

void QtWord::setTextBackColor(const QColor &color)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    QAxObject *shade = selection->querySubObject("Font")->querySubObject("Shading");
    int value = color.blue()*65536+color.green()*256+color.red();
    shade->setProperty("BackgroundPatternColor ", value);
}

void QtWord::setTextBackColor(Word::WdColorIndex colorIndex)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    QAxObject *shade = selection->querySubObject("Shading");
    shade->setProperty("BackgroundPatternColorIndex ", colorIndex);
}

void QtWord::setTextAlignment(Word::WdParagraphAlignment align)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("ParagraphFormat")->setProperty("Alignment",align);
}

void QtWord::setTextStyle(Word::WdBuiltinStyle style)
{
    Q_UNUSED(style);
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("ParagraphFormat")->setProperty("Style",style);
}

void QtWord::setTextUnderline(Word::WdUnderline underline)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    selection->querySubObject("Font")->setProperty("Underline",underline);
}

void QtWord::setTextUnderlineColor(const QColor &color)
{
    QAxObject* selection = m_word->querySubObject("Selection");
    int value = color.blue()*65536+color.green()*256+color.red();
    selection->querySubObject("Font")->setProperty("UnderlineColor",value);
}

void QtWord::insertNewline()
{
    QAxObject *selection = m_word->querySubObject("Selection");
    selection->dynamicCall("TypeParagraph(void");
}

void QtWord::insertImage(const QString &name)
{
    Q_UNUSED(name);
}

void QtWord::insertImage(QPixmap pixmap)
{
    Q_UNUSED(pixmap);
}

void QtWord::insertHeaderFooter(QtWordHeaderFooter headerfooter)
{
    Q_UNUSED(headerfooter);
}

QtWordTable QtWord::insertTable(int rows, int cols)
{
    QAxObject* tables = m_document->querySubObject("Tables");
    QAxObject* selection = m_word->querySubObject("Selection");
    QAxObject* range = selection->querySubObject("Range");
    QVariantList params;
    params.append(range->asVariant());
    params.append(rows);
    params.append(cols);
    tables->querySubObject("Add(QAxObject*, int, int, QVariant&, QVariant&)", params);
    QAxObject* table = selection->querySubObject("Tables(1)");
//    table->setProperty("Style",Word::wdStyleNormal);
    QAxObject* Borders = table->querySubObject("Borders");
    Borders->setProperty("InsideLineStyle",1);
    Borders->setProperty("OutsideLineStyle",1);
    return QtWordTable(m_word, table);
}

QtWordTable QtWord::table(int index)
{
    QAxObject* tables = m_document->querySubObject("Tables");
    int count = tableCount();
    if(index < 1 || index > count){
        index = 1;
    }
    QAxObject* table = tables->querySubObject("Item(int)", index);
    return QtWordTable(m_word, table);
}

int QtWord::tableCount()
{
    QAxObject *tables = m_document->querySubObject("Tables");
    int count = tables->property("Count").toInt();
    return count;
}

void QtWord::setPageOrientation(Word::WdOrientation orient)
{
    QAxObject *selection = m_word->querySubObject("Selection");
    selection->querySubObject("PageSetUp")->setProperty("Orientation", orient);
}

void QtWord::setPageViewType(Word::WdViewType type)
{
    //ActiveDocument.ActiveWindow.View.Type = wdNormalView
    QAxObject *activeWindow = m_word->querySubObject("ActiveWindow");
    activeWindow->querySubObject("View")->setProperty("Type", type);
}

void QtWord::moveForEnd()
{
    QAxObject* selection = m_word->querySubObject("Selection");
    QVariantList params;
    params.append(6);
    params.append(0);
    selection->dynamicCall("EndOf(QVariant&, QVariant&)", params).toInt();
}

QString QtWord::text()
{
    QAxObject* selection = m_word->querySubObject("Selection");
    QString str = selection->dynamicCall("GetText(void)").toString();
    return str;
}


