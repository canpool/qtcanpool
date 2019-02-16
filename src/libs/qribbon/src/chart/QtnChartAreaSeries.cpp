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
#include <QMap>

#include "QtnChartAreaSeries.h"
#include "QtnChartAreaSeriesPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartMarker.h"
#include "QtnChartAxis.h"



using namespace Qtitan;

namespace Qtitan
{
    /* ChartAreaSeriesPrivate */
    class ChartAreaSeriesPrivate : public ChartPointSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartAreaSeries)
    public:
        explicit ChartAreaSeriesPrivate();
        ~ChartAreaSeriesPrivate();
    public:
        void init();
    public:
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        int m_transparency;
        ChartBorder* m_border;
        ChartFillDrawHelper* m_fillHelper;
    };
}

/*!
    \class Qtitan::ChartAreaSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAreaSeriesPrivate::ChartAreaSeriesPrivate()
{
    m_border = Q_NULL;
    m_fillHelper = Q_NULL;
    m_transparency = 135;
}

ChartAreaSeriesPrivate::~ChartAreaSeriesPrivate()
{
    delete m_border;
}

void ChartAreaSeriesPrivate::init()
{
    QTN_P(ChartAreaSeries)
    m_border = new ChartBorder(&p);
    m_fillHelper = new ChartFillDrawHelper(&p);
    m_fillHelper->setFillStyle(Qt::LinearGradientPattern);
}

ChartSeriesItem* ChartAreaSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    if (qobject_cast<ChartSplineAreaSeries*>(qtn_p_ptr))
        return new ChartSplineAreaSeriesItem(series, areaItem);
    else if (qobject_cast<ChartStackedSplineAreaSeries*>(qtn_p_ptr))
        return new ChartStackedSplineAreaSeriesItem(series, areaItem);
    else if (qobject_cast<ChartStackedAreaSeries*>(qtn_p_ptr))
        return new ChartStackedAreaSeriesItem(series, areaItem);
    else if (qobject_cast<ChartAreaSeries*>(qtn_p_ptr))
        return new ChartAreaSeriesItem(series, areaItem);
    return Q_NULL;
}

/* ChartAreaSeries */
ChartAreaSeries::ChartAreaSeries()
    : ChartPointSeries(*new ChartAreaSeriesPrivate, Q_NULL) 
{
    QTN_D(ChartAreaSeries);
    d.init();
}

ChartAreaSeries::ChartAreaSeries(ChartAreaSeriesPrivate& d, QObject* parent)
    : ChartPointSeries(d, parent) 
{
    d.init();
}

ChartAreaSeries::~ChartAreaSeries()
{
}

void ChartAreaSeries::setThickness(int thickness)
{
    QTN_D(ChartAreaSeries);
    d.m_border->setThickness(thickness);
}

int ChartAreaSeries::thickness() const
{
    QTN_D(const ChartAreaSeries);
    return d.m_border->thickness();
}

int ChartAreaSeries::transparency() const 
{
    QTN_D(const ChartAreaSeries);
    return d.m_transparency;
}

void ChartAreaSeries::setTransparency(int transparency) 
{
    QTN_D(ChartAreaSeries);
    d.m_transparency = transparency;
    chartUpdate();
}

ChartFillDrawHelper* ChartAreaSeries::fillHelper() const 
{
    QTN_D(const ChartAreaSeries);
    return d.m_fillHelper;
}

ChartBorder* ChartAreaSeries::border() const 
{
    QTN_D(const ChartAreaSeries);
    return d.m_border;
}


