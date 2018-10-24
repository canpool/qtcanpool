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
#include <QPainter>

#include "QtnChartBubbleSeries.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartMarker.h"
#include "QtnChartAxis.h"
#include "QtnChartMath.h"


using namespace Qtitan;


/* ChartBubbleSeriesPrivate */
ChartBubbleSeriesPrivate::ChartBubbleSeriesPrivate()
{
    m_colorAlternate = true;
    m_transparency = 175;
    m_minSize = 1.5;
    m_maxSize = 3.5;
}

ChartBubbleSeriesPrivate::~ChartBubbleSeriesPrivate()
{
}

void ChartBubbleSeriesPrivate::init_1()
{
    QTN_P(ChartBubbleSeries)
    p.setObjectName("ChartBubbleSeries");
    p.setLabel(new ChartBubbleSeriesLabel());
}

ChartSeriesItem* ChartBubbleSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartBubbleSeriesItem(series, areaItem);
}

void ChartBubbleSeriesPrivate::correctAxisSideMargins(ChartAxis* axis, qreal /*minValue*/, qreal /*maxValue*/, qreal& correction)
{
    if (!axis->isValuesAxis())
        correction = qMax(correction, m_maxSize * 0.5);
}

int ChartBubbleSeriesPrivate::seriesPointValueIndex() const 
{
    return 1;
}


/* ChartBubbleSeries */
ChartBubbleSeries::ChartBubbleSeries()
    : ChartPointSeries(*new ChartBubbleSeriesPrivate, Q_NULL)
{
    QTN_D(ChartBubbleSeries);
    d.init_1();
}

ChartBubbleSeries::~ChartBubbleSeries()
{
}

int ChartBubbleSeries::transparency() const 
{
    QTN_D(const ChartBubbleSeries)
    return d.m_transparency;
}

void ChartBubbleSeries::setTransparency(int transparency) 
{
    QTN_D(ChartBubbleSeries)
    d.m_transparency = transparency;
    chartUpdate();
}

qreal ChartBubbleSeries::minSize() const 
{
    QTN_D(const ChartBubbleSeries)
    return d.m_minSize;
}

void ChartBubbleSeries::setMinSize(qreal minSize) 
{
    QTN_D(ChartBubbleSeries)
    d.m_minSize = minSize;
    chartUpdate();
}

qreal ChartBubbleSeries::maxSize() const 
{
    QTN_D(const ChartBubbleSeries)
    return d.m_maxSize;
}

void ChartBubbleSeries::setMaxSize(qreal maxSize) 
{
    QTN_D(ChartBubbleSeries)
    d.m_maxSize = maxSize;
    chartUpdate();
}

/* ChartBubbleSeriesItem */
ChartBubbleSeriesItem::ChartBubbleSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartPointSeriesItem(series, areaItem)
{
    m_minValue = 0.0;
    m_maxValue = 0.0;
}

void ChartBubbleSeriesItem::paint(QPainter* p)
{
    if (ChartArea2DItem* item = qobject_cast<ChartArea2DItem*>(areaItem()))
    {
        if (ChartAreaFormItem* formItem = item->areaFormItem())
        {
            QRect bounds = formItem->bounds();
            p->save();
            p->setClipRect(bounds);

            ChartSeriesItem::paint(p);

            p->restore();
        }
    }
}

ChartSeriesPointItem* ChartBubbleSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartBubbleSeriesPointItem(point, parentItem);
}

void ChartBubbleSeriesItem::createPointsItem()
{
    ChartSeriesItem::createPointsItem();

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartBubbleSeriesPointItem* pointItem = (ChartBubbleSeriesPointItem*)m_pointsItem->at(i);

        qreal size = pointItem->point()->valueY1();

        if (i == 0)
            m_minValue = m_maxValue = size;
        else
        {
            m_minValue = qMin(m_minValue, size);
            m_maxValue = qMax(m_maxValue, size);
        }
    }
}

/* ChartBubbleSeriesPointItem */
ChartBubbleSeriesPointItem::ChartBubbleSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartPointSeriesPointItem(point, parentItem)
{
}

QColor ChartBubbleSeriesPointItem::startColorPoint() const
{
    QColor color(startColor());
    color.setAlpha(((ChartBubbleSeries*)seriesItem()->series())->transparency());
    return color;
}

QColor ChartBubbleSeriesPointItem::finalColorPoint() const
{
    QColor color(finalColor());
    color.setAlpha(((ChartBubbleSeries*)seriesItem()->series())->transparency());
    return color;
}

void ChartBubbleSeriesPointItem::paint(QPainter* p)
{
    if (ChartBubbleSeriesItem* item = (ChartBubbleSeriesItem*)seriesItem())
    {
        if (ChartBubbleSeries* series = (ChartBubbleSeries*)item->series())
        {
            if (ChartAxisItem* axisItem = item->axisItemX())
            {
                qreal size = 0.0;
                qreal value = m_point->valueY1();

                if (series->minSize() < ChartMath::QTN_EPSILON_1 && item->m_maxValue > ChartMath::QTN_EPSILON_1)
                    size = value * series->maxSize() / item->m_maxValue;
                else if (qFabs(item->m_maxValue - item->m_minValue) < ChartMath::QTN_EPSILON_1)
                    size = series->minSize();
                else
                    size = series->minSize() + (value - item->m_minValue) * (series->maxSize() - series->minSize())  / (item->m_maxValue - item->m_minValue);

                if (qFabs(size) < ChartMath::QTN_EPSILON_1) 
                    size = 0.5;

                QColor startColor = startColorPoint();
                QColor colorBorder = startColor.dark();

                ((ChartPointSeries*)series)->qtn_d().m_marker->paint(p, screenPoint(), 
                    (int)axisItem->distanceToPoint(size), startColor, finalColorPoint(), colorBorder);
            }
        }
    }
}


/* ChartBubbleSeriesLabel */
ChartBubbleSeriesLabel::ChartBubbleSeriesLabel(QObject* p)
    : ChartSeriesLabel(*new ChartBubbleSeriesLabelPrivate, p)
{
}

ChartBubbleSeriesLabel::~ChartBubbleSeriesLabel()
{
}

/* ChartBubbleSeriesLabelPrivate */
ChartBubbleSeriesLabelPrivate::ChartBubbleSeriesLabelPrivate()
{
}

ChartBubbleSeriesLabelPrivate::~ChartBubbleSeriesLabelPrivate()
{
}

GraphicsItem* ChartBubbleSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
{
    QTN_P(ChartBubbleSeriesLabel)
    return new ChartBubbleSeriesLabelItem(&p, pointItem, parentItem);
}

/* ChartBubbleSeriesLabelItem */
ChartBubbleSeriesLabelItem::ChartBubbleSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartArea2DSeriesLabelItem(label, pointItem, parentItem)
{
}

ChartBubbleSeriesLabelItem::~ChartBubbleSeriesLabelItem()
{
}

void ChartBubbleSeriesLabelItem::paint(QPainter* p)
{
    QPointF anchorPoint = ((ChartPointSeriesPointItem*)m_pointItem)->screenPoint();
    ChartArea2DSeriesLabelItem::paint(p, anchorPoint);
}

void ChartBubbleSeriesLabelItem::calculateLayout()
{
    QPointF anchorPoint = ((ChartPointSeriesPointItem*)m_pointItem)->screenPoint();
    ChartArea2DSeriesLabelItem::calculateLayout(anchorPoint);
}
