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

#include "QtnChartPieSeries.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartPieSeriesPrivate.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChart.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartTextPainter.h"
#include "QtnChartPainter.h"
#include "QtnChartArea.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartPieDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartPieDrawHelper::ChartPieDrawHelper(qreal beginAngle, qreal endAngle, const ChartEllipse& ellipse, int depthPercent, int holePercent)
{
    m_ellipse = ellipse;
    m_beginAngle = beginAngle;
    m_sweepAngle = endAngle - m_beginAngle;
    m_depth = m_ellipse.m_majorSemiaxis * depthPercent / 50.0;
    m_holePercent = holePercent;
    updateBoundingRect();
}

qreal ChartPieDrawHelper::beginAngle() const 
{
    return m_beginAngle;
}

int ChartPieDrawHelper::holePercent() const 
{
    return m_holePercent;
}

void ChartPieDrawHelper::updateBoundingRect(ChartEllipse& ellipse, BoundingRect& boundingRect)
{
    boundingRect.update(ellipse.calcEllipsePoint(m_beginAngle));
    boundingRect.update(ellipse.calcEllipsePoint(m_beginAngle + m_sweepAngle));

    qreal normalizedStartAngle = ChartMath::normalizeRadian(m_beginAngle);
    qreal normalizedFinishAngle = normalizedStartAngle + m_sweepAngle;
    if (normalizedFinishAngle < normalizedStartAngle) 
    {
        qreal temp = normalizedStartAngle;
        normalizedStartAngle = normalizedFinishAngle;
        normalizedFinishAngle = temp;
    }
    qreal beginAngle = -4.0 * ChartMath::QTN_PI;
    qreal endAngle = 4.0 * ChartMath::QTN_PI;
    for (int multiplier = 0;; multiplier++) 
    {
        qreal angle = beginAngle + multiplier * ChartMath::QTN_PI_2;
        if (ChartMath::fuzzyCmp(angle, endAngle) > 0)
            break;
        if (ChartMath::fuzzyCmp(angle, normalizedStartAngle) > 0 && 
            ChartMath::fuzzyCmp(angle, normalizedFinishAngle) < 0)
            boundingRect.update(ellipse.calcEllipsePoint(angle));
    }
}

qreal ChartPieDrawHelper::calcAxisPercent(qreal factor)
{
    return factor * m_holePercent / 100.0;
}

void ChartPieDrawHelper::updateBoundingRect()
{
    BoundingRect boundingRect;
    updateBoundingRect(m_ellipse, boundingRect);

    if (m_holePercent > 0)
    {
        ChartEllipse innerEllipse(m_ellipse.center(), calcAxisPercent(m_ellipse.m_majorSemiaxis), 
            calcAxisPercent(m_ellipse.m_minorSemiaxis));
        updateBoundingRect(innerEllipse, boundingRect);
    }
    else
    {
        boundingRect.update(m_ellipse.center());
    }

    m_boundingRect = QRectF(boundingRect.xMin, boundingRect.yMin, 
        boundingRect.xMax - boundingRect.xMin, boundingRect.yMax - boundingRect.yMin);
}

QPointF ChartPieDrawHelper::calculateCenter(const QPointF& basePoint) const
{
    QPointF center = m_ellipse.center();
    center += basePoint;
    return center;
}

qreal ChartPieDrawHelper::halfAngle() const
{
    return m_ellipse.calcEllipseSectorFinishAngle(m_ellipse.calcEllipseSectorArea(m_beginAngle, m_sweepAngle + m_beginAngle) / 2, m_beginAngle);
}

QPointF ChartPieDrawHelper::endPoint() const
{
    return m_ellipse.calcEllipsePoint(m_sweepAngle + m_beginAngle);
}

QPointF ChartPieDrawHelper::startPoint() const
{
    return m_ellipse.calcEllipsePoint(m_beginAngle);
}

QPointF ChartPieDrawHelper::centerPoint() const
{
    return m_ellipse.center();
}

