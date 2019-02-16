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
#include <qnumeric.h>

#include "QtnChartLineSeries.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartMarker.h"
#include "QtnChartMath.h"

using namespace Qtitan;

namespace Qtitan
{
    /* ChartLineSeriesItem */
    class ChartLineSeriesItem : public ChartArea2DSeriesItem
    {
    public:
        explicit ChartLineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem, bool sortPoints = true);

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);
    };

    /* ChartStepLineSeriesItem */
    class  ChartStepLineSeriesItem : public ChartArea2DSeriesItem
    {
    public:
        explicit ChartStepLineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    protected:
        virtual ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    };

    /* ChartStepLineSeriesPointItem */
    class  ChartStepLineSeriesPointItem : public ChartPointSeriesPointItem
    {
    public:
        explicit ChartStepLineSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
    public:
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);
    };

    /* ChartSplineSeriesItem */
    class ChartSplineSeriesItem : public ChartLineSeriesItem
    {
    public:
        ChartSplineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
    };
};


namespace Qtitan
{
    /* ChartLineSeriesPrivate */
    class ChartLineSeriesPrivate : public ChartPointSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartLineSeries)
    public:
        explicit ChartLineSeriesPrivate();
        ~ChartLineSeriesPrivate();

    public:
        void init();

    public:
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        ChartLineDrawHelper* m_line;
        bool m_shadow; 
        friend class ChartLineSeriesItem;
        friend class ChartSplineSeriesItem;
    };
}

/* ChartLineSeriesPrivate */
ChartLineSeriesPrivate::ChartLineSeriesPrivate()
{
    m_line = Q_NULL;
    m_shadow = true; 
}

ChartLineSeriesPrivate::~ChartLineSeriesPrivate()
{
    delete m_line;
}

void ChartLineSeriesPrivate::init()
{
    QTN_P(ChartLineSeries)
    m_line = new ChartLineDrawHelper(&p);
    m_line->setThickness(2);
    m_shadow = true;
}

ChartSeriesItem* ChartLineSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    if (qobject_cast<ChartStepLineSeries*>(qtn_p_ptr))
        return new ChartStepLineSeriesItem(series, areaItem);
    else if (qobject_cast<ChartSplineSeries*>(qtn_p_ptr))
        return new ChartSplineSeriesItem(series, areaItem);
    if (qobject_cast<ChartLineSeries*>(qtn_p_ptr))
        return new ChartLineSeriesItem(series, areaItem);
    return Q_NULL;
}


/* ChartLineSeries */
ChartLineSeries::ChartLineSeries()
    : ChartPointSeries(*new ChartLineSeriesPrivate, Q_NULL)
{
    QTN_D(ChartLineSeries)
    d.init();
}

ChartLineSeries::ChartLineSeries(ChartLineSeriesPrivate& d, QObject* parent)
    : ChartPointSeries(d, parent)
{
    d.init();
}

ChartLineSeries::~ChartLineSeries()
{
}

void ChartLineSeries::setShadow(bool shadow)
{
    QTN_D(ChartLineSeries)
    d.m_shadow = shadow;
}

bool ChartLineSeries::shadow() const
{
    QTN_D(const ChartLineSeries)
    return d.m_shadow;
}

void ChartLineSeries::setThickness(int thickness)
{
    QTN_D(ChartLineSeries)
    return d.m_line->setThickness(thickness);
}

int ChartLineSeries::thickness() const
{
    QTN_D(const ChartLineSeries)
    return d.m_line->thickness();
}

void ChartLineSeries::setDashStyle(DashStyle dashStyle)
{
    QTN_D(ChartLineSeries)
    d.m_line->setDashStyle(dashStyle);
}

DashStyle ChartLineSeries::dashStyle() const
{
    QTN_D(const ChartLineSeries)
    return d.m_line->dashStyle();
}


/* ChartLineSeriesItem */
ChartLineSeriesItem::ChartLineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem, bool bSortPoints)
    : ChartArea2DSeriesItem(series,areaItem, bSortPoints)
{
}

ChartSeriesPointItem* ChartLineSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPointSeriesPointItem(point, parentItem);
}

void ChartLineSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);

    ChartLineSeries* lineSeries = (ChartLineSeries*)series();

    int count = m_pointsItem->count();
    if (count > 1)
    {
        QPointF pointPrev;
        bool validPrev = false;

        for (int i = 0; i < count; i++)
        {
            ChartPointSeriesPointItem* pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(i);

            if (qIsNaN(pointItem->m_internalValue))
            {
                validPrev = false;
                continue;
            }

            QPointF pointNext = pointItem->screenPoint();

            if (validPrev)
            {
                lineSeries->qtn_d().m_line->draw(p, pointPrev, pointNext, beginColor());
                if (lineSeries->shadow())
                {
                    QColor col = beginColor().lighter(100);
                    col.setAlpha(160);
                    QPointF offset(0, lineSeries->thickness()*0.5);
                    lineSeries->qtn_d().m_line->draw(p, pointPrev+offset, pointNext+offset, col);

                    col.setAlpha(90);
                    QPointF offset1(0, offset.y() + lineSeries->thickness()*0.5);
                    lineSeries->qtn_d().m_line->draw(p, pointPrev+offset1, pointNext+offset1, col);

                    col.setAlpha(40);
                    QPointF offset2(0, offset1.y() + lineSeries->thickness()*0.5);
                    lineSeries->qtn_d().m_line->draw(p, pointPrev+offset2, pointNext+offset2, col);

                    col.setAlpha(20);
                    QPointF offset3(0, offset2.y() + lineSeries->thickness()*0.5);
                    lineSeries->qtn_d().m_line->draw(p, pointPrev+offset3, pointNext+offset3, col);

                }
            }

            pointPrev = pointNext;
            validPrev = true;
        }
    }

    ChartSeriesItem::paint(p);

    ChartPainter::painter()->unsetAntialiasing(p);
}

void ChartLineSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -1, -1);

    ChartPointSeries* pointSeries = static_cast<ChartPointSeries*>(m_data->series());

    QPointF ptCenter(rect.center().x(), rect.center().y()) ;
    int size = 10;

    ChartPainter::painter()->drawFillLine(p, QPointF(rect.left(), ptCenter.y()), QPointF(rect.right(), ptCenter.y()), beginColor(), 1);
    pointSeries->qtn_d().m_marker->paint(p, ptCenter, size, beginColor(), endColor(), beginColor().dark());
}



namespace Qtitan
{
    /* ChartStepLineSeriesPrivate */
    class ChartStepLineSeriesPrivate : public ChartLineSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartStepLineSeries)
    public:
        explicit ChartStepLineSeriesPrivate();
        ~ChartStepLineSeriesPrivate();
    public:
        bool m_invertedStep;
    private:
        friend class ChartStepLineSeriesItem;
    };
}

/* ChartStepLineSeriesPrivate */
ChartStepLineSeriesPrivate::ChartStepLineSeriesPrivate()
{
    m_invertedStep = false;
}

ChartStepLineSeriesPrivate::~ChartStepLineSeriesPrivate()
{
}

/* ChartStepLineSeries */
ChartStepLineSeries::ChartStepLineSeries()
    : ChartLineSeries(*new ChartStepLineSeriesPrivate, Q_NULL)
{
}

ChartStepLineSeries::~ChartStepLineSeries()
{
}

bool ChartStepLineSeries::invertedStep() const 
{
    QTN_D(const ChartStepLineSeries)
    return d.m_invertedStep;
}

void ChartStepLineSeries::setInvertedStep(bool invertedStep) 
{
    QTN_D(ChartStepLineSeries)
    d.m_invertedStep = invertedStep;
    chartUpdate();
}

/* ChartStepLineSeriesItem */
ChartStepLineSeriesItem::ChartStepLineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartArea2DSeriesItem(series,areaItem, true)
{

}
ChartSeriesPointItem* ChartStepLineSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartStepLineSeriesPointItem(point, parentItem);
}

