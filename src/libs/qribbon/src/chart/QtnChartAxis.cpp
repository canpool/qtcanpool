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
#include <math.h>
#include <qmath.h>
#include <qnumeric.h>
#include "QtnChart.h"
#include "QtnChartAxis.h"
#include "QtnChartAxisPrivate.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartArea2D.h"
#include "QtnChartScaleTypeMap.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartThemePrivate.h"
#include "QtnAbstractSeries.h"
#include "QtnChartPainter.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartMath.h"

using namespace Qtitan;

/* ChartAxisRange */
/*!
    \class Qtitan::ChartAxisRange
    \inmodule QtitanChart
    \internal
*/
ChartAxisRange::ChartAxisRange(QObject* parent)
    : ChartComponent(parent) 
{
    Q_ASSERT(parent);

    m_axis = (ChartAxis*)parent;
    m_minValue = 0.0;
    m_maxValue = 1.0;

    m_viewMinValue = 0.0;
    m_viewMaxValue = 1.0;

    m_sideMargins = true;
    m_showZeroLevel = false;

    m_autoRange = true;
    m_viewAutoRange = true;

    m_zoomLimit = 0.1;
}

ChartAxisRange::~ChartAxisRange()
{
}

qreal ChartAxisRange::maxValue() const 
{
    return m_maxValue;
}

qreal ChartAxisRange::minValue() const 
{
    return m_minValue;
}

bool ChartAxisRange::sideMargins() const 
{
    return m_sideMargins;
}

bool ChartAxisRange::showZeroLevel() const 
{
    return m_showZeroLevel;
}

bool ChartAxisRange::isAutoRange() const 
{
    return m_autoRange;
}

qreal ChartAxisRange::viewMaxValue() const 
{
    return m_viewMaxValue;
}

qreal ChartAxisRange::viewMinValue() const 
{
    return  m_viewMinValue;
}

bool ChartAxisRange::isViewAutoRange() const 
{
    return m_viewAutoRange;
}

qreal ChartAxisRange::zoomLimit() const 
{
    return m_zoomLimit;
}

void ChartAxisRange::setZoomLimit(qreal zoomLimit) 
{
    m_zoomLimit = zoomLimit;
}

void ChartAxisRange::setSideMargins(bool sideMargins)
{
    m_sideMargins = sideMargins;
    chartUpdate();
}

void ChartAxisRange::setShowZeroLevel(bool showZeroLevel)
{
    m_showZeroLevel = showZeroLevel;
    chartUpdate();
}

void ChartAxisRange::setAutoRange(bool autRange)
{
    m_autoRange = autRange;
    chartUpdate();
}

void ChartAxisRange::setViewAutoRange(bool autRange)
{
    if (m_viewAutoRange != autRange)
    {
        m_viewAutoRange = autRange;

        if (m_viewAutoRange)
        {
            m_viewMinValue = m_minValue;
            m_viewMaxValue = m_maxValue;
        }
        m_axis->chartUpdate(UpdateLayout);
    }
}

void ChartAxisRange::setMaxValue(qreal maxValue)
{
    m_maxValue = maxValue;

    if (m_viewAutoRange)
        m_viewMaxValue = maxValue;

    if (m_axis) 
        m_axis->chartUpdate(UpdateLayout);
}

void ChartAxisRange::setMinValue(qreal minValue)
{
    m_minValue = minValue;

    if (m_viewAutoRange)
        m_viewMinValue = minValue;

    if (m_axis) 
        m_axis->chartUpdate(UpdateLayout);
}


void ChartAxisRange::setViewMaxValue(qreal maxValue)
{
    m_viewMaxValue = maxValue;
    if (m_axis) 
        m_axis->chartUpdate(UpdateLayout);
}

void ChartAxisRange::setViewMinValue(qreal minValue)
{
    m_viewMinValue = minValue;
    if (m_axis) 
        m_axis->chartUpdate(UpdateLayout);
}


void ChartAxisRange::updateRange(ChartAxisItem* item)
{
    if (!m_autoRange)
        return;

    bool first = true;

    qreal minValue = 0.0;
    qreal maxValue = 1.0;

    for (int i = 0; i < item->m_listDataTables.size(); i++)
    {
        ChartSeriesItem* seriesItem = (ChartSeriesItem*)item->m_listDataTables[i];

        ChartDataTablePrivate* series = seriesItem->data();

        if (!series)
            continue;

        for (int j = 0; j < seriesItem->pointsItem()->count(); j++)
        {
            ChartSeriesPointItem* pointItem = (ChartSeriesPointItem*)seriesItem->pointsItem()->at(j);

            ChartDataPoint* point = pointItem->point();

            qreal value = m_axis->isValuesAxis() ? pointItem->internalValue() : point->valueX();

            if (/*ChartMath::isNan(value)*/qIsNaN(value))
                continue;

            if (first)
            {
                minValue = maxValue = value;
                first = false;
            }
            else
            {
                maxValue = qMax(maxValue, value);
                minValue = qMin(minValue, value);
            }
/*
            if (m_axis->isValuesAxis() && point->valueLength() == 4)
            {
                qreal value = point->value(1);

                maxValue = qMax(maxValue, value);
                minValue = qMin(minValue, value);
            }
*/
            if (m_axis->isValuesAxis())
                pointItem->updateMinMaxRange(minValue, maxValue);
        }
    }

    m_minValue = minValue;
    m_maxValue = maxValue;

    correct(item);

    if (m_viewAutoRange)
    {
        m_viewMinValue = m_minValue;
        m_viewMaxValue = m_maxValue;
    }
}

