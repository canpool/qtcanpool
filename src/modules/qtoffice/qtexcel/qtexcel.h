/***************************************************************************
**                                                                        **
**  QtWord, an easy to write exist Microsoft Excel (xls/xlsx) file format **
**          based on ActiveX                                              **
**  Copyright (C) 2017-2020 Canpool                                       **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Canpool (maminjie)                                   **
**  Original Author: yinrenjieone @ QTCN （1.0.3）
**                   http://www.qtcn.org/bbs/read-htm-tid-44953.html
**        reference: wdg,
**  Website/Contact: canpool@163.com / QQ1074867723                       **
**             Date: 17.11.11                                             **
**          Version: 0.1.0                                                **
****************************************************************************/
#ifndef QTEXCEL_H
#define QTEXCEL_H

#include <QObject>
#include <QPixmap>

#if defined(Q_OS_WIN)
#include <QAxObject>

// decl definitions for shared library compilation/usage:
#if defined(QTEXCEL_COMPILE_LIBRARY)
#  define QXL_LIB_DECL Q_DECL_EXPORT
#elif defined(QTEXCEL_USE_LIBRARY)
#  define QXL_LIB_DECL Q_DECL_IMPORT
#else
#  define QXL_LIB_DECL
#endif

namespace Excel {

// 指定对象的水平对齐方式
enum XlHAlign{
    xlHAlignCenter = -4108,             // 居中
    xlHAlignCenterAcrossSelection = 7,  // 跨列居中
    xlHAlignDistributed = -4117,        // 分散对齐
    xlHAlignFill = 5,                   // 填充。
    xlHAlignGeneral = 1,                // 按数据类型对齐。
    xlHAlignJustify = -4130,            // 两端对齐。
    xlHAlignLeft = -4131,               // 左对齐。
    xlHAlignRight = -4152               // 右对齐。
};

// 指定对象的垂直对齐方式。
enum XlVAlign{
    xlVAlignBottom = -4107,             // 靠下
    xlVAlignCenter = -4108,             // 居中
    xlVAlignDistributed = -4117,        // 分散对齐
    xlVAlignJustify = -4130,            // 两端对齐
    xlVAlignTop = -4160                 // 靠上
};

enum XlBordersIndex{
    xlDiagonalDown = 5, // 从区域中每个单元格的左上角至右下角的边框。
    xlDiagonalUp = 6,   // 从区域中每个单元格的左下角至右上角的边框。
    xlEdgeBottom = 9,   // 区域底部的边框。
    xlEdgeLeft = 7,     // 区域左边的边框。
    xlEdgeRight = 10,   // 区域右边的边框。
    xlEdgeTop = 8,      // 区域顶部的边框。
    xlInsideHorizontal = 12,    // 区域中所有单元格的水平边框（区域以外的边框除外）。
    xlInsideVertical = 11       // 区域中所有单元格的垂直边框（区域以外的边框除外）。
};

// 指定边框的线条样式。
enum XlLineStyle{
    xlContinuous = 1,       // 实线。
    xlDash = -4115,         // 虚线。
    xlDashDot = 4,          // 点划相间线。
    xlDashDotDot = 5,       // 划线后跟两个点。
    xlDot = -4118,          // 点式线。
    xlDouble = -4119,       // 双线。
    xlLineStyleNone = -4142,// 无线条。
    xlSlantDashDot = 13     // 倾斜的划线。
};

// 指定某一区域周围的边框的粗细
enum XlBorderWeight{
    xlHairline = 1,     // 细线（最细的边框）。
    xlMedium = -4138,   // 中等。
    xlThick = 4,        // 粗（最宽的边框）。
    xlThin = 2          // 细。
};

}

class QXL_LIB_DECL QtExcel : public QObject
{
    Q_OBJECT
public:
    explicit QtExcel(QObject *parent = nullptr);
    QtExcel(const QString &name, QObject *parent = nullptr);
    ~QtExcel();

    bool open(bool visible = false);
    bool open(const QString &name, bool visible = false);
    void close();
    bool isOpen();
    void save();
    void saveAs(const QString &name);

    // read
    QVariant readAll();
    void castVariant2ListListVariant(const QVariant &var, QList<QList<QVariant> > &res);

    // sheet
    bool selectSheet(const QString &name);
    bool selectSheet(int index);
    bool deleteSheet(const QString &name);
    bool deleteSheet(int index);

