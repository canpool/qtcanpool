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

#include "QtnChart2DSeries.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "Qtn2DSeriesPrivate.h"
#include "QtnChartDataPoint.h"
#include "QtnChartArea2D.h"
#include "QtnGraphicsItem.h"
#include "QtnChartTextPainter.h"
#include "QtnChartMath.h"
#include "QtnChartPainter.h"
#include "QtnChart.h"

#include "QtnChartAxis.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartTitle.h"

using namespace Qtitan;

/*!
    \class Qtitan::Chart2DSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
Chart2DSeriesPrivate::Chart2DSeriesPrivate()
{
    m_data = new ChartDataTablePrivate();
    m_secondaryAxisX = false;
    m_secondaryAxisY = false;
}

Chart2DSeriesPrivate::~Chart2DSeriesPrivate()
{
}

void Chart2DSeriesPrivate::linkData()
{
    QTN_P(Chart2DSeries);
    m_data->setParent(p.parent());
    m_data->setSeries(&p);
}

void Chart2DSeriesPrivate::unlinkData()
{
    if (ChartArea* area = m_data->area())
    {
        if (isSeriesArea(area))
            return;
        m_data->setArea(Q_NULL);
    }
}

ChartArea* Chart2DSeriesPrivate::createArea(QObject* parent)
{
    return new ChartArea2D(parent);
}

bool Chart2DSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    return dynamic_cast<ChartArea2D*>(area);
}

void Chart2DSeriesPrivate::setArea(ChartArea* area)
{
    m_data->setArea(area);
}


/* Chart2DSeries */
Chart2DSeries::Chart2DSeries(Chart2DSeriesPrivate& d, QObject* parent)
    : AbstractSeries(d, parent)
{
}

Chart2DSeries::~Chart2DSeries()
{
}

ChartDataPoint* Chart2DSeries::addAxisPointY(double value, const QString& axisLabel)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addAxisPointY(value, axisLabel);
}

ChartDataPoint* Chart2DSeries::addY(qreal value, const QString& label, const QColor& color)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addY(value, false, label, color);
}

ChartDataPoint* Chart2DSeries::addY(const QVariant& value, const QString& label, const QColor& color)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addY(value, label, color);
}

ChartDataPoint* Chart2DSeries::addXY(qreal xvalue, qreal yvalue, const QString& label, const QColor& color)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addXY(xvalue, yvalue, label, color);
}

ChartDataPoint* Chart2DSeries::addXYY1(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label, const QColor& color)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addXYY1(xvalue, yvalue, yvalue1, label, color);
}

ChartDataPoint* Chart2DSeries::addXY(const QVariant& xvalue, const QVariant& yvalue, const QString& label, const QColor& color)
{
    QTN_D(Chart2DSeries);
    return d.m_data->addXY(xvalue, yvalue, label, color);
}

int Chart2DSeries::count() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->count();
}

ChartDataPoint* Chart2DSeries::at(int index) const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->at(index);
}

int Chart2DSeries::indexOf(ChartDataPoint* point) const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->indexOf(point);
}

void Chart2DSeries::clear()
{
    QTN_D(Chart2DSeries);
    d.m_data->clear();
}

ChartDataTablePrivate* Chart2DSeries::data() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data;
}

QColor Chart2DSeries::color() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->color();
}

void Chart2DSeries::setName(const QString& name)
{
    QTN_D(Chart2DSeries);
    d.m_data->setName(name);
}

QString Chart2DSeries::name() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->name();
}

void Chart2DSeries::setLegendText(const QString& legendText)
{
    QTN_D(Chart2DSeries);
    d.m_data->setLegendText(legendText);
}

QString Chart2DSeries::legendText() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->legendText();
}


void Chart2DSeries::setPointLegendFormat(const QString& strFormat)
{
    QTN_D(Chart2DSeries);
    d.m_data->setPointLegendFormat(strFormat);
}

QString Chart2DSeries::pointLegendFormat() const
{
    QTN_D(const Chart2DSeries);
    return d.m_data->pointLegendFormat();
}

void Chart2DSeries::setSecondaryAxisX(bool secondaryAxis) 
{
    QTN_D(Chart2DSeries)
    d.m_secondaryAxisX = secondaryAxis;
    chartUpdate();
}

bool Chart2DSeries::isSecondaryAxisX() const 
{
    QTN_D(const Chart2DSeries)
    return d.m_secondaryAxisX;
}

void Chart2DSeries::setSecondaryAxisY(bool secondaryAxis) 
{
    QTN_D(Chart2DSeries)
    d.m_secondaryAxisY = secondaryAxis;
    chartUpdate();
}