void ChartAxisRange::correct(ChartAxisItem* item)
{
    bool logarithmic = false;
    qreal zero = logarithmic ? 1.0 : 0.0;

    if (m_sideMargins)
    {
        qreal correction = m_axis->calcSideMarginsValue(m_minValue, m_maxValue);

        QVector<ChartSeriesItem*>& arrSeries = item->m_listDataTables;

        for (int i = 0; i < arrSeries.size(); i++)
        {
            ChartSeriesItem* seriesItem = arrSeries.at(i);
            seriesItem->data()->series()->qtn_d().correctAxisSideMargins(m_axis, m_minValue, m_maxValue, correction);
        }

        if (m_minValue > zero - ChartMath::QTN_EPSILON_1 && m_showZeroLevel)
            m_minValue = 0.0;
        else if (!logarithmic || m_minValue - correction > ChartMath::QTN_EPSILON_1)
            m_minValue -= correction;

        m_maxValue += correction;
    }

    if (m_showZeroLevel)
    {
        if (m_minValue > 0.0)
            m_minValue = 0.0;

        if (m_maxValue < 0.0)
            m_maxValue = 0.0;
    }

    if (qFabs(m_minValue - m_maxValue) < ChartMath::QTN_EPSILON_1)
    {
        qreal value = qMax(0.5, qFabs(m_minValue) / 10.0);
        m_maxValue += value;
        m_minValue -= value;
    }
}


/*!
    \class Qtitan::ChartAxisPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAxisPrivate::ChartAxisPrivate()
{
    m_visible         = true;
    m_reversed        = false;
    m_gridSpacingAuto = true;
    m_allowZoom       = true;
    m_interlaced      = false;
    m_secondary       = false;
    m_thickness   = 1;
    m_minorCount  = 1;
    m_gridSpacing = 1;
}

ChartAxisPrivate::~ChartAxisPrivate()
{
}

void ChartAxisPrivate::init()
{
    QTN_P(ChartAxis)
    // m_area = area;

    m_alignment = ChartAxis::AxisNear;
    m_scaleType = ChartScaleNumerical;

    // m_axisID = axisID;

    m_gridLines = new ChartAxisGridLines(&p);
    m_label     = new ChartAxisLabel(&p);
    m_title     = new ChartAxisTitle(&p);
    m_title->setText(tr("Axis Title"));

    m_tickMarks = new ChartAxisTickMarks(&p);

    m_range = new ChartAxisRange(&p);

    m_interlacedFillHelper = new ChartFillDrawHelper(&p);
    m_interlacedFillHelper->setFillStyle(Qt::NoBrush);

    m_scaleTypeMap = ChartScaleTypeMap::create(&p);
}

void ChartAxisPrivate::setSecondary(bool secondary)
{
    m_secondary = secondary;
}

ChartFillDrawHelper* ChartAxisPrivate::interlacedFillHelper() const
{
    QTN_P(const ChartAxis)
    if (m_interlacedFillHelper->fillStyle() != Qt::NoBrush)
        return m_interlacedFillHelper;
    return ChartTheme::themeTopParent(&p)->area2DTheme()->axisTheme()->m_interlacedFillHelper;
}

QColor ChartAxisPrivate::interlacedStartColor() const
{
    QTN_P(const ChartAxis)
    if (m_clrInterlaced.isValid())
        return m_clrInterlaced;
    return ChartTheme::themeTopParent(&p)->area2DTheme()->axisTheme()->m_interlacedColor;
}

void ChartAxisPrivate::setInterlacedStartColor(const QColor& color) 
{
    QTN_P(ChartAxis)
    m_clrInterlaced = color;
    p.chartUpdate();
}

QColor ChartAxisPrivate::interlacedFinalColor() const
{
    QTN_P(const ChartAxis)
    if (m_clrInterlaced2.isValid())
        return m_clrInterlaced2;
    return ChartTheme::themeTopParent(&p)->area2DTheme()->axisTheme()->m_interlacedColor2;
}

void ChartAxisPrivate::setInterlacedFinalColor(const QColor& color) 
{
    QTN_P(ChartAxis)
    m_clrInterlaced2 = color;
    p.chartUpdate();
}

ChartAxisItem* ChartAxisPrivate::createItem(GraphicsItem* parentItem)
{
    QTN_P(ChartAxis)
    return new ChartAxisItem(&p, parentItem);
}

/* ChartAxis */
ChartAxis::ChartAxis(ChartArea2D* area, int axisID)
    : ChartComponent(area)
{
    QTN_INIT_PRIVATE(ChartAxis);
    QTN_D(ChartAxis);
    d.init();
    d.m_axisID = axisID;
}

ChartAxis::~ChartAxis()
{
    QTN_FINI_PRIVATE();
}

ChartArea2D* ChartAxis::area() const 
{
    return qobject_cast<ChartArea2D*>(parent());
}

ChartAxisLabel* ChartAxis::label() const 
{
    QTN_D(const ChartAxis)
    return d.m_label;
}

ChartAxisTickMarks* ChartAxis::tickMarks() const 
{
    QTN_D(const ChartAxis)
    return d.m_tickMarks;
}

bool ChartAxis::isInterlaced() const 
{
    QTN_D(const ChartAxis)
    return d.m_interlaced;
}

