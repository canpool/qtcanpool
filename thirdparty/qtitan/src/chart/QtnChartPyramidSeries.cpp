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

#include "QtnChartPyramidSeries.h"
#include "QtnChartMath.h"
#include "QtnChartPyramidSeriesPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartTextPainter.h"


using namespace Qtitan;

/* ChartPieSeriesBasePrivate */
ChartPyramidSeriesPrivate::ChartPyramidSeriesPrivate()
{
    m_data = new ChartDataTablePrivate();
    m_isSeries3D = false;
    m_pointGap = 4;
    m_colorAlternate = true;
    m_isCircular = false;
    m_transparency = 255;
    m_depthPercent = 10;
    m_angle = 0.0;
}

ChartPyramidSeriesPrivate::~ChartPyramidSeriesPrivate()
{
}

void ChartPyramidSeriesPrivate::init()
{
    QTN_P(ChartPyramidSeries);
    m_border = new ChartBorder(&p);
    m_fillHelper = new ChartFillDrawHelper(&p);
    m_fillHelper->setFillStyle(Qt::LinearGradientPattern);
//    m_fillHelper->setGradientDirection(ChartFillDrawHelper::FromCenterVertical);
    p.setLabel(new ChartPyramidSeriesLabel());
}

void ChartPyramidSeriesPrivate::linkData()
{
    QTN_P(ChartPyramidSeries);
    m_data->setParent(p.parent());
    m_data->setSeries(&p);
}

void ChartPyramidSeriesPrivate::unlinkData()
{
    if (ChartArea* area= m_data->area())
    {
        if (isSeriesArea(area))
            return;
        m_data->setArea(Q_NULL);
    }
}

bool ChartPyramidSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    if (qobject_cast<ChartPyramidArea*>(area) == NULL)
        return false;
    return area->qtn_d().m_listDataTables.size() == 0;
}

ChartArea* ChartPyramidSeriesPrivate::createArea(QObject* parent)
{
    return new ChartPyramidArea(parent);
}

ChartSeriesItem* ChartPyramidSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartPyramidSeriesItem(series, areaItem);
}


/* ChartPyramidSeries */
ChartPyramidSeries::ChartPyramidSeries()
    : AbstractSeries(*new ChartPyramidSeriesPrivate, Q_NULL)
{
    QTN_D(ChartPyramidSeries);
    d.init();
}

ChartPyramidSeries::ChartPyramidSeries(ChartPyramidSeriesPrivate& d, QObject* parent)
    : AbstractSeries(d, parent)
{
    d.init();
}

ChartPyramidSeries::~ChartPyramidSeries()
{
}

void ChartPyramidSeries::setSeries3D(bool series3D)
{
    QTN_D(ChartPyramidSeries)
    if (d.m_isSeries3D != series3D)
    {
        d.m_isSeries3D = series3D;
        chartUpdate();
    }
}

bool ChartPyramidSeries::isSeries3D() const
{
    QTN_D(const ChartPyramidSeries)
    return d.m_isSeries3D;
}

void ChartPyramidSeries::setDepthPercent(int depthPercent)
{
    QTN_D(ChartPyramidSeries)
    if (isCircular())
        depthPercent = 10.0;
    d.m_depthPercent = ChartMath::clamp(depthPercent, 1, 45);
    chartUpdate();
}

int ChartPyramidSeries::depthPercent() const 
{
    QTN_D(const ChartPyramidSeries)
    return d.m_depthPercent;
}

void ChartPyramidSeries::setRotation(qreal angle)
{
    QTN_D(ChartPyramidSeries)
    if ( d.m_angle != angle )
    {
        d.m_angle = angle;

        if (d.m_angle < -90.0)
            d.m_angle += 180.0;
        else if (d.m_angle > 90.0)
            d.m_angle -= 180.0;
        chartUpdate();
    }
}

qreal ChartPyramidSeries::rotation() const 
{
    QTN_D(const ChartPyramidSeries)
    return d.m_angle;
}

void ChartPyramidSeries::setCircular(bool circular)
{
    QTN_D(ChartPyramidSeries)
    if (d.m_isCircular != circular)
    {
        d.m_isCircular = circular;

        if (d.m_isCircular)
            d.m_depthPercent = 10;

        chartUpdate();
    }
}

