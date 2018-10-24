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
#include "QtnChartBarSeries.h"

#include <QMap>

#include "QtnChart.h"
#include "Qtn2DSeriesPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartArea2D.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartAxis.h"


using namespace Qtitan;


namespace Qtitan
{
    /* ChartBarSeriesItem */
    class ChartBarSeriesItem : public ChartArea2DSeriesItem
    {
    public:
        ChartBarSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);

        void beforeUpdateRange();

        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    protected:
        int m_barCount;
        int m_barIndex;

        bool m_firstStack;

        friend class ChartBarSeriesPointItem;
    };


    /* ChartStackedBarSeriesItem */
    class ChartStackedBarSeriesItem : public ChartBarSeriesItem
    {
    public:
        ChartStackedBarSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual ~ChartStackedBarSeriesItem();

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        void updateRange();

    protected:
        friend class ChartStackedBarSeriesPointItem;
    };


    /* ChartBarSeriesPointItem */
    class ChartBarSeriesPointItem : public ChartSeriesPointItem
    {
    public:
        ChartBarSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);

    public:
        virtual QRectF getScreenRect() const;

    public:
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);
    };

    /* ChartStackedBarSeriesPointItem */
    class ChartStackedBarSeriesPointItem : public ChartBarSeriesPointItem
    {
    public:
        ChartStackedBarSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);

    public:
        QRectF getScreenRect() const;

    public:
        virtual void paint(QPainter* p);

    protected:
        qreal m_valueFrom;
        qreal m_valueTo;
        friend class ChartStackedBarSeriesItem;
    };

    /* ChartBarSeriesLabelItem */
    class ChartBarSeriesLabelItem : public ChartArea2DSeriesLabelItem
    {
    public:
        ChartBarSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);

    public:
        virtual QPointF locationPoint() const;
        virtual qreal locationAngle() const;

        virtual void paint(QPainter* p);
        void calculateLayout();
    };
};

namespace Qtitan
{
    /* ChartBarSeriesPrivate */
    class ChartBarSeriesPrivate : public Chart2DSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartBarSeries)
    public:
        explicit ChartBarSeriesPrivate();
        ~ChartBarSeriesPrivate();

    public:
        void init();

    public:
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual void correctAxisSideMargins(ChartAxis* axis, qreal minValue, qreal maxValue, qreal& correction);

    public:
        qreal m_barWidth;
        ChartFillDrawHelper* m_fillHelper;
        ChartBorder* m_border;
        bool m_sideBySide;
        int m_barDistanceFixed;
    };
}

/* ChartBarSeriesPrivate */
ChartBarSeriesPrivate::ChartBarSeriesPrivate()
{
    m_fillHelper = Q_NULL;
    m_border = Q_NULL;
}

ChartBarSeriesPrivate::~ChartBarSeriesPrivate()
{
    delete m_border;
}

void ChartBarSeriesPrivate::init()
{
    QTN_P(ChartBarSeries)
    m_barWidth = 0.6;
    m_sideBySide = true;

    m_barDistanceFixed = 1;

    m_border = new ChartBorder(&p);
    m_fillHelper = new ChartFillDrawHelper(&p);

    m_fillHelper->setFillStyle(Qt::LinearGradientPattern);
    m_fillHelper->setGradientDirection(ChartFillDrawHelper::RightToLeft);

    p.setLabel(new ChartBarSeriesLabel());
}

ChartSeriesItem* ChartBarSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartBarSeriesItem(series, areaItem);
}

void ChartBarSeriesPrivate::correctAxisSideMargins(ChartAxis* axis, qreal /*minValue*/, qreal /*maxValue*/, qreal& correction)
{
    if (!axis->isValuesAxis())
        correction = qMax(correction, 0.5);
}

/* ChartBarSeries */
ChartBarSeries::ChartBarSeries()
    : Chart2DSeries(*new ChartBarSeriesPrivate, Q_NULL)
{
    QTN_D(ChartBarSeries)
    d.init();
}

