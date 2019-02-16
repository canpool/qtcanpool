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

#include "QtnChartArea2D.h"
#include "QtnChartAxisPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartAxisItemPrivate.h"
#include "Qtn2DSeriesPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChart2DSeries.h"
#include "QtnChartThemePrivate.h"
#include "QtnChart.h"
#include "QtnChartAxis.h"
#include "QtnChartPainter.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartArea2DPrivate
    \inmodule QtitanChart
    \internal
*/
ChartArea2DPrivate::ChartArea2DPrivate()
{
    m_axisX = Q_NULL;
    m_axisY = Q_NULL;
    m_secondaryAxisX = Q_NULL;
    m_secondaryAxisY = Q_NULL;
    m_rotated = false;
    m_allowZoom = false;
    m_allowScroll = true;
}

ChartArea2DPrivate::~ChartArea2DPrivate()
{
}

void ChartArea2DPrivate::init()
{
    QTN_P(ChartArea2D)

    m_axisX = new ChartAxisX(&p, QTN_CHART_X_PRIMARY_AXIS);
    m_axisY = new ChartAxisY(&p, QTN_CHART_Y_PRIMARY_AXIS);

    m_secondaryAxisX = new ChartAxisX(&p, QTN_CHART_X_SECONDARY_AXIS);
    m_secondaryAxisX->qtn_d().setSecondary(true);
    m_secondaryAxisX->setVisible(false);
    m_secondaryAxisX->setAlignment(ChartAxis::AxisFar);
    m_secondaryAxisX->setGridLinesVisible(false);
    m_secondaryAxisX->setInterlaced(false);

    m_secondaryAxisY = new ChartAxisY(&p, QTN_CHART_Y_SECONDARY_AXIS);
    m_secondaryAxisY->qtn_d().setSecondary(true);
    m_secondaryAxisY->setVisible(false);
    m_secondaryAxisY->setAlignment(ChartAxis::AxisFar);
    m_secondaryAxisY->setGridLinesVisible(false);

    m_rotated = false;

    m_allowZoom = false;
    m_allowScroll = true;
}

ChartAreaItem* ChartArea2DPrivate::createItem(GraphicsItem* parent)
{
    QTN_P(ChartArea2D);
    return new ChartArea2DItem(&p, parent);
}

/*!
    \class Qtitan::ChartArea2DItem
    \inmodule QtitanChart
    \internal
*/
ChartArea2DItem::ChartArea2DItem(ChartArea* area, GraphicsItem* p)
    : ChartAreaItem(area, p)
{
    m_axisItem = Q_NULL;
    m_areaForm = Q_NULL;
}

ChartArea2DItem::~ChartArea2DItem()
{
}

ChartAreaFormItem* ChartArea2DItem::areaFormItem() const 
{
    return m_areaForm;
}

ChartAxisItem* ChartArea2DSeriesItem::axisItemX() const 
{
    return m_axisItemX;
}

ChartAxisItem* ChartArea2DSeriesItem::axisItemY() const 
{
    return m_axisItemY;
}

void ChartArea2DItem::addAxisItem(GraphicsItem* parentItem, ChartAxis* axis)
{
    if (ChartAxisItem* axisItem = (ChartAxisItem*)axis->qtn_d().createItem(parentItem))
        axisItem->createItem();
}

GraphicsItem* ChartArea2DItem::axisItem() const
{
    return m_axisItem;
}