bool ChartPyramidSeries::isCircular() const 
{
    QTN_D(const ChartPyramidSeries)
    return d.m_isCircular;
}

int ChartPyramidSeries::transparency() const
{
    QTN_D(const ChartPyramidSeries)
    return d.m_transparency;
}

void ChartPyramidSeries::setTransparency(int transparency)
{
    QTN_D(ChartPyramidSeries)
    if (d.m_transparency != transparency)
    {
        d.m_transparency = transparency;
        chartUpdate();
    }
}

int ChartPyramidSeries::pointGap() const 
{
    QTN_D(const ChartPyramidSeries);
    return d.m_pointGap;
}

void ChartPyramidSeries::setPointGap(int pointGap) 
{
    QTN_D(ChartPyramidSeries);
    if (d.m_pointGap != pointGap)
    {
        d.m_pointGap = pointGap;
        chartUpdate();
    }
}

ChartDataPoint* ChartPyramidSeries::add(qreal value, const QString& label)
{
    QTN_D(ChartPyramidSeries);
    ChartDataPoint* pnt = d.m_data->addAxisPointY(value, tr(""));
    pnt->setLabel(label);
    return pnt;
}

int ChartPyramidSeries::count() const
{
    QTN_D(const ChartPyramidSeries);
    return d.m_data->count();
}

ChartDataTablePrivate* ChartPyramidSeries::data() const
{
    QTN_D(const ChartPyramidSeries);
    return d.m_data;
}


/* ChartPyramidSeriesItem */ 
ChartPyramidSeriesItem::ChartPyramidSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartSeriesItem(series, areaItem)
{
    m_allowedPointGap = 0;
}

ChartPyramidSeriesItem::~ChartPyramidSeriesItem()
{
}

void ChartPyramidSeriesItem::calculatePointLayout(const QRect& boundingRect)
{
    qreal sum = 0;
    QRectF rectShape;
    int nonEmptyCount = 0;
    if (!calculateSeriesParams(boundingRect, sum, nonEmptyCount, rectShape))
        return;

    QVector<ChartDataPoint*> dataPointsSorted;
    sortDataPoints(ChartPyramidSeriesPartItem::pntHeight, dataPointsSorted);

    qreal currOffset = 0.0;
    for (int i = 0, count = dataPointsSorted.count(); i < count; i++)
    {
        ChartDataPoint* dataPointNext = dataPointsSorted[i];
        int dataPointIndex = findDataPointIndex(dataPointNext);
        ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(dataPointIndex);
        qreal height = partItem->dataPointScreenPoint(ChartPyramidSeriesPartItem::pntHeight).x();

        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntOffsets, QPoint(currOffset, currOffset + height));
        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntLeftTop, rectShape.topLeft());
        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntRightBottom, rectShape.bottomRight());
        currOffset += height + m_allowedPointGap;
    }

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPyramidSeriesPartItem* pointItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i);
        pointItem->calculateBoundingRect();
    }
}

void ChartPyramidSeriesItem::calculateLabelLayout()
{
    for (int i = 0, count = m_labelsItem->count(); i < count; i++)
        ((ChartSeriesLabelItem*)m_labelsItem->at(i))->calculateLayout();
}

QRectF ChartPyramidSeriesItem::internalBoundingRect() const
{
    return m_internalRect;
}

ChartSeriesPointItem* ChartPyramidSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPyramidSeriesPartItem(point, parentItem);
}

