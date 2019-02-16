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
#include <QStyleOption>
#include <qevent.h>

#include "QtnChart.h"
#include "QtnChartPrivate.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartArea2D.h"
#include "QtnChartArea.h"
#include "QtnChartAxis.h"
#include "QtnChartTitle.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartLegend.h"
#include "QtnGraphicsItem.h"
#include "QtnChartComponent.h"
#include "QtnChartDataPoint.h"


using namespace Qtitan;

/*!
    \class Qtitan::ChartPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPrivate::ChartPrivate()
{
    m_border = Q_NULL;
    m_theme = Q_NULL;
    m_legend = Q_NULL;
    m_areasLayoutSpacing = 10;
    m_hitInfoFlags = ChartHitInfo::HitNone;
    m_areasOrientation = Qt::Horizontal;
    m_background = Q_NULL;

    m_chartItem = Q_NULL;
    m_updateOptions = false;
}

ChartPrivate::~ChartPrivate()
{
    delete m_background;
}

void ChartPrivate::init()
{
    QTN_P(Chart);
    p.setMouseTracking(true);

    m_border = new ChartBorder(&p);
    m_theme = new ChartTheme(&p);
    m_legend = new ChartLegend(&p);

    m_fillBackground = new ChartFillDrawHelper(m_legend);
    m_fillBackground->setFillStyle(Qt::NoBrush);
}

Chart2DItem* ChartPrivate::createItem(const QRect& boundingRect)
{
    QTN_P(Chart);
    Chart2DItem* contentItem = new Chart2DItem(&p);
    contentItem->createItem();
    contentItem->calculateItem(boundingRect);
    return contentItem;
}

ChartFillDrawHelper* ChartPrivate::fillHelper() const 
{
    QTN_P(const Chart);
    if (m_fillBackground->fillStyle() != Qt::NoBrush)
        return m_fillBackground;
    return ChartTheme::themeTopParent(&p)->frameTheme()->m_fillHelper;
}

void ChartPrivate::updateChart()
{
    QTN_P(Chart);
    int index = 0;
    for (int i = 0, count = p.series().count(); i < count; i++)
    {
        AbstractSeries* series = p.series().at(i);
        if (!series)
            continue;

        if (!series->isVisible())
            continue;

        if (ChartDataTablePrivate* data = series->data())
        {
            if (series->isAlternateColor())
            {
                for (int j = 0; j < data->count(); j++)
                    data->setPointPaletteIndex(j, index++);
            }
            else
                data->setPaletteIndex(index++);
        }
    }
}

void ChartPrivate::mouseReleaseEvent(Chart2DItem* item, const QPoint& pt, Qt::MouseButton button)
{
    QTN_P(Chart);
    if (m_hitInfoFlags != ChartHitInfo::HitNone)
    {
        hitTestInternal(item, pt, &m_lastHit);

        m_lastHit.m_mouseButton = button;
        m_lastHit.m_ptHit = pt;
        if ((m_lastHit.m_hitInfo & m_hitInfoFlags) != 0)
        {
            emit p.chartMouseUp(&m_lastHit);
            return;
        }
        else
            emit p.chartMouseUp(&m_lastHit);
    }
}

void ChartPrivate::mouseMoveEvent(Chart2DItem* item, const QPoint& pt, Qt::MouseButton button)
{
    QTN_P(Chart);
    if (m_hitInfoFlags != ChartHitInfo::HitNone)
    {
        hitTestInternal(item, pt, &m_lastHit);

        m_lastHit.m_mouseButton = button;
        m_lastHit.m_ptHit = pt;
        if ((m_lastHit.m_hitInfo & m_hitInfoFlags) != 0)
        {
            emit p.chartMouseTrack(&m_lastHit);
            return;
        }
        else
            emit p.chartMouseTrack(&m_lastHit);
    }
}

bool ChartPrivate::hitTestInternal(Chart2DItem* item, const QPoint& pt, ChartHitInfo* hitInfo)
{
    hitInfo->m_index1 = -1;
    hitInfo->m_index2 = -1;

    if (!item)
    {
        hitInfo->m_hitInfo = ChartHitInfo::HitNone;
        return true;
    }
    else if (item->hitTest(pt, hitInfo, m_hitInfoFlags))
        return true;
    return false;
}


/* Chart */
Chart::Chart(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(Chart);
    QTN_D(Chart);
    d.init();
}

Chart::~Chart()
{
    QTN_FINI_PRIVATE();
}

const SeriesList& Chart::series() const
{
    QTN_D(const Chart);
    return d.m_listSeries;
}

void Chart::appendSeries(AbstractSeries* series)
{
    QTN_D(Chart);
    if (!series)
    {
        Q_ASSERT(false);
        return;
    }
    series->setParent(this);
    series->qtn_d().linkData();
    d.m_listSeries.append(series);
}