void ChartArea2DItem::updateRange()
{
    for (int i = 0, count = seriesItem()->count(); i < count; i++)
    {
        ChartArea2DSeriesItem* item = (ChartArea2DSeriesItem*)seriesItem()->at(i);
        item->beforeUpdateRange();
    }

    for (int i = 0, count = m_axisItem->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)m_axisItem->at(i);

        for (int j = 0, countJ = seriesItem()->count(); j < countJ; j++)
        {
            ChartSeriesItem* item = (ChartSeriesItem*)seriesItem()->at(j);

            if (!item->series()->isVisible())
                continue;

            Chart2DSeries* series = dynamic_cast<Chart2DSeries*>(item->data()->series());

            Q_ASSERT(series);
            if (!series)
                continue;


            bool valAxisX = !axisItem->axis()->isValuesAxis();

            if (axisItem->axis()->isSecondary() != (valAxisX ? series->isSecondaryAxisX() : series->isSecondaryAxisY()))
                continue;

            axisItem->m_listDataTables.append(item);
                
            if (valAxisX)
                ((ChartArea2DSeriesItem*)item)->m_axisItemX = axisItem;
            else
                ((ChartArea2DSeriesItem*)item)->m_axisItemY = axisItem;
        }
    }

    for (int i = 0, count = m_axisItem->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)m_axisItem->at(i);
        axisItem->axis()->updateRange(axisItem);
        axisItem->addLegendItems();
    }

    for (int i = 0, count = seriesItem()->count(); i < count; i++)
    {
        ChartArea2DSeriesItem* item = (ChartArea2DSeriesItem*)seriesItem()->at(i);
        item->afterUpdateRange();
    }
}

void ChartArea2DItem::checkLabelBounds(const QRectF& boundingRect)
{
    if (boundingRect.top() < m_boundingRect.top())
        m_rcLabelPadding.setTop(qMax(m_rcLabelPadding.top(), m_boundingRect.top() - (int)boundingRect.top()));

    if (boundingRect.right() > m_boundingRect.right())
        m_rcLabelPadding.setRight(qMax(m_rcLabelPadding.right(), (int)boundingRect.right() - m_boundingRect.right()));

    if (boundingRect.left() < m_boundingRect.left())
        m_rcLabelPadding.setLeft(qMax(m_rcLabelPadding.left(), m_boundingRect.left() - (int)boundingRect.left()));

    if (boundingRect.bottom() > m_boundingRect.bottom())
        m_rcLabelPadding.setBottom(qMax(m_rcLabelPadding.bottom(), (int)boundingRect.bottom() - m_boundingRect.bottom()));
}

int ChartArea2DItem::calculateAxisLabelSize()
{
    int labelSize = -1;
    for (int i = 0, count = m_axisItem->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)m_axisItem->at(i);
        labelSize = qMax(labelSize,  axisItem->calcLabelSize());
    }
    return labelSize;
}