bool ChartPyramidSeriesItem::calculateSeriesParams(const QRect& rectArea, qreal& sum, int& nonEmptyCount, QRectF& rectShape)
{
    ChartPyramidSeries* pyramidSeries = qobject_cast<ChartPyramidSeries*>(series());

    sum = 0.0;
    nonEmptyCount = 0;
    rectShape = QRectF();

    bool outside = false;
    qreal maxDistance = 15.0;

    bool rightLabels = false;
    bool leftLabels = false;

    m_maxDataLabelSize = QRectF();
    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        if (ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i))
        {
            if (ChartDataPoint* point = partItem->point())
            {
                qreal val = point->valueY();
                if (val < 0.0)
                    continue;

                nonEmptyCount++;

                sum += qFabs(val);

                if (ChartPyramidSeriesLabel* label = (ChartPyramidSeriesLabel*)pyramidSeries->label())
                {
                    QString strDPLabel(label->pointLabel(partItem->point()));
                    ChartTextPainter painter(strDPLabel, label);
                    QSizeF szDataLabelSize = painter.size();

                    m_maxDataLabelSize.setWidth(qMax(m_maxDataLabelSize.width(), szDataLabelSize.width()));
                    m_maxDataLabelSize.setHeight(qMax(m_maxDataLabelSize.height(), szDataLabelSize.height()));

                    if (label->isVisible())
                    {
            //            maxDistance = qMax(maxDistance, dataPointLabelDistance(i));
                        outside = true;

                        if (label->position() == LabelRight)
                            rightLabels = true;
                        else if (label->position() == LabelLeft)
                            leftLabels = true;
                    }

                }
            }
        }
    }

    rectShape = rectArea;

    qreal defIntervalX = rectShape.width() * 3.0 / 100.0;
    qreal defIntervalY = rectShape.height() * 5.0 / 100.0;

    if (outside)
    {
        if (rightLabels && leftLabels)
        {
            rectShape.adjust(m_maxDataLabelSize.width() + maxDistance, defIntervalY, -(m_maxDataLabelSize.width() + maxDistance), -defIntervalY );
        }
        else if (leftLabels)
        {
            rectShape.adjust(defIntervalX, defIntervalY, -defIntervalX, -defIntervalY);
            rectShape.setLeft(rectShape.left() + (m_maxDataLabelSize.width() + maxDistance));
        }
        else if (rightLabels)
        {
            rectShape.adjust(defIntervalX, defIntervalY, -defIntervalX, -defIntervalY);
            rectShape.setRight(rectShape.right() - (m_maxDataLabelSize.width() + maxDistance));
        }
    }
    else
        rectShape.adjust(defIntervalX, defIntervalY, -defIntervalX, -defIntervalY);

    qreal height = rectShape.height();
    int minHeight = (nonEmptyCount - 1) * 2;
    qreal pureHeight = height - (nonEmptyCount - 1) * pyramidSeries->pointGap();

    if (pureHeight < 0)
    {
        m_allowedPointGap = (int)(height - minHeight) / (nonEmptyCount - 1);
        pureHeight = height - (nonEmptyCount - 1) * m_allowedPointGap;
    }
    else
        m_allowedPointGap = pyramidSeries->pointGap();

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i);
        ChartDataPoint* point = partItem->point();

        if (point == NULL)
            continue;

        qreal val = point->valueY();

        if (val < 0)
            continue;

        qreal percent = (qFabs(sum) < ChartMath::QTN_EPSILON_1) ? 0 : (qFabs(val) * 100.0 / sum);

        qreal dataPointHeight = percent * pureHeight / 100.0;

        partItem->setDataPointScreenPoint(ChartPyramidSeriesPartItem::pntHeight, QPointF(dataPointHeight, dataPointHeight));
    }
    return true;
}

void ChartPyramidSeriesItem::sortDataPoints(int screenPointIndex, QVector<ChartDataPoint*>& dataPointsSorted)
{
    dataPointsSorted.clear();

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i);
        ChartDataPoint* point = partItem->point();

        qreal height = partItem->dataPointScreenPoint(screenPointIndex).x();
        bool inserted = false;

        for (int j = 0; j < dataPointsSorted.size(); j++)
        {
            ChartDataPoint* dataPointSorted = dataPointsSorted[j];
            int index = findDataPointIndex(dataPointSorted);

            ChartPyramidSeriesPartItem* partItemSorted = (ChartPyramidSeriesPartItem*)m_pointsItem->at(index);

            qreal sortedHeight = partItemSorted->dataPointScreenPoint(screenPointIndex).x();
            if (height <= sortedHeight)
            {
                dataPointsSorted.insert(j, point);
                inserted = true;
                break;
            }
        }

        if (!inserted)
            dataPointsSorted.append(point);
    }
}

