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
#include "QtnChartArea.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartDataPoint.h"
#include "QtnChartTitle.h"
#include "QtnChartTitlePrivate.h"
#include "QtnSeriesPrivate.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartAreaPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAreaPrivate::ChartAreaPrivate()
{
}

ChartAreaPrivate::~ChartAreaPrivate()
{
}

void ChartAreaPrivate::init()
{
    QTN_P(ChartArea);
    m_form = new ChartAreaForm(&p);
}

void ChartAreaPrivate::createTitleItem(GraphicsItem* parentItem)
{
    for (int i = 0, count = m_listTitles.count(); i < count; i++)
    {
        GraphicsItem* titleItem = m_listTitles.at(i)->qtn_d().createItem(parentItem);
        Q_UNUSED(titleItem)
    }
}

void ChartAreaPrivate::calculateTitleItem(QRect& rcChart, GraphicsItem* parentItem)
{
    QRect rc(rcChart);
    for (int i = 0, count = parentItem->count(); i < count; i++)
        ((ChartTitleItem*)parentItem->at(i))->calculateItem(rcChart, rc);
}

bool ChartAreaPrivate::seriesAdded(ChartDataTablePrivate* series)
{
    m_listDataTables.append(series);
    return true;
}

int ChartAreaPrivate::seriesRemoved(ChartDataTablePrivate* series)
{
    for (int i = 0, count = m_listDataTables.count(); i < count; i++)
    {
        if (m_listDataTables.at(i) == series)
        {
            m_listDataTables.removeAt(i);
            break;
        }
    }
    return m_listDataTables.count();
}

ChartAreaItem* ChartAreaPrivate::createItem(GraphicsItem* parent)
{
    Q_UNUSED(parent)
    return Q_NULL;
}

void ChartAreaPrivate::calculateSeriesLayout(ChartAreaItem* item)
{
    if (GraphicsItem* seriesItem = item->seriesItem())
    {
        for (int i = 0, count = seriesItem->count(); i < count; i++)
            ((ChartSeriesItem*)seriesItem->at(i))->calculateLayout();
    }
}


/* ChartArea */
ChartArea::ChartArea(QObject* parent)
    : ChartComponent(parent)
{
    QTN_INIT_PRIVATE(ChartArea);
    QTN_D(ChartArea)
    d.init();
}

ChartArea::ChartArea(ChartAreaPrivate& d, QObject* parent)
    : ChartComponent(parent)
{
    qtn_d_ptr = &d; 
    qtn_d_ptr->setPublic(this);
    d.init();
}

ChartArea::~ChartArea()
{
    clear();
    QTN_FINI_PRIVATE();
}

void ChartArea::clear()
{
    QTN_D(ChartArea)
    for (int i = d.m_listDataTables.count() - 1; i >= 0; i--)
        d.m_listDataTables.at(i)->setArea(Q_NULL);
}

void ChartArea::appendSeries(AbstractSeries* series)
{
    series->qtn_d().setArea(this);
}

void ChartArea::removeSeries(AbstractSeries* series)
{
    series->qtn_d().setArea(Q_NULL);
}

const QList<ChartTitle*>& ChartArea::titles() const
{
    QTN_D(const ChartArea)
    return d.m_listTitles;
}

const QBrush& ChartArea::backgroundBrush() const
{
    QTN_D(const ChartArea)
    return d.m_form->backgroundBrush();
}

void ChartArea::setBackgroundBrush(const QBrush& brush)
{
    QTN_D(ChartArea)
    d.m_form->setBackgroundBrush(brush);
}

const QColor& ChartArea::borderColor() const
{
    QTN_D(const ChartArea)
    return d.m_form->borderColor();
}

const QColor& ChartArea::backgroundColor() const
{
    QTN_D(const ChartArea)
    return d.m_form->backgroundColor();
}

void ChartArea::setBackgroundColor(const QColor& color)
{
    QTN_D(ChartArea)
    d.m_form->setBackgroundColor(color);
}

void ChartArea::setBorderColor(const QColor& color)
{
    QTN_D(ChartArea)
    d.m_form->setBorderColor(color);
}

/*!
    \class Qtitan::ChartAreaItem
    \inmodule QtitanChart
    \internal
*/
ChartAreaItem::ChartAreaItem(ChartArea* area, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_area = area;
    m_labelsItem = Q_NULL;
    m_seriesItem = Q_NULL;

    m_titlesItem = new GraphicsItem(this);
}

void ChartAreaItem::createItem()
{
    m_area->qtn_d().createTitleItem(m_titlesItem);
}

int ChartAreaItem::calculateAxisLabelSize()
{
    return -1;
}

void ChartAreaItem::calculateItem(QRect& boundingRect, int labelSize)
{
    Q_UNUSED(labelSize);
    m_area->qtn_d().calculateTitleItem(boundingRect, m_titlesItem);

    m_boundingRect = boundingRect;
    if (!m_seriesItem)
        return;

    m_area->qtn_d().calculateSeriesLayout(this);
}

void ChartAreaItem::updateRange()
{
}

void ChartAreaItem::paint(QPainter* p)
{
    GraphicsItem::paint(p);
}

QRect ChartAreaItem::boundingRect() const 
{
    return m_boundingRect;
}

ChartArea* ChartAreaItem::area() const 
{
    return m_area;
}

GraphicsItem* ChartAreaItem::labelsItem() const 
{
    return m_labelsItem;
}

GraphicsItem* ChartAreaItem::seriesItem() const 
{
    return m_seriesItem;
}