/*!
    \class Qtitan::ChartAreaSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartAreaSeriesItem::ChartAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartArea2DSeriesItem(series,areaItem, true)
{
}

ChartSeriesPointItem* ChartAreaSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPointSeriesPointItem(point, parentItem);
}

QColor ChartAreaSeriesItem::startColor() const
{
    QColor clr = beginColor();
    return QColor(clr.red(), clr.green(), clr.blue(), ((ChartAreaSeries*)series())->transparency());
}

QColor ChartAreaSeriesItem::finalColor() const
{
    QColor clr = endColor();
    return QColor(clr.red(), clr.green(), clr.blue(), ((ChartAreaSeries*)series())->transparency());
}

QColor ChartAreaSeriesItem::borderColor() const
{
    ChartAreaSeries* areaSeries = (ChartAreaSeries*)series();

    if (areaSeries->border()->color().isValid())
        return areaSeries->border()->color();
    return beginColor().dark();
}

void ChartAreaSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -1, -1);

    ChartAreaSeries* areaSeries = static_cast<ChartAreaSeries*>(m_data->series());

    QPointF ptCenter(rect.center().x(), rect.top() + 2) ;

    QVector<QPointF> arrPoints;
    arrPoints.append(QPointF(rect.left(), rect.bottom() + 1));
    arrPoints.append(QPointF(ptCenter.x()-5, rect.top() + 7));
    arrPoints.append(QPointF(ptCenter.x()+4, rect.top() + 2));
    arrPoints.append(QPointF(rect.right(), rect.bottom() + 1));

    areaSeries->fillHelper()->drawGradientPolygon(p, arrPoints, startColor(), finalColor());

    QColor color = borderColor();
    ChartPainter::painter()->setAntialiasing(p);
    ChartPainter::painter()->drawFillLine(p, arrPoints[0], arrPoints[1], color, 1);
    ChartPainter::painter()->drawFillLine(p, arrPoints[1], arrPoints[2], color, 1);
    ChartPainter::painter()->drawFillLine(p, arrPoints[2], arrPoints[3], color, 1);
    ChartPainter::painter()->unsetAntialiasing(p);

    ((ChartPointSeries*)areaSeries)->qtn_d().m_marker->paint(p, arrPoints[1], 6, 
        beginColor(), endColor(), beginColor().dark());

    ((ChartPointSeries*)areaSeries)->qtn_d().m_marker->paint(p, arrPoints[2], 6, 
        beginColor(), endColor(), beginColor().dark());
}

void ChartAreaSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);

    ChartAreaSeries* areaSeries = (ChartAreaSeries*)series();

    ChartAxis* axisY = axisItemY()->axis();

    qreal zero = qMax(0.0, axisY->minValue());

    int count = m_pointsItem->count();
    if (count > 1)
    {
        ChartPointSeriesPointItem* pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(0);

        QPointF pointPrev = screenPoint(pointItem->point()->valueX(), pointItem->m_internalValue);

        QVector<QPointF> arrPoints;
        arrPoints.append(screenPoint(pointItem->point()->valueX(), zero));

        for (int i = 1; i < count; i++)
        {
            pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(i);
            QPointF pointNext = pointItem->screenPoint();

            arrPoints.append(pointPrev);

            pointPrev = pointNext;
        }

        arrPoints.append(pointPrev);

        pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(count - 1);

        arrPoints.append(screenPoint(pointItem->point()->valueX(), zero));

        areaSeries->fillHelper()->drawGradientPolygon(p, arrPoints, startColor(), finalColor());


        arrPoints.remove(0);
        arrPoints.remove(arrPoints.size() - 1);

        if (areaSeries->border()->isVisible())
            ChartPainter::painter()->drawFillPolyline(p, arrPoints, borderColor(), areaSeries->border()->thickness());
    }

    ChartSeriesItem::paint(p);

    ChartPainter::painter()->unsetAntialiasing(p);
}

/* ChartSplineAreaSeries */
ChartSplineAreaSeries::ChartSplineAreaSeries()
{
}

ChartSplineAreaSeries::~ChartSplineAreaSeries()
{
}


