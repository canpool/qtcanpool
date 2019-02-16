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
#include <QVariant>
#include "QtnChartDataPointPrivate.h"
#include "QtnSeriesPrivate.h"
#include "QtnAbstractSeries.h"
#include "QtnChart.h"
#include "QtnChartArea.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartPalette.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartAreaPrivate.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartDataPointPrivate
    \inmodule QtitanChart
    \internal
*/
ChartDataPointPrivate::ChartDataPointPrivate()
{
    memset(&m_values, 0, sizeof(m_values));
}

ChartDataPointPrivate::~ChartDataPointPrivate()
{
}

void ChartDataPointPrivate::init()
{
    m_parent = Q_NULL;
    m_palIndex = 0;
    m_index = -1;
    m_colorPoint = QColor();
    m_special = false;
    if (m_strArgX.isEmpty())
        m_strArgX = QString::number(m_argX);
    else
    {
        m_argX = m_strArgX.toDouble();
    }
}

/* ChartDataPoint */
ChartDataPoint::ChartDataPoint(qreal xvalue, qreal yvalue, const QString& label, const QColor& color)
{
    QTN_INIT_PRIVATE(ChartDataPoint);
    QTN_D(ChartDataPoint)

    d.m_argX = xvalue;
    d.m_values[0] = yvalue;
    d.m_length = 1;

    d.m_labelText = label;
    d.m_colorPoint = color;

    d.init();
}

ChartDataPoint::ChartDataPoint(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label, const QColor& color)
{
    QTN_INIT_PRIVATE(ChartDataPoint);
    QTN_D(ChartDataPoint)

    d.m_argX = xvalue;
    d.m_values[0] = yvalue;
    d.m_values[1] = yvalue1;
    d.m_length = 2;

    d.m_labelText = label;
    d.m_colorPoint = color;

    d.init();
}

ChartDataPoint::~ChartDataPoint()
{
    QTN_FINI_PRIVATE();
}

qreal ChartDataPoint::valueX() const
{
    QTN_D(const ChartDataPoint)
    return d.m_argX;
}

void ChartDataPoint::setValueX(qreal value)
{
    QTN_D(ChartDataPoint)
    d.m_argX = value;
}

qreal ChartDataPoint::valueY() const
{
    QTN_D(const ChartDataPoint)
    if (0 >= d.m_length)
        return 0.0;

    return d.m_values[0];
}

void ChartDataPoint::setValueY(qreal value)
{
    QTN_D(ChartDataPoint)
    if (1 >= d.m_length)
        d.m_values[0] = value;
}

qreal ChartDataPoint::valueY1() const
{
    QTN_D(const ChartDataPoint)
        if (1 > d.m_length)
            return 0.0;

    return d.m_values[1];
}

void ChartDataPoint::setValueY1(qreal value)
{
    QTN_D(ChartDataPoint)
        if (1 > d.m_length)
            d.m_values[1] = value;
}

void ChartDataPoint::setLegendText(const QString& legendText) 
{
    QTN_D(ChartDataPoint)
    d.m_legentText = legendText;
    if (d.m_parent)
        d.m_parent->chartUpdate();
}

QString ChartDataPoint::legendText() const 
{
    QTN_D(const ChartDataPoint)
    return d.m_legentText;
}

QColor ChartDataPoint::color() const 
{
    QTN_D(const ChartDataPoint)
    return d.m_colorPoint;
}

void ChartDataPoint::setColor(const QColor& clr) 
{
    QTN_D(ChartDataPoint)
    d.m_colorPoint = clr;
    if (d.m_parent)
        d.m_parent->chartUpdate();
}

QString ChartDataPoint::tooltip() const 
{
    QTN_D(const ChartDataPoint)
    return d.m_tooltipText;
}

void ChartDataPoint::setTooltip(const QString& tooltip) 
{
    QTN_D(ChartDataPoint)
    d.m_tooltipText = tooltip;
}

QString ChartDataPoint::label() const 
{
    QTN_D(const ChartDataPoint)
    return d.m_labelText;
}

void ChartDataPoint::setLabel(const QString& label) 
{
    QTN_D(ChartDataPoint)
    d.m_labelText = label;
    if (d.m_parent)
        d.m_parent->chartUpdate();
}

AbstractSeries* ChartDataPoint::series() const
{
    QTN_D(const ChartDataPoint)
    return d.m_parent->series();
}

/*!
    \class Qtitan::ChartDataTablePrivate
    \inmodule QtitanChart
    \internal
*/
ChartDataTablePrivate::ChartDataTablePrivate(QObject* parent)
    : ChartComponent(parent)
{
    m_impl = Q_NULL;
    m_area = Q_NULL;
    m_argumentScaleType = ChartScaleQualitative;
    m_valueScaleType = ChartScaleNumerical;
    m_palIndex = 0;
    //    m_visible = true;
}

ChartDataTablePrivate::~ChartDataTablePrivate()
{
    clear();
    setArea(Q_NULL);
}

ChartDataPoint* ChartDataTablePrivate::addAxisPointY(double value, const QString& axisLabel)
{
//    ChartDataPoint* point = new ChartDataPoint(value, axisLabel);
    ChartDataPoint* point = new ChartDataPoint(0.0, value);
    point->qtn_d().m_strArgX = axisLabel;
    point->qtn_d().m_parent = this;
    m_data.append(point);
    return point;
}