void ChartAxis::setInterlaced(bool interlaced) 
{
    QTN_D(ChartAxis)
    d.m_interlaced = interlaced;
    chartUpdate();
}

ChartAxis* ChartAxis::perpendecularAxis() const 
{
    return Q_NULL;
}

void ChartAxis::updateAxisPos(const QRect& boundingRect)
{
    QTN_D(ChartAxis)
    d.m_boundingRect = boundingRect;
}

ChartAxisTitle* ChartAxis::title() const 
{
    QTN_D(const ChartAxis)
    return d.m_title;
}

const QRect& ChartAxis::boundingRect() const
{
    QTN_D(const ChartAxis)
    return d.m_boundingRect;
}

ChartAxis::Alignment ChartAxis::alignment() const 
{
    QTN_D(const ChartAxis)
    return d.m_alignment;
}

void ChartAxis::setAlignment(ChartAxis::Alignment alignment) 
{
    QTN_D(ChartAxis)
    d.m_alignment = alignment;
    chartUpdate();
}

bool ChartAxis::isVisible() const 
{
    QTN_D(const ChartAxis)
    return d.m_visible;
}

void ChartAxis::setVisible(bool visible) 
{
    QTN_D(ChartAxis)
    d.m_visible = visible;
    chartUpdate();
}

int ChartAxis::thickness() const 
{
    QTN_D(const ChartAxis)
    return d.m_thickness;
}

void ChartAxis::setThickness(int thickness) 
{
    QTN_D(ChartAxis)
    d.m_thickness = thickness;
    chartUpdate();
}

int ChartAxis::minorCount() const 
{
    QTN_D(const ChartAxis)
    return d.m_minorCount;
}

void ChartAxis::setMinorCount(int minorCount) 
{
    QTN_D(ChartAxis)
    d.m_minorCount = minorCount;
    chartUpdate();
}

void ChartAxis::setAxisID(int axisID)
{
    QTN_D(ChartAxis)
    d.m_axisID = axisID;
}

int ChartAxis::axisID() const
{
    QTN_D(const ChartAxis)
    return d.m_axisID;
}

void ChartAxis::setReversed(bool revered) 
{
    QTN_D(ChartAxis)
    d.m_reversed = revered;
    chartUpdate();
}

bool ChartAxis::isReversed() const 
{
    QTN_D(const ChartAxis)
    return d.m_reversed;
}

bool ChartAxis::gridSpacingAuto() const 
{
    QTN_D(const ChartAxis)
    return d.m_gridSpacingAuto;
}

void ChartAxis::setGridSpacingAuto(bool gridSpacingAuto) 
{
    QTN_D(ChartAxis)
    d.m_gridSpacingAuto = gridSpacingAuto;
    chartUpdate();
}

qreal ChartAxis::gridSpacing() const 
{
    QTN_D(const ChartAxis)
    return d.m_gridSpacing;
}

void ChartAxis::setGridSpacing(qreal gridSpacing) 
{
    QTN_D(ChartAxis)
    d.m_gridSpacing = gridSpacing;
    chartUpdate();
}

bool ChartAxis::isSecondary() const 
{
    QTN_D(const ChartAxis)
    return d.m_secondary;
}

bool ChartAxis::isAllowZoom() const 
{
    QTN_D(const ChartAxis)
    return d.m_allowZoom;
}

void ChartAxis::setAllowZoom(bool bAllowZoom) 
{
    QTN_D(ChartAxis)
    d.m_allowZoom = bAllowZoom;
}

qreal ChartAxis::valueFromPoint(const QPoint& pt)
{
    QTN_D(ChartAxis)
    qreal range = viewMaxValue() - viewMinValue();

    if (range == 0.0)
        return 0.0;


    bool revered = isReversed();

    qreal value = 0.0;

    qreal axisSize = isVertical() ? qFabs(qreal(d.m_boundingRect.top() - d.m_boundingRect.bottom())) : qFabs(qreal(d.m_boundingRect.left() - d.m_boundingRect.right()));
    qreal pointOffset = 0;

    if (revered)
    {
        if (isVertical())
            pointOffset = (qreal)(pt.y() - d.m_boundingRect.bottom());
        else
            pointOffset = (qreal)(d.m_boundingRect.left() - pt.x());
    }
    else
    {
        if (isVertical())
            pointOffset = (qreal)(d.m_boundingRect.bottom() - pt.y());
        else
            pointOffset = (qreal)(pt.x() - d.m_boundingRect.left());
    }
    value = viewMinValue() + range / axisSize * pointOffset;

//    value -= 1. / 2;

    return value;
}

qreal ChartAxis::pointFromValue(qreal val)
{
    QTN_D(ChartAxis)
    qreal range = viewMaxValue() - viewMinValue();
    bool vertical = isVertical();

//    if (val == 0)
//        return vertical ? m_boundingRect.bottom() : m_boundingRect.left();

//    val += 1.0 / 2;

    qreal point = 0.0;
    qreal axisSize = vertical ? (qreal)(d.m_boundingRect.bottom() - d.m_boundingRect.top()) : (qreal)(d.m_boundingRect.right() - d.m_boundingRect.left());

    if (vertical)
        point = (qreal)d.m_boundingRect.bottom() - ((val - viewMinValue()) * axisSize / range);
    else
        point = (qreal)d.m_boundingRect.left()  + ((val - viewMinValue()) * axisSize / range);

    return point;
}