/*!
    \class Qtitan::ChartSplineAreaSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartSplineAreaSeriesItem::ChartSplineAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartAreaSeriesItem(series,areaItem)
{
}

ChartSeriesPointItem* ChartSplineAreaSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPointSeriesPointItem(point, parentItem);
}

void ChartSplineAreaSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p/*, false*/);

    ChartSplineAreaSeries* areaSeries = (ChartSplineAreaSeries*)series();

    ChartAxis* axisY = axisItemY()->axis();

    qreal zero = qMax(0.0, axisY->minValue());

    int count = m_pointsItem->count();
    if (count > 1)
    {
        ChartPointSeriesPointItem* pointItem = (ChartPointSeriesPointItem*)m_pointsItem->at(0);

        QVector<QPointF> arrPoints;
        QPointF bottomPoint(screenPoint(pointItem->point()->valueX(), zero));

        arrPoints.append(bottomPoint);

        for (int i = 0; i < count; i++)
        {
            pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(i);
            QPointF pointNext = pointItem->screenPoint();

            arrPoints.append(pointNext);
        }

        pointItem =  (ChartPointSeriesPointItem*)m_pointsItem->at(count - 1);
        arrPoints.append(screenPoint(pointItem->point()->valueX(), zero));

        areaSeries->fillHelper()->drawFillSplinePolygon(p, arrPoints, startColor(), finalColor());

        arrPoints.remove(0);
        arrPoints.remove(arrPoints.size() - 1);

        ChartPainter::painter()->drawFillSpline(p, arrPoints, borderColor(), areaSeries->thickness());
    }

    ChartSeriesItem::paint(p);

    ChartPainter::painter()->unsetAntialiasing(p);
}


namespace Qtitan
{
    /* ChartStackedAreaSeriesPrivate */
    class ChartStackedAreaSeriesPrivate : public ChartAreaSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartAreaSeries)
    public:
        explicit ChartStackedAreaSeriesPrivate();
        ~ChartStackedAreaSeriesPrivate();
    public:
        qreal m_stackHeight;
    };
}

/*!
    \class Qtitan::ChartStackedAreaSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
ChartStackedAreaSeriesPrivate::ChartStackedAreaSeriesPrivate()
{
    m_transparency = 135;
    m_stackHeight = 0;
}

ChartStackedAreaSeriesPrivate::~ChartStackedAreaSeriesPrivate()
{
}


/* ChartStackedAreaSeries */
ChartStackedAreaSeries::ChartStackedAreaSeries()
    : ChartAreaSeries(*new ChartStackedAreaSeriesPrivate, Q_NULL)
{
}

ChartStackedAreaSeries::~ChartStackedAreaSeries()
{
}

void ChartStackedAreaSeries::setStackHeight(qreal stackHeight) 
{
    QTN_D(ChartStackedAreaSeries);
    d.m_stackHeight = stackHeight;
    chartUpdate();
}

qreal ChartStackedAreaSeries::stackHeight() const 
{
    QTN_D(const ChartStackedAreaSeries);
    return d.m_stackHeight;
}


