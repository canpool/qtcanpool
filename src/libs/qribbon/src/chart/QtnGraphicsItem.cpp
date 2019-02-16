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

#include "QtnChartPainter.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnGraphicsItem.h"
#include "QtnChart.h"
#include "QtnChartLegend.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartComponent.h"
#include "QtnChartTitle.h"
#include "QtnChartArea.h"
#include "QtnAbstractSeries.h"
#include "QtnChart2DSeries.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartTitlePrivate.h"

using namespace Qtitan;

/*!
    \class Qtitan::GraphicsItem
    \inmodule QtitanChart
    \internal
*/
GraphicsItem::GraphicsItem(QObject* parent)
    : QObject(parent)
{
    if (GraphicsItem* item = qobject_cast<GraphicsItem*>(parent))
        item->m_listItem.append(this);
}

GraphicsItem::~GraphicsItem()
{
    GraphicsItem* item = Q_NULL;
    foreach (item, m_listItem)
        delete item;

    m_listItem.clear();
}

int GraphicsItem::count() const
{
    return m_listItem.count();
}

GraphicsItem* GraphicsItem::at(int index) const
{
    return index >= 0 && index < m_listItem.count() ? m_listItem.at(index) : Q_NULL;
}

void GraphicsItem::paint(QPainter* p)
{
    for (int i = 0, count = m_listItem.count(); i < count; i++)
        m_listItem[i]->paint(p);
}

GraphicsItem* GraphicsItem::parentItem() const
{
    return qobject_cast<GraphicsItem*>(parent());
}

void GraphicsItem::appendItem(GraphicsItem* childItem)
{
    if (childItem)
    {
        m_listItem.append(childItem);
        childItem->setParent(this);
    }
}

GraphicsItem** GraphicsItem::children()
{
    return m_listItem.data();
}

bool GraphicsItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    for (int i = 0, count = m_listItem.count(); i < count; i++)
    {
        if (m_listItem[i]->hitTest(pt, hitInfo, hitInfoFlags))
            return true;
    }
    return false;
}



/*!
    \class Qtitan::Chart2DItem
    \inmodule QtitanChart
    \internal
*/
Chart2DItem::Chart2DItem(Chart* chart)
    : GraphicsItem(chart)
{
    m_chart = chart;
    m_legendItem = Q_NULL;
    m_titlesItem = Q_NULL;
    m_areaItem = Q_NULL;
}

Chart2DItem::~Chart2DItem()
{
}

ChartLegendItem* Chart2DItem::legendItem() const 
{ 
    return m_legendItem; 
}

void Chart2DItem::createItem()
{
    m_titlesItem = new GraphicsItem(this);

    m_legendItem = m_chart->legend()->qtn_d().createItem(this);

    m_areaItem = new GraphicsItem(this);

    for (int indexArea = 0, count = m_chart->areas().count(); count > indexArea; indexArea++)
    {
        ChartArea* area = m_chart->areas().at(indexArea);

        if (ChartAreaItem* areaItem = area->qtn_d().createItem(m_areaItem))
        {
            areaItem->createItem();

            for (int i = 0, countSeries = m_chart->series().count(); i < countSeries; i++)
            {
                AbstractSeries* series = m_chart->series().at(i);
                if (!series->isVisible())
                    continue;

                if (series->data()->area() != area)
                    continue;


                ChartSeriesItem* seriesItem = series->qtn_d().createItem((ChartDataTablePrivate*)series->data(), areaItem);

                if (seriesItem)
                {
                    seriesItem->createPointsItem();

                    if (m_legendItem)
                        seriesItem->addToLegend(m_legendItem);
                }
            }
            areaItem->updateRange();
        }
    }

    for (int i = 0, count = m_chart->titles().count(); i < count; i++)
    {
        GraphicsItem* titleItem = m_chart->titles().at(i)->qtn_d().createItem(m_titlesItem);
        Q_UNUSED(titleItem)
    }
}

