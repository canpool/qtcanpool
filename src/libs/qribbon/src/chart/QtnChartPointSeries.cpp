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

#include "QtnChartPointSeries.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartPalette.h"
#include "QtnChart.h"
#include "QtnChartMarker.h"
#include "QtnChartArea.h"
#include "QtnChartMath.h"
#include "QtnChartArea2D.h"
#include "QtnChartAxis.h"
#include "QtnChartAxisItemPrivate.h"


using namespace Qtitan;

/*!
    \class Qtitan::ChartPointSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPointSeriesPrivate::ChartPointSeriesPrivate()
{
    m_marker = Q_NULL;
}

ChartPointSeriesPrivate::~ChartPointSeriesPrivate()
{
    delete m_marker;
}

void ChartPointSeriesPrivate::init()
{
    QTN_P(ChartPointSeries);
    m_marker = new ChartMarker(&p);
    p.setLabel(new ChartPointSeriesLabel());
}

ChartSeriesItem* ChartPointSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartPointSeriesItem(series, areaItem);
}


/* ChartPointSeries */
ChartPointSeries::ChartPointSeries()
    : Chart2DSeries(*new ChartPointSeriesPrivate, Q_NULL)
{
    QTN_D(ChartPointSeries)
    d.init();
}

ChartPointSeries::ChartPointSeries(ChartPointSeriesPrivate& d, QObject* parent)
    : Chart2DSeries(d, parent)
{
    d.init();
}   

ChartPointSeries::~ChartPointSeries()
{
}

int ChartPointSeries::markerSize() const
{
    QTN_D(const ChartPointSeries)
    return d.m_marker->size();
}

void ChartPointSeries::setMarkerSize(int size)
{
    QTN_D(ChartPointSeries)
    d.m_marker->setSize(size);
}

void ChartPointSeries::setMarkerBorderVisible(bool borderVisible)
{
    QTN_D(ChartPointSeries)
    d.m_marker->setBorderVisible(borderVisible);
}

bool ChartPointSeries::markerBorderVisible() const
{
    QTN_D(const ChartPointSeries)
    return d.m_marker->borderVisible();
}

MarkerType ChartPointSeries::markerType() const
{
    QTN_D(const ChartPointSeries)
    return d.m_marker->type();
}

void ChartPointSeries::setMarkerType(MarkerType type)
{
    QTN_D(ChartPointSeries)
    d.m_marker->setType(type);
}

bool ChartPointSeries::isMarkerVisible() const
{
    QTN_D(const ChartPointSeries)
    return d.m_marker->isVisible();
}

void ChartPointSeries::setMarkerVisible(bool visible)
{
    QTN_D(ChartPointSeries)
    d.m_marker->setVisible(visible);
}


/*!
    \class Qtitan::ChartPointSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartPointSeriesItem::ChartPointSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartArea2DSeriesItem(series, areaItem)
{
}

ChartSeriesPointItem* ChartPointSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPointSeriesPointItem(point, parentItem);
}

void ChartPointSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -2, -2);

    ChartPointSeries* impl = static_cast<ChartPointSeries*>(m_data->series());

    QPointF ptCenter(rect.center().x(), rect.center().y()) ;
    int size = qMin(rect.width(), rect.height());

    impl->qtn_d().m_marker->paint(p, ptCenter, size, beginColor(), endColor(), beginColor().dark());
}

/*!
    \class Qtitan::ChartSeriesPointItem
    \inmodule QtitanChart
    \internal
*/
ChartSeriesPointItem::ChartSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
: GraphicsItem(parentItem)
{
    m_point = point;
//    m_internalValue = m_point->value(0);
    m_internalValue = m_point->valueY();
}

ChartSeriesPointItem::~ChartSeriesPointItem()
{
}

QColor ChartSeriesPointItem::startColor() const
{
    ChartSeriesItem* item = seriesItem();

    if (m_point->color().isValid())
        return m_point->color();

    if (item->series()->isAlternateColor())
        return ChartTheme::themeTopParent(this)->chartPalette()->lightColor(m_point->qtn_d().m_palIndex);

    return item->beginColor();
}

QColor ChartSeriesPointItem::finalColor() const
{
    ChartSeriesItem* item = seriesItem();

    if (m_point->color().isValid())
        return ChartTheme::lightColor(m_point->color());

    if (item->series()->isAlternateColor())
        return ChartTheme::themeTopParent(this)->chartPalette()->darkColor(m_point->qtn_d().m_palIndex);
    return item->endColor();
}

QString ChartSeriesPointItem::legendName() const
{
    QString strText = m_point->legendText();
    if (!strText.isEmpty())
        return strText;
    return QString();
}

void ChartSeriesPointItem::drawLegendComponent(const QRect& rect)
{
    seriesItem()->drawLegendComponent(rect);
}

ChartDataPoint* ChartSeriesPointItem::point() const 
{
    return m_point;
}

ChartSeriesItem* ChartSeriesPointItem::seriesItem() const 
{
    return (ChartSeriesItem*)(parentItem()->parentItem());
}

GraphicsItem* ChartSeriesItem::pointsItem() const 
{
    return m_pointsItem;
}

qreal ChartSeriesPointItem::internalValue() const 
{
    return m_internalValue;
}

void ChartSeriesPointItem::updateMinMaxRange(qreal& /*minValue*/, qreal& /*maxValue*/) const 
{
}