void ChartStepLineSeriesItem::paint(QPainter* p)
{
    ChartStepLineSeries* lineSeries = (ChartStepLineSeries*)series();

    int count = m_pointsItem->count();
    if (count > 1)
    {
        QPointF pointPrev;
        ChartPointSeriesPointItem* pPointPrev = Q_NULL;

        for (int i = 0; i < count; i++)
        {
            ChartPointSeriesPointItem* pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(i);

            if (qIsNaN(pointItem->m_internalValue))
            {
                pPointPrev = Q_NULL;
                continue;
            }

            QPointF pointNext = pointItem->screenPoint();

            if (pPointPrev)
            {
                QPointF pointMiddle;
                if (!lineSeries->invertedStep())
                {
                    pointMiddle = screenPoint(pointItem->point()->valueX(),
                        pPointPrev->m_internalValue);
                }
                else
                {
                    pointMiddle = screenPoint(pPointPrev->point()->valueX(),
                        pointItem->m_internalValue);
                }

                lineSeries->qtn_d().m_line->draw(p, pointPrev, pointMiddle, beginColor());
                lineSeries->qtn_d().m_line->draw(p, pointMiddle, pointNext, beginColor());
            }
            pointPrev = pointNext;
            pPointPrev = pointItem;
        }
    }

    ChartSeriesItem::paint(p);
}

void ChartStepLineSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.normalized();
    rect.adjust(1, 1, -1, -1);

    ChartPointSeries* pointSeries = static_cast<ChartPointSeries*>(data()->series());

    QPointF ptCenter(rect.center().x()+1, rect.center().y());
    int size = 8;

    ChartPainter::painter()->drawFillLine(p, QPointF(rect.left(), rect.top()), QPointF(rect.left(), ptCenter.y()), beginColor(), 1);
    ChartPainter::painter()->drawFillLine(p, QPointF(rect.left(), ptCenter.y()), QPointF(rect.right(), ptCenter.y()), beginColor(), 1);
    ChartPainter::painter()->drawFillLine(p, QPointF(rect.right(), ptCenter.y()), QPointF(rect.right(), rect.top()), beginColor(), 1);

    pointSeries->qtn_d().m_marker->paint(p, ptCenter, size, beginColor(), endColor(), beginColor().dark());
}

/* ChartStepLineSeriesPointItem */
ChartStepLineSeriesPointItem::ChartStepLineSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartPointSeriesPointItem(point, parentItem)
{
}

void ChartStepLineSeriesPointItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -1, -1);

    ChartPointSeries* pointSeries = static_cast<ChartPointSeries*>(seriesItem()->data()->series());

    QPointF ptCenter(rect.center().x(), rect.center().y()) ;
    int size = 8;

    ChartPainter::painter()->drawFillLine(p, QPointF(rect.left(), boundingRect.bottom()), QPointF(boundingRect.left(), ptCenter.y()), seriesItem()->beginColor(), 1);
    ChartPainter::painter()->drawFillLine(p, QPointF(rect.left(), ptCenter.y()), QPointF(boundingRect.right(), ptCenter.y()), seriesItem()->beginColor(), 1);
    ChartPainter::painter()->drawFillLine(p, QPointF(rect.right(), ptCenter.y()), QPointF(boundingRect.right(), boundingRect.top()), seriesItem()->beginColor(), 1);

    pointSeries->qtn_d().m_marker->paint(p, ptCenter, size, startColor(), finalColor(), startColor().dark());
}

/* ChartSplineSeries */
ChartSplineSeries::ChartSplineSeries()
{
}

ChartSplineSeries::~ChartSplineSeries()
{
}

/* ChartSplineSeriesItem */
ChartSplineSeriesItem::ChartSplineSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartLineSeriesItem(series, areaItem)
{
}

ChartSeriesPointItem* ChartSplineSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPointSeriesPointItem(point, parentItem);
}

void ChartSplineSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);

    ChartSplineSeries* splineSeries = (ChartSplineSeries*)series();

    int count = m_pointsItem->count();

    QVector<QPointF> points;

    for (int i = 0; i < count; i++)
    {
        ChartPointSeriesPointItem* pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(i);
        if (qIsNaN(pointItem->m_internalValue))
        {
            if (points.size() > 1)
                splineSeries->qtn_d().m_line->drawSpline(p, points, beginColor());
            points.clear();
            continue;
        }

        QPointF pointNext = pointItem->screenPoint();
        points.append(pointNext);
    }

    if (points.size() > 1)
        splineSeries->qtn_d().m_line->drawSpline(p, points, beginColor());

    ChartSeriesItem::paint(p);

    ChartPainter::painter()->unsetAntialiasing(p);
}