void ChartArea2DItem::calculateItem(QRect& boundingRect, int labelSize)
{
    m_area->qtn_d().calculateTitleItem(boundingRect, m_titlesItem);
    m_rcLabelPadding = QRect();

    QRect rcAreaBounds = boundingRect;
    for (int index = 0; index < 2; index++)
    {
        const int AXIS_GAP = 10;

        boundingRect = rcAreaBounds;
        boundingRect.adjust(m_rcLabelPadding.left(), m_rcLabelPadding.top(), -m_rcLabelPadding.right(), -m_rcLabelPadding.bottom());

        m_boundingRect = boundingRect;

        QRect rcInnerBounds(boundingRect);

        m_rcLabelPadding = QRect();

        QRect rcPane(boundingRect);

        ((ChartArea2D*)m_area)->updateLayout(this, boundingRect);

        ChartArea* chartArea = area();

        for (int i = 0, count = m_axisItem->count(); i < count; i++)
        {
            ChartAxisItem* axisItem = (ChartAxisItem*)m_axisItem->at(i);
            ChartAxis* axis = axisItem->axis();

            axisItem->calcSize(rcPane, labelSize);
            int size = axisItem->size();

            if (!axis->isVisible())
                continue;

            if (axis->isVertical())
            {
                if (axis->alignment() == ChartAxis::AxisNear)
                {
                    if (rcPane.left() != boundingRect.left()) 
                        rcPane.setLeft(rcPane.left() + AXIS_GAP);
                    rcPane.setLeft(rcPane.left() + size);
                }
                else
                {
                    if (rcPane.right() != boundingRect.right()) 
                        rcPane.setRight(rcPane.right() - AXIS_GAP);
                    rcPane.setRight(rcPane.right() - size);
                }
            }
            else
            {
                if (axis->alignment() == ChartAxis::AxisNear)
                {
                    if (rcPane.bottom() != boundingRect.bottom()) 
                        rcPane.setBottom(rcPane.bottom() - AXIS_GAP);
                    rcPane.setBottom(rcPane.bottom() - size);
                }
                else
                {
                    if (rcPane.top() != boundingRect.top()) 
                        rcPane.setTop(rcPane.top() + AXIS_GAP);
                    rcPane.setTop(rcPane.top() + size);
                }
            }
        }

        for (int i = m_axisItem->count() - 1; i >= 0; i--)
        {
            ChartAxisItem* axisItem = (ChartAxisItem*)m_axisItem->at(i);
            ChartAxis* axis = axisItem->axis();
            int size = axisItem->size();

            if (axis->isVertical())
            {
                if (axis->alignment() == ChartAxis::AxisNear)
                {
                    if (boundingRect.left() != m_boundingRect.left()) 
                        boundingRect.setLeft(boundingRect.left() + AXIS_GAP);
                    axisItem->setBoundingRect(QRect(QPoint(boundingRect.left(), rcPane.top()), QPoint(boundingRect.left() + size, rcPane.bottom())));
                    boundingRect.setLeft(boundingRect.left() + size);
                }
                else
                {
                    if (boundingRect.right() != m_boundingRect.right()) 
                        boundingRect.setRight(boundingRect.right()- AXIS_GAP);
                    axisItem->setBoundingRect(QRect(QPoint(boundingRect.right() - size + 1, rcPane.top()), QPoint(boundingRect.right(), rcPane.bottom())));
                    boundingRect.setRight(boundingRect.right() - size);
                }
            }
            else
            {
                if (axis->alignment() == ChartAxis::AxisNear)
                {
                    if (boundingRect.bottom() != m_boundingRect.bottom()) 
                        boundingRect.setBottom(boundingRect.bottom() - AXIS_GAP);
                    axisItem->setBoundingRect(QRect(QPoint(rcPane.left(), boundingRect.bottom() - size), QPoint(rcPane.right(), boundingRect.bottom())));
                    boundingRect.setBottom(boundingRect.bottom() - size);
                }
                else
                {
                    if (boundingRect.top() != m_boundingRect.top()) 
                        boundingRect.setTop(boundingRect.top() + AXIS_GAP);
                    axisItem->setBoundingRect(QRect(QPoint(rcPane.left(), boundingRect.top()), QPoint(rcPane.right(), boundingRect.top() + size - 1)));
                    boundingRect.setTop(boundingRect.top() + size);
                }
            }
        }

        // ??? adjusted(0,-1,0,-1)
        m_areaForm->calculateItem(boundingRect.adjusted(0, -1, 0, -1));

        if (m_areaForm->m_boundingRect.width() < 1 || m_areaForm->m_boundingRect.height() < 1)
            break;

        chartArea->qtn_d().calculateSeriesLayout(this);

        for (int i = 0, count = m_labelsItem->count(); i < count; i++)
            ((ChartArea2DSeriesLabelItem*)m_labelsItem->at(i))->calculateLayout();

        if( m_rcLabelPadding.isNull())
            break;
    }
}

void ChartArea2DItem::createItem()
{
    ChartAreaItem::createItem();

    ChartArea2D* chartArea = (ChartArea2D*)area();

    GraphicsItem* item = new GraphicsItem(this);
    m_axisItem = new GraphicsItem(this);

    addAxisItem(m_axisItem, chartArea->axisX());
    addAxisItem(m_axisItem, chartArea->secondaryAxisX());

    addAxisItem(m_axisItem, chartArea->axisY());
    addAxisItem(m_axisItem, chartArea->secondaryAxisY());

    m_areaForm = (ChartAreaFormItem*)chartArea->qtn_d().m_form->createItem(item);

    m_seriesItem = m_areaForm->createSeriesItem();

    m_labelsItem = new GraphicsItem(this);
}