/*!
    \class Qtitan::ChartPointSeriesPointItem
    \inmodule QtitanChart
    \internal
*/
ChartPointSeriesPointItem::ChartPointSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartSeriesPointItem(point, parentItem)
{
}

ChartPointSeriesPointItem::~ChartPointSeriesPointItem()
{
}

QPointF ChartPointSeriesPointItem::screenPoint() const
{
    ChartPointSeriesItem* item = (ChartPointSeriesItem*)seriesItem();

    return item->screenPoint(m_point->valueX(), m_internalValue);
}

void ChartPointSeriesPointItem::paint(QPainter* p)
{
    if (::qIsNaN(m_internalValue))
        return;

    QColor col = startColor();
    QColor col2 = finalColor();
    QColor colorBorder = startColor().dark();
    ((ChartPointSeries*)seriesItem()->series())->qtn_d().m_marker->paint(p, screenPoint(), col, col2, colorBorder);
}

void ChartPointSeriesPointItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rect = boundingRect;
    rect.adjust(1, 1, -2, -2);

    ChartPointSeries* impl = static_cast<ChartPointSeries*>(seriesItem()->data()->series());

    QPointF ptCenter(rect.center().x(), rect.center().y()) ;
    int size = qMin(rect.width(), rect.height());

    impl->qtn_d().m_marker->paint(p, ptCenter, size, startColor(), finalColor(), startColor().dark());
}

bool ChartPointSeriesPointItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if (hitInfoFlags & ChartHitInfo::HitDataPoint)
    {
        ChartMarker* mark = ((ChartPointSeries*)seriesItem()->series())->qtn_d().m_marker;
//        if (mark && mark->isVisible())
        {
            int size = mark->size()*0.5;

            QPointF pnt = screenPoint();
            QRectF rectBounds(pnt, pnt);
            rectBounds.adjust(-size, -size, size, size);
            if (rectBounds.contains(pt))
            {
                if (ChartDataPoint* dataPoint = point())
                {
                    if (ChartPointSeries* series = (ChartPointSeries*)dataPoint->series())
                    {
                        if (Chart* chart = series->getChartObject())
                        {
                            const SeriesList& seriesList = chart->series();

                            hitInfo->m_index1 = seriesList.indexOf(series);
                            hitInfo->m_index2 = series->indexOf(dataPoint);

                            hitInfo->m_hitInfo = ChartHitInfo::HitDataPoint;
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

/* ChartPointSeriesLabelItem */
class ChartPointSeriesLabelItem : public ChartArea2DSeriesLabelItem
{
public:
    ChartPointSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
    virtual ~ChartPointSeriesLabelItem();
public:
    virtual QPointF locationPoint() const;
    virtual qreal locationAngle() const;
};

/*!
    \class Qtitan::ChartPointSeriesLabelItem
    \inmodule QtitanChart
    \internal
*/
ChartPointSeriesLabelItem::ChartPointSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartArea2DSeriesLabelItem(label, pointItem, parentItem)
{
}

ChartPointSeriesLabelItem::~ChartPointSeriesLabelItem()
{
}

QPointF ChartPointSeriesLabelItem::locationPoint() const
{
    return ((ChartPointSeriesPointItem*)m_pointItem)->screenPoint();
}

qreal ChartPointSeriesLabelItem::locationAngle() const
{
    ChartArea2DSeriesItem* item = (ChartArea2DSeriesItem*)m_pointItem->seriesItem();

    ChartArea2D* area = (ChartArea2D*)item->area();

    qreal lineAngle = ((ChartPointSeriesLabel*)m_label)->angle();

    if (item->axisItemX()->axis()->isReversed())
        lineAngle = 180 - lineAngle;

    if (item->axisItemY()->axis()->isReversed())
        lineAngle = - lineAngle;

    if (area->isRotated())
    {
        lineAngle = 90 - lineAngle;
    }
    return lineAngle;
}

namespace Qtitan
{
    /* ChartPointSeriesLabelPrivate */
    class ChartPointSeriesLabelPrivate : public ChartSeriesLabelPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPointSeriesLabel)
    public:
        explicit ChartPointSeriesLabelPrivate();
        virtual ~ChartPointSeriesLabelPrivate();
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
    public:
        int m_angle;
    };
}

/*!
    \class Qtitan::ChartPointSeriesLabelPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPointSeriesLabelPrivate::ChartPointSeriesLabelPrivate()
{
    m_angle = 45;
}

ChartPointSeriesLabelPrivate::~ChartPointSeriesLabelPrivate()
{
}

GraphicsItem* ChartPointSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
{
    QTN_P(ChartPointSeriesLabel)
    return new ChartPointSeriesLabelItem(&p, pointItem, parentItem);
}

/* ChartPointSeriesLabel */
ChartPointSeriesLabel::ChartPointSeriesLabel(QObject* p)
    : ChartSeriesLabel(*new ChartPointSeriesLabelPrivate, p)
{
}

ChartPointSeriesLabel::~ChartPointSeriesLabel()
{
}

void ChartPointSeriesLabel::setAngle(int angle) 
{
    QTN_D(ChartPointSeriesLabel);
    d.m_angle = angle;
    chartUpdate();
}

int ChartPointSeriesLabel::angle() const 
{
    QTN_D(const ChartPointSeriesLabel);
    return d.m_angle;
}



