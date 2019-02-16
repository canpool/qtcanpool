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

#include "QtnChartFunnelSeries.h"
#include "QtnChartMath.h"
#include "QtnChartFunnelSeriesPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartTextPainter.h"


using namespace Qtitan;

/* ChartFunnelSeriesPrivate */
ChartFunnelSeriesPrivate::ChartFunnelSeriesPrivate()
{
    m_neckHeight = 50.0;
    m_neckHeightValue = false;
    m_neckWidth = 50;
    m_colorAlternate = true;

    m_depthPercent = 10;
}

ChartFunnelSeriesPrivate::~ChartFunnelSeriesPrivate()
{
}

void ChartFunnelSeriesPrivate::init()
{
    QTN_P(ChartFunnelSeries);
    m_border = new ChartBorder(&p);
    m_fillHelper = new ChartFillDrawHelper(&p);
    m_fillHelper->setFillStyle(Qt::LinearGradientPattern);
//    m_fillHelper->setGradientDirection(ChartFillDrawHelper::FromCenterVertical);
    p.setLabel(new ChartFunnelSeriesLabel());
}

void ChartFunnelSeriesPrivate::linkData()
{
    QTN_P(ChartFunnelSeries);
    m_data->setParent(p.parent());
    m_data->setSeries(&p);
}

void ChartFunnelSeriesPrivate::unlinkData()
{
    if (ChartArea* area= m_data->area())
    {
        if (isSeriesArea(area))
            return;
        m_data->setArea(Q_NULL);
    }
}

ChartArea* ChartFunnelSeriesPrivate::createArea(QObject* parent)
{
    return new ChartFunnelArea(parent);
}

bool ChartFunnelSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    if (!dynamic_cast<ChartFunnelArea*>(area))
        return false;

    return area->qtn_d().m_listDataTables.size() == 0;
}

ChartSeriesItem* ChartFunnelSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartFunnelSeriesItem(series, areaItem);
}


/* ChartFunnelSeries */
ChartFunnelSeries::ChartFunnelSeries()
    : ChartPyramidSeries(*new ChartFunnelSeriesPrivate(), Q_NULL)
{
    QTN_D(ChartFunnelSeries)
    d.init();
}

ChartFunnelSeries::~ChartFunnelSeries()
{
}

void ChartFunnelSeries::setNeckHeightInPercents(qreal height)
{
    QTN_D(ChartFunnelSeries)
    d.m_neckHeight = ChartMath::clamp(height, 0.0, 100.0);
    d.m_neckHeightValue = false;
    chartUpdate();
}

void ChartFunnelSeries::setNeckHeightInValues(qreal value)
{
    QTN_D(ChartFunnelSeries)
    d.m_neckHeight = value;
    d.m_neckHeightValue = true;
}

bool ChartFunnelSeries::isNeckHeightValue() const 
{
    QTN_D(const ChartFunnelSeries)
    return d.m_neckHeightValue;
}

qreal ChartFunnelSeries::neckHeight() const 
{
    QTN_D(const ChartFunnelSeries)
    return d.m_neckHeight;
}

void ChartFunnelSeries::setNeckWidth(int width)
{
    QTN_D(ChartFunnelSeries)
    d.m_neckWidth = ChartMath::clamp(width, 10, 100);
    chartUpdate();
}

int ChartFunnelSeries::neckWidth() const 
{
    QTN_D(const ChartFunnelSeries)
    return d.m_neckWidth;
}