ChartBarSeries::ChartBarSeries(ChartBarSeriesPrivate& d, QObject* parent)
    : Chart2DSeries(d, parent)
{
    d.init();
}

ChartBarSeries::~ChartBarSeries()
{
}

qreal ChartBarSeries::width() const 
{
    QTN_D(const ChartBarSeries)
    return d.m_barWidth;
}

void ChartBarSeries::setWidth(qreal width) 
{
    QTN_D(ChartBarSeries)
    d.m_barWidth = width;
    chartUpdate();
}

void ChartBarSeries::setSideBySide(bool sideBySide) 
{
    QTN_D(ChartBarSeries)
    d.m_sideBySide = sideBySide;
    chartUpdate();
}

bool ChartBarSeries::isSideBySide() const 
{
    QTN_D(const ChartBarSeries)
    return d.m_sideBySide;
}

int ChartBarSeries::distanceFixed() const 
{
    QTN_D(const ChartBarSeries)
    return d.m_barDistanceFixed;
}

void ChartBarSeries::setDistanceFixed(int barDistanceFixed) 
{
    QTN_D(ChartBarSeries)
    d.m_barDistanceFixed = barDistanceFixed;
    chartUpdate();
}


namespace Qtitan
{
    /* ChartStackedBarSeriesPrivate */
    class ChartStackedBarSeriesPrivate : public ChartBarSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartStackedBarSeries)
    public:
        explicit ChartStackedBarSeriesPrivate();
        ~ChartStackedBarSeriesPrivate();
    public:
        void init();
    public:
        virtual ChartArea* createArea(QObject* parent);
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual void correctAxisSideMargins(ChartAxis* axis, qreal minValue, qreal maxValue, qreal& correction);
    public:
        qreal m_stackHeight;
        int m_stackGroup;
    };
}

/* ChartBarSeriesPrivate */
ChartStackedBarSeriesPrivate::ChartStackedBarSeriesPrivate()
{
    m_stackHeight = 0.0;
    m_stackGroup = 0;
}

ChartStackedBarSeriesPrivate::~ChartStackedBarSeriesPrivate()
{
}

void ChartStackedBarSeriesPrivate::init()
{
    QTN_P(ChartBarSeries)
    ChartBarSeriesLabel* label = new ChartBarSeriesLabel();
    label->setPosition(ChartBarSeriesLabel::ChartBarLabelCenter);
    p.setLabel(label);
}

ChartArea* ChartStackedBarSeriesPrivate::createArea(QObject* parent)
{
    return new ChartArea2D(parent);
}

ChartSeriesItem* ChartStackedBarSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartStackedBarSeriesItem(series, areaItem);
}

bool ChartStackedBarSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    return dynamic_cast<ChartArea2D*>(area) != Q_NULL;
}

void ChartStackedBarSeriesPrivate::correctAxisSideMargins(ChartAxis* axis, qreal minValue, qreal maxValue, qreal& correction)
{
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);

    if (!axis->isValuesAxis())
        correction = qMax(correction, 0.5);
}


/* ChartBarSeries */
ChartStackedBarSeries::ChartStackedBarSeries()
    : ChartBarSeries(*new ChartStackedBarSeriesPrivate, Q_NULL)
{
    QTN_D(ChartStackedBarSeries);
    d.init();
}

ChartStackedBarSeries::~ChartStackedBarSeries()
{
}

void ChartStackedBarSeries::setStackHeight(qreal stackHeight) 
{
    QTN_D(ChartStackedBarSeries);
    d.m_stackHeight = stackHeight;
    chartUpdate();
}

qreal ChartStackedBarSeries::stackHeight() const 
{
    QTN_D(const ChartStackedBarSeries);
    return d.m_stackHeight;
}