int ChartPyramidSeriesItem::findDataPointIndex(ChartDataPoint* dataPoint) const
{
    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointsItem->at(i);
        const ChartDataPoint* point = partItem->point();
        if (dataPoint == point)
            return i;
    }
    return -1;
}

void ChartPyramidSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);
    ChartSeriesItem::paint(p);
    ChartPainter::painter()->unsetAntialiasing(p);
}

void ChartPyramidSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    Q_UNUSED(p)
    Q_UNUSED(boundingRect)
    Q_ASSERT(false);
/*
    if (ChartPyramidSeries* pyramidSeries = qobject_cast<ChartPyramidSeries*>(series()))
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

void ChartPyramidSeriesItem::drawLegendComponent(QPainter* p, QRect boundingRect, const QColor& startColor, 
    const QColor& finalColor, const QColor& clrBorder)
{
    if (ChartPyramidSeries* pyramidSeries = qobject_cast<ChartPyramidSeries*>(series()))
    {
        boundingRect.adjust(1, 1, -1, -1);

        QVector<QPointF> points;

        points.append(QPointF((boundingRect.left() + boundingRect.right()) * 0.5, boundingRect.top()));

        points.append(QPointF(boundingRect.right(), boundingRect.bottom() ));
        points.append(QPointF(boundingRect.left(), boundingRect.bottom()));

        ChartPainter::painter()->setAntialiasing(p);
        pyramidSeries->qtn_d().m_fillHelper->drawGradientPolygon(p, points, startColor, finalColor);

        if (pyramidSeries->qtn_d().m_border->isVisible())
            ChartPainter::painter()->drawBoundedPolygon(p, points, clrBorder, 1);
        ChartPainter::painter()->unsetAntialiasing(p);
    }
}



/* ChartPyramidSeriesPartItem */
ChartPyramidSeriesPartItem::ChartPyramidSeriesPartItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartSeriesPointItem(point, parentItem)
{
    m_isValidRect = true;
}

ChartPyramidSeriesPartItem::~ChartPyramidSeriesPartItem()
{
}

void ChartPyramidSeriesPartItem::calculateBoundingRect()
{
    QRectF rectPyramid (m_pointLeftTop, m_pointRightBottom);
    m_rectBounds = QRectF(QPointF(rectPyramid.left(), rectPyramid.top() + m_pointOffsets.x()), 
        QPointF(rectPyramid.right(), rectPyramid.top() + m_pointOffsets.y()));
}

void ChartPyramidSeriesPartItem::calculatePyramidPoints(const QRectF& rectPyramid, qreal depth, qreal xCenterOffset, 
    qreal y1, qreal y2, QPointF& pointLeft, QPointF& pointRight)
{
    QRectF rect = rectPyramid;
    rect.adjust(depth, 0, -depth, -depth);

    qreal maxXOffset = rect.width() * 0.5;

    if (qFabs(xCenterOffset) > maxXOffset)
    {
        if (xCenterOffset < 0.0)
            xCenterOffset = -maxXOffset; 
        else
            xCenterOffset = maxXOffset;
    }

    if (xCenterOffset != 0)
    {
        qreal halfWidth = rect.width() * 0.5;
        depth *= (halfWidth - qFabs(xCenterOffset)) / halfWidth;
    }

    y1 = rect.top() + qMax(0.0, y1);
    y2 = (y2 < 0.0) ? rect.bottom() : rect.top() + y2;

    qreal angle = ChartMath::angle(rect.width() * 0.5, rect.height());

    qreal dy = (y2 - y1);
    qreal dx = qFabs(dy / qTan(angle));
    qreal dxTop = (dx * (y1 - rect.top()) / dy);
    qreal dxBottom = dxTop + dx;
    qreal dx1 = (dxTop + dxBottom) * 0.5;

    qreal y = (y1 + y2) * 0.5;

    if (y1 > rect.top())
    {
        qreal ratioTop = (rect.bottom() - y1) / rect.height();
        qreal depthTop = depth * (1.0 - ratioTop);

        qreal ratioBottom = (rect.bottom() - y2) / rect.height();
        qreal depthBottom = depth * (1.0 - ratioBottom);

        y = (y1 - 0.5 * depthTop + y2 - 0.5 * depthBottom) * 0.5;
    }

    pointLeft.setX(rect.center().x() - dx1);
    pointLeft.setY(y);

    pointRight.setX(rect.center().x() + dx1);
    pointRight.setY(y);
}

