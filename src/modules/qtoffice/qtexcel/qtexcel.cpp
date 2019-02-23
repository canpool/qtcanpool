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
#include "qtexcel.h"
#include <QAxWidget>
#include <QDir>
#include <QDebug>

static QString colToString(int column)
{
    if(column < 1){
        return QString("A");
    }
    int C[3] = {0};
    int n = column;
    for(int i = 0; i < 3; i++){
        int k = (n - 1) / 26;
        C[i] = n - 26 * k;
        if(k > 0){
            n = k;
        }else{
            break;
        }
    }

    QString strCol("");
    for(int i = 2; i >= 0; i--){
        if(C[i]){
            strCol.append(C[i] + 'A' - 1);
        }
    }
    return strCol;
}

static QString cellString(int row, int column)
{
    QString strCol = colToString(column);
    if(row < 1){
        row = 1;
    }
    QString strRange = QString("%1%2").arg(strCol).arg(row);
    return strRange;
}

static QString cellString(QString cell)
{
    if(cell.isEmpty()){
        return QString("A1");
    }
    cell = cell.toUpper();
    QStringList cells = cell.split(":");
    int count = cells.count();
    QRegExp regExp = QRegExp("^[A-Z]+[1-9]+[0-9]*$");
    if(count == 1){
        if(regExp.exactMatch(cells.at(0))){
            return cell;
        }else{
            return QString("A1");
        }
    }else if(count >= 2){
        QString startCell = cells.at(0);
        if(!regExp.exactMatch(startCell)){
            startCell = QString("A1");
        }
        QString endCell = cells.at(1);
        if(!regExp.exactMatch(endCell)){
            endCell = QString("B1");
        }
        return QString("%1:%2").arg(startCell).arg(endCell);
    }
    return QString("A1");
}

#if defined(Q_OS_WIN)
QtExcel::QtExcel(QObject *parent)
    : QObject(parent)
{
    m_fileName = QString("");
    init();
}

QtExcel::QtExcel(const QString &name, QObject *parent)
    : QObject(parent)
{
    m_fileName = name;
    init();
}

void QtExcel::init()
{
    m_excel = nullptr;
    m_workBooks = nullptr;
    m_workBook = nullptr;
    m_sheets = nullptr;
    m_sheet = nullptr;
    m_bOpened = false;

    m_excel = new QAxObject("Excel.Application", this);
}

QtExcel::~QtExcel()
{
    close();
}

bool QtExcel::open(bool visible)
{
    if(!QFile::exists(m_fileName)){
        qWarning("file is not exsits!");
        return false;
    }
    if(!m_excel){
        m_excel = new QAxObject("Excel.Application", this);
    }
    m_excel->setProperty("Visible", visible);
    m_workBooks = m_excel->querySubObject("Workbooks");
    m_workBooks->dynamicCall("Open(const QString&)", QDir::toNativeSeparators(m_fileName));
    m_workBook = m_excel->querySubObject("ActiveWorkBook");
    m_sheets = m_workBook->querySubObject("WorkSheets"); // or "Sheets"
    m_sheet = m_sheets->querySubObject("Item(int)", 1);
    m_bOpened = true;
    return true;
}

bool QtExcel::open(const QString &name, bool visible)
{
    m_fileName = name;
    return open(visible);
}

void QtExcel::close()
{
    if(m_workBook){
        m_workBook->dynamicCall("Close(Boolean)", false);
    }
    if(m_excel){
        m_excel->dynamicCall("Quit()");
    }
    if(m_sheet){
        delete m_sheet; m_sheet = nullptr;
    }
    if(m_sheets){
        delete m_sheets; m_sheets = nullptr;
    }
    if(m_workBook){
        delete m_workBook; m_workBook = nullptr;
    }
    if(m_workBooks){
        delete m_workBooks; m_workBooks = nullptr;
    }
    if(m_excel){
        delete m_excel; m_excel = nullptr;
    }
}

bool QtExcel::isOpen()
{
    return m_bOpened;
}

void QtExcel::save()
{
    if(m_workBook){
        m_workBook->dynamicCall("Save()");
    }
}

