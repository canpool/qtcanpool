/***************************************************************************
**                                                                        **
**  QtXmlExcel, an easy to write Microsoft Excel (xls/xlsx) file format   **
**          based on XML Excel 2003 (*.xml)                               **
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
**        reference: liudianwu                                            **
**  Website/Contact: canpool@163.com / QQ1074867723                       **
**             Date: 17.11.11                                             **
**          Version: 0.1.0                                                **
****************************************************************************/
#ifndef QTXMLEXCEL_H
#define QTXMLEXCEL_H

#include <QObject>

class Worksheet{
public:
    Worksheet();
    ~Worksheet();
public:
    QString sheetName;
    QString titleName;
    QStringList headers;
    QList<int> columnsWidth;
    QStringList contents;   // list<rowContent>, rowContent: xx;xx;xx
};

class QtXmlExcel : public QObject
{
    Q_OBJECT
public:
    explicit QtXmlExcel(QObject *parent = 0);

    static QtXmlExcel *instance();
    static void close();

    void toExcel(QString fileName, QList<Worksheet> worksheets);

private:
    static QtXmlExcel *m_instance;
    QStringList m_xml;
};

#endif // QTXMLEXCEL_H