static int qtnRound(qreal value)
{
    int x = (int)floor(value);
    return fabs(x - value) < fabs(x + 1 - value) ? x : x + 1;
}

qreal ChartAxis::valueToPoint(qreal x) const
{
    QTN_D(const ChartAxis)
    qreal minVal = qMax(viewMinValue(), minValue());
    qreal maxVal = qMin(viewMaxValue(), maxValue());

    bool revered = isReversed();

    qreal value;

    if (revered)
    {
        if (isVertical())
            value = d.m_boundingRect.top() + (x - minVal) / (maxVal - minVal) * (d.m_boundingRect.height());
        else
            value = d.m_boundingRect.right() - (x - minVal) / (maxVal - minVal) * (d.m_boundingRect.width());
    }
    else
    {
        if (isVertical())
            value = d.m_boundingRect.bottom() - (x - minVal) / (maxVal - minVal) * (d.m_boundingRect.height());
        else
            value = d.m_boundingRect.left() + (x - minVal) / (maxVal - minVal) * (d.m_boundingRect.width());
    }

    return qtnRound(value);
}

qreal ChartAxis::maxValue() const
{
    QTN_D(const ChartAxis)
    return d.m_range->maxValue();
}

void ChartAxis::setMaxValue(qreal max)
{
    QTN_D(ChartAxis)
    d.m_range->setMaxValue(max);
}

qreal ChartAxis::minValue() const
{
    QTN_D(const ChartAxis)
    return d.m_range->minValue();
}

void ChartAxis::setMinValue(qreal min)
{
    QTN_D(ChartAxis)
    d.m_range->setMinValue(min);
}

void ChartAxis::setFixedRange(qreal minValue, qreal maxValue)
{
    QTN_D(ChartAxis)
    d.m_range->setMinValue(minValue);
    d.m_range->setMaxValue(maxValue);
}

void ChartAxis::setAutoRange(bool autoRange)
{
    QTN_D(ChartAxis)
    d.m_range->setAutoRange(autoRange);
}

qreal ChartAxis::viewMinValue() const
{
    QTN_D(const ChartAxis)
    return d.m_range->viewMinValue();
}

qreal ChartAxis::viewMaxValue() const
{
    QTN_D(const ChartAxis)
    return d.m_range->viewMaxValue();
}

void ChartAxis::setFixedViewRange(qreal minValue, qreal maxValue)
{
    QTN_D(ChartAxis)
    d.m_range->setViewMaxValue(maxValue);
    d.m_range->setViewMinValue(minValue);
}

void ChartAxis::setViewAutoRange(bool autoRange)
{
    QTN_D(ChartAxis)
    d.m_range->setViewAutoRange(autoRange);
}

bool ChartAxis::isViewAutoRange() const
{
    QTN_D(const ChartAxis)
    return d.m_range->isViewAutoRange();
}

void ChartAxis::setSideMargins(bool sideMargins)
{
    QTN_D(ChartAxis)
    d.m_range->setSideMargins(sideMargins);
}

bool ChartAxis::sideMargins() const
{
    QTN_D(const ChartAxis)
    return d.m_range->sideMargins();
}

void ChartAxis::setShowZeroLevel(bool showZeroLevel)
{
    QTN_D(ChartAxis)
    d.m_range->setShowZeroLevel(showZeroLevel);
}

bool ChartAxis::showZeroLevel() const
{
    QTN_D(const ChartAxis)
    return d.m_range->showZeroLevel();
}

void ChartAxis::updateRange(ChartAxisItem* item)
{
    QTN_D(ChartAxis)
    if (item->m_listDataTables.size() > 0)
    {
        ChartDataTablePrivate* series = item->m_listDataTables[0]->data();
        d.m_scaleType = isValuesAxis() ?  series->valueScaleType() : series->argScaleType();
    }

    if (d.m_scaleTypeMap)
    {
        delete d.m_scaleTypeMap;
        d.m_scaleTypeMap = Q_NULL;
    }

    d.m_scaleTypeMap = ChartScaleTypeMap::create(this);

    if (!isValuesAxis())
        d.m_scaleTypeMap->updateSeries(item);

    for (int i = 0; i < item->m_listDataTables.size(); i++)
    {
        ChartSeriesItem* seriesItem = item->m_listDataTables[i];
        seriesItem->updateRange();
    }

    d.m_range->updateRange(item);
}

bool ChartAxis::isGridLinesVisible() const
{
    QTN_D(const ChartAxis)
    return d.m_gridLines->isVisible();
}

void ChartAxis::setGridLinesVisible(bool visible)
{
    QTN_D(ChartAxis)
    d.m_gridLines->setVisible(visible);
}

bool ChartAxis::isMinorGridLinesVisible() const
{
    QTN_D(const ChartAxis)
    return d.m_gridLines->isMinorVisible();
}

void ChartAxis::setMinorGridLinesVisible(bool minorVisible)
{
    QTN_D(ChartAxis)
    d.m_gridLines->setMinorVisible(minorVisible);
}

QColor ChartAxis::gridLinesColor() const
{
    QTN_D(const ChartAxis)
    return d.m_gridLines->color();
}

QColor ChartAxis::minorGridLinesColor() const
{
    QTN_D(const ChartAxis)
    return d.m_gridLines->minorColor();
}

QColor ChartAxis::color() const
{
    QTN_D(const ChartAxis)
    if (d.m_color.isValid())
        return d.m_color;

    return ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_color;
}