QRectF ChartPieDrawHelper::boundingRect() const
{
    return QRectF((float)(m_ellipse.m_ptCenter.x() - m_ellipse.m_majorSemiaxis), (float)(m_ellipse.m_ptCenter.y() - m_ellipse.m_minorSemiaxis),
        (float)(2 * m_ellipse.m_majorSemiaxis), (float)(2 * m_ellipse.m_minorSemiaxis));
}

void ChartPieDrawHelper::drawPie(QPainter* p, const QColor color, const QColor color2, const QPointF& basePoint)
{
    QPointF center = calculateCenter(basePoint);

    QRectF boundingRect(m_boundingRect);
    boundingRect.translate(basePoint);

    ChartPainter::painter()->drawGradientPie(p, center, m_ellipse.m_majorSemiaxis, m_ellipse.m_minorSemiaxis, -ChartMath::radian2Degree(m_beginAngle), 
        -ChartMath::radian2Degree(m_sweepAngle), m_depth, m_holePercent, boundingRect, color2, color);
}

void ChartPieDrawHelper::drawBoundedPie(QPainter* p, const QColor color, int thickness, const QPointF& basePoint)
{
    if (thickness == 0)
        return;

    QPointF center = calculateCenter(basePoint);
    ChartPainter::painter()->drawBoundedPie(p, center, m_ellipse.m_majorSemiaxis, m_ellipse.m_minorSemiaxis, -ChartMath::radian2Degree(m_beginAngle), 
        -ChartMath::radian2Degree(m_sweepAngle), m_depth, m_holePercent, color, thickness);
}

void ChartPieDrawHelper::drawTorus(QPainter* p, const QColor color, const QColor color2)
{
    QPointF center = calculateCenter(QPointF());
    ChartPainter::painter()->drawGradientTorus(p, center, m_ellipse.m_majorSemiaxis, m_ellipse.m_minorSemiaxis, 
        -ChartMath::radian2Degree(m_beginAngle), -ChartMath::radian2Degree(m_sweepAngle), m_depth, m_holePercent, m_boundingRect, color2, color);
}

/*!
    \class Qtitan::ChartPieAreaPrivate
    \inmodule QtitanChart
    \internal
*/
ChartAreaItem* ChartPieAreaPrivate::createItem(GraphicsItem* parent)
{
    QTN_P(ChartPieArea)
    return new ChartPieAreaItem(&p, parent);
}

void ChartPieAreaPrivate::calculateSeriesLayout(ChartAreaItem* item)
{
    if (item->count() > 0)
    {
        if (GraphicsItem* grItem = item->seriesItem())
        {
            for (int i = 0, count = grItem->count(); i < count; i++)
            {
                ChartPieSeriesItem* seriesItem = (ChartPieSeriesItem*)grItem->at(i);
                seriesItem->calculatePointLayout(item->boundingRect());
                seriesItem->calculateLabelLayout();
            }
        }
    }
}


/*!
    \class Qtitan::ChartPieArea
    \inmodule QtitanChart
    \internal
*/
ChartPieArea::ChartPieArea(QObject* parent)
    : ChartArea(*new ChartPieAreaPrivate, parent)
{
}

ChartPieArea::~ChartPieArea()
{
}