bool Chart2DSeries::isSecondaryAxisY() const 
{
    QTN_D(const Chart2DSeries)
    return d.m_secondaryAxisY;
}


/*!
    \class Qtitan::ChartArea2DSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartArea2DSeriesItem::ChartArea2DSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem, bool sortPoints)
    : ChartSeriesItem(series, areaItem)
{
    m_sortPoints = sortPoints;
    m_axisItemX = m_axisItemY = Q_NULL;
}

QPointF ChartArea2DSeriesItem::screenPoint(qreal x, qreal y) const
{
    ChartArea2DItem* area = (ChartArea2DItem*)areaItem();

    ChartAreaFormItem* formItem = area->areaFormItem();

    return formItem->screenPoint(this, x, y);
}

#ifdef Q_OS_WIN
typedef int (_cdecl *COMPAREFUNC)(const void *, const void*);
#else
typedef int (*COMPAREFUNC)(const void *, const void*);
#endif
int comparePoints(const ChartSeriesPointItem** ppPoint1, const ChartSeriesPointItem** ppPoint2)
{
//    qreal d = (*ppPoint1)->point()->qtn_d().m_innerArg - (*ppPoint2)->point()->qtn_d().m_innerArg;
    qreal d = (*ppPoint1)->point()->valueX() - (*ppPoint2)->point()->valueX();

    if (d > 0)
        return 1;
    if (d < 0)
        return -1;
    return 0;
}

void ChartArea2DSeriesItem::beforeUpdateRange()
{
}

void ChartArea2DSeriesItem::afterUpdateRange()
{
    if (m_sortPoints)
        qsort(m_pointsItem->children(), (size_t) m_pointsItem->count(), sizeof(ChartSeriesPointItem*), (COMPAREFUNC)comparePoints);
}



/*!
    \class Qtitan::ChartArea2DSeriesLabelItem
    \inmodule QtitanChart
    \internal
*/
ChartArea2DSeriesLabelItem::ChartArea2DSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : ChartSeriesLabelItem(label, pointItem, parentItem)
{
}

ChartArea2DSeriesLabelItem::~ChartArea2DSeriesLabelItem()
{
}

QRect ChartArea2DSeriesLabelItem::formRect() const
{
    if (ChartSeriesItem* seriseItem = m_pointItem->seriesItem())
    {
        if (ChartArea2DItem* areaItem = (ChartArea2DItem*)seriseItem->areaItem())
            return areaItem->areaFormItem()->bounds();
    }
    return QRect();
}

QPointF ChartArea2DSeriesLabelItem::locationPoint() const
{
    return QPointF(0.0, 0.0);
}

qreal ChartArea2DSeriesLabelItem::locationAngle() const
{
    return 0.0;
}

void ChartArea2DSeriesLabelItem::calculateLayout()
{
    if (ChartDataPoint* seriesPoint = m_pointItem->point())
    {
        QString text = m_label->pointLabel(seriesPoint);
        calculateLayout(locationPoint(), locationAngle(), text);
    }
}

void ChartArea2DSeriesLabelItem::calculateLayout(const QPointF& locationPoint, qreal locationAngle, const QString& text)
{
    qreal lineAngle = ChartMath::degree2Radian(locationAngle);

    int borderThickness = m_label->border()->thickness();

    QRect rcPaneRect = formRect();
    rcPaneRect.adjust(-1, -1, 1, 1);

    if (!rcPaneRect.contains((int)locationPoint.x(), (int)locationPoint.y()))
        return;

    ChartTextPainter painter(text, m_label);
    QSizeF szText = painter.size();

    QPointF ptStartPoint(qRound(locationPoint.x()), qRound(locationPoint.y()));
    QPointF ptFinishPoint(qRound(locationPoint.x() + (cos(lineAngle) * m_label->lineLength())),
        qRound(locationPoint.y() - (sin(lineAngle) * m_label->lineLength())));

    QRectF rcInnerBounds;
    QRectF boundingRect = ChartSeriesLabelConnectorPainterBase::calcBorderBoundsForTangentDrawing(ptFinishPoint, lineAngle, szText, borderThickness, rcInnerBounds);

    ChartSeriesItem* seriseItem = m_pointItem->seriesItem();
    ChartArea2DItem* areaItem = (ChartArea2DItem*)seriseItem->areaItem();
    areaItem->checkLabelBounds(boundingRect);
}

void ChartArea2DSeriesLabelItem::calculateLayout(QPointF centerPoint)
{
    QString text(m_label->pointLabel(m_pointItem->point()));
    calculateLayout(centerPoint, text);
}