void ChartAxis::setColor(const QColor& color) 
{
    QTN_D(ChartAxis)
    d.m_color = color;
    chartUpdate();
}


/* ChartAxisX */
ChartAxisX::ChartAxisX(ChartArea2D* area, int axisID)
    : ChartAxis(area, axisID)
{
    qtn_d().m_minorCount = 4;
    qtn_d().m_interlaced = true;
}

bool ChartAxisX::isVertical() const
{
    if (area()->isRotated())
        return true;
    return false;
}

bool ChartAxisX::isValuesAxis() const
{
    return false;
}

qreal ChartAxisX::calcSideMarginsValue(qreal minValue, qreal maxValue) const
{
    if (qtn_d().m_scaleType == ChartScaleNumerical)
        return (maxValue - minValue) / 15.0;
    return 0.5;
}

ChartAxis* ChartAxisX::perpendecularAxis() const
{
    Chart* chart = getChartObject();
    if (chart == Q_NULL)
        return Q_NULL;

    if (ChartComponent* p = qobject_cast<ChartComponent*>(parent()))
    {
        ChartAxis* axisY = isSecondary() ? chart->axis(QTN_CHART_Y_SECONDARY_AXIS, p) 
            : chart->axis(QTN_CHART_Y_PRIMARY_AXIS, p);
        if (axisY)
            return axisY;
    }

    if (isSecondary())
        return chart->axis(QTN_CHART_Y_SECONDARY_AXIS);

    return chart->axis(QTN_CHART_Y_PRIMARY_AXIS);
}


/* ChartAxisY */
ChartAxisY::ChartAxisY(ChartArea2D* area, int axisID)
    : ChartAxis(area, axisID)
{
    qtn_d().m_minorCount = 2;
    qtn_d().m_range->setShowZeroLevel(true);
}

bool ChartAxisY::isVertical() const
{
    if (area()->isRotated())
        return false;
    return true;
}

bool ChartAxisY::isValuesAxis() const
{
    return true;
}

qreal ChartAxisY::calcSideMarginsValue(qreal minValue, qreal maxValue) const
{
    if (qtn_d().m_scaleType != ChartScaleNumerical)
        return 0.5;

    qreal delta;
    if (minValue > 0 && maxValue > 0)
        delta = maxValue;
    else if (minValue < 0 && maxValue < 0)
        delta = -minValue;
    else
        delta = maxValue - minValue;

    return delta / 10.0;
}

ChartAxis* ChartAxisY::perpendecularAxis() const
{
    Chart* chart = getChartObject();
    if (chart == Q_NULL)
        return Q_NULL;

    if (ChartComponent* p = qobject_cast<ChartComponent*>(parent()))
    {
        ChartAxis* axis = isSecondary() ? chart->axis(QTN_CHART_X_SECONDARY_AXIS, p) 
            : chart->axis(QTN_CHART_X_PRIMARY_AXIS, p);
        if (axis)
            return axis;
    }

    if (isSecondary())
        return chart->axis(QTN_CHART_X_SECONDARY_AXIS);

    return chart->axis(QTN_CHART_X_PRIMARY_AXIS);
}

/*!
    \class Qtitan::ChartAxisLabelPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAxisLabelPrivate::ChartAxisLabelPrivate()
{
}

ChartAxisLabelPrivate::~ChartAxisLabelPrivate()
{
}

void ChartAxisLabelPrivate::init()
{
    m_textColor = QColor();
    m_visible = true;

    m_antialiasing = false;

    m_font = QFont(QObject::tr("Tahoma"), 8);
    m_font.setWeight(QFont::Normal);

    m_angle = 0;

    m_strFormat = QObject::tr("");
}

ChartAxis& ChartAxisLabelPrivate::axis() const 
{
    QTN_P(const ChartAxisLabel);
    ChartAxis* axis = qobject_cast<ChartAxis*>(p.parent());
    Q_ASSERT(axis != Q_NULL);
    return *axis;
}


/* ChartAxisLabel */
ChartAxisLabel::ChartAxisLabel(QObject* parent)
    : ChartTextComponent(parent)
{
    QTN_INIT_PRIVATE(ChartAxisLabel);
    QTN_D(ChartAxisLabel);
    d.init();
}

ChartAxisLabel::~ChartAxisLabel()
{
    QTN_FINI_PRIVATE();
}

const QFont& ChartAxisLabel::font() const 
{
    QTN_D(const ChartAxisLabel);
    return d.m_font;
}

int ChartAxisLabel::angle() const 
{
    QTN_D(const ChartAxisLabel);
    return d.m_angle;
}

void ChartAxisLabel::setAngle(int angle) 
{
    QTN_D(ChartAxisLabel);
    d.m_angle = angle;
    chartUpdate();
}

