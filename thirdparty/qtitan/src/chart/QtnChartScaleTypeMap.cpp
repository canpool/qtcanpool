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
#include <math.h>

#include "QtnChartAxis.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartScaleTypeMap.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartAxisPrivate.h"
#include "QtnSeriesPrivate.h"


using namespace Qtitan;

/*!
    \class Qtitan::ChartScaleTypeMap
    \inmodule QtitanChart
    \internal
*/
ChartScaleTypeMap::ChartScaleTypeMap(QObject* parent)
    : ChartComponent(parent)
{
}

ChartScaleTypeMap::~ChartScaleTypeMap()
{
}

ChartAxis* ChartScaleTypeMap::axis() const 
{
    return qobject_cast<ChartAxis*>(parent());
}

ChartScaleTypeMap* ChartScaleTypeMap::create(ChartAxis* axis)
{
    if (axis->qtn_d().m_scaleType == ChartScaleNumerical)
        return new ChartNumericalScaleTypeMap(axis);

    if (axis->qtn_d().m_scaleType == ChartScaleQualitative)
        return new ChartQualitativeScaleTypeMap(axis);

    if (axis->qtn_d().m_scaleType == ChartScaleDateTime)
        return new ChartDateTimeScaleTypeMap(axis);

    return Q_NULL;
}

void ChartScaleTypeMap::updateSeries(ChartAxisItem* item)
{
    ChartAxis* chartAxis = axis();

    QVector<ChartSeriesItem*>& arrSeries = item->m_listDataTables;
    for (int i = 0, count = arrSeries.size(); i < count; i++)
        updateSeries(arrSeries.at(i)->data());

    if (!chartAxis->isValuesAxis())
    {
        for (int i = 0, count = arrSeries.size(); i < count; i++)
        {
            ChartDataTablePrivate* series = arrSeries.at(i)->data();
            for (int j = 0, countJ = series->count(); j < countJ; j++)
                updateSeriesPointArgument(series->at(j));
        }
    }
}

void ChartScaleTypeMap::updateSeries(ChartDataTablePrivate* series)
{
    Q_UNUSED(series);
}

void ChartScaleTypeMap::updateSeriesPointArgument(ChartDataPoint* point)
{
    Q_UNUSED(point);
}

/*!
    \class Qtitan::ChartNumericalScaleTypeMap
    \inmodule QtitanChart
    \internal
*/
ChartNumericalScaleTypeMap::ChartNumericalScaleTypeMap(ChartAxis* axis)
    : ChartScaleTypeMap(axis)
{
    m_minValue = 0;
    m_maxValue = 0;
}

QString ChartNumericalScaleTypeMap::internalToValue(const QString& strFormat, qreal mark) const
{
    if (fabs(mark) < 1e-8)
        return QString("0");
    return QString(strFormat.isEmpty() ? "%1" : strFormat).arg((float)mark);
}

qreal ChartNumericalScaleTypeMap::valueToInternal(const QString& strValue) const
{
    return strValue.toDouble();
}

void ChartNumericalScaleTypeMap::updateSeriesPointArgument(ChartDataPoint* point)
{
    Q_UNUSED(point);
//    point->qtn_d().m_innerArg = point->qtn_d().m_argX;
}

/*!
    \class Qtitan::ChartDateTimeScaleTypeMap
    \inmodule QtitanChart
    \internal
*/
ChartDateTimeScaleTypeMap::ChartDateTimeScaleTypeMap(ChartAxis* axis)
    : ChartScaleTypeMap(axis)
{
}

ChartDateTimeScaleTypeMap::~ChartDateTimeScaleTypeMap()
{
}

qreal ChartDateTimeScaleTypeMap::valueToInternal(const QString& strValue) const
{
    Q_UNUSED(strValue);
    return 0;
}

void ChartDateTimeScaleTypeMap::updateSeriesPointArgument(ChartDataPoint* point)
{
    if (!point->qtn_d().m_strArgX.isEmpty())
    {
//        point->setIndex((QDate)d);
    }
    else
        point->setValueX(point->qtn_d().m_argX);
}

QString ChartDateTimeScaleTypeMap::internalToValue(const QString& strFormat, qreal mark) const
{
    Q_UNUSED(mark);
    if (strFormat.isEmpty())
    {
        QString str;
        return str;
    }
    else
    {
        QString str;
        return str;
    }
}

/*!
    \class Qtitan::ChartQualitativeScaleTypeMap
    \inmodule QtitanChart
    \internal
*/
ChartQualitativeScaleTypeMap::ChartQualitativeScaleTypeMap(ChartAxis* axis)
    : ChartScaleTypeMap(axis)
{
    m_storage = new QStringList;//new ChartKeep();
}

ChartQualitativeScaleTypeMap::~ChartQualitativeScaleTypeMap()
{
    delete m_storage;
}

int ChartQualitativeScaleTypeMap::fillNextsList(ChartDataTablePrivate* series, int i, QStringList& nexts, const QString& argument_i)
{
    nexts.clear();

    for (int j = i + 1; j < series->count(); j++)
    {
        QString argument_j = series->at(j)->qtn_d().m_strArgX;
        if (argument_j == argument_i)
            continue;

        if (nexts.indexOf(argument_j) >= 0)
            continue;

        int index = m_storage->indexOf(argument_j);
        if (index != -1)
            return index;

        nexts.append(argument_j);
    }
    return -1;
}

void ChartQualitativeScaleTypeMap::updateSeries(ChartDataTablePrivate* series)
{
    int i = 0;
    QStringList nexts;

    while (i < series->count())
    {
        QString argument_i = series->at(i)->qtn_d().m_strArgX;

        if (m_storage->indexOf(argument_i) != -1)
        {
            i++;
            continue;
        }

        int index = fillNextsList(series, i, nexts, argument_i);
        if (index == -1)
        {
            m_storage->append(argument_i);
            m_storage->append(nexts);
        }
        else
        {
            for (int i = 0; i < nexts.size(); i++)
                m_storage->insert(index + i, nexts[i]);
            m_storage->insert(index, argument_i);
        }
        i += (int)nexts.size() + 1;
    }
}

void ChartQualitativeScaleTypeMap::updateSeriesPointArgument(ChartDataPoint* point)
{
    point->setValueX(valueToInternal(point->qtn_d().m_strArgX));
}

qreal ChartQualitativeScaleTypeMap::valueToInternal(const QString& str) const
{
    return m_storage->indexOf(str);
}

QString ChartQualitativeScaleTypeMap::internalToValue(const QString& strFormat, qreal mark) const
{
    int index = (int)mark;

    if (!(index >= 0 && index < m_storage->size()))
        return QString();

    QString str = m_storage->at(index);

    if (strFormat.isEmpty())
        return str;

    return str;
}