bool Chart::removeSeries(AbstractSeries* series)
{
    QTN_D(Chart);
    if (!series)
    {
        Q_ASSERT(false);
        return false;
    }
    series->qtn_d().unlinkData();
    return d.m_listSeries.removeOne(series);
}

void Chart::clearSeries()
{
    QTN_D(Chart);
    delete d.m_chartItem;
    d.m_chartItem = Q_NULL;

    for (int j = 0; j < d.m_listSeries.size(); ++j) 
    {
        AbstractSeries* series = d.m_listSeries.at(j);
        d.m_listSeries.removeAt(j--);
        series->qtn_d().unlinkData();
        delete series;
    }

/*
    for (int i = 0; i < d.m_listSeries.count(); ++i) 
    {
        AbstractSeries* series = d.m_listSeries.at(i);
        d.m_listSeries[i] = Q_NULL;
        series->qtn_d().unlinkData();
        delete series;
    }
    d.m_listSeries.clear();
*/
    for (int i = 0; i < d.m_listArea.count(); ++i) 
    {
        ChartArea* area = d.m_listArea.at(i);
        d.m_listArea[i] = Q_NULL;
        delete area;
    }
    d.m_listArea.clear();
    updateView();
    update();
}

const TitleList& Chart::titles() const 
{
    QTN_D(const Chart);
    return d.m_listTitle;
}

void Chart::appendTitle(ChartTitle* title)
{
    QTN_D(Chart);
    if (title)
    {
        title->setParent(this);
        d.m_listTitle.append(title);
    }
}

void Chart::clearTitles()
{
    QTN_D(Chart);
    for (int i = 0, count = d.m_listTitle.count(); count > i; i++)
    {
        ChartTitle* chartTitle = d.m_listTitle.at(i);
        delete chartTitle;
    }
    d.m_listTitle.clear();
    updateView();
}

const AreaList& Chart::areas() const
{
    QTN_D(const Chart);
    return d.m_listArea;
}

void Chart::appendArea(ChartArea* area)
{
    QTN_D(Chart);
    if (area)
    {
        area->setParent(this);
        d.m_listArea.append(area);
    }
}

void Chart::clearAreas()
{
    //TODO
}

ChartAxis* Chart::axis(int axisID) const
{
    QTN_D(const Chart);
    ChartAxis* axis = Q_NULL;
    for (int i = 0, count = d.m_listArea.count(); count > i && axis == Q_NULL; i++)
    {
        ChartArea* panel = d.m_listArea.at(i);
        if (ChartArea2D* area2D = qobject_cast<ChartArea2D*>(panel))
            axis = area2D->axisByCustomId(axisID);
    }
    return axis;
}

ChartLegend* Chart::legend() const
{
    QTN_D(const Chart);
    return d.m_legend;
}

void Chart::setTheme(Chart::Theme theme)
{
    QTN_D(Chart);
    d.m_theme->setTheme(theme);
}

Chart::Theme Chart::theme() const
{
    QTN_D(const Chart);
    return d.m_theme->theme();
}

bool Chart::isBorderVisible() const
{
    QTN_D(const Chart);
    return d.m_border->isVisible();
}

void Chart::setBorderVisible(bool visible)
{
    QTN_D(Chart);
    if (d.m_border->isVisible() != visible)
    {
        d.m_border->setVisible(visible);
        chartUpdate();
    }
}

const QColor& Chart::borderColor() const
{
    QTN_D(const Chart);
    if (!d.m_border->color().isValid())
        return d.m_theme->frameTheme()->m_borderColor;

    return d.m_border->color();
}

void Chart::setBorderColor(const QColor& clr)
{
    QTN_D(Chart);
    if (!clr.isValid())
        return;

    if (d.m_border->color() != clr)
    {
        d.m_border->setColor(clr);
        chartUpdate();
    }
}

QBrush Chart::backgroundBrush() const
{
    QTN_D(const Chart);
    if (d.m_background)
        return *d.m_background;
    return QBrush();
}

void Chart::setBackgroundBrush(const QBrush& brush)
{
    QTN_D(Chart);

    if (d.m_background)
    {
        delete d.m_background;
        d.m_background = Q_NULL;
    }

    d.m_background = new QBrush(brush);

    chartUpdate();
}

int Chart::borderThickness() const
{
    QTN_D(const Chart);
    return d.m_border->thickness();
}

void Chart::setBorderThickness(int thickness)
{
    QTN_D(Chart);
    if (d.m_border->thickness() != thickness)
    {
        d.m_border->setThickness(thickness);
        chartUpdate();
    }
}

int Chart::borderRounded() const
{
    QTN_D(const Chart);

    int roundLoc = d.m_border->borderRounded();
    if (roundLoc != -1)
        return roundLoc;

    return d.m_theme->frameTheme()->m_round;
}