/*!
    \class Qtitan::ChartPieSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesItem::ChartPieSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
    : ChartSeriesItem(series, areaItem)
{
}

ChartPieSeriesItem::~ChartPieSeriesItem()
{
}

void ChartPieSeriesItem::calculatePointLayout(const QRect& boundingRect)
{
    calculateValues();
    calcInternalArea(boundingRect);

    qreal circleStartAngle = ChartMath::degree2Radian(((ChartPieSeriesBase*)series())->rotation());

    ChartEllipse ellipse(QPointF(), m_internalRect.width() * 0.5, m_internalRect.height() * 0.5);

    qreal angle = ellipse.calcEllipseAngleFromCircleAngle(circleStartAngle);

    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPieSeriesSliceItem* pointItem = (ChartPieSeriesSliceItem*)m_pointsItem->at(i);
        pointItem->calculateLayout(angle);
    }
}

void ChartPieSeriesItem::calcInternalArea(const QRectF& boundingRect)
{
    if (AbstractSeries* series = m_data->series())
    {
        if (ChartPieSeriesLabel* label = (ChartPieSeriesLabel*)series->label())
        {
            QRectF internalRect = boundingRect;
            if (label->isVisible() && !label->isInside())
            {
                QSizeF sz(0.0, 0.0);
                for (int i = 0, count = m_pointsItem->count(); i < count; i++)
                {
                    ChartPieSeriesSliceItem* pointItem = (ChartPieSeriesSliceItem*)m_pointsItem->at(i);

                    QString text(label->pointLabel(pointItem->point()));
                    ChartTextPainter painter(text, label);
                    sz.setWidth(::qMax(sz.width(),  painter.size().width()));
                    sz.setHeight(::qMax(sz.height(), painter.size().height()));
                }

                qreal lineLength = (qreal)series->label()->lineLength();
                qreal x = 6.0 + lineLength + sz.width();
                qreal y = 6.0 + lineLength + sz.height();
                internalRect.adjust(x, y, -x, -y);
            }
            else
                internalRect.adjust(10, 10, -10, -10);

            qreal width = qMin(internalRect.width(), internalRect.height());

            if (width < 0.0)
            {
                width = 1.0;
                internalRect = QRectF(boundingRect.center(), QSize(width, width));
            }
            m_internalRect = QRectF(QPointF((internalRect.left() + internalRect.right() - width) * 0.5,
                (internalRect.top() + internalRect.bottom() - width) * 0.5), QSizeF(width, width));
        }
    }
}

void ChartPieSeriesItem::calculateLabelLayout()
{
    for (int i = 0, count = m_labelsItem->count(); i < count; i++)
        ((ChartSeriesLabelItem*)m_labelsItem->at(i))->calculateLayout();
}

QRectF ChartPieSeriesItem::internalBoundingRect() const
{
    return m_internalRect;
}

ChartSeriesPointItem* ChartPieSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    return new ChartPieSeriesSliceItem(point, parentItem);
}

qreal ChartPieSeriesItem::calculateAmountValues() const
{
    qreal sum = 0;
    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPieSeriesSliceItem* pointItem = (ChartPieSeriesSliceItem*)m_pointsItem->at(i);
        ChartDataPoint* point = pointItem->point();
//        qreal val = point->value(0);
        qreal val = point->valueY();
        if (val >= 0)
            sum += val;
    }
    return sum;
}

void ChartPieSeriesItem::setupValuesSlices(qreal sum)
{
    for (int i = 0, count = m_pointsItem->count(); i < count; i++)
    {
        ChartPieSeriesSliceItem* pointItem = (ChartPieSeriesSliceItem*)m_pointsItem->at(i);
        ChartDataPoint* point = pointItem->point();
//        qreal val = point->value(0);
        qreal val = point->valueY();
        pointItem->m_value = val >= 0.0 ? val / sum : 0;
    }
}

void ChartPieSeriesItem::calculateValues()
{
    if (m_pointsItem->count() == 1)
    {
        if (ChartPieSeriesSliceItem* pointItem = (ChartPieSeriesSliceItem*)m_pointsItem->at(0))
            pointItem->m_value = 1.0;
        return;
    }

    qreal amount = calculateAmountValues();
    if (amount == 0.0)
        amount = 1.0;
    setupValuesSlices(amount);
}

void ChartPieSeriesItem::paint(QPainter* p)
{
    ChartPainter::painter()->setAntialiasing(p);
    ChartSeriesItem::paint(p);
    ChartPainter::painter()->unsetAntialiasing(p);
}

void ChartPieSeriesItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rc = boundingRect;
    rc.adjust(1, 1, -1, -1);

    ChartPainter::painter()->setAntialiasing(p);

    ChartEllipse explodeEllipse(QPointF(rc.left(), rc.bottom()), rc.width(), rc.height());

    ChartPieDrawHelper pie(0, ChartMath::QTN_PI_2, explodeEllipse, 0, 0);
    pie.drawPie(p, beginColor(), endColor(), QPointF(0.0, 0.0));

    if (ChartPieSeries* pieSeries = qobject_cast<ChartPieSeries*>(series()))
    {
        if (pieSeries->qtn_d().m_border->isVisible())
            pie.drawBoundedPie(p, pieSeries->borderColor(), 1, QPointF(0.0, 0.0));
    }
    else
    {
        QColor clrBorder = ChartTheme::themeTopParent(this)->pieTheme()->m_borderColor;
        pie.drawBoundedPie(p, clrBorder, 1, QPointF(0.0, 0.0));
    }
    ChartPainter::painter()->unsetAntialiasing(p);
}

/*!
    \class Qtitan::ChartPieSeriesSliceItem
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesSliceItem::ChartPieSeriesSliceItem(ChartDataPoint* point, GraphicsItem* parentItem)
    : ChartSeriesPointItem(point, parentItem)
{
    m_value = 0.0;
    m_pieDrawHelper = Q_NULL;
}

ChartPieSeriesSliceItem::~ChartPieSeriesSliceItem()
{
    delete m_pieDrawHelper;
}

ChartPieDrawHelper* ChartPieSeriesSliceItem::getPie() const 
{
    return m_pieDrawHelper;
}

QPointF ChartPieSeriesSliceItem::basePoint() const 
{
    return m_basePoint;
}

void ChartPieSeriesSliceItem::calculateLayout(qreal& beginAngle)
{
    delete m_pieDrawHelper;
    m_pieDrawHelper = Q_NULL;

    if (ChartPieSeriesItem* item = (ChartPieSeriesItem*)seriesItem())
    {
        QPointF basePoint = item->internalBoundingRect().center();
        QSizeF size = item->internalBoundingRect().size();

        ChartEllipse ellipse(QPointF(), size.width() * 0.5, size.height() * 0.5);
        qreal pieArea = ellipse.area() * m_value;
        qreal endAngle = ellipse.calcEllipseSectorFinishAngle(pieArea, beginAngle);

        int depth = ((ChartPieSeriesBase*)item->series())->depth();
        int holePercent = ((ChartPieSeriesBase*)item->series())->holePercent();

        if (m_point->qtn_d().m_special)
        {
            if (ChartPieSeriesBase* series = ((ChartPieSeriesBase*)item->series()))
            {
                ChartPieDrawHelper pie(beginAngle, endAngle, ellipse, depth, holePercent);

                int distance = series->explodeRelativeSizePercent();
                qreal angle = pie.halfAngle();
                qreal radius = ellipse.calcEllipseRadius(angle);
                QPointF centerPnt = pie.centerPoint();
                qreal explodeX = centerPnt.x() + distance / 100.0 * radius * qCos(angle);
                qreal explodeY = centerPnt.y() - distance / 100.0 * radius * qSin(angle);

                QPointF explodeLocation(explodeX, explodeY);
                ChartEllipse explodeEllipse(explodeLocation, ellipse.m_majorSemiaxis, ellipse.m_minorSemiaxis);
                m_pieDrawHelper = new ChartPieDrawHelper(beginAngle, endAngle, explodeEllipse, depth, holePercent);
            }
        }
        else
            m_pieDrawHelper = new ChartPieDrawHelper(beginAngle, endAngle, ellipse, depth, holePercent);

        m_basePoint = basePoint;
        beginAngle = endAngle;
    }
}

void ChartPieSeriesSliceItem::paint(QPainter* p)
{
    if (!m_pieDrawHelper)
        return;

    m_pieDrawHelper->drawPie(p, startColor(), finalColor(), m_basePoint);

    ChartPieSeries* series = qobject_cast<ChartPieSeries*>(seriesItem()->series());
    Q_ASSERT(series != Q_NULL);

    if (series->qtn_d().m_border->isVisible())
        m_pieDrawHelper->drawBoundedPie(p, series->borderColor(), series->qtn_d().m_border->thickness(), m_basePoint);
}

void ChartPieSeriesSliceItem::drawLegendComponent(QPainter* p, const QRect& boundingRect)
{
    QRect rc = boundingRect;
    rc.adjust(2, 2, -2, -2);

    ChartPainter::painter()->setAntialiasing(p);

    int size = qMax(rc.width()/2, rc.height()/2);
    ChartEllipse explodeEllipse(rc.center(), size, size);

    ChartPieSeries* series = qobject_cast<ChartPieSeries*>(seriesItem()->series());
    bool hole = series && series->holePercent() > 0;

    ChartPieDrawHelper pie(0, ChartMath::QTN_PI + ChartMath::QTN_PI_2 + 0.4, explodeEllipse, 0, hole ? 40 : 0);
    if (hole)
        pie.drawTorus(p, startColor(), finalColor());
    else
        pie.drawPie(p, startColor(), finalColor(), QPointF(0, 0));

    if (series)
    {
        if (series->qtn_d().m_border->isVisible())
            pie.drawBoundedPie(p, series->borderColor(), 1, QPointF(0.0, 0.0));
    }
    else
    {
        QColor clrBorder = ChartTheme::themeTopParent(this)->pieTheme()->m_borderColor;
        pie.drawBoundedPie(p, clrBorder, 1, QPointF(0.0, 0.0));
    }
    ChartPainter::painter()->unsetAntialiasing(p);
}



/*!
    \class Qtitan::ChartPieAreaItem
    \inmodule QtitanChart
    \internal
*/
ChartPieAreaItem::ChartPieAreaItem(ChartArea* area, GraphicsItem* parent)
    : ChartAreaItem(area, parent)
{
    m_seriesItem = new GraphicsItem(this);
    m_labelsItem = new ChartPieLabelsItem(this);
}