void ChartStackedBarSeries::setStackGroup(int stackGroup) 
{
    QTN_D(ChartStackedBarSeries);
    d.m_stackGroup = stackGroup;
    chartUpdate();
}

int ChartStackedBarSeries::stackGroup() const 
{
    QTN_D(const ChartStackedBarSeries);
    return d.m_stackGroup;
}


/* ChartBarSeries */
ChartBarSeriesItem::ChartBarSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartArea2DSeriesItem(series, areaItem)
{
    m_barCount = 0;
    m_barIndex = -1;
    m_firstStack = true;
}

ChartSeriesPointItem* ChartBarSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartBarSeriesPointItem(point, parentItem);
}

void ChartBarSeriesItem::beforeUpdateRange()
{
    ChartAreaItem* area = areaItem();
    for (int i = 0; i < area->seriesItem()->count(); i++)
    {
        ChartBarSeriesItem* item = (ChartBarSeriesItem*)area->seriesItem()->at(i);

        ChartBarSeries* impl = qobject_cast<ChartBarSeries*>(item->data()->series());
        if (!impl)
            continue;

        if (item != this)
            return;

        break;
    }

    int barCount = 0;

    QMap<int, int> mapGroupToIndex;
    for (int i = 0; i < area->seriesItem()->count(); i++)
    {
        ChartBarSeriesItem* item = (ChartBarSeriesItem*)area->seriesItem()->at(i);

        ChartBarSeries* impl = qobject_cast<ChartBarSeries*>(item->data()->series());
        if (!impl)
            continue;

        ChartStackedBarSeries* pStackedStyle = qobject_cast<ChartStackedBarSeries*>(impl);

        if (pStackedStyle)
        {
            if (mapGroupToIndex.contains(pStackedStyle->stackGroup()))
            {
                item->m_barIndex = mapGroupToIndex.value(pStackedStyle->stackGroup());
                item->m_firstStack = false;
            }
            else
            {
                mapGroupToIndex[pStackedStyle->stackGroup()] = barCount;

                item->m_barIndex = barCount;
                item->m_firstStack = true;

                barCount++;
            }
        }
        else
        {
            item->m_barIndex = barCount;
            item->m_firstStack = true;

            if (impl->isSideBySide())
                barCount++;
        }
    }

    for (int i = 0; i < area->seriesItem()->count(); i++)
    {
        ChartBarSeriesItem* item = (ChartBarSeriesItem*)area->seriesItem()->at(i);

        ChartBarSeries* impl = qobject_cast<ChartBarSeries*>(item->data()->series());
        if (!impl)
            continue;

        if (impl->isSideBySide())
        {
            item->m_barCount = barCount;
        }
        else
        {
            item->m_barIndex = 0;
            item->m_barCount = 1;
        }
    }
}

void ChartBarSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;

    ChartBarSeries* series = (ChartBarSeries*)data()->series();
    series->qtn_d().m_fillHelper->drawGradientRect(p, boundingRect, beginColor(), endColor());
    ChartPainter::painter()->drawInnerBorder(p, rect, beginColor().dark(), 1);
}


/* ChartStackedBarSeriesItem */
ChartStackedBarSeriesItem::ChartStackedBarSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartBarSeriesItem(series,areaItem)
{
    m_barCount = 0;
    m_barIndex = -1;
}

ChartStackedBarSeriesItem::~ChartStackedBarSeriesItem()
{
}

ChartSeriesPointItem* ChartStackedBarSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartStackedBarSeriesPointItem(point, parentItem);
}