/*!
    \class Qtitan::ChartFunnelSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartFunnelSeriesItem::ChartFunnelSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartPyramidSeriesItem(series, areaItem)
{
}

ChartFunnelSeriesItem::~ChartFunnelSeriesItem()
{
}

void ChartFunnelSeriesItem::calculatePointLayout(const QRect& boundingRect)
{
    ChartFunnelSeries* funnelSeries = qobject_cast<ChartFunnelSeries*>(series());

    qreal sum = 0.0;
    QRectF rectShape;
    int nonEmptyCount = 0;

    m_neckSize = QSize();

    if (!calculateSeriesParams(boundingRect, sum, nonEmptyCount, rectShape))
        return;

    m_neckSize.setWidth(qMax(10., rectShape.width() * 0.5 * (qreal)funnelSeries->neckWidth() / 100.0));
    m_neckSize.setHeight(funnelSeries->isNeckHeightValue() ? 0 : rectShape.height() * funnelSeries->neckHeight() / 100.0);

    QVector<ChartDataPoint*> dataPointsSorted;
    sortDataPoints(ChartPyramidSeriesPartItem::pntHeight, dataPointsSorted);

    qreal currOffset = 0.0;
    for (int i = dataPointsSorted.count() - 1; i >= 0; i--)
    {
        ChartDataPoint* dataPointNext = dataPointsSorted[i];
        int dataPointIndex = findDataPointIndex(dataPointNext);
        ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(dataPointIndex);
        qreal height = partItem->dataPointScreenPoint(ChartPyramidSeriesPartItem::pntHeight).x();

        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntOffsets, QPoint(currOffset, currOffset + height));
        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntLeftTop, rectShape.topLeft());
        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntRightBottom, rectShape.bottomRight());

        if (funnelSeries->isNeckHeightValue())
        {
            qreal value = dataPointNext->valueY();
            if (qFabs(value) <= funnelSeries->neckHeight())
            {
                m_neckSize.setHeight(m_neckSize.height() + height);
                m_neckSize.setHeight(m_neckSize.height() + m_allowedPointGap);
            }
        }

        currOffset += height + m_allowedPointGap;
    }

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPyramidSeriesPartItem* pointItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i);
        pointItem->calculateBoundingRect();
    }
}

void ChartFunnelSeriesItem::calculateLabelLayout()
{
    for (int i = 0, count = m_labelsItem->count(); i < count; i++)
        ((ChartSeriesLabelItem*)m_labelsItem->at(i))->calculateLayout();
}

QRectF ChartFunnelSeriesItem::internalBoundingRect() const
{
    return m_internalRect;
}

/* FunnelpointsItem */
class FunnelpointsItem : public GraphicsItem
{
public:
    FunnelpointsItem(QObject* parent) : GraphicsItem(parent) {}
    virtual void paint(QPainter* p);
};

void FunnelpointsItem::paint(QPainter* p)
{
    for (int i = m_listItem.count() - 1; i >= 0; i--)
        m_listItem[i]->paint(p);
}

void ChartFunnelSeriesItem::createPointsItem()
{
    m_pointsItem = new FunnelpointsItem(this);
    m_labelsItem = m_areaItem->labelsItem();

    for (int i = 0, count = m_data->count(); i < count; i++)
    {
        ChartDataPoint* point = m_data->at(i);
        if (ChartSeriesPointItem* pointItem = createSeriesPointItem(point, m_pointsItem))
        {
            if (ChartSeriesLabel* label = m_data->series()->label())
            {
                if (label->isVisible())
                    label->qtn_d().createItem(pointItem, m_labelsItem);
            }
        }
    }
}

ChartSeriesPointItem* ChartFunnelSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartFunnelSeriesPartItem(point, parentItem);
}

void ChartFunnelSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);
    ChartPyramidSeriesItem::paint(p);
    ChartPainter::painter()->unsetAntialiasing(p);
}

void ChartFunnelSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    Q_UNUSED(p)
    Q_UNUSED(boundingRect)
    Q_ASSERT(false);
/*
    if (ChartFunnelSeries* funnelSeries = qobject_cast<ChartFunnelSeries*>(series()))
    {
        QColor clrBorder;

        if (!funnelSeries->border()->color().isValid())
            clrBorder = funnelSeries->border()->color();
        else
            clrBorder  = funnelSeries->color().dark();

        funnelSeries->qtn_d().m_fillHelper->drawGradientPolygon(p, points, startColor, finalColor, clrBorder);
    }
*/
}

void ChartFunnelSeriesItem::drawLegendComponent(QPainter* p, QRect boundingRect, const QColor& startColor, 
    const QColor& finalColor, const QColor& clrBorder)
{
    if (ChartFunnelSeries* funnelSeries = qobject_cast<ChartFunnelSeries*>(series()))
    {
        boundingRect.adjust(1, 1, -1, -1);

        QVector<QPointF> points;

        int sz = boundingRect.height() / 3;
        int hsz = (boundingRect.width() - sz) * 0.5 - 1;

        points.append(QPointF(boundingRect.left(), boundingRect.top()));
        points.append(QPointF(boundingRect.right(), boundingRect.top()));

        points.append(QPointF(boundingRect.right() - hsz, boundingRect.bottom()));
        points.append(QPointF(boundingRect.left() + hsz, boundingRect.bottom()));

        ChartPainter::painter()->setAntialiasing(p);
        funnelSeries->qtn_d().m_fillHelper->drawGradientPolygon(p, points, startColor, finalColor);

        if (funnelSeries->qtn_d().m_border->isVisible())
            ChartPainter::painter()->drawBoundedPolygon(p, points, clrBorder, 1);
        ChartPainter::painter()->unsetAntialiasing(p);
    }
}