/*!
    \class Qtitan::ChartPieSeriesBasePrivate
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesBasePrivate::ChartPieSeriesBasePrivate()
{
    m_data = new ChartDataTablePrivate();
    m_holePercent = 0;
    m_explodedRelativeSize= 10;
    m_rotation = 0;
    m_colorAlternate = true;
}

ChartPieSeriesBasePrivate::~ChartPieSeriesBasePrivate()
{
}

void ChartPieSeriesBasePrivate::linkData()
{
    QTN_P(ChartPieSeriesBase);
    m_data->setParent(p.parent());
    m_data->setSeries(&p);
}

void ChartPieSeriesBasePrivate::unlinkData()
{
    if (ChartArea* area= m_data->area())
    {
        if (isSeriesArea(area))
            return;
        m_data->setArea(Q_NULL);
    }
}


/* ChartPieSeriesBase */
ChartPieSeriesBase::ChartPieSeriesBase(ChartPieSeriesBasePrivate& d, QObject* parent)
    : AbstractSeries(d, parent)
{
}

ChartPieSeriesBase::~ChartPieSeriesBase()
{
}

ChartDataPoint* ChartPieSeriesBase::add(qreal value, bool special, const QString& label, const QColor& color)
{
    QTN_D(ChartPieSeriesBase);
    return d.m_data->addY(value, special, label, color);
}