void ChartStackedBarSeriesItem::updateRange()
{
    ChartBarSeriesItem::updateRange();

    ChartAreaItem* area = areaItem();

    bool found = false;

    for (int i = 0; i < area->seriesItem()->count(); i++)
    {
        ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(i);

        if (qobject_cast<ChartStackedBarSeries*>(item->data()->series()))
        {
            ChartStackedBarSeriesItem* stackedBarSeriesItem = (ChartStackedBarSeriesItem*)item;

            if (stackedBarSeriesItem->m_firstStack && item == this)
            {
                found = true;
                break;
            }
        }
    }
    if (!found)
        return;

    QMap<qreal, qreal> mapValues;

    for (int j = 0; j < area->seriesItem()->count(); j++)
    {
        ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(j);

        if (qobject_cast<ChartStackedBarSeries*>(item->data()->series()))
        {
            ChartStackedBarSeriesItem* stackedBarSeriesItem = (ChartStackedBarSeriesItem*)item;
            if (stackedBarSeriesItem->m_barIndex != m_barIndex)
                continue;

            for (int i = 0; i < stackedBarSeriesItem->m_pointsItem->count(); i++)
            {
                ChartStackedBarSeriesPointItem* pointItem = (ChartStackedBarSeriesPointItem*)stackedBarSeriesItem->m_pointsItem->at(i);

//                qreal value = pointItem->point()->value(0);
                qreal value = pointItem->point()->valueY();

                pointItem->m_valueFrom = mapValues[pointItem->point()->valueX()];
                pointItem->m_internalValue = pointItem->m_valueTo = pointItem->m_valueFrom + value;

                mapValues[pointItem->point()->valueX()] = pointItem->m_valueTo;
            }
        }
    }

    qreal stackHeight = ((ChartStackedBarSeries*)series())->stackHeight();
    if (stackHeight != 0)
    {
        for (int j = 0; j < area->seriesItem()->count(); j++)
        {
            ChartSeriesItem* item = (ChartSeriesItem*)area->seriesItem()->at(j);

            if (qobject_cast<ChartStackedBarSeries*>(item->data()->series()))
            {
                ChartStackedBarSeriesItem* stackedBarSeriesItem = (ChartStackedBarSeriesItem*)item;
                if (stackedBarSeriesItem->m_barIndex != m_barIndex)
                    continue;

                for (int i = 0; i < stackedBarSeriesItem->m_pointsItem->count(); i++)
                {
                    ChartStackedBarSeriesPointItem* pointItem = (ChartStackedBarSeriesPointItem*)stackedBarSeriesItem->m_pointsItem->at(i);

                    qreal dLimit = mapValues[pointItem->point()->valueX()];

                    pointItem->m_valueFrom = pointItem->m_valueFrom * stackHeight / dLimit;
                    pointItem->m_internalValue = pointItem->m_valueTo = pointItem->m_valueTo * stackHeight / dLimit;
                }
            }
        }
    }
}


/* ChartBarSeriesPointItem */
ChartBarSeriesPointItem::ChartBarSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartSeriesPointItem(point, parentItem)
{
}

QRectF ChartBarSeriesPointItem::getScreenRect() const
{
    ChartBarSeriesItem* item = (ChartBarSeriesItem*)seriesItem();
    ChartBarSeries* seriesStyle = (ChartBarSeries*)item->series();

    ChartAxisItem* axisXView = item->axisItemX();
    ChartAxisItem* axisYView = item->axisItemY();

    if (item->m_barIndex != -1)
    {
        int count = item->m_barCount;
        int index = item->m_barIndex;

        qreal offset = (1 + seriesStyle->distanceFixed()) / axisXView->scale();

        qreal barWidth = (seriesStyle->width() - offset * (count - 1)) / count;

        QPointF ptTopLeft(item->screenPoint(m_point->valueX() -
            seriesStyle->width() * 0.5 + barWidth * index + offset * index, internalValue()));

        qreal zero = qMax(0.0, axisYView->axis()->minValue());

        QPointF ptBottomRight(item->screenPoint(m_point->valueX() -
            seriesStyle->width() * 0.5 + barWidth * index + offset * index + barWidth, zero));

        QRectF rc(ptTopLeft, ptBottomRight);
        rc.setWidth(rc.width() /*+ 1*/);
        rc.setHeight(rc.height() /*+ 1*/);

        return rc;
    }
    return QRectF(0, 0, 0, 0);
}