void ChartArea2DItem::paint(QPainter* p)
{
    if (m_areaForm->m_boundingRect.width() < 1 || m_areaForm->m_boundingRect.height() < 1)
        return;
    ChartAreaItem::paint(p);
}

bool ChartArea2DItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if (ChartAreaItem::hitTest(pt, hitInfo, hitInfoFlags))
        return true;

    if (m_boundingRect.contains(pt) && (hitInfoFlags & ChartHitInfo::HitArea) != 0)
    {
        hitInfo->m_hitInfo = ChartHitInfo::HitArea;

        for (int i = 0, count = m_axisItem->count(); i < count; i++)
        {
            if (ChartAxisItem* axisItem =  dynamic_cast<ChartAxisItem*>(m_axisItem->at(i)))
            {
                ChartHitInfo hitTest;
                if (axisItem->hitTest(pt, &hitTest, hitInfoFlags))
                {
                    *hitInfo = hitTest;
                    return true;
                }
                else
                {
                    QRect rectAxis;
                    QRect rect = axisItem->bounds();
                    if (axisItem->isVertical())
                        rectAxis = QRect(QPoint(m_boundingRect.left(), rect.top()), QPoint(m_boundingRect.right(), rect.bottom()));
                    else
                        rectAxis = QRect(QPoint(rect.left(), m_boundingRect.top()), QPoint(rect.right(), m_boundingRect.bottom()));

                    if (rectAxis.contains(pt))
                    {
                        if (dynamic_cast<ChartAxisX*>(axisItem->axis()))
                        {
                            hitInfo->m_index1 = axisItem->axis()->axisID();
                            hitInfo->m_index2 = axisItem->axis()->perpendecularAxis()->axisID();
                        }
                        else
                        {
                            hitInfo->m_index2 = axisItem->axis()->axisID();
                            hitInfo->m_index1 = axisItem->axis()->perpendecularAxis()->axisID();
                        }
                        return true;
                    }
                }
            }
        }
        return true;
    }
    return false;
}



/* ChartArea2D */ 
ChartArea2D::ChartArea2D(QObject* parent)
    : ChartArea(*new ChartArea2DPrivate, parent)
{
    QTN_D(ChartArea2D)
    d.init();
}

ChartArea2D::~ChartArea2D()
{
    QTN_D(ChartArea2D)
    delete d.m_axisX;
    delete d.m_axisY;
    delete d.m_secondaryAxisX;
    delete d.m_secondaryAxisY;
    delete d.m_form;
}

ChartAxis* ChartArea2D::axisX() const 
{
    QTN_D(const ChartArea2D)
    return d.m_axisX;
}

ChartAxis* ChartArea2D::axisY() const 
{
    QTN_D(const ChartArea2D)
    return d.m_axisY;
}

ChartAxis* ChartArea2D::secondaryAxisX() const 
{
    QTN_D(const ChartArea2D)
    return d.m_secondaryAxisX;
}

ChartAxis* ChartArea2D::secondaryAxisY() const 
{
    QTN_D(const ChartArea2D)
    return d.m_secondaryAxisY;
}

ChartAxis* ChartArea2D::axisByCustomId(int axisID) const
{
    QTN_D(const ChartArea2D)
    if (d.m_axisX->axisID() == axisID)
        return d.m_axisX;
    if (d.m_axisY->axisID() == axisID)
        return d.m_axisY;
    if (d.m_secondaryAxisX->axisID() == axisID)
        return d.m_secondaryAxisX;
    if (d.m_secondaryAxisY->axisID() == axisID)
        return d.m_secondaryAxisY;
    return Q_NULL;
}