int ChartPieSeriesBase::count() const
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_data->count();
}

ChartDataTablePrivate* ChartPieSeriesBase::data() const
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_data;
}

int ChartPieSeriesBase::depth() const
{
    return 0;
}

int ChartPieSeriesBase::holePercent() const 
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_holePercent;
}

void ChartPieSeriesBase::setHolePercent(int holePercent) 
{
    QTN_D(ChartPieSeriesBase);
    d.m_holePercent = holePercent;
    chartUpdate();
}

bool ChartPieSeriesBase::isExploded() const
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_explodedRelativeSize > 0;
}

int ChartPieSeriesBase::explodeRelativeSizePercent() const 
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_explodedRelativeSize;
}

void ChartPieSeriesBase::setExplodeRelativeSizePercent(int explodedRelativeSize) 
{
    QTN_D(ChartPieSeriesBase);
    d.m_explodedRelativeSize = explodedRelativeSize;
    chartUpdate();
}

int ChartPieSeriesBase::rotation() const 
{
    QTN_D(const ChartPieSeriesBase);
    return d.m_rotation;
}

void ChartPieSeriesBase::setRotation(int rotation) 
{
    QTN_D(ChartPieSeriesBase);
    d.m_rotation = rotation;
    chartUpdate();
}


/*!
    \class Qtitan::ChartPieSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesPrivate::ChartPieSeriesPrivate()
{
    m_border = Q_NULL;
}

ChartPieSeriesPrivate::~ChartPieSeriesPrivate()
{
}

void ChartPieSeriesPrivate::init()
{
    QTN_P(ChartPieSeries);
    m_border = new ChartBorder(&p);
    p.setLabel(new ChartPieSeriesLabel());
}

ChartArea* ChartPieSeriesPrivate::createArea(QObject* parent)
{
    return new ChartPieArea(parent);
}

bool ChartPieSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    if (!dynamic_cast<ChartPieArea*>(area))
        return false;

    return area->qtn_d().m_listDataTables.size() == 0;
}

ChartSeriesItem* ChartPieSeriesPrivate::createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem)
{
    return new ChartPieSeriesItem(series, areaItem);
}

/* ChartPieSeries */
ChartPieSeries::ChartPieSeries()
    : ChartPieSeriesBase(*new ChartPieSeriesPrivate, Q_NULL)
{
    QTN_D(ChartPieSeries);
    d.init();
}