void ChartPyramidSeriesPartItem::setDataPointScreenPoint(int screenPointIndex, const QPointF& pnt)
{
    if (screenPointIndex == pntHeight)
        m_pointHeight = pnt; 
    else if (screenPointIndex == pntOffsets)
        m_pointOffsets = pnt; 
    else if (screenPointIndex == pntLeftTop)
        m_pointLeftTop = pnt;
    else if (screenPointIndex == pntRightBottom)
        m_pointRightBottom = pnt;
}

QPointF ChartPyramidSeriesPartItem::dataPointScreenPoint(int screenPointIndex) const 
{ 
    if (screenPointIndex == pntHeight)
        return m_pointHeight; 
    else if (screenPointIndex == pntOffsets)
        return m_pointOffsets; 
    else if (screenPointIndex == pntLeftTop)
        return m_pointLeftTop;
    else if (screenPointIndex == pntRightBottom)
        return m_pointRightBottom;
    return QPointF();
}

QColor ChartPyramidSeriesPartItem::borderColor() const
{
    if (ChartPyramidSeries* funnelSeries = qobject_cast<ChartPyramidSeries*>(seriesItem()->series()))
    {
        QColor borderClr = funnelSeries->qtn_d().m_border->color();
        if (borderClr.isValid())
            return borderClr;
    }
    return startColor().dark();
}

QPointF ChartPyramidSeriesPartItem::markerPoint()
{
    if (ChartPyramidSeries* pyramidSeries = qobject_cast<ChartPyramidSeries*>(seriesItem()->series()))
    {
        if (ChartPyramidSeriesLabel* label = (ChartPyramidSeriesLabel*)pyramidSeries->label())
        {
            LabelPosition pos = label->position();
            if (pos == LabelRight || pos == LabelLeft)
            {
                QRectF rectShape(dataPointScreenPoint(pntLeftTop), dataPointScreenPoint(pntRightBottom));
                QPointF pointOffsets = dataPointScreenPoint(pntOffsets);

                qreal depth = pyramidSeries->isSeries3D() ? rectShape.height() * pyramidSeries->depthPercent() / 100.0 : 0.0; 
                qreal offset = pyramidSeries->isSeries3D() && !pyramidSeries->isCircular() ? qSin(ChartMath::degree2Radian(pyramidSeries->rotation())) * rectShape.width() * 0.5 : 0.0;

                QPointF pointLeft;
                QPointF pointRight;
                calculatePyramidPoints(rectShape, depth, offset, pointOffsets.x(), pointOffsets.y(), pointLeft, pointRight);
                if (pos == LabelRight)
                    return pointRight;
                return pointLeft;
            }
        }
    }
    return m_rectBounds.center();
}