void ChartAxisLabel::setAntialiasing(bool antialiasing) 
{
    QTN_D(ChartAxisLabel);
    d.m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartAxisLabel::antialiasing() const 
{
    QTN_D(const ChartAxisLabel);
    return d.m_antialiasing;
}

bool ChartAxisLabel::isVisible() const 
{
    QTN_D(const ChartAxisLabel);
    return d.m_visible;
}

void ChartAxisLabel::setVisible(bool visible) 
{
    QTN_D(ChartAxisLabel);
    d.m_visible = visible;
    chartUpdate();
}

QString ChartAxisLabel::format() const 
{
    QTN_D(const ChartAxisLabel);
    return d.m_strFormat;
}

void ChartAxisLabel::setFormat(const QString& strFormat) 
{
    QTN_D(ChartAxisLabel);
    d.m_strFormat = strFormat;
    chartUpdate();
}

void ChartAxisLabel::setFont(const QFont& font)
{
    QTN_D(ChartAxisLabel);
    d.m_font = font;
    chartUpdate();
}

void ChartAxisLabel::setTextColor(const QColor& txtColor)
{
    QTN_D(ChartAxisLabel);
    d.m_textColor = txtColor;
    chartUpdate();
}

QColor ChartAxisLabel::textColor() const
{
    QTN_D(const ChartAxisLabel);
    return d.m_textColor.isValid() ? d.m_textColor : 
        ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_axisLabelTextColor;
}

/*!
    \class Qtitan::ChartAxisTitlePrivate
    \inmodule QtitanChart
    \internal
*/
ChartAxisTitlePrivate::ChartAxisTitlePrivate()
{
    m_font = QFont(QObject::tr("Tahoma"), 12);
    m_font.setWeight(QFont::Normal);

    m_textColor = QColor();
    m_antialiasing = true;
    m_alignment = Qt::AlignCenter;
    m_visible = false;
}

/* ChartAxisTitle */
ChartAxisTitle::ChartAxisTitle(QObject* parent)
    : ChartTextComponent(parent)
{
    QTN_INIT_PRIVATE(ChartAxisTitle);
}

ChartAxisTitle::~ChartAxisTitle()
{
    QTN_FINI_PRIVATE();
}

const QString& ChartAxisTitle::text() const 
{
    QTN_D(const ChartAxisTitle)
    return d.m_strText;
}

const QFont& ChartAxisTitle::font() const 
{
    QTN_D(const ChartAxisTitle)
    return d.m_font;
}

Qt::Alignment ChartAxisTitle::alignment() const 
{
    QTN_D(const ChartAxisTitle)
    return d.m_alignment;
}

bool ChartAxisTitle::isVisible() const 
{
    QTN_D(const ChartAxisTitle)
    return d.m_visible;
}

void ChartAxisTitle::setAntialiasing(bool antialiasing) 
{
    QTN_D(ChartAxisTitle)
    d.m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartAxisTitle::antialiasing() const 
{
    QTN_D(const ChartAxisTitle)
    return d.m_antialiasing;
}

void ChartAxisTitle::setText(const QString& title)
{
    QTN_D(ChartAxisTitle)
    d.m_strText = title;
    chartUpdate();
}

void ChartAxisTitle::setFont(const QFont& font)
{
    QTN_D(ChartAxisTitle)
    d.m_font = font;
    chartUpdate();
}

void ChartAxisTitle::setTextColor(const QColor& txtColor)
{
    QTN_D(ChartAxisTitle)
    d.m_textColor = txtColor;
    chartUpdate();
}

void ChartAxisTitle::setAlignment(Qt::Alignment alignment)
{
    QTN_D(ChartAxisTitle)
    d.m_alignment = alignment;
    chartUpdate();
}

void ChartAxisTitle::setVisible(bool visible)
{
    QTN_D(ChartAxisTitle)
    d.m_visible = visible;
    chartUpdate();
}

const QColor& ChartAxisTitle::textColor() const
{
    QTN_D(const ChartAxisTitle)
    return d.m_textColor.isValid() ? d.m_textColor : 
        ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_axisTitleTextColor;
}

/*!
    \class Qtitan::ChartAxisTickMarksPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAxisTickMarksPrivate::ChartAxisTickMarksPrivate()
{
}

ChartAxisTickMarksPrivate::~ChartAxisTickMarksPrivate()
{
}

void ChartAxisTickMarksPrivate::init()
{
    m_visible = true;
    m_minorVisible = true;
    m_crossAxis = false;
    m_thickness = 1;
    m_minorThickness = 1;
    m_length = 5;
    m_minorLength = 2;
}

/*!
    \class Qtitan::ChartAxisTickMarks
    \inmodule QtitanChart
    \internal
*/
ChartAxisTickMarks::ChartAxisTickMarks(QObject* parent)
    : ChartComponent(parent)
{
    QTN_INIT_PRIVATE(ChartAxisTickMarks);
    QTN_D(ChartAxisTickMarks);
    d.init();
}

ChartAxisTickMarks::~ChartAxisTickMarks()
{
}

bool ChartAxisTickMarks::isVisible() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_visible;
}

bool ChartAxisTickMarks::isMinorVisible() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_minorVisible;
}

int ChartAxisTickMarks::thickness() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_thickness;
}

int ChartAxisTickMarks::minorThickness() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_minorThickness;
}

int ChartAxisTickMarks::length() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_length;
}

int ChartAxisTickMarks::minorLength() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_minorLength;
}

bool ChartAxisTickMarks::isCrossAxis() const 
{
    QTN_D(const ChartAxisTickMarks);
    return d.m_crossAxis;
}

void ChartAxisTickMarks::setVisible(bool visible) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_visible = visible;
    chartUpdate();
}

void ChartAxisTickMarks::setMinorVisible(bool visible) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_minorVisible = visible;
    chartUpdate();
}

void ChartAxisTickMarks::setThickness(int thickness) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_thickness = thickness;
    chartUpdate();
}

void ChartAxisTickMarks::setMinorThickness(int thickness) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_minorThickness = thickness;
    chartUpdate();
}

void ChartAxisTickMarks::setLength(int length) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_length = length;
    chartUpdate();
}