ChartPieSeries::~ChartPieSeries()
{
}

QColor ChartPieSeries::borderColor() const
{
    QTN_D(const ChartPieSeries);
    if (d.m_border->color().isValid())
        return d.m_border->color();

    return ChartTheme::themeTopParent(this)->pieTheme()->m_borderColor;
}


/*!
    \class Qtitan::ChartSliceLabelItem
    \inmodule QtitanChart
    \internal
*/
ChartSliceLabelItem::ChartSliceLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartSeriesLabelItem(label, pointItem, parentItem)
{
}

void ChartSliceLabelItem::calculateLayout()
{
}

ChartPieSeriesLabel::PieLabelPosition ChartSliceLabelItem::position() const
{
    return ((ChartPieSeriesLabel*)m_label)->position();
}

void ChartSliceLabelItem::paint(QPainter* p)
{
    qreal lineAngle = 0.0;

    int borderThickness = m_label->border()->thickness();

    QRectF realBounds(((ChartPieSeriesSliceItem*)m_pointItem)->getPie()->boundingRect());

    if (realBounds.width() < 1 || realBounds.height() < 1)
        return;

    QPointF locationPoint = ((ChartPieSeriesLabel*)m_label)->qtn_d().calculateLocationPointAndAngles((ChartPieSeriesSliceItem*)m_pointItem,
        borderThickness, lineAngle);

    ChartPieSeriesLabel::PieLabelPosition pos = position();

    QString text(m_label->pointLabel(m_pointItem->point()));

    if (pos == ChartPieSeriesLabel::PieLabelInside)
    {
        ChartTextPainter painter(text, m_label);
        QSizeF size = painter.size();

        QRectF bounds(0, 0, size.width() + 2 * borderThickness, size.height() + 2 * borderThickness);
        bounds.translate(locationPoint.x() - bounds.width() * 0.5, locationPoint.y() - bounds.height() * 0.5);
        // bounds.round();

        QRectF innerBounds = bounds;
        innerBounds.adjust((float)-borderThickness, (float)-borderThickness, (float)borderThickness, (float)borderThickness);

        QPointF labelPoint = innerBounds.topLeft();

        QColor clrBackColor = m_label->backgroundColor();
        m_label->qtn_d().m_fillHelper->drawGradientRect(p, bounds, clrBackColor, clrBackColor);

        painter.setLocation(labelPoint);

        painter.drawBoundedText(p, textColor());

        if (borderThickness)
            m_label->border()->drawBorder(p, bounds, borderColor());
    }
    else if (pos == ChartPieSeriesLabel::PieLabelRadial)
    {
        ChartTextPainter painter(text, m_label);
        QSizeF size = painter.size();

        QRectF bounds(0, 0, size.width() + 2 * borderThickness, size.height() + 2 * borderThickness);
        bounds.translate(-bounds.width() * 0.5, - bounds.height() * 0.5);
        // bounds.round();

        QRectF innerBounds = bounds;
        innerBounds.adjust((qreal)borderThickness, (qreal)borderThickness, (qreal)borderThickness, (qreal)borderThickness);

        qreal angle = (qreal)(-(int)ChartMath::radian2Degree(lineAngle) + 90);
        angle = (qreal)ChartMath::normalizeDegree(angle);

        angle = angle <= 180 ? angle - 90 : angle + 90;

        p->save();

        ChartPainter::painter()->pushTransform(p, locationPoint.x(), locationPoint.y(), 0);
        ChartPainter::painter()->pushRotateTransform(p, angle);

        ChartPainter::painter()->setAntialiasing(p);

        QPointF labelPoint = innerBounds.topLeft();

        QColor clrBackColor = m_label->backgroundColor();
        m_label->qtn_d().m_fillHelper->drawGradientRect(p, bounds, clrBackColor, clrBackColor);

        painter.setLocation(labelPoint);

        painter.drawBoundedText(p, textColor());

        if (borderThickness)
            m_label->border()->drawBorder(p, bounds, borderColor());

        ChartPainter::painter()->unsetAntialiasing(p);
        ChartPainter::painter()->popRotateTransform(p);
        ChartPainter::painter()->popTransform(p);
        p->restore();
    }

    else if (pos == ChartPieSeriesLabel::PieLabelOutside)
    {
        ChartTextPainter painter(text, m_label);
        QSizeF size = painter.size();

        QPointF startPoint(locationPoint);
        QPointF finishPoint(locationPoint.x() + (float)(qCos(lineAngle) * m_label->lineLength()),
            locationPoint.y() - (float)(qSin(lineAngle) * m_label->lineLength()));

        QRectF innerBounds;
        QRectF bounds = ChartSeriesLabelConnectorPainterBase::calcBorderBoundsForTangentDrawing(finishPoint, lineAngle, size, borderThickness, innerBounds);
        //  bounds.round();
        //  innerBounds.round();

        QPointF offest = ((ChartPieLabelsItem*)parent())->calculateOffset(lineAngle, bounds);

        bounds.translate(offest);
        innerBounds.translate(offest);
        finishPoint += offest;

        QPointF labelPoint = innerBounds.topLeft();

        QColor clrBackColor = m_label->backgroundColor();
        m_label->qtn_d().m_fillHelper->drawGradientRect(p, bounds, clrBackColor, clrBackColor);

        painter.setLocation(labelPoint);
        painter.drawBoundedText(p, textColor());


        if (m_label->showLines())
        {
            ChartSeriesLabelLineConnectorPainter linePainter(startPoint, finishPoint, lineAngle, bounds);
            linePainter.drawFillLine(p, m_label, linkColor());
        }

        if (borderThickness)
            m_label->border()->drawInnerBorder(p, bounds, borderColor());

        ((ChartPieLabelsItem*)parent())->addLabelRectangle(bounds);
    }
}

