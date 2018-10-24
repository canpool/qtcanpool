/****************************************************************************
**
** Qtitan Library by Developer Machines (Charts and Diagrams for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <QDomDocument>
#include "QtnChartPalette.h"

using namespace Qtitan;

ChartPalette::ChartPalette(QObject* parent)
    : ChartComponent(parent)
{
}

ChartPalette::~ChartPalette()
{
}

QColor ChartPalette::lightColor(int index) const
{
    if (m_arrEntries.size() == 0)
        return QColor(Qt::white);

    return m_arrEntries[index % m_arrEntries.size()].color(QPalette::Light);
}

QColor ChartPalette::darkColor(int index) const
{
    if (m_arrEntries.size() == 0)
        return QColor(Qt::black);

    return m_arrEntries[index % m_arrEntries.size()].color(QPalette::Dark);
}

int ChartPalette::getCount() const 
{
    return m_arrEntries.size();
}

QVector<QPalette>& ChartPalette::entries() 
{
    return m_arrEntries;
}
/*
static QColor stringToColor(const QString& strColor)
{
    int r = strColor.section(',', 0, 0).toInt();
    int g = strColor.section(',', 1, 1).toInt();
    int b = strColor.section(',', 2, 2).toInt();
    int alpha = 255;

    QString aStr = strColor.section(',', 3, 3);
    alpha = aStr.isEmpty() ? alpha : aStr.toInt();

    return QColor(r,g,b,alpha);
}
*/
void ChartPalette::extractXML(const QDomElement& el)
{
    m_arrEntries.clear();
    QPalette pal;
    QDomElement child = el.firstChildElement();
    while (!child.isNull()) 
    {
        QString strColor = child.text();
        if (!strColor.isEmpty())
        {
            QStringList strList = strColor.split(QLatin1Char(';'));
            for (int i = 0, count = strList.count(); count > i; i++)
                pal.setColor(i == 0 ? QPalette::Light : QPalette::Dark, strList.at(i));
            m_arrEntries.append(pal);
        }
        child = child.nextSiblingElement();
    }
}