void ChartBarSeriesPointItem::paint(QPainter* p)
{
    ChartArea2DItem* areaItem = (ChartArea2DItem*)seriesItem()->areaItem();
    ChartArea2D* area = (ChartArea2D*)areaItem->area();
    ChartBarSeries* impl = (ChartBarSeries*)seriesItem()->data()->series();

    QRectF rc(getScreenRect());

    if (area->isRotated())
    {
        ChartFillDrawHelper* fillStyle = impl->qtn_d().m_fillHelper->createRotatedHelper();
        fillStyle->drawGradientRect(p, rc, startColor(), finalColor());
        delete fillStyle;
    }
    else
        impl->qtn_d().m_fillHelper->drawGradientRect(p, rc, startColor(), finalColor());

    QColor clrBorder = impl->qtn_d().m_border->color();
    if (!clrBorder.isValid()) 
        clrBorder = startColor().dark();

    impl->qtn_d().m_border->drawInnerBorder(p, rc, clrBorder);
}

void ChartBarSeriesPointItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -1, -1);

    ChartBarSeries* impl = (ChartBarSeries*)seriesItem()->data()->series();

    impl->qtn_d().m_fillHelper->drawGradientRect(p, rect, startColor(), finalColor());
    ChartPainter::painter()->drawInnerBorder(p, boundingRect, startColor().dark(), 1);
}