void ChartAxisTickMarks::setMinorLength(int length) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_minorLength = length;
    chartUpdate();
}

void ChartAxisTickMarks::setCrossAxis(bool crossAxis) 
{
    QTN_D(ChartAxisTickMarks);
    d.m_crossAxis = crossAxis;
    chartUpdate();
}


/* ChartAxisGridLines */ 
ChartAxisGridLines::ChartAxisGridLines(QObject* parent)
    : ChartComponent(parent)
{
    m_visible = true;
    m_minorVisible = false;

    m_line = new ChartLineDrawHelper(this);
    m_minorLine = new ChartLineDrawHelper(this);
}

ChartAxisGridLines::~ChartAxisGridLines()
{
}

bool ChartAxisGridLines::isVisible() const 
{
    return m_visible;
}

bool ChartAxisGridLines::isMinorVisible() const 
{
    return m_minorVisible;
}

void ChartAxisGridLines::setVisible(bool visible) 
{
    m_visible = visible;
    chartUpdate();
}

ChartLineDrawHelper* ChartAxisGridLines::lineHelper() const 
{
    return m_line;
}

ChartLineDrawHelper* ChartAxisGridLines::minorLineHelper() const 
{
    return m_minorLine;
}

void ChartAxisGridLines::setMinorVisible(bool minorVisible) 
{
    m_minorVisible = minorVisible;
    chartUpdate();
}

QColor ChartAxisGridLines::color() const
{
    QColor color = m_color;
    if (!m_color.isValid())
    {
        color = ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_gridLinesColor;
        if (color.alpha() == 255)
            color.setAlpha(125);
    }
    return color;
}

QColor ChartAxisGridLines::minorColor() const
{
    if (m_minorColor.isValid())
        return m_minorColor;
    return ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_gridLinesMinorColor;
}

/*!
    \class Qtitan::ChartAxisIntervalStreak
    \inmodule QtitanChart
    \internal
*/
ChartAxisIntervalStreak::ChartAxisIntervalStreak()
{
    m_legendVisible = true;
    m_visible = true;

    m_fillHelper = new ChartFillDrawHelper(this);
    m_fillHelper->setFillStyle(Qt::SolidPattern);

    m_axisMinValue = 0;
    m_axisMaxValue = 0;
}

ChartAxisIntervalStreak::~ChartAxisIntervalStreak()
{
}

QString ChartAxisIntervalStreak::getName() const 
{
    return m_strName;
}

void ChartAxisIntervalStreak::setVisible(bool visible) 
{
    m_visible = visible;
    chartUpdate();
}

bool ChartAxisIntervalStreak::isVisible() const 
{
    return m_visible;
}

ChartFillDrawHelper* ChartAxisIntervalStreak::fillHelper() const 
{
    return m_fillHelper;
}

QString ChartAxisIntervalStreak::getAxisMinValue() const 
{
    return m_strAxisMinValue;
}

qreal ChartAxisIntervalStreak::getAxisMinValueInternal() const 
{
    return m_axisMinValue;
}

void ChartAxisIntervalStreak::setAxisMinValue(const QString& value) 
{
    m_strAxisMinValue = value;
    chartUpdate();
}

void ChartAxisIntervalStreak::setAxisMinValue(qreal value) 
{
    m_axisMinValue = value;
    m_strAxisMinValue = QString();
    chartUpdate();
}

QString ChartAxisIntervalStreak::getAxisMaxValue() const 
{
    return m_strAxisMaxValue;
}

qreal ChartAxisIntervalStreak::getAxisMaxValueInternal() const 
{
    return m_axisMaxValue;
}

void ChartAxisIntervalStreak::setAxisMaxValue(const QString& value) 
{
    m_strAxisMaxValue = value;
    chartUpdate();
}

void ChartAxisIntervalStreak::setAxisMaxValue(qreal value) 
{
    m_axisMaxValue = value;
    m_strAxisMaxValue = QString();
    chartUpdate();
}

bool ChartAxisIntervalStreak::isLegendVisible() const 
{
    return m_legendVisible;
}

void ChartAxisIntervalStreak::setLegendVisible(bool legendVisible) 
{
    m_legendVisible = legendVisible;
    chartUpdate();
}

QColor ChartAxisIntervalStreak::color() const 
{
    return m_color;
}

QColor ChartAxisIntervalStreak::getColor2() const 
{
    return m_clrColor2;
}

void ChartAxisIntervalStreak::setLegendName(const QString& name) 
{
    m_strLegendName = name;
    chartUpdate();
}

void ChartAxisIntervalStreak::setName(const QString& title)
{
    m_strName = title;
    chartUpdate();
}

void ChartAxisIntervalStreak::setColor(const QColor& clrColor)
{
    m_color = clrColor;
    chartUpdate();
}

void ChartAxisIntervalStreak::setColor2(const QColor& clrColor)
{
    m_clrColor2 = clrColor;
    chartUpdate();
}

QString ChartAxisIntervalStreak::getLegendName() const
{
    return m_strLegendName.isEmpty() ? m_strName : m_strLegendName;
}

QColor ChartAxisIntervalStreak::getActualColor() const
{
    return m_color.isValid() ? m_color : 
        ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_stripColor;
}

QColor ChartAxisIntervalStreak::getActualColor2() const
{
    return m_clrColor2.isValid() ? m_clrColor2 : 
        ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_stripColor2;
}