/*!
    \class Qtitan::ChartStackedAreaSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartStackedAreaSeriesItem::ChartStackedAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartAreaSeriesItem(series,areaItem)
{
}

ChartSeriesPointItem* ChartStackedAreaSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartStackedAreaSeriesPointItem(point, parentItem);
}


void ChartStackedAreaSeriesItem::updateRange()
{
    ChartAreaSeriesItem::updateRange();

    ChartAreaItem* area = areaItem();


    for (int i = 0; i < area->seriesItem()->count(); i++)
    {
        ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(i);

        if (dynamic_cast<ChartStackedAreaSeries*>(item->data()->series()))
        {
            if (item != this)
                return;

            break;
        }
    }

    QMap<qreal, qreal> mapValues;

    for (int j = 0; j < area->seriesItem()->count(); j++)
    {
        ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(j);

        if (!dynamic_cast<ChartStackedAreaSeries*>(item->data()->series()))
            continue;

        ChartStackedAreaSeriesItem* stackedAreaSeriesItem = (ChartStackedAreaSeriesItem*)item;

        for (int i = 0; i < stackedAreaSeriesItem->m_pointsItem->count(); i++)
        {
            ChartStackedAreaSeriesPointItem* pointItem = (ChartStackedAreaSeriesPointItem*)stackedAreaSeriesItem->m_pointsItem->at(i);

//            qreal value = pointItem->point()->value(0);
            qreal value = pointItem->point()->valueY();

            pointItem->m_valueFrom = mapValues[pointItem->point()->valueX()];
            pointItem->m_internalValue = pointItem->m_valueTo = pointItem->m_valueFrom + value;

            mapValues[pointItem->point()->valueX()] = pointItem->m_valueTo;
        }
    }


    qreal stackHeight = ((ChartStackedAreaSeries*)series())->stackHeight();
    if (stackHeight != 0)
    {
        for (int j = 0; j < area->seriesItem()->count(); j++)
        {
            ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(j);

            if (dynamic_cast<ChartStackedAreaSeries*>(item->data()->series()))
            {
                ChartStackedAreaSeriesItem* stackedAreaSeriesItem = (ChartStackedAreaSeriesItem*)item;

                for (int i = 0; i < stackedAreaSeriesItem->m_pointsItem->count(); i++)
                {
                    ChartStackedAreaSeriesPointItem* pointItem = (ChartStackedAreaSeriesPointItem*)stackedAreaSeriesItem->m_pointsItem->at(i);

                    qreal dLimit = mapValues[pointItem->point()->valueX()];

                    pointItem->m_valueFrom = pointItem->m_valueFrom * stackHeight / dLimit;
                    pointItem->m_internalValue = pointItem->m_valueTo = pointItem->m_valueTo * stackHeight / dLimit;
                }
            }
        }

    }
}

void ChartStackedAreaSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);

    ChartStackedAreaSeries* impl = (ChartStackedAreaSeries*)series();

    int count = m_pointsItem->count();
    if (count > 1)
    {
        ChartStackedAreaSeriesPointItem* pointItem = Q_NULL;

        QVector<QPointF> arrPoints;

        for (int i = 0; i < count; i++)
        {
            pointItem =  (ChartStackedAreaSeriesPointItem*)m_pointsItem->at(i);

            QPointF point = screenPoint(pointItem->point()->valueX(), pointItem->m_valueTo);
            arrPoints.append(point);
        }

        if (impl->border()->isVisible())
            ChartPainter::painter()->drawFillPolyline(p, arrPoints, borderColor(), impl->border()->thickness());

        for (int i = count - 1; i >= 0; i--)
        {
            pointItem =  (ChartStackedAreaSeriesPointItem*)m_pointsItem->at(i);

            QPointF point = screenPoint(pointItem->point()->valueX(), pointItem->m_valueFrom);
            arrPoints.append(point);
        }
        impl->fillHelper()->drawGradientPolygon(p, arrPoints, startColor(), finalColor());
    }

    ChartSeriesItem::paint(p);
    ChartPainter::painter()->unsetAntialiasing(p);
}

/*!
    \class Qtitan::ChartStackedAreaSeriesPointItem
    \inmodule QtitanChart
    \internal
*/
ChartStackedAreaSeriesPointItem::ChartStackedAreaSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartPointSeriesPointItem(point, parentItem)
{
}


/* ChartStackedSplineAreaSeries */
ChartStackedSplineAreaSeries::ChartStackedSplineAreaSeries()
{
}

ChartStackedSplineAreaSeries::~ChartStackedSplineAreaSeries()
{
}

/*!
    \class Qtitan::ChartStackedSplineAreaSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartStackedSplineAreaSeriesItem::ChartStackedSplineAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartStackedAreaSeriesItem(series, areaItem)
{
}

void ChartStackedSplineAreaSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);

    ChartStackedSplineAreaSeries* areaSeries = (ChartStackedSplineAreaSeries*)series();

    int count = m_pointsItem->count();
    if (count > 1)
    {
        ChartStackedAreaSeriesPointItem* pointItem = Q_NULL;

        QVector<QPointF> arrPoints;

        for (int i = 0; i < count; i++)
        {
            pointItem =  (ChartStackedAreaSeriesPointItem*)m_pointsItem->at(i);
            QPointF point = screenPoint(pointItem->point()->valueX(), pointItem->m_valueTo);
            arrPoints.append(point);
        }

        if (areaSeries->border()->isVisible())
            ChartPainter::painter()->drawFillSpline(p, arrPoints, borderColor(), areaSeries->border()->thickness());

        for (int i = count - 1; i >= 0; i--)
        {
            pointItem =  (ChartStackedAreaSeriesPointItem*)m_pointsItem->at(i);
            QPointF point = screenPoint(pointItem->point()->valueX(), pointItem->m_valueFrom);
            arrPoints.append(point);
        }
        areaSeries->fillHelper()->drawFillSplinePolygon(p, arrPoints, startColor(), finalColor(), true);
    }

    ChartSeriesItem::paint(p);

    ChartPainter::painter()->unsetAntialiasing(p);
}