void QtExcel::saveAs(const QString &name)
{
    if(name.isEmpty()){
        return;
    }
    m_workBook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(name));
}

QVariant QtExcel::readAll()
{
    QVariant var;
    if(m_sheet != nullptr && !m_sheet->isNull()){
        QAxObject *usedRange = m_sheet->querySubObject("UsedRange");
        if(usedRange == nullptr || usedRange->isNull()){
            return var;
        }
        var = usedRange->dynamicCall("Value");
        delete usedRange;
    }
    return var;
}

void QtExcel::castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res)
{
    QVariantList varRows = var.toList();
    if(varRows.isEmpty()) return;
    const int rowCount = varRows.size();
    QVariantList rowData;
    for(int i = 0; i < rowCount; i++){
        rowData = varRows[i].toList();
        res.append(rowData);
    }
}

bool QtExcel::selectSheet(const QString &name)
{
    QAxObject *sheet = m_sheets->querySubObject("Item(const QString&)", name);
    if(sheet){
        m_sheet = sheet;
        m_sheet->dynamicCall("Select()");
        return true;
    }
    return false;
}

bool QtExcel::selectSheet(int index)
{
    QAxObject *sheet = m_sheets->querySubObject("Item(int)", index);
    if(sheet){
        m_sheet = sheet;
        m_sheet->dynamicCall("Select()");
        return true;
    }
    return false;
}

bool QtExcel::deleteSheet(const QString &name)
{
    QAxObject *sheet = m_sheets->querySubObject("Item(const QString&)", name);
    if(sheet){
        sheet->dynamicCall("delete");
        if(m_sheet == sheet){
            m_sheet = nullptr;
        }
        return true;
    }
    return false;
}

bool QtExcel::deleteSheet(int index)
{
    QAxObject *sheet = m_sheets->querySubObject("Item(int)", index);
    if(sheet){
        sheet->dynamicCall("delete");
        if(m_sheet == sheet){
            m_sheet = nullptr;
        }
        return true;
    }
    return false;
}

bool QtExcel::prependSheet(const QString &name)
{
    return insertSheet(1, name);
}

bool QtExcel::insertSheet(const QString &name)
{
    QAxObject *sheet = m_sheets->querySubObject("Add()");
    if(sheet){
        m_sheet = sheet;
        sheet->setProperty("Name", name);
        return true;
    }
    return false;
}

bool QtExcel::insertSheet(int index, const QString &name)
{
    QAxObject *referSheet = m_sheets->querySubObject("Item(int)", index);
    if(!referSheet) return false;
    QAxObject *sheet = m_sheets->querySubObject("Add(QAxObject *)", referSheet->asVariant());
    if(sheet){
        m_sheet = sheet;
        sheet->setProperty("Name", name);
        return true;
    }
    return false;
}

bool QtExcel::insertSheet(const QString &referName, const QString &name)
{
    QAxObject *referSheet = m_sheets->querySubObject("Item(const QString&)", referName);
    if(!referSheet) return false;
    QAxObject *sheet = m_sheets->querySubObject("Add(QVariant)", referSheet->asVariant());
    if(sheet){
        m_sheet = sheet;
        sheet->setProperty("Name", name);
        return true;
    }
    return false;
}

bool QtExcel::appendSheet(const QString &name)
{
    QAxObject *referSheet = m_sheets->querySubObject("Item(int)", sheetsCount());
    if(!referSheet) return false;
    QAxObject *sheet = m_sheets->querySubObject("Add(QVariant)", referSheet->asVariant());
    if(sheet){
        referSheet->dynamicCall("Move(QVariant)", sheet->asVariant());
        m_sheet = sheet;
        sheet->setProperty("Name", name);
        return true;
    }
    return false;
}

int QtExcel::sheetsCount() const
{
    return m_sheets->property("Count").toInt();
}

QString QtExcel::currentSheetName() const
{
    return m_sheet->property("Name").toString();
}