/*!
    \class Qtitan::ChartFunnelSeriesPartItem
    \inmodule QtitanChart
    \internal
*/
ChartFunnelSeriesPartItem::ChartFunnelSeriesPartItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartPyramidSeriesPartItem(point, parentItem)
{
}

ChartFunnelSeriesPartItem::~ChartFunnelSeriesPartItem()
{
}

QPointF ChartFunnelSeriesPartItem::markerPoint()
{
    if (ChartFunnelSeriesItem* funnelSeriesItem = qobject_cast<ChartFunnelSeriesItem*>(seriesItem()))
    {
        if (ChartFunnelSeries* funnelSeries = qobject_cast<ChartFunnelSeries*>(funnelSeriesItem->series()))
        {
            if (ChartFunnelSeriesLabel* label = (ChartFunnelSeriesLabel*)funnelSeries->label())
            {
                LabelPosition pos = label->position();
                if (pos == LabelRight || pos == LabelLeft)
                {
                    QRectF rectShape(dataPointScreenPoint(pntLeftTop), dataPointScreenPoint(pntRightBottom));
                    QPointF ptOffsets = dataPointScreenPoint(pntOffsets);

                    qreal depth = funnelSeries->isSeries3D() ? rectShape.height() * funnelSeries->depthPercent() / 100. : 0.0; 

                    QPointF pointLeft;
                    QPointF pointRight;
                    calculateFunnelPoints(rectShape, depth, funnelSeriesItem->m_neckSize, ptOffsets.x(), ptOffsets.y(), pointLeft, pointRight);

                    if (pos == LabelRight)
                        return pointRight;
                    return pointLeft;
                }
            }
        }
    }
    return m_rectBounds.center();
}

void ChartFunnelSeriesPartItem::calculateFunnelPoints(const QRectF& rectFunnel, qreal depth, const QSizeF& szNeckSize, 
    qreal y1, qreal y2, QPointF& pointLeft, QPointF& pointRight)
{
    QRectF rect = rectFunnel;
    QSizeF szNeck = szNeckSize;

    if (depth >= 1.0)
    {
        qreal depth2 = depth * 0.5;
        qreal height = rect.height();
        rect.adjust(0.0, depth2, 0.0, -depth2);

        qreal ratio = rect.height() / height;
        szNeck.setHeight(szNeck.height()* ratio);
        y1 *= ratio;
        y2 *= ratio;

        height = rect.height();
    }

    y1 += rect.top();
    y2 += rect.top();

    rect.setBottom(rect.bottom()-szNeck.height());

    qreal y = (y1 + y2) * 0.5;

    if (y >= rect.bottom())
    {
        pointLeft.setX(rect.center().x() - szNeck.width() * 0.5);
        pointLeft.setY(y);

        pointRight.setX(rect.center().x() + szNeck.width() * 0.5);
        pointRight.setY(y);

        return;
    }

    if (y2 > rect.bottom())
    {
        pointLeft.setX(rect.center().x() - szNeck.width() * 0.5);
        pointLeft.setY(rect.bottom());

        pointRight.setX(rect.center().x() + szNeck.width() * 0.5);
        pointRight.setY(rect.bottom());
        return;
    }

    qreal neckWidth2 = szNeck.width() * 0.5;
    qreal angle = ChartMath::angle(rect.width() * 0.5 - neckWidth2, rect.height());

    qreal yBottom = y2;

    qreal dy = (yBottom - y1);
    qreal dx = dy / qTan(angle);
    qreal dyBottom = rect.bottom() - y2;
    qreal dxBottom = neckWidth2 + (dx * dyBottom / dy);
    qreal dxTop = dxBottom + dx;
    qreal dx1 = (dxTop + dxBottom) * 0.5;

    pointLeft.setX(rect.center().x() - dx1);
    pointLeft.setY(y);

    pointRight.setX(rect.center().x() + dx1);
    pointRight.setY(y);
}