ChartDataPoint* ChartDataTablePrivate::addY(qreal value, bool special, const QString& label, const QColor& color)
{
//    ChartDataPoint* point = new ChartDataPoint(value, label, color);
    ChartDataPoint* point = new ChartDataPoint(0.0, value, label, color);
    point->qtn_d().m_special = special;
    point->qtn_d().m_parent = this;
    m_data.append(point);
    return point;
}

ChartDataPoint* ChartDataTablePrivate::addXY(qreal xvalue, qreal yvalue, const QString& label, const QColor& color)
{
    ChartDataPoint* point = new ChartDataPoint(xvalue, yvalue, label, color);
    point->qtn_d().m_parent = this;
    m_data.append(point);
    return point;
}

ChartDataPoint* ChartDataTablePrivate::addXYY1(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label, const QColor& color)
{
    ChartDataPoint* point = new ChartDataPoint(xvalue, yvalue, yvalue1, label, color);
    point->qtn_d().m_parent = this;
    m_data.append(point);
    return point;
}

ChartDataPoint* ChartDataTablePrivate::addY(const QVariant& value, const QString& label, const QColor& color)
{
    return addY(value.toDouble(), false, label, color);
}

ChartDataPoint* ChartDataTablePrivate::addXY(const QVariant& xvalue, const QVariant& yvalue, const QString& label, const QColor& color)
{
    return addXY(xvalue.toDouble(), yvalue.toDouble(), label, color);
}

AbstractSeries* ChartDataTablePrivate::setSeries(AbstractSeries* series)
{
    if (!series)
        return series;

    if (m_impl)
    {
        delete m_impl;
        m_impl = Q_NULL;
    }

    m_impl = series;

    getChartObject()->seriesImplChanged(this);
    chartUpdate();
    return series;
}

QColor ChartDataTablePrivate::color() const
{
    AbstractSeries* aSeries = series();
    if (aSeries->color().isValid())
        return ChartTheme::lightColor(aSeries->color());
    return ChartTheme::themeTopParent(this)->chartPalette()->darkColor(paletteIndex());
}

bool ChartDataTablePrivate::setArea(ChartArea* area)
{
    if (area == m_area)
        return true;

    if (area)
    {
        if (!m_impl)
            return false;

        if (!m_impl->qtn_d().isSeriesArea(area))
            return false;
    }

    if (m_area)
    {
        m_area->qtn_d().seriesRemoved(this);
        m_area = Q_NULL;
    }

    if (area)
    {
        if (!area->qtn_d().seriesAdded(this))
            return false;
    }

    m_area = area;

    if (area)
        chartUpdate();

    return true;
}

int ChartDataTablePrivate::count() const
{
    return m_data.count();
}

void ChartDataTablePrivate::append(ChartDataPoint* data)
{
    data->qtn_d().m_parent = this;
    m_data.append(data);
}

ChartDataPoint* ChartDataTablePrivate::at(int index) const
{
    if (index >= 0 && index < m_data.count())
        return m_data.at(index);
    return Q_NULL;
}

int ChartDataTablePrivate::indexOf(ChartDataPoint* point) const
{
    return m_data.indexOf(point);
}

void ChartDataTablePrivate::clear()
{
    for (int i = m_data.size() - 1; i >= 0; i--)
    {
        ChartDataPoint* point = m_data.at(i);
        m_data.remove(i);
        delete point;
    }
}    

AbstractSeries* ChartDataTablePrivate::series() const 
{
    return m_impl;
}

void ChartDataTablePrivate::setArgScaleType(ChartScaleType scaleType) 
{
    m_argumentScaleType = scaleType;
    chartUpdate();
}

ChartScaleType ChartDataTablePrivate::argScaleType() const 
{
    return m_argumentScaleType;
}

void ChartDataTablePrivate::setValueScaleType(ChartScaleType scaleType) 
{
    m_valueScaleType = scaleType;
    chartUpdate();
}

ChartScaleType ChartDataTablePrivate::valueScaleType() const 
{
    return m_valueScaleType;
}

void ChartDataTablePrivate::setPaletteIndex(int index)
{
    m_palIndex = index;
}

int ChartDataTablePrivate::paletteIndex() const 
{
    return m_palIndex;
}

void ChartDataTablePrivate::setName(const QString& name) 
{
    m_strName = name;
    chartUpdate();
}

QString ChartDataTablePrivate::name() const 
{
    return m_strName;
}

void ChartDataTablePrivate::setLegendText(const QString& legendText) 
{
    m_strLegendText = legendText;
    chartUpdate();
}

QString ChartDataTablePrivate::legendText() const 
{
    return m_strLegendText;
}

void ChartDataTablePrivate::setPointPaletteIndex(int indexPnt, int indexPal)
{
    if (ChartDataPoint* point = at(indexPnt))
        point->qtn_d().m_palIndex = indexPal;
}

void ChartDataTablePrivate::setPointLegendFormat(const QString& strFormat) 
{
    m_strPointLegendFormat = strFormat;
    chartUpdate();
}

QString ChartDataTablePrivate::pointLegendFormat() const 
{
    return m_strPointLegendFormat;
}

ChartArea* ChartDataTablePrivate::area() const 
{
    return m_area;
}