QString QtExcel::sheetName(int index) const
{
    QAxObject *sheet = m_sheets->querySubObject("Item(int)", index); // "Sheets(int)","WorkSheets(int)"
    if(sheet){
        return sheet->property("Name").toString();
    }
    return QString("");
}

void QtExcel::setCellText(int row, int column, const QString &text)
{
    QString cell = cellString(row, column);
    setCellText(cell, text);
}

void QtExcel::setCellText(const QString &cell, const QString &text)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->dynamicCall("SetValue(const QString&)", text);
}

void QtExcel::clearCell(int row, int column)
{
    QString cell = cellString(row, column);
    clearCell(cell);
}

void QtExcel::clearCell(const QString &cell)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->dynamicCall("ClearContents()");
}

void QtExcel::mergeCells(const QString &range, bool isMerge)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    rangeObj->setProperty("VerticalAlignment", Excel::xlVAlignCenter);
    rangeObj->setProperty("WrapText", true); // auto newline
    rangeObj->setProperty("MergeCells", isMerge);
}

void QtExcel::mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn, bool isMerge)
{
    QString range = QString("%1:%2").arg(cellString(topLeftRow, topLeftColumn))
            .arg(cellString(bottomRightRow, bottomRightColumn));
    mergeCells(range, isMerge);
}

QVariant QtExcel::cellValue(int row, int column)
{
    Q_ASSERT(m_sheet);
    QAxObject *cell = m_sheet->querySubObject("Cells(int,int)", row, column);
    if(cell){
        return cell->property("Value");
    }
    return QVariant();
}

QString QtExcel::getRange(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn)
{
    QString range = QString("%1:%2").arg(cellString(topLeftRow, topLeftColumn))
            .arg(cellString(bottomRightRow, bottomRightColumn));
    return range;
}

void QtExcel::setCellPicture(int row, int column, const QString &pixmap, double width, double height)
{
    QString cell = cellString(row, column);
    setCellPicture(cell, pixmap, width, height);
}

void QtExcel::setCellPicture(const QString &cell, const QString &pixmap, double width, double height)
{
//    Q_ASSERT(m_sheet);
//    QString checkCell = cellString(cell);
//    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
//    range->dynamicCall("Select()");
//    range->dynamicCall("SetValue(const QString&)", pixmap);

    Q_ASSERT(m_sheet);
    Q_UNUSED(cell);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
//    range->dynamicCall("Select()");
    QAxObject *shapes = m_sheet->querySubObject("Shapes");
    shapes->dynamicCall("AddPicture( QString&, bool, bool, double, double, double, double",
                        pixmap,true,true,
                        range->property("Left").toDouble()+5,
                        range->property("Top").toDouble()+5,
                        width,height);
}

void QtExcel::setCellFont(int row, int column, const QFont &font)
{
    QString cell = cellString(row, column);
    setCellFont(cell, font);
}

void QtExcel::setCellFont(const QString &cell, const QFont &font)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    fontObj->setProperty("Name", font.family());
    fontObj->setProperty("Size", font.pointSize());
    fontObj->setProperty("Bold", font.bold());
    fontObj->setProperty("Italic", font.italic());
}

void QtExcel::setCellFontName(int row, int column, const QString &name)
{
    Q_ASSERT(m_sheet);
    QString cell = cellString(row, column);
    setCellFontName(cell, name);
}

void QtExcel::setCellFontName(const QString &cell, const QString &name)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    fontObj->setProperty("Name", name);
}

void QtExcel::setCellFontBold(int row, int column, bool bold)
{
    Q_ASSERT(m_sheet);
    QString cell = cellString(row, column);
    setCellFontBold(cell, bold);
}

void QtExcel::setCellFontBold(const QString &cell, bool bold)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    fontObj->setProperty("Bold", bold);
}

void QtExcel::setCellFontSize(int row, int column, int size)
{
    Q_ASSERT(m_sheet);
    QString cell = cellString(row, column);
    setCellFontSize(cell, size);
}

void QtExcel::setCellFontSize(const QString &cell, int size)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    if(size < 1){
        size = 1;
    }
    fontObj->setProperty("Size", size);
}