/*!
    \class Qtitan::ChartPieSeriesLabelPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPieSeriesLabelPrivate::ChartPieSeriesLabelPrivate()
{
    m_position = ChartPieSeriesLabel::PieLabelOutside;
}

ChartPieSeriesLabelPrivate::~ChartPieSeriesLabelPrivate()
{
}

GraphicsItem* ChartPieSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
{
    QTN_P(ChartPieSeriesLabel)
    return new ChartSliceLabelItem(&p, pointItem, parentItem);
}

QPointF ChartPieSeriesLabelPrivate::calculateLocationPointAndAngles(ChartPieSeriesSliceItem* pointItem, int borderThickness, qreal& lineAngle)
{
    Q_UNUSED(borderThickness);
    QTN_P(ChartPieSeriesLabel)
    lineAngle = pointItem->getPie()->halfAngle();

    int holePercent = pointItem->getPie()->holePercent();

    if (holePercent > 0 && p.isInside())
    {
        qreal fraction = holePercent / 100.0;
        qreal width = pointItem->getPie()->boundingRect().width() * 0.5;
        qreal height = pointItem->getPie()->boundingRect().height() * 0.5;
        qreal minWidth = width * fraction;
        qreal minHeight = height * fraction;
        if (minWidth <= 0.0 || minHeight <= 0.0)
            return QPointF(0, 0);

        qreal beginAngle = pointItem->getPie()->beginAngle();
        qreal halfAngle = pointItem->getPie()->halfAngle();
        QPointF center = pointItem->getPie()->calculateCenter(pointItem->basePoint());
        ChartEllipse maxEllipse(center, width, height);
        ChartEllipse minEllipse(center, minWidth, minHeight);
        ChartPieDrawHelper maxPie(beginAngle, halfAngle, maxEllipse, 0, 0);
        ChartPieDrawHelper minPie(beginAngle, halfAngle, minEllipse, 0, 0);
        return QPointF((maxPie.endPoint().x() + minPie.endPoint().x()) * 0.5f, (maxPie.endPoint().y() + minPie.endPoint().y()) * 0.5f);
    }

    QRectF realBounds(pointItem->getPie()->boundingRect());

    ChartEllipse actualEllipse(pointItem->getPie()->calculateCenter(pointItem->basePoint()), realBounds.width() * 0.5, realBounds.height() * 0.5);
    ChartPieDrawHelper actualPie(pointItem->getPie()->beginAngle(), pointItem->getPie()->halfAngle(), actualEllipse, 0, 0);

    if (!p.isInside())
        return actualPie.endPoint();

    if (pointItem->m_value == 1)
    {
        lineAngle = 0;
        return actualPie.centerPoint();
    }

    return QPointF((actualPie.centerPoint().x() + actualPie.endPoint().x()) * 0.5f, (actualPie.centerPoint().y() + actualPie.endPoint().y()) * 0.5f);
}

/* ChartPieSeriesLabel */
ChartPieSeriesLabel::ChartPieSeriesLabel(QObject* p)
    : ChartSeriesLabel(*new ChartPieSeriesLabelPrivate, p)
{
}