void ChartAxisIntervalStreak::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    m_fillHelper->drawGradientRect(p, boundingRect.adjusted(1, 1, -1, -1), getActualColor(), getActualColor2());
}

/* ChartAxisCustomLabel */
ChartAxisCustomLabel::ChartAxisCustomLabel()
{
    m_axisValue = 0;
}

ChartAxisCustomLabel::~ChartAxisCustomLabel()
{
}

void ChartAxisCustomLabel::setText(const QString& customLabel)
{
    m_strText = customLabel;
    chartUpdate();
}

QString ChartAxisCustomLabel::axisValue() const 
{
    return m_strAxisValue;
}

qreal ChartAxisCustomLabel::axisValueInternal() const 
{
    return m_axisValue;
}

void ChartAxisCustomLabel::setAxisValue(const QString& value) 
{
    m_strAxisValue = value;
    chartUpdate();
}

void ChartAxisCustomLabel::setAxisValue(qreal value) 
{
    m_axisValue = value;
    m_strAxisValue = QString();
    chartUpdate();
}

QString ChartAxisCustomLabel::text() const 
{
    return m_strText;
}

/* ChartAxisConstantLine */
ChartAxisConstantLine::ChartAxisConstantLine()
{
    m_legendVisible = true;
    m_visible = true;

    m_alignment = Qt::AlignLeft;
    m_antialiasing = false;
    //  m_font = QFont::getTahoma8();

    m_showTextBelow = false;
    m_showBehind = false;

    m_line = new ChartLineDrawHelper(this);

    m_axisValue = 0;
}

ChartAxisConstantLine::~ChartAxisConstantLine()
{
    delete m_line;
}

const QString& ChartAxisConstantLine::text() const 
{
    return m_strText;
}

const QFont& ChartAxisConstantLine::font() const 
{
    return m_font;
}

Qt::Alignment ChartAxisConstantLine::alignment() const 
{
    return m_alignment;
}

void ChartAxisConstantLine::setAntialiasing(bool antialiasing) 
{
    m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartAxisConstantLine::antialiasing() const 
{
    return m_antialiasing;
}

void ChartAxisConstantLine::setVisible(bool visible) 
{
    m_visible = visible;
    chartUpdate();
}

bool ChartAxisConstantLine::isVisible() const 
{
    return m_visible;
}

ChartLineDrawHelper* ChartAxisConstantLine::lineHelper() const 
{
    return m_line;
}

QString ChartAxisConstantLine::axisValue() const 
{
    return m_strAxisValue;
}

qreal ChartAxisConstantLine::axisValueInternal() const 
{
    return m_axisValue;
}

void ChartAxisConstantLine::setAxisValue(const QString& value) 
{
    m_strAxisValue = value;
    chartUpdate();
}

void ChartAxisConstantLine::setAxisValue(qreal value) 
{
    m_axisValue = value;
    m_strAxisValue = QString();
    chartUpdate();
}

bool ChartAxisConstantLine::isLegendVisible() const 
{
    return m_legendVisible;
}

void ChartAxisConstantLine::setLegendVisible(bool legendVisible) 
{
    m_legendVisible = legendVisible;
    chartUpdate();
}

void ChartAxisConstantLine::showTextBelow(bool showTextBelow) 
{
    m_showTextBelow = showTextBelow;
    chartUpdate();
}

bool ChartAxisConstantLine::isTextBelow() const 
{
    return m_showTextBelow;
}

const QColor& ChartAxisConstantLine::color() const 
{
    return m_color;
}

void ChartAxisConstantLine::setColor(const QColor& color) 
{
    m_color = color;
    chartUpdate();
}

void ChartAxisConstantLine::setLegendName(const QString& strName) 
{
    m_strLegendName = strName;
    chartUpdate();
}
bool ChartAxisConstantLine::isShowBehind() const 
{
    return m_showBehind;
}

void ChartAxisConstantLine::setShowBehind(bool showBehind) 
{
    m_showBehind = showBehind;
    chartUpdate();
}

void ChartAxisConstantLine::setText(const QString& constantLine)
{
    m_strText = constantLine;
    chartUpdate();
}

void ChartAxisConstantLine::setFont(const QFont& font)
{
    m_font = font;
    chartUpdate();
}

void ChartAxisConstantLine::setTextColor(const QColor& txtColor)
{
    m_textColor = txtColor;
    chartUpdate();
}

void ChartAxisConstantLine::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
    chartUpdate();
}

QString ChartAxisConstantLine::legendName() const
{
    if (!m_strLegendName.isEmpty())
        return m_strLegendName;

    return m_strText;
}

QColor ChartAxisConstantLine::textColor() const
{
    return m_textColor;
}

QColor ChartAxisConstantLine::getActualTextColor() const
{
    if (m_textColor.isValid())
        return m_textColor;

    return ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_constantLineTextColor;
}

QColor ChartAxisConstantLine::getActualColor() const
{
    if (m_color.isValid())
        return m_color;
    return ChartTheme::themeTopParent(this)->area2DTheme()->axisTheme()->m_constantLineColor;
}

void ChartAxisConstantLine::drawLegendComponent(QPainter* p, QRect boundingRect)
{
    boundingRect.adjust(1, 1, -1, -1);
    int nCenter = boundingRect.center().y();
    m_line->draw(p, QPointF((qreal)boundingRect.left(), (qreal)nCenter), QPointF((qreal)boundingRect.right(), (qreal)nCenter), getActualColor());
}