    bool prependSheet(const QString &name); // in the first
    bool insertSheet(const QString &name); // before active sheet
    bool insertSheet(int index, const QString &name); // before index
    bool insertSheet(const QString &referName, const QString &name);  // before referName
    bool appendSheet(const QString &name); // in the end

    int sheetsCount() const;
    QString currentSheetName() const;
    QString sheetName(int index) const;

    // cell
    void setCellText(int row, int column, const QString &text);
    void setCellText(const QString &cell, const QString &text); // cell: "A7"
    void clearCell(int row, int column);
    void clearCell(const QString &cell);
    void mergeCells(const QString &range, bool isMerge = true); // range: "A5:C7"
    void mergeCells(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn, bool isMerge = true);
    QVariant cellValue(int row, int column);
    QString getRange(int topLeftRow, int topLeftColumn, int bottomRightRow, int bottomRightColumn);

    // picture
    void setCellPicture(int row, int column, const QString &pixmap,
                        double width, double height);
    void setCellPicture(const QString &cell, const QString &pixmap,
                        double width, double height);

    // font
    // cell: "A7" or "A5:C7"
    void setCellFont(int row, int column, const QFont &font);
    void setCellFont(const QString &cell, const QFont &font);
    void setCellFontName(int row, int column, const QString &name);
    void setCellFontName(const QString &cell, const QString &name);
    void setCellFontBold(int row, int column, bool bold);
    void setCellFontBold(const QString &cell, bool bold);
    void setCellFontSize(int row, int column, int size);
    void setCellFontSize(const QString &cell, int size);
    void setCellFontItalic(int row, int column, bool italic);
    void setCellFontItalic(const QString &cell, bool italic);

    // color
    void setCellTextColor(int row, int column, const QColor &color);
    void setCellTextColor(const QString &cell, const QColor &color);
    void setCellBackColor(int row, int column, const QColor &color);
    void setCellBackColor(const QString &cell, const QColor &color);
    void setCellBorderColor(int row, int column, const QColor &color);
    void setCellBorderColor(const QString &cell, const QColor &color);

    // alignment
    void setCellTextHAlignment(int row, int column, Excel::XlHAlign align);
    void setCellTextHAlignment(const QString &cell, Excel::XlHAlign align);
    void setCellTextVAlignment(int row, int column, Excel::XlVAlign align);
    void setCellTextVAlignment(const QString &cell, Excel::XlVAlign align);

    // format
    void setCellWrapText(int row, int column, bool isWrap);
    void setCellWrapText(const QString &cell, bool isWrap);
    void setRowHeight(int row, int height);
    void setColumnWidth(int column, int width);
    void setRowAutoFit(int row);
    void setColumnAutoFit(int column);

    void setCellNumberFormatLocal(int row, int column);
    void setCellNumberFormatLocal(const QString &cell);

    // borders
    void setInsideLineStyle(const QString &range, Excel::XlLineStyle style);
    void setOutsideLineStyle(const QString &range, Excel::XlLineStyle style);
    void setBorderLineStyle(const QString &range, Excel::XlLineStyle style); //
    void setBorderLineStyle(const QString &range, Excel::XlBordersIndex borderIndex, Excel::XlLineStyle style);

    void setInsideLineColor(const QString &range, const QColor &color);
    void setOutsideLineColor(const QString &range, const QColor &color);
    void setBorderLineColor(const QString &range, const QColor &color); // equal to setCellBorderColor(cell, color)
    void setBorderLineColor(const QString &range, Excel::XlBordersIndex borderIndex, const QColor &color);

    void setInsideLineWeight(const QString &range, Excel::XlBorderWeight weight);
    void setOutsideLineWeight(const QString &range, Excel::XlBorderWeight weight);
    void setBorderWeight(const QString &range, Excel::XlBorderWeight weight);
    void setBorderWeight(const QString &range, Excel::XlBordersIndex borderIndex, Excel::XlBorderWeight weight);

private:
    void init();

private:
    QAxObject *m_excel;
    QAxObject *m_workBooks;
    QAxObject *m_workBook;
    QAxObject *m_sheets;
    QAxObject *m_sheet;
    bool m_bOpened;
    QString m_fileName;
};
#endif
#endif // QTEXCEL_H
