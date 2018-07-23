/***************************************************************************
**                                                                        **
**  QtHtmlWord, an easy to write Microsoft Word (doc/docx) file format    **
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
**  Website/Contact: canpool@163.com / QQ1074867723                       **
**             Date: 17.11.23                                             **
**          Version: 0.1.0                                                **
**                                                                        **
****************************************************************************
****************************************************************************/
#ifndef QTHTMLWORD_H
#define QTHTMLWORD_H

#include <QObject>
#include <QFont>

enum ParagraphAlign
{
    alignParagraphLeft,
    alignParagraphCenter,
    alignParagraphRight
};

enum CellVerticalAlign
{
    cellAlignVerticalTop,
    cellAlignVerticalCenter,
    cellAlignVerticalBottom
};

class HtmlText
{
public:
    HtmlText();

    QFont font;
    bool isUnderline;
    QString align;
    QString text;
};

class TableCell
{
public:
    TableCell();
    ~TableCell();

    int width;
    int colspan;
    int rowspan;
    QString valign;
    HtmlText text;
};

// row
class TableRow
{
public:
    TableRow();
    ~TableRow();

    QList<TableCell*> tableCells;
};

class QtHtmlTable
{
public:
    QtHtmlTable(int rows);
    ~QtHtmlTable();

    bool insertCellText(int row, int colspan, const QString &text,
                        ParagraphAlign align = alignParagraphCenter, CellVerticalAlign valign = cellAlignVerticalCenter);
    bool insertCellText(int row, int rowspan, int colspan, const QString &text,
                        ParagraphAlign align = alignParagraphCenter, CellVerticalAlign valign = cellAlignVerticalCenter);
    void insertFrontText(const QString &text, ParagraphAlign align = alignParagraphCenter);
    void insertBackText(const QString &text, ParagraphAlign align = alignParagraphCenter);

    void setTextFont(const QFont &font);
    void setTextFontSize(int size);
    void setTextFontName(const QString &name);
    void setTextFontBold(bool bold);
    void setTextUnderline(bool underline);

private:
    QString ParagraphAlignment(ParagraphAlign align);
    QString cellVerticalAlignment(CellVerticalAlign valign);

private:
    QList<TableRow *> m_tableRows;
    QList<HtmlText> m_frontTexts;
    QList<HtmlText> m_backTexts;

    QFont m_font;
    bool m_textUnderline;

    friend class QtHtmlWord;
};

class QtHtmlWord : public QObject
{
    Q_OBJECT
public:
    explicit QtHtmlWord(QObject *parent = 0);
    ~QtHtmlWord();

    void saveAs(const QString &fileName);

    // table
    QtHtmlTable* insertTable(int rows);
    QtHtmlTable* table(int index);
    void setTableWidth(int width);

private:
    int m_tableWidth;
    QList<QtHtmlTable *> m_tables;
};

#endif // QTHTMLWORD_H