ChartPieSeriesLabel::~ChartPieSeriesLabel()
{
}

ChartPieSeriesLabel::PieLabelPosition ChartPieSeriesLabel::position() const 
{
    QTN_D(const ChartPieSeriesLabel)
    return d.m_position;
}

void ChartPieSeriesLabel::setPosition(ChartPieSeriesLabel::PieLabelPosition pos) 
{
    QTN_D(ChartPieSeriesLabel)
    d.m_position = pos;
    chartUpdate();
}


/*!
    \class Qtitan::ChartPieLabelsItem
    \inmodule QtitanChart
    \internal
*/
ChartPieLabelsItem::ChartPieLabelsItem(GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
}

void ChartPieLabelsItem::addLabelRectangle(QRectF& rc)
{
    m_arrLabels.append(rc);
}

QPointF ChartPieLabelsItem::calculateOffset(qreal/*lineAngle*/, const QRectF& rc)
{
    Q_UNUSED(rc);
    return QPointF(0.0, 0.0);
}

void ChartPieLabelsItem::paint(QPainter* p)
{
    for (int i = 0; i < m_listItem.size(); i++)
        m_listItem[i]->paint(p);
}

bool ChartPieSeriesLabel::isInside() const
{
    PieLabelPosition pos = position();
    return pos == PieLabelInside || pos == PieLabelRadial;
}