void ChartArea2DSeriesLabelItem::calculateLayout(QPointF centerPoint, const QString& text)
{
    QRect rcPaneRect = formRect();
    rcPaneRect.adjust(-1, -1, 1, 1);

    if (!rcPaneRect.contains((int)centerPoint.x(), (int)centerPoint.y()))
        return;

    int borderThickness = m_label->border()->thickness();

    ChartTextPainter painter(text, m_label);
    QSizeF szText = painter.size();

    QRectF boundingRect(0, 0, szText.width() + 2 * borderThickness, szText.height() + 2 * borderThickness);
    boundingRect.translate(centerPoint.x() - boundingRect.width() * 0.5, centerPoint.y() - boundingRect.height() * 0.5);
    // boundingRect.round();

    if (ChartSeriesItem* seriseItem = m_pointItem->seriesItem())
    {
        if (ChartArea2DItem* areaItem = (ChartArea2DItem*)seriseItem->areaItem())
            areaItem->checkLabelBounds(boundingRect);
    }
}

void ChartArea2DSeriesLabelItem::paint(QPainter* p)
{
    QString text(m_label->pointLabel(m_pointItem->point()));
    paint(p, locationPoint(), locationAngle(), text);
}

void ChartArea2DSeriesLabelItem::paint(QPainter* p, const QPointF& locationPoint, qreal locationAngle, const QString& text)
{
    qreal lineAngle = ChartMath::degree2Radian(locationAngle);

    int borderThickness = m_label->border()->thickness();

    QRect rcPaneRect = formRect();
    rcPaneRect.adjust(-1, -1, 1, 1);

    if (!rcPaneRect.contains((int)locationPoint.x(), (int)locationPoint.y()))
        return;

    ChartTextPainter painter(text, m_label);
    QSizeF szText = painter.size();


    QPointF ptStartPoint(qRound(locationPoint.x()), qRound(locationPoint.y()));
    QPointF ptFinishPoint(qRound(locationPoint.x() + (cos(lineAngle) * m_label->lineLength())),
        qRound(locationPoint.y() - (sin(lineAngle) * m_label->lineLength())));

    QRectF rcInnerBounds;
    QRectF boundingRect = ChartSeriesLabelConnectorPainterBase::calcBorderBoundsForTangentDrawing(ptFinishPoint, lineAngle, szText, borderThickness, rcInnerBounds);

    QPointF ptLabelPoint = rcInnerBounds.topLeft();

    QColor clrBackColor = m_label->backgroundColor();
    m_label->qtn_d().m_fillHelper->drawGradientRect(p, boundingRect, clrBackColor, clrBackColor);

    painter.setLocation(ptLabelPoint);

    painter.drawBoundedText(p, textColor());

    if (m_label->showLines())
    {
        ChartSeriesLabelLineConnectorPainter linePainter(ptStartPoint, ptFinishPoint, lineAngle, boundingRect);
        linePainter.drawFillLine(p, m_label, linkColor());
    }

    if (borderThickness)
        m_label->border()->drawInnerBorder(p, boundingRect, borderColor());
}

void ChartArea2DSeriesLabelItem::paint(QPainter* p, const QPointF& centerPoint)
{
    QString text(m_label->pointLabel(m_pointItem->point()));
    paint(p, centerPoint, text);
}

void ChartArea2DSeriesLabelItem::paint(QPainter* p, const QPointF& centerPoint, const QString& text)
{
    QRect rcPaneRect = formRect();
    rcPaneRect.adjust(-1, -1, 1, 1);

    if (!rcPaneRect.contains((int)centerPoint.x(), (int)centerPoint.y()))
        return;

    int borderThickness = m_label->border()->thickness();

    ChartTextPainter painter(text, m_label);
    QSizeF szText = painter.size();

    QRectF boundingRect(0, 0, szText.width() + 2.0 * borderThickness, szText.height() + 2.0 * borderThickness);
    boundingRect.translate(centerPoint.x() - boundingRect.width() * 0.5, centerPoint.y() - boundingRect.height() * 0.5);
    // boundingRect.round();

    QRectF rcInnerBounds = boundingRect;
    //    rcInnerBounds.adjust((qreal)-borderThickness, (qreal)-borderThickness, (qreal)borderThickness, (qreal)borderThickness);
    rcInnerBounds.adjust((qreal)borderThickness, (qreal)borderThickness, (qreal)-borderThickness, (qreal)-borderThickness);

    QPointF ptLabelPoint = rcInnerBounds.topLeft();


    QColor clrBackColor = m_label->backgroundColor();
    m_label->qtn_d().m_fillHelper->drawGradientRect(p, boundingRect, clrBackColor, clrBackColor);

    painter.setLocation(ptLabelPoint);

    painter.drawBoundedText(p, textColor());

    if (borderThickness)
        m_label->border()->drawInnerBorder(p, boundingRect, borderColor());
}