ChartAxis* ChartArea2D::axis(int axisID) const
{
    QTN_D(const ChartArea2D)
    if (QTN_CHART_X_PRIMARY_AXIS == axisID)
        return d.m_axisX;
    if (QTN_CHART_Y_PRIMARY_AXIS == axisID)
        return d.m_axisY;
    if (QTN_CHART_X_SECONDARY_AXIS == axisID)
        return d.m_secondaryAxisX;
    if (QTN_CHART_Y_SECONDARY_AXIS == axisID)
        return d.m_secondaryAxisY;
    return Q_NULL;
}


bool ChartArea2D::isRotated() const 
{
    QTN_D(const ChartArea2D)
    return d.m_rotated;
}

void ChartArea2D::setRotated(bool rotated) 
{
    QTN_D(ChartArea2D)
    d.m_rotated = rotated;
    chartUpdate();
}

bool ChartArea2D::isZoomAllowed() const 
{
    QTN_D(const ChartArea2D)
    return d.m_allowZoom;
}

void ChartArea2D::setZoomAllowed(bool allowed) 
{
    QTN_D(ChartArea2D)
    d.m_allowZoom = allowed;
}

bool ChartArea2D::isScrollAllowed() const 
{
    QTN_D(const ChartArea2D)
    return d.m_allowScroll;
}

void ChartArea2D::setScrollAllowed(bool allowed) 
{
    QTN_D(ChartArea2D)
    d.m_allowScroll = allowed;
    chartUpdate();
}

QRect ChartArea2D::boundingRect() const
{
    QTN_D(const ChartArea2D)
    return d.m_boundingRect;
}

void ChartArea2D::updateLayout(ChartAreaItem* item, QRect boundingRect)
{
    Q_UNUSED(boundingRect);
    QTN_D(ChartArea2D)
    d.m_boundingRect = item->boundingRect();
}

/*!
    \class Qtitan::ChartAreaForm
    \inmodule QtitanChart
    \internal
*/
ChartAreaForm::ChartAreaForm(ChartArea* area)
    : ChartComponent(area) 
{
    m_backgroundFillHelper = new ChartFillDrawHelper(this);
    m_backgroundFillHelper->setFillStyle(Qt::NoBrush);
    //m_backgroundBrush = QBrush(QColor());
}

ChartAreaForm::~ChartAreaForm()
{
}

GraphicsItem* ChartAreaForm::createItem(GraphicsItem* parent)
{
    return new ChartAreaFormItem(this, parent);
}

ChartAreas2DTheme* ChartAreaForm::theme() const
{
    return ChartTheme::themeTopParent(this)->area2DTheme();
}

const QBrush& ChartAreaForm::backgroundBrush() const
{
//    if (m_backgroundBrush.color().isValid() || m_backgroundBrush.gradient() || !m_backgroundBrush.texture().isNull())
//        return m_backgroundBrush;
    return m_backgroundBrush;
}

void ChartAreaForm::setBackgroundBrush(const QBrush& brush) 
{
    m_backgroundBrush = brush;
    chartUpdate();
}

ChartFillDrawHelper* ChartAreaForm::fillHelper() const 
{
    return m_backgroundFillHelper;
}

ChartFillDrawHelper* ChartAreaForm::currentFillHelper() const
{
    if (m_backgroundFillHelper->fillStyle() != Qt::NoBrush)
        return m_backgroundFillHelper;

    return theme()->m_backgroundFillHelper;
}

QRect ChartAreaFormItem::bounds() const 
{
    return m_boundingRect;
}

const QColor& ChartAreaForm::backgroundColor() const
{
    return m_backgroundColor;
//    return theme()->m_backgroundColor;
}

void ChartAreaForm::setBackgroundColor(const QColor& color)
{
    if (m_backgroundColor != color)
    {
        m_backgroundColor = color;
        chartUpdate();
    }
}

const QColor& ChartAreaForm::borderColor() const
{
    if (m_borderColor.isValid())
        return m_borderColor;

    return theme()->m_borderColor;
}

void ChartAreaForm::setBorderColor(const QColor& color) 
{
    if (m_borderColor != color)
    {
        m_borderColor = color;
        chartUpdate();
    }
}