void ChartPyramidSeriesPartItem::paint(QPainter* p)
{
    if (ChartPyramidSeries* pyramidSeries = qobject_cast<ChartPyramidSeries*>(seriesItem()->series()))
    {
        QRectF rectPyramid(dataPointScreenPoint(pntLeftTop), dataPointScreenPoint(pntRightBottom));

        m_isValidRect = rectPyramid.isValid();

        if (!m_isValidRect)
            return;

        rectPyramid = rectPyramid.normalized();

        QPointF pointOffsets = dataPointScreenPoint(pntOffsets);
        
        int transparency = pyramidSeries->transparency();
        QColor clrStart = startColor(); 
        clrStart.setAlpha(transparency);
        QColor clrFinal = finalColor();
        clrFinal.setAlpha(transparency);

        if (pyramidSeries->isSeries3D())
        {
            qreal depth = rectPyramid.height() * pyramidSeries->depthPercent() / 100.0; 
            qreal offset = qSin(ChartMath::degree2Radian(pyramidSeries->rotation())) * rectPyramid.width() * 0.5;

            if (pyramidSeries->isCircular())
            {
                QLinearGradient gradientLeft(rectPyramid.bottomLeft(), rectPyramid.bottomRight());
                gradientLeft.setColorAt(0.0, clrStart.darker());
                gradientLeft.setColorAt(0.5, clrStart.light());
                gradientLeft.setColorAt(1.0, clrStart.darker());

                ChartPainter::painter()->drawPyramid3D(p, rectPyramid, depth, gradientLeft, QColor(), 
                    borderColor(), offset, pointOffsets.x(), pointOffsets.y(), transparency < 255,  true, clrStart.dark(150));
            }
            else
            {
                QBrush brFill = pyramidSeries->qtn_d().m_fillHelper->brushGradient(rectPyramid, clrStart, clrFinal);
                ChartPainter::painter()->drawPyramid3D(p, rectPyramid, depth, /*clrStart*/brFill, clrStart.light(), 
                    borderColor(), offset, pointOffsets.x(), pointOffsets.y(), transparency < 255, false, clrStart.dark(150));
            }
        }
        else
        {
            QBrush brFill = pyramidSeries->qtn_d().m_fillHelper->brushGradient(rectPyramid, clrStart, clrFinal);
            ChartPainter::painter()->drawPyramid(p, rectPyramid, pointOffsets.x(), pointOffsets.y(), brFill, borderColor(), transparency < 255);
        }
    }
}

void ChartPyramidSeriesPartItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    if (ChartPyramidSeriesItem* item = qobject_cast<ChartPyramidSeriesItem*>(seriesItem()))
        item->drawLegendComponent(p, boundingRect, startColor(), finalColor(), borderColor());
}

/*!
    \class Qtitan::ChartPyramidArea
    \inmodule QtitanChart
    \internal
*/
ChartPyramidArea::ChartPyramidArea(QObject* parent)
    : ChartArea(*new ChartPyramidAreaPrivate, parent)
{
}

ChartPyramidArea::~ChartPyramidArea()
{
}

/*!
    \class Qtitan::ChartPyramidAreaPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAreaItem* ChartPyramidAreaPrivate::createItem(GraphicsItem* parent)
{
    QTN_P(ChartPyramidArea)
    return new ChartPyramidAreaItem(&p, parent);
}

void ChartPyramidAreaPrivate::calculateSeriesLayout(ChartAreaItem* item)
{
    if (item->count() > 0)
    {
        if (GraphicsItem* grItem = item->seriesItem())
        {
            for (int i = 0, count = grItem->count(); i < count; i++)
            {
                ChartPyramidSeriesItem* seriesItem = (ChartPyramidSeriesItem*)grItem->at(i);
                seriesItem->calculatePointLayout(item->boundingRect());
                seriesItem->calculateLabelLayout();
            }
        }
    }
}

/* ChartPyramidAreaItem */
ChartPyramidAreaItem::ChartPyramidAreaItem(ChartArea* area, GraphicsItem* parent)
    : ChartAreaItem(area, parent)
{
    m_seriesItem = new GraphicsItem(this);
    m_labelsItem = new GraphicsItem(this);
}


/*!
    \class Qtitan::ChartPyramidSeriesLabelPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPyramidSeriesLabelPrivate::ChartPyramidSeriesLabelPrivate()
{
    m_nPosition = LabelRight;
}

ChartPyramidSeriesLabelPrivate::~ChartPyramidSeriesLabelPrivate()
{
}

GraphicsItem* ChartPyramidSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
{
    QTN_P(ChartPyramidSeriesLabel)
    return new ChartPyramidSeriesLabelItem(&p, pointItem, parentItem);
}

/* ChartPyramidSeriesLabel */
ChartPyramidSeriesLabel::ChartPyramidSeriesLabel()
    : ChartSeriesLabel(*new ChartPyramidSeriesLabelPrivate, Q_NULL)
{
//    m_lineLength = 20;
}

ChartPyramidSeriesLabel::ChartPyramidSeriesLabel(ChartPyramidSeriesLabelPrivate& d, QObject* parent)
    : ChartSeriesLabel(d, parent)
{
}

ChartPyramidSeriesLabel::~ChartPyramidSeriesLabel()
{
}