void QtExcel::setCellFontItalic(int row, int column, bool italic)
{
    Q_ASSERT(m_sheet);
    QString cell = cellString(row, column);
    setCellFontItalic(cell, italic);
}

void QtExcel::setCellFontItalic(const QString &cell, bool italic)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    fontObj->setProperty("Italic", italic);
}

void QtExcel::setCellTextColor(int row, int column, const QColor &color)
{
    QString cell = cellString(row, column);
    setCellTextColor(cell,color);
}

void QtExcel::setCellTextColor(const QString &cell, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *fontObj = range->querySubObject("Font");
    int value = color.blue()*65536+color.green()*256+color.red();
    fontObj->setProperty("Color", value);
}

void QtExcel::setCellBackColor(int row, int column, const QColor &color)
{
    QString cell = cellString(row, column);
    setCellBackColor(cell, color);
}

void QtExcel::setCellBackColor(const QString &cell, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *interior = range->querySubObject("Interior");
//    int value = color.blue()*65536+color.green()*256+color.red();
//    interior->setProperty("Color", value);
    interior->setProperty("Color", color);
}

void QtExcel::setCellBorderColor(int row, int column, const QColor &color)
{
    QString cell = cellString(row, column);
    setCellBorderColor(cell, color);
}

void QtExcel::setCellBorderColor(const QString &cell, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    QAxObject *border = range->querySubObject("Borders");
    int value = color.blue()*65536+color.green()*256+color.red();
    border->setProperty("Color", value);
}

void QtExcel::setCellTextHAlignment(int row, int column, Excel::XlHAlign align)
{
    QString cell = cellString(row,column);
    setCellTextHAlignment(cell,align);
}

void QtExcel::setCellTextHAlignment(const QString &cell, Excel::XlHAlign align)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->setProperty("HorizontalAlignment", align);
}

void QtExcel::setCellTextVAlignment(int row, int column, Excel::XlVAlign align)
{
    QString cell = cellString(row, column);
    setCellTextVAlignment(cell, align);
}

void QtExcel::setCellTextVAlignment(const QString &cell, Excel::XlVAlign align)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->setProperty("VerticalAlignment", align);
}

void QtExcel::setCellWrapText(int row, int column, bool isWrap)
{
    QString cell = cellString(row, column);
    setCellWrapText(cell, isWrap);
}

void QtExcel::setCellWrapText(const QString &cell, bool isWrap)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->setProperty("WrapText", isWrap);
}

void QtExcel::setRowHeight(int row, int height)
{
    if(row < 1 || height < 1) return;
    Q_ASSERT(m_sheet);
    QString rowsName = QString("%1:%2").arg(row).arg(row);
    QAxObject *rowObj =  m_sheet->querySubObject("Rows(const QString&)", rowsName);
    rowObj->setProperty("RowHeight", height);
}

void QtExcel::setColumnWidth(int column, int width)
{
    if(column < 1 || width < 1) return;
    Q_ASSERT(m_sheet);
    QString colString = colToString(column);
    QString columnsName = QString("%1:%2").arg(colString).arg(colString);
    QAxObject *colObj = m_sheet->querySubObject("Columns(const QString&)", columnsName);
    colObj->setProperty("ColumnWidth", width);
}

void QtExcel::setRowAutoFit(int row)
{
    if(row < 1) return;
    Q_ASSERT(m_sheet);
    QString rowsName = QString("%1:%2").arg(row).arg(row);
    QAxObject *rowObj =  m_sheet->querySubObject("Rows(const QString&)", rowsName);
    rowObj->dynamicCall("AutoFit()");
}

void QtExcel::setColumnAutoFit(int column)
{
    if(column < 1) return;
    Q_ASSERT(m_sheet);
    QString colString = colToString(column);
    QString columnsName = QString("%1:%2").arg(colString).arg(colString);
    QAxObject *colObj = m_sheet->querySubObject("Columns(const QString&)", columnsName);
    colObj->dynamicCall("AutoFit()");
}

void QtExcel::setCellNumberFormatLocal(int row, int column)
{
    QString cell = cellString(row, column);
    setCellNumberFormatLocal(cell);
}