bool ChartBarSeriesPointItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if (hitInfoFlags & ChartHitInfo::HitDataPoint)
    {
        QRectF rectBounds = getScreenRect();
        if (rectBounds.contains(pt))
        {
            if (ChartDataPoint* dataPoint = point())
            {
                if (ChartBarSeries* series = (ChartBarSeries*)dataPoint->series())
                {
                    if (Chart* chart = series->getChartObject())
                    {
                        const SeriesList& seriesCollection = chart->series();

                        hitInfo->m_index1 = seriesCollection.indexOf(dataPoint->series());
                        hitInfo->m_index2 = series->indexOf(dataPoint);

                        hitInfo->m_hitInfo = ChartHitInfo::HitDataPoint;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/* ChartStackedBarSeriesPointItem */
ChartStackedBarSeriesPointItem::ChartStackedBarSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartBarSeriesPointItem(point, parentItem)
{
}

QRectF ChartStackedBarSeriesPointItem::getScreenRect() const
{
    ChartStackedBarSeriesItem* item = (ChartStackedBarSeriesItem*)seriesItem();
    ChartStackedBarSeries* seriesStyle = static_cast<ChartStackedBarSeries*>(item->series());

    if (item->m_barIndex != -1)
    {
        int count = item->m_barCount;
        int index = item->m_barIndex;

        qreal offset = 3.0 / item->axisItemX()->scale();

        qreal barWidth = (seriesStyle->width() - offset * (count - 1)) / count;

        QPointF ptTopLeft(item->screenPoint(m_point->valueX() -
            seriesStyle->width() * 0.5 + barWidth * index + offset * index, m_valueTo));

        QPointF ptBottomRight(item->screenPoint(m_point->valueX() -
            seriesStyle->width() * 0.5 + barWidth * index + offset * index + barWidth, m_valueFrom));

        QRectF rc(ptTopLeft, ptBottomRight);
        rc.setWidth(rc.width());
        rc.setHeight(rc.height());
        return rc;
    }

    return QRectF(0.0, 0.0, 0.0, 0.0);
}

void ChartStackedBarSeriesPointItem::paint(QPainter* p)
{
    ChartBarSeriesPointItem::paint(p);
}


namespace Qtitan
{
    /* ChartStackedBarSeriesPrivate */
    class ChartBarSeriesLabelPrivate : public ChartSeriesLabelPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartBarSeriesLabel)
    public:
        explicit ChartBarSeriesLabelPrivate();
        virtual ~ChartBarSeriesLabelPrivate();
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
    public:
        ChartBarSeriesLabel::BarLabelPosition m_position;
    };
}

ChartBarSeriesLabelPrivate::ChartBarSeriesLabelPrivate()
{
    m_position = ChartBarSeriesLabel::ChartBarLabelTop;
}

ChartBarSeriesLabelPrivate::~ChartBarSeriesLabelPrivate()
{
}

GraphicsItem* ChartBarSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
{
    QTN_P(ChartBarSeriesLabel)
    return new ChartBarSeriesLabelItem(&p, pointItem, parentItem);
}


/* ChartBarSeriesLabel */
ChartBarSeriesLabel::ChartBarSeriesLabel(QObject* p)
    : ChartSeriesLabel(*new ChartBarSeriesLabelPrivate, p)  
{
}

ChartBarSeriesLabel::~ChartBarSeriesLabel()
{
}

/* ChartBarSeriesLabelItem */
ChartBarSeriesLabelItem::ChartBarSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartArea2DSeriesLabelItem(label, pointItem, parentItem)
{
}

void ChartBarSeriesLabel::setPosition(BarLabelPosition position) 
{
    QTN_D(ChartBarSeriesLabel)
    d.m_position = position;
    chartUpdate();
}

ChartBarSeriesLabel::BarLabelPosition ChartBarSeriesLabel::position() const 
{
    QTN_D(const ChartBarSeriesLabel)
    return d.m_position;
}

qreal ChartBarSeriesLabelItem::locationAngle() const
{
    ChartBarSeriesItem* item = (ChartBarSeriesItem*)m_pointItem->seriesItem();

    if (((ChartBarSeriesLabel*)m_label)->position() == ChartBarSeriesLabel::ChartBarLabelTop)
    {
        qreal value = m_pointItem->internalValue();

        if (((ChartArea2D*)item->area())->isRotated())
        {
            bool top = value >= 0;
            if (item->axisItemY()->axis()->isReversed())
                top = !top;

            return top ? 0.0 : 180.0;
        }
        else
        {
            bool top = value >= 0;
            if (item->axisItemY()->axis()->isReversed())
                top = !top;

            return top ? 90.0 : 270.0;
        }
    }
    return 0;
}

QPointF ChartBarSeriesLabelItem::locationPoint() const
{
    QRectF rc(((ChartBarSeriesPointItem*)m_pointItem)->getScreenRect());

    if (((ChartBarSeriesLabel*)m_label)->position() == ChartBarSeriesLabel::ChartBarLabelTop)
    {
        qreal angle = locationAngle();

        if (angle == 0.0)
            return QPointF(rc.right(), rc.top() + rc.height() * 0.5);

        if (angle == 180.0)
            return QPointF(rc.left(), rc.top() + rc.height() * 0.5);

        if (angle == 90.0)
            return QPointF(rc.left() + rc.width() * 0.5, rc.top());

        if (angle == 270.0)
            return QPointF(rc.left() + rc.width() * 0.5, rc.bottom());

        return rc.center();
    }
    else
        return rc.center();
}

void ChartBarSeriesLabelItem::calculateLayout()
{
    if (((ChartBarSeriesLabel*)m_label)->position() == ChartBarSeriesLabel::ChartBarLabelTop)
        ChartArea2DSeriesLabelItem::calculateLayout();
    else
        ChartArea2DSeriesLabelItem::calculateLayout(locationPoint());
}

void ChartBarSeriesLabelItem::paint(QPainter* p)
{
    if (((ChartBarSeriesLabel*)m_label)->position() == ChartBarSeriesLabel::ChartBarLabelTop)
        ChartArea2DSeriesLabelItem::paint(p);
    else
        ChartArea2DSeriesLabelItem::paint(p, locationPoint());
}