LabelPosition ChartPyramidSeriesLabel::position() const 
{
    QTN_D(const ChartPyramidSeriesLabel)
    return d.m_nPosition;
}

void ChartPyramidSeriesLabel::setPosition(LabelPosition pos) 
{
    QTN_D(ChartPyramidSeriesLabel)
    d.m_nPosition = pos;
    chartUpdate();
}

bool ChartPyramidSeriesLabel::isInside() const
{
    LabelPosition pos = position();
    return pos == LabelCenter;

}

/* ChartPyramidSeriesLabelItem */
ChartPyramidSeriesLabelItem::ChartPyramidSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartSeriesLabelItem(label, pointItem, parentItem)
{
}

void ChartPyramidSeriesLabelItem::calculateLayout()
{
}

LabelPosition ChartPyramidSeriesLabelItem::position() const
{
    return ((ChartPyramidSeriesLabel*)m_label)->position();
}

void ChartPyramidSeriesLabelItem::paint(QPainter* p)
{
    ChartPyramidSeriesLabel* label = qobject_cast<ChartPyramidSeriesLabel*>(m_label);

    qreal lineAngle = 0.0;

    int borderThickness = m_label->border()->thickness();

    QPointF anchorPoint;
    bool center = false;

    ChartPyramidSeriesPartItem* partItem = (ChartPyramidSeriesPartItem*)m_pointItem;

    if (!partItem->m_isValidRect)
        return;

    if (label->position() == LabelRight)
    {
        anchorPoint = partItem->markerPoint();
        lineAngle = ChartMath::degree2Radian(0.0);
    }
    else if (label->position() == LabelLeft)
    {
        anchorPoint = partItem->markerPoint();
        lineAngle = ChartMath::degree2Radian(180.0);
    }
    else
    {
        anchorPoint = partItem->m_rectBounds.center();
        center = true;
    }

    QString text(m_label->pointLabel(m_pointItem->point()));
    if (text.isEmpty() || text == tr(" "))
        return;

    ChartTextPainter painter(text, m_label);
    QSizeF size = painter.size();

    QPointF startPoint(ChartMath::round(anchorPoint.x()), ChartMath::round(anchorPoint.y()));
    QPointF finishPoint(ChartMath::round(anchorPoint.x() + (qCos(lineAngle) * m_label->lineLength())),
        ChartMath::round(anchorPoint.y() - (qSin(lineAngle) * m_label->lineLength())));

    QRectF innerBounds;
    QRectF bounds;

    if (center)
    {
        bounds = QRectF(0, 0, size.width() + 2 * borderThickness, size.height() + 2 * borderThickness);
        bounds.translate(anchorPoint.x() - bounds.width() * 0.5, anchorPoint.y() - bounds.height() * 0.5);

        innerBounds = bounds;
        innerBounds.adjust((qreal)borderThickness, (qreal)borderThickness, (qreal)-borderThickness, (qreal)-borderThickness);
    }
    else
    {
        QPointF startPoint(ChartMath::round(anchorPoint.x()), ChartMath::round(anchorPoint.y()));
        QPointF finishPoint(ChartMath::round(anchorPoint.x() + (qCos(lineAngle) * m_label->lineLength())),
            ChartMath::round(anchorPoint.y() - (qSin(lineAngle) * m_label->lineLength())));

        bounds = ChartSeriesLabelConnectorPainterBase::calcBorderBoundsForTangentDrawing(finishPoint, lineAngle, size, borderThickness, innerBounds);
    }

    QPointF labelPoint(innerBounds.x(), innerBounds.y());

    QColor clrBackColor = m_label->backgroundColor();
    m_label->qtn_d().m_fillHelper->drawGradientRect(p, bounds, clrBackColor, clrBackColor);

    painter.setLocation(labelPoint);

    painter.drawBoundedText(p, textColor());

    if (m_label->showLines() && !center)
    {
        ChartSeriesLabelLineConnectorPainter linePainter(startPoint, finishPoint, lineAngle, bounds);
        linePainter.drawFillLine(p, m_label, linkColor());
    }

    if (borderThickness)
        m_label->border()->drawBorder(p, bounds, borderColor());
}