void ChartFunnelSeriesPartItem::paint(QPainter* p)
{
    if (ChartFunnelSeriesItem* funnelSeriesItem = qobject_cast<ChartFunnelSeriesItem*>(seriesItem()))
    {
        if (ChartFunnelSeries* funnelSeries = qobject_cast<ChartFunnelSeries*>(funnelSeriesItem->series()))
        {
            QRectF rectFunnel(dataPointScreenPoint(pntLeftTop), dataPointScreenPoint(pntRightBottom));

            m_isValidRect = rectFunnel.isValid();

            if (!m_isValidRect)
                return;

            rectFunnel = rectFunnel.normalized();
            QPointF ptOffsets = dataPointScreenPoint(pntOffsets);
            QSizeF szNeck = funnelSeriesItem->m_neckSize;

            int transparency = funnelSeries->transparency();
            QColor clrStart = startColor(); 
            clrStart.setAlpha(transparency);
            QColor clrFinal = finalColor();
            clrFinal.setAlpha(transparency);

            if (funnelSeries->isSeries3D())
            {
                qreal depth = rectFunnel.height() * funnelSeries->depthPercent() / 100.0; 

                QLinearGradient gradientLeft(rectFunnel.bottomLeft(), rectFunnel.bottomRight());
                gradientLeft.setColorAt(0.0, clrStart.darker());
                gradientLeft.setColorAt(0.5, clrStart.light());
                gradientLeft.setColorAt(1.0, clrStart.darker());

                ChartPainter::painter()->drawFunnel3D(p, rectFunnel, depth, szNeck.height(), szNeck.width(), 
                    gradientLeft, borderColor(), ptOffsets.x(), ptOffsets.y(), funnelSeries->isCircular(), clrFinal.dark(150));
            }
            else
            {
                QBrush brushFill = funnelSeries->qtn_d().m_fillHelper->brushGradient(rectFunnel, clrStart, clrFinal);
                ChartPainter::painter()->drawFunnel(p, rectFunnel, szNeck.height(), szNeck.width(), brushFill, 
                    borderColor(), ptOffsets.x(), ptOffsets.y());
            }
        }
    }
}

void ChartFunnelSeriesPartItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    if (ChartFunnelSeriesItem* item = qobject_cast<ChartFunnelSeriesItem*>(seriesItem()))
        item->drawLegendComponent(p, boundingRect, startColor(), finalColor(), borderColor());
}


/*!
    \class Qtitan::ChartFunnelArea
    \inmodule QtitanChart
    \internal
*/
ChartFunnelArea::ChartFunnelArea(QObject* parent)
    : ChartArea(*new ChartFunnelAreaPrivate, parent)
{
}

ChartFunnelArea::~ChartFunnelArea()
{
}

/*!
    \class Qtitan::ChartFunnelAreaPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAreaItem* ChartFunnelAreaPrivate::createItem(GraphicsItem* parent)
{
    QTN_P(ChartFunnelArea)
    return new ChartFunnelAreaItem(&p, parent);
}

void ChartFunnelAreaPrivate::calculateSeriesLayout(ChartAreaItem* item)
{
    if (item->count() > 0)
    {
        if (GraphicsItem* grItem = item->seriesItem())
        {
            for (int i = 0, count = grItem->count(); i < count; i++)
            {
                ChartFunnelSeriesItem* seriesItem = (ChartFunnelSeriesItem*)grItem->at(i);
                seriesItem->calculatePointLayout(item->boundingRect());
                seriesItem->calculateLabelLayout();
            }
        }
    }
}

/* ChartFunnelAreaItem */
ChartFunnelAreaItem::ChartFunnelAreaItem(ChartArea* area, GraphicsItem* parent)
    : ChartAreaItem(area, parent)
{
    m_seriesItem = new GraphicsItem(this);
    m_labelsItem = new GraphicsItem(this);
}


/* ChartFunnelSeriesLabel */
ChartFunnelSeriesLabel::ChartFunnelSeriesLabel()
    : ChartPyramidSeriesLabel(*new ChartPyramidSeriesLabelPrivate, Q_NULL)
{
}

ChartFunnelSeriesLabel::~ChartFunnelSeriesLabel()
{
}