void QtExcel::setCellNumberFormatLocal(const QString &cell)
{
    Q_ASSERT(m_sheet);
    QString checkCell = cellString(cell);
    QAxObject *range = m_sheet->querySubObject("Range(const QString&)", checkCell);
    range->setProperty("NumberFormatLocal", "@");
}

void QtExcel::setInsideLineStyle(const QString &range, Excel::XlLineStyle style)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlInsideHorizontal)");
    border->setProperty("LineStyle", style);
    border = rangeObj->querySubObject("Borders(xlInsideVertical)");
    border->setProperty("LineStyle", style);
}

void QtExcel::setOutsideLineStyle(const QString &range, Excel::XlLineStyle style)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlEdgeLeft)");
    border->setProperty("LineStyle", style);
    border = rangeObj->querySubObject("Borders(xlEdgeTop)");
    border->setProperty("LineStyle", style);
    border = rangeObj->querySubObject("Borders(xlEdgeRight)");
    border->setProperty("LineStyle", style);
    border = rangeObj->querySubObject("Borders(xlEdgeBottom)");
    border->setProperty("LineStyle", style);
}

void QtExcel::setBorderLineStyle(const QString &range, Excel::XlLineStyle style)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders");
    border->setProperty("LineStyle", style);
}

void QtExcel::setBorderLineStyle(const QString &range, Excel::XlBordersIndex borderIndex, Excel::XlLineStyle style)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(XlBordersIndex)", borderIndex);
    border->setProperty("LineStyle", style);
}

void QtExcel::setInsideLineColor(const QString &range, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlInsideHorizontal)");
    border->setProperty("Color", color);
    border = rangeObj->querySubObject("Borders(xlInsideVertical)");
    border->setProperty("Color", color);
}

void QtExcel::setOutsideLineColor(const QString &range, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlEdgeLeft)");
    border->setProperty("Color", color);
    border = rangeObj->querySubObject("Borders(xlEdgeTop)");
    border->setProperty("Color", color);
    border = rangeObj->querySubObject("Borders(xlEdgeRight)");
    border->setProperty("Color", color);
    border = rangeObj->querySubObject("Borders(xlEdgeBottom)");
    border->setProperty("Color", color);
}

void QtExcel::setBorderLineColor(const QString &range, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders");
    border->setProperty("Color", color);
}

void QtExcel::setBorderLineColor(const QString &range, Excel::XlBordersIndex borderIndex, const QColor &color)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(XlBordersIndex)", borderIndex);
    border->setProperty("Color", color);
}

void QtExcel::setInsideLineWeight(const QString &range, Excel::XlBorderWeight weight)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlInsideHorizontal)");
    border->setProperty("Weight", weight);
    border = rangeObj->querySubObject("Borders(xlInsideVertical)");
    border->setProperty("Weight", weight);
}

void QtExcel::setOutsideLineWeight(const QString &range, Excel::XlBorderWeight weight)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(xlEdgeLeft)");
    border->setProperty("Weight", weight);
    border = rangeObj->querySubObject("Borders(xlEdgeTop)");
    border->setProperty("Weight", weight);
    border = rangeObj->querySubObject("Borders(xlEdgeRight)");
    border->setProperty("Weight", weight);
    border = rangeObj->querySubObject("Borders(xlEdgeBottom)");
    border->setProperty("Weight", weight);
}

void QtExcel::setBorderWeight(const QString &range, Excel::XlBorderWeight weight)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders");
    border->setProperty("Weight", weight);
}

void QtExcel::setBorderWeight(const QString &range, Excel::XlBordersIndex borderIndex, Excel::XlBorderWeight weight)
{
    Q_ASSERT(m_sheet);
    QString checkRange = cellString(range);
    QAxObject *rangeObj = m_sheet->querySubObject("Range(const QString&)", checkRange);
    QAxObject *border = rangeObj->querySubObject("Borders(XlBordersIndex)", borderIndex);
    border->setProperty("Weight", weight);
}

#endif