void Chart::setBorderRounded(int round)
{
    QTN_D(Chart);
    if (d.m_border->borderRounded() != round)
    {
        d.m_border->setBorderRounded(round);
        chartUpdate();
    }
}

void Chart::setAreasOrientation(Qt::Orientation orientation)
{
    QTN_D(Chart);
    d.m_areasOrientation = orientation;
    chartUpdate();
}

Qt::Orientation Chart::areasOrientation() const
{
    QTN_D(const Chart);
    return d.m_areasOrientation;
}

ChartAxis* Chart::axis(int axisID, ChartComponent* owner) const
{
    ChartAxis* axis = Q_NULL;
    if (ChartArea2D* area2D = qobject_cast<ChartArea2D*>(owner))
        axis = area2D->axis(axisID);
    return axis;
}

void Chart::setAreasSpacing(int spacing)
{
    QTN_D(Chart);
    d.m_areasLayoutSpacing = spacing;
    chartUpdate();
}

int Chart::areasSpacing() const
{
    QTN_D(const Chart);
    return d.m_areasLayoutSpacing;
}

QRect Chart::plotAreas() const
{
    QTN_D(const Chart);

    QRect rect;
    for (int i = 0, count = d.m_listArea.count(); count > i; i++)
    {
        ChartArea* area = d.m_listArea.at(i);
        if (ChartArea2D* area2D = qobject_cast<ChartArea2D*>(area))
        {
            rect |= area2D->boundingRect();

            if (area2D->axisX()->isVisible())
            {
                QRect rc = area2D->axisX()->boundingRect();
                rect.setHeight(rect.height() - rc.height());
            }
        }
    }
    return rect;
}

void Chart::chartUpdate(ChartComponent::UpdateOptions opt)
{
    switch(opt)
    {
        case ChartComponent::UpdateView:
                updateView();
            break;
        case ChartComponent::UpdateLayout:
                updateLayout();
            break;
        case ChartComponent::UpdateRedraw:
                updateRedraw();
            break;
        default:
            break;
    }
}

void Chart::seriesImplChanged(ChartDataTablePrivate* data)
{
    QTN_D(Chart);

    AbstractSeries* series = data->series();
    Q_ASSERT(series != Q_NULL);

    ChartArea* area = data->area();
    if (area)
    {
        if (series->qtn_d().isSeriesArea(area))
            return;
        data->setArea(Q_NULL);
    }

    for (int i = 0, count = d.m_listArea.count(); i < count; i++)
    {
        ChartArea* area = dynamic_cast<ChartArea*>(d.m_listArea.at(i));
        if (!area)
            continue;

        if (data->setArea(area))
            return;
    }

    area = series->qtn_d().createArea(this);

    data->setArea(area);
    d.m_listArea.append(area);
}

void Chart::exportToFile(const QString& fileName)
{
    QStyleOption opt;
    opt.init(this);

    QPixmap px = QPixmap::grabWidget(this, opt.rect);
    if (!px.isNull())
        px.save(fileName, "PNG");
}

void Chart::enableMouseTrackingMode(unsigned long hitInfoFlags)
{
    QTN_D(Chart);
    d.m_hitInfoFlags = (ChartHitInfo::HitInfoTest)hitInfoFlags;
}

void Chart::updateView()
{
    QTN_D(Chart);
    d.m_updateOptions = true;
    if (d.m_chartItem)
        update();
}

void Chart::updateLayout()
{
    QTN_D(Chart);
    if (d.m_chartItem)
    {
        QStyleOption opt;
        opt.init(this);
        d.m_chartItem->calculateItem(opt.rect);
    }
}

void Chart::updateRedraw()
{
    repaint();
}

void Chart::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(Chart);

    QPainter p(this);
    QStyleOption opt;
    opt.init(this);

    if (d.m_chartItem == Q_NULL || d.m_updateOptions)
    {
        d.updateChart();
        if (d.m_chartItem)
        {
            delete d.m_chartItem;
            d.m_chartItem = Q_NULL;
        }
        d.m_chartItem = d.createItem(opt.rect);
    }

    d.m_chartItem->paint(&p);

    d.m_updateOptions = false;
}

void Chart::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
}

void Chart::mouseReleaseEvent(QMouseEvent* event)
{
    QTN_D(Chart);
    d.mouseReleaseEvent(d.m_chartItem, event->pos(), event->button());
}

void Chart::mouseMoveEvent(QMouseEvent* event)
{
    QTN_D(Chart);
    d.mouseMoveEvent(d.m_chartItem, event->pos(), event->button());
}

void Chart::leaveEvent(QEvent* event)
{
    Q_UNUSED(event);
    QTN_D(Chart);
    d.mouseMoveEvent(d.m_chartItem, QPoint(-1, -1), Qt::NoButton);
}

void Chart::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QSize delta = event->size() - event->oldSize();
    if (delta.isNull())
        return;

    updateLayout();
}