static void calculateItem(GraphicsItem* areaItems, const QRect& boundingRect, int gap, int size, Qt::Orientation orientation, int labelSize)
{
    for (int i = 0, count = areaItems->count(); i < count; i++)
    {
        ChartAreaItem* areaItem = (ChartAreaItem*)areaItems->at(i);

        QRect rectArea = orientation == Qt::Horizontal ?
            QRect(QPoint(boundingRect.left() + i * (size + gap), boundingRect.top()), QPoint(boundingRect.left() + i * (size + gap) + size, boundingRect.bottom())) :
        QRect(QPoint(boundingRect.left(), boundingRect.top() + i * (size + gap)), QPoint(boundingRect.right(), boundingRect.top() + i * (size + gap) + size));

        areaItem->calculateItem(rectArea, labelSize);
    }
}

void Chart2DItem::calculateItem(const QRect& rect)
{
    QRect boundingRect = rect;
    m_boundingRect = boundingRect;

    if (m_chart->isBorderVisible())
    {
        int thickness = m_chart->borderThickness();
        boundingRect.adjust(thickness, thickness, -thickness, -thickness);
    }

    QRect rc(boundingRect);
    for (int i = 0, count = m_titlesItem->count(); i < count; i++)
    {
        ChartTitleItem* titleItem = (ChartTitleItem*)m_titlesItem->at(i);
        titleItem->calculateItem(rc, boundingRect);
    }
    boundingRect.adjust(10, 10, -10, -10);

    if (m_legendItem)
        m_legendItem->calculateItem(boundingRect);

    int count = m_areaItem->count();
    if (count == 0)
        return;

    int gap = m_chart->areasSpacing();
    Qt::Orientation orientation = m_chart->areasOrientation();

    int size = orientation == Qt::Horizontal ? (boundingRect.width() - gap * (count - 1)) / count :
        (boundingRect.height() - gap * (count - 1)) / count;

    ::calculateItem(m_areaItem, boundingRect, gap, size, orientation, -1);

    if (orientation == Qt::Vertical)
    {
        int labelSize = -1;
        if (count > 1)
        {
            for (int i = 0; i < count; i++)
            {
                ChartAreaItem* areaItem = (ChartAreaItem*)m_areaItem->at(i);
                labelSize = qMax(labelSize, areaItem->calculateAxisLabelSize());
            }
        }
        if (labelSize != -1)
            ::calculateItem(m_areaItem, boundingRect, gap, size, orientation, labelSize);
    }
}

void Chart2DItem::paint(QPainter* p)
{
    int xround, yround;
    xround = yround = m_chart->borderRounded();

    const QBrush& brush = m_chart->backgroundBrush();
    if (brush.style() != Qt::NoBrush)
        ChartPainter::painter()->drawGradientBackground(p, m_boundingRect, brush, xround, yround);
    else
        m_chart->qtn_d().fillHelper()->drawGradientRect(p, m_boundingRect, xround, yround);

    if (m_chart->isBorderVisible() && m_chart->borderThickness() > 0)
    {   
        QPainter::RenderHints hints;
        if (xround > 0)
        {
            hints = p->renderHints();
            p->setRenderHint(QPainter::Antialiasing, true);
            ChartPainter::painter()->drawInnerBorder(p, m_boundingRect, m_chart->borderColor(), 
                m_chart->borderThickness(), xround, yround);
            p->setRenderHint(QPainter::Antialiasing, false);
            p->setRenderHints(hints, true);
        }
        else
            ChartPainter::painter()->drawInnerBorder(p, m_boundingRect.adjusted(0, 0, -1, -1), 
            m_chart->borderColor(), m_chart->borderThickness());
    }

    if (m_areaItem)
        m_areaItem->paint(p);
    
    if (m_legendItem)
        m_legendItem->paint(p);

    if (m_titlesItem)
        m_titlesItem->paint(p);
}

bool Chart2DItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if (!m_boundingRect.contains(pt))
    {
        hitInfo->m_hitInfo = ChartHitInfo::HitNone;
        return true;
    }

    if (m_areaItem && m_areaItem->hitTest(pt, hitInfo, hitInfoFlags))
        return true;

    if (m_legendItem && m_legendItem->hitTest(pt, hitInfo, hitInfoFlags))
        return true;

    if (m_titlesItem && m_titlesItem->hitTest(pt, hitInfo, hitInfoFlags))
        return true;

    hitInfo->m_hitInfo = ChartHitInfo::HitChartArea;
    return true;
}