/*!
    \class Qtitan::ChartAreaFormItem
    \inmodule QtitanChart
    \internal
*/
ChartAreaFormItem::ChartAreaFormItem(ChartAreaForm* pane, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_form = pane;
    m_ptOldPosition = QPoint(-1, -1);
}

ChartAreaFormItem::~ChartAreaFormItem()
{
}

void ChartAreaFormItem::calculateItem(const QRect& boundingRect)
{
    m_boundingRect = boundingRect;
}

ChartAxisItem* ChartAreaFormItem::axisItem(ChartAxis* axis) const
{
    ChartArea2DItem* areaItem = (ChartArea2DItem*)parentItem()->parentItem();

    GraphicsItem* axisItems =  areaItem->m_axisItem;

    for (int i = 0; i < axisItems->count(); i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        if (axisItem->axis() == axis)
            return axisItem;
    }
    return Q_NULL;
}

void ChartAreaFormItem::drawGridLines(QPainter* p, ChartAxisItem* axisItem)
{
    axisItem->drawGridLines(p, m_boundingRect);
}

void ChartAreaFormItem::drawConstantLines(QPainter* p, ChartAxisItem* axisItem, bool behind)
{
    axisItem->drawConstantLines(p, m_boundingRect, behind);
}

void ChartAreaFormItem::drawStrips(QPainter* p, ChartAxisItem* axisItem)
{
    return axisItem->drawStrips(p, m_boundingRect);
}


void ChartAreaFormItem::drawInterlaced(QPainter* p, ChartAxisItem* axisItem)
{
    axisItem->drawInterlaced(p, m_boundingRect);
}

void ChartAreaFormItem::paint(QPainter* p)
{
    QBrush br = m_form->backgroundBrush();
    if (br.style() != Qt::NoBrush)
    {
    }
    else if (m_form->backgroundColor().isValid())
    {
        QColor color = m_form->backgroundColor();
        m_form->currentFillHelper()->drawGradientRect(p, m_boundingRect, color, color);
    }
    else
        m_form->currentFillHelper()->drawGradientRect(p, m_boundingRect);

    ChartPainter::painter()->drawBoundedRect(p, m_boundingRect, m_form->borderColor(), 1);

    ChartArea2DItem* areaItem = (ChartArea2DItem*)parentItem()->parentItem();
    GraphicsItem* axisItems =  areaItem->m_axisItem;

//    p->save();
//    p->setClipRect(m_boundingRect);

    for (int i = 0, count = axisItems->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        drawInterlaced(p, axisItem);
    }

    for (int i = 0, count = axisItems->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        drawStrips(p, axisItem);
    }

    for (int i = 0, count = axisItems->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        drawGridLines(p, axisItem);
    }

    for (int i = 0, count = axisItems->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        drawConstantLines(p, axisItem, true);
    }

    m_seriesItem->paint(p);

    for (int i = 0, count = axisItems->count(); i < count; i++)
    {
        ChartAxisItem* axisItem = (ChartAxisItem*)axisItems->at(i);
        drawConstantLines(p, axisItem, false);
    }
//    p->restore();
}

QPointF ChartAreaFormItem::screenPoint(const ChartSeriesItem* item, qreal x, qreal y) const
{
//    QRect boundingRect = bounds();

    QPointF res;

    ChartAxisItem* axisItemX = ((ChartArea2DSeriesItem*)item)->m_axisItemX;
    ChartAxisItem* axisItemY = ((ChartArea2DSeriesItem*)item)->m_axisItemY;

    res.setX((float)axisItemX->valueToPoint(x));
    res.setY((float)axisItemY->valueToPoint(y));

    ChartArea2DItem* areaItem = (ChartArea2DItem*)parentItem()->parentItem();
    ChartArea2D* area = (ChartArea2D*)areaItem->area();

    if (area->isRotated())
        res = QPointF(res.y(), res.y());

    return res;
}

GraphicsItem* ChartAreaFormItem::createSeriesItem()
{
    m_seriesItem = new GraphicsItem(this);

    return m_seriesItem;
}




