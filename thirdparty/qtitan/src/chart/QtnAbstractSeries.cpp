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
#include "QtnAbstractSeries.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartDataPointPrivate.h"
#include "QtnChart.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartDataPoint.h"
#include "QtnChartMath.h"
#include "QtnChartPainter.h"
#include "QtnChartAreaPrivate.h"
#include "QtnChartPalette.h"
#include "QtnChartPointSeriesPrivate.h"


using namespace Qtitan;

/*!
    \class Qtitan::AbstractSeriesPrivate
    \inmodule QtitanChart
    \internal
*/
AbstractSeriesPrivate::AbstractSeriesPrivate()
{
    m_label = Q_NULL;
    m_colorAlternate = false;
    m_visible = true;
}

AbstractSeriesPrivate::~AbstractSeriesPrivate()
{
}

ChartArea* AbstractSeriesPrivate::createArea(QObject* parent) 
{
    Q_UNUSED(parent);
    return Q_NULL;
}

bool AbstractSeriesPrivate::isSeriesArea(ChartArea* area) const
{
    Q_UNUSED(area);
    return false;
}

void AbstractSeriesPrivate::correctAxisSideMargins(ChartAxis* axis, qreal minValue, qreal maxValue, qreal& correction)
{
    Q_UNUSED(axis);
    Q_UNUSED(minValue);
    Q_UNUSED(maxValue);
    Q_UNUSED(correction);
}

int AbstractSeriesPrivate::seriesPointValueIndex() const 
{
    return 0;
}

void AbstractSeriesPrivate::setArea(ChartArea* area)
{
    Q_UNUSED(area);
}

/* AbstractSeries */
AbstractSeries::AbstractSeries(AbstractSeriesPrivate& d, QObject* parent)
    : ChartComponent(parent)
{
    qtn_d_ptr = &d; 
    qtn_d_ptr->setPublic(this);
}

AbstractSeries::~AbstractSeries()
{
    QTN_FINI_PRIVATE();
}

ChartArea* AbstractSeries::area()
{
    return data()->area();
}

ChartSeriesLabel* AbstractSeries::label() const 
{
    QTN_D(const AbstractSeries);
    return d.m_label;
}

QColor AbstractSeries::color() const 
{
    QTN_D(const AbstractSeries);
    return d.m_color;
}

void AbstractSeries::setColor(const QColor& color) 
{
    QTN_D(AbstractSeries);
    d.m_color = color;
    chartUpdate();
}

bool AbstractSeries::isAlternateColor() const 
{
    QTN_D(const AbstractSeries);
    return d.m_colorAlternate;
}

void AbstractSeries::setAlternateColor(bool alternate) 
{
    QTN_D(AbstractSeries);
    d.m_colorAlternate = alternate;
    chartUpdate();
}

void AbstractSeries::setVisible(bool visible) 
{
    QTN_D(AbstractSeries);
    d.m_visible = visible;
    chartUpdate();
}

bool AbstractSeries::isVisible() const 
{
    QTN_D(const AbstractSeries);
    return d.m_visible;
}

void AbstractSeries::setLabel(ChartSeriesLabel* label)
{
    QTN_D(AbstractSeries);
    if (d.m_label)
    {
        delete d.m_label;
        d.m_label = Q_NULL;
    }
    d.m_label = label;

    if (d.m_label)
        d.m_label->setParent(this);
}


/*!
    \class Qtitan::ChartSeriesLabelPrivate
    \inmodule QtitanChart
    \internal
*/
ChartSeriesLabelPrivate::ChartSeriesLabelPrivate()
{
}

ChartSeriesLabelPrivate::~ChartSeriesLabelPrivate()
{
}

void ChartSeriesLabelPrivate::init()
{
    QTN_P(ChartSeriesLabel);
    m_font = QFont(QObject::tr("Tahoma"), 8);
    m_font.setWeight(QFont::Normal);

    m_border = new ChartBorder(&p);

    m_fillHelper = new ChartFillDrawHelper(&p);
    m_fillHelper->setFillStyle(Qt::SolidPattern);

    m_antialiasing = false;
    m_visible = true;
    m_showLines = true;
    m_lineLength = 10;
}

GraphicsItem* ChartSeriesLabelPrivate::createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem) 
{
    Q_UNUSED(pointItem);
    Q_UNUSED(parentItem);
    return Q_NULL;
}

/* ChartSeriesLabel */
ChartSeriesLabel::ChartSeriesLabel(QObject* p)
    : ChartTextComponent(p)
{
    QTN_INIT_PRIVATE(ChartSeriesLabel);
    QTN_D(ChartSeriesLabel);
    d.init();
}

ChartSeriesLabel::ChartSeriesLabel(ChartSeriesLabelPrivate& d, QObject* p)
    : ChartTextComponent(p)
    , qtn_d_ptr(&d) 
{
    qtn_d_ptr->setPublic(this);
    d.init();
}

ChartSeriesLabel::~ChartSeriesLabel()
{
    QTN_FINI_PRIVATE();
}

const QFont& ChartSeriesLabel::font() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_font;
}

const QColor& ChartSeriesLabel::textColor() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_textColor;
}

void ChartSeriesLabel::setShowLines(bool showLines) 
{
    QTN_D(ChartSeriesLabel)
    d.m_showLines = showLines;
    chartUpdate();
}

bool ChartSeriesLabel::showLines() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_showLines;
}

int ChartSeriesLabel::lineLength() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_lineLength;
}

void ChartSeriesLabel::setLineLength(int lineLength) 
{
    QTN_D(ChartSeriesLabel)
    d.m_lineLength = lineLength;
    chartUpdate();
}

void ChartSeriesLabel::setVisible(bool visible) 
{
    QTN_D(ChartSeriesLabel)
    if (d.m_visible != visible)
    {
        d.m_visible = visible;
        emit visibleChanged(visible);
        chartUpdate();
    }
}

bool ChartSeriesLabel::isVisible() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_visible;
}

void ChartSeriesLabel::setAntialiasing(bool antialiasing) 
{
    QTN_D(ChartSeriesLabel)
    d.m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartSeriesLabel::antialiasing() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_antialiasing;
}

QString ChartSeriesLabel::format() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_strFormat;
}

void ChartSeriesLabel::setFormat(const QString& format) 
{
    QTN_D(ChartSeriesLabel)
    d.m_strFormat = format;
    chartUpdate();
}

void ChartSeriesLabel::setTextColor(const QColor& color) 
{
    QTN_D(ChartSeriesLabel)
    d.m_textColor = color;
    chartUpdate();
}

void ChartSeriesLabel::setLineColor(const QColor& color) 
{
    QTN_D(ChartSeriesLabel)
    d.m_lineColor = color;
    chartUpdate();
}

void ChartSeriesLabel::setBackgroundColor(const QColor& color) 
{
    QTN_D(ChartSeriesLabel)
    d.m_backColor = color;
    chartUpdate();
}

QColor ChartSeriesLabel::backgroundColor() const 
{
    QTN_D(const ChartSeriesLabel)
    if (d.m_backColor.isValid())
        return d.m_backColor;
    return ChartTheme::themeTopParent(this)->seriesTheme()->labelTheme()->m_backgroundColor;
}

QColor ChartSeriesLabel::lineColor() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_lineColor;
}

ChartBorder* ChartSeriesLabel::border() const 
{
    QTN_D(const ChartSeriesLabel)
    return d.m_border;
}

void ChartSeriesLabel::setFont(const QFont& font)
{
    QTN_D(ChartSeriesLabel)
    if (d.m_font != font) 
    {
        d.m_font = font;
        chartUpdate();
        emit fontChanged(font);
    }
}

QString ChartSeriesLabel::pointLabel(ChartDataPoint* point, int valueIndex) const
{
    Q_UNUSED(valueIndex);
    QTN_D(const ChartSeriesLabel)
    if (!point->label().isEmpty())
        return point->label();

    valueIndex = valueIndex == - 1 ? point->qtn_d().m_parent->series()->qtn_d().seriesPointValueIndex() : valueIndex;
    qreal value = valueIndex == 1 ? point->valueY1() : point->valueY();

    QString strValue;
    if (fabs(value) < 1e-6)
        strValue = QObject::tr("0");
    else if (!d.m_strFormat.isEmpty())
        strValue.sprintf(d.m_strFormat.toStdString().c_str(), value);
    else
        strValue = QString("%1").arg(value);
    return strValue;
}


/*!
    \class Qtitan::ChartSeriesLabelItem
    \inmodule QtitanChart
    \internal
*/
ChartSeriesLabelItem::ChartSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_label = label;
    m_pointItem = pointItem;
}

ChartSeriesLabelItem::~ChartSeriesLabelItem()
{
}

void ChartSeriesLabelItem::calculateLayout()
{
}

QColor ChartSeriesLabelItem::textColor() const
{
    if (m_label->textColor().isValid())
        return m_label->textColor();

    QColor txtColor = ChartTheme::themeTopParent(this)->seriesTheme()->labelTheme()->m_textColor;
    if (txtColor.isValid())
        return txtColor;

    return m_pointItem->startColor().dark();
}

QColor ChartSeriesLabelItem::borderColor() const
{
    QColor clrBorderColor = m_label->border()->color();
    if (clrBorderColor.isValid())
        return clrBorderColor;

    clrBorderColor = ChartTheme::themeTopParent(this)->seriesTheme()->labelTheme()->m_borderColor;
    if (clrBorderColor.isValid())
        return clrBorderColor;

    return m_pointItem->startColor().dark();
}

QColor ChartSeriesLabelItem::linkColor() const
{
    QColor clrLineColor = m_label->lineColor();
    if (clrLineColor.isValid())
        return clrLineColor;

    clrLineColor = ChartTheme::themeTopParent(this)->seriesTheme()->labelTheme()->m_connectorColor;
    if (clrLineColor.isValid())
        return clrLineColor;

    return m_pointItem->startColor().dark();
}

/*!
    \class Qtitan::ChartSeriesLabelConnectorPainterBase
    \inmodule QtitanChart
    \internal
*/
ChartSeriesLabelConnectorPainterBase::ChartSeriesLabelConnectorPainterBase(QPointF startPoint, QPointF finishPoint, QRectF bounds)
{
    m_startPoint = startPoint;
    m_finishPoint = finishPoint;
    m_bounds = bounds;
}

enum ChartAngleOrientation
{
    ChartAngleQuadrant1,
    ChartAngleQuadrant2,
    ChartAngleQuadrant3,
    ChartAngleQuadrant4,
    ChartAngle0,
    ChartAngle90,
    ChartAngle180,
    ChartAngle270,
};

static ChartAngleOrientation getAngleOrientation(qreal angle, qreal deadAngle)
{
    angle = ChartMath::normalizeRadian(angle);
    angle = angle < 0 ? ChartMath::QTN_PI2 + angle : angle;
    ChartAngleOrientation disposition;
    if (ChartMath::fuzzyCmp(angle, 0, deadAngle) == 0)
        disposition = ChartAngle0;
    else if (ChartMath::fuzzyCmp(angle, ChartMath::QTN_PI_2, deadAngle) == 0)
        disposition = ChartAngle90;
    else if (ChartMath::fuzzyCmp(angle, ChartMath::QTN_PI, deadAngle) == 0)
        disposition = ChartAngle180;
    else if (ChartMath::fuzzyCmp(angle, 3 * ChartMath::QTN_PI_2, deadAngle) == 0)
        disposition = ChartAngle270;
    else if (angle > 0 && angle < ChartMath::QTN_PI_2)
        disposition = ChartAngleQuadrant1;
    else if (angle > ChartMath::QTN_PI_2 && angle < ChartMath::QTN_PI)
        disposition = ChartAngleQuadrant2;
    else if (angle > ChartMath::QTN_PI && angle < 3 * ChartMath::QTN_PI_2)
        disposition = ChartAngleQuadrant3;
    else
        disposition = ChartAngleQuadrant4;
    return disposition;
}


QRectF ChartSeriesLabelConnectorPainterBase::calcBorderBoundsForTangentDrawing(const QPointF& point, qreal angle, const QSizeF& textSize, int borderThickness, QRectF& innerBounds)
{
    const qreal DeadAngle = ChartMath::QTN_DEGRAD * 10;

    QRectF bounds(QPointF(0, 0), QSizeF((float)floor(textSize.width() + 2 * borderThickness), (float)floor(textSize.height() + 2 * borderThickness)));
    QPointF topLeft = point;

    ChartAngleOrientation disposition = getAngleOrientation(angle, DeadAngle);
    switch (disposition)
    {
    case ChartAngle0:
        topLeft += QPointF(0, -bounds.height() * 0.5);
        break;
    case ChartAngle90:
        topLeft += QPointF(-bounds.width() * 0.5, -bounds.height());
        break;
    case ChartAngle180:
        topLeft += QPointF(-bounds.width(), -bounds.height() * 0.5);
        break;
    case ChartAngle270:
        topLeft += QPointF(-bounds.width() * 0.5, 0);
        break;
    case ChartAngleQuadrant1:
        topLeft += QPointF(0, -bounds.height());
        break;
    case ChartAngleQuadrant2:
        topLeft += QPointF(-bounds.width(), -bounds.height());
        break;
    case ChartAngleQuadrant3:
        topLeft += QPointF(-bounds.width(), 0);
        break;
    case ChartAngleQuadrant4:
        break;
    default:
        break;
    }
    bounds.translate(topLeft);
    //  bounds.round();

    innerBounds = bounds;
    innerBounds.adjust((float)borderThickness, (float)borderThickness, (float)-borderThickness*2, (float)-borderThickness*2);
    return bounds;
}

/*!
    \class Qtitan::ChartSeriesLabelLineConnectorPainter
    \inmodule QtitanChart
    \internal
*/
ChartSeriesLabelLineConnectorPainter::ChartSeriesLabelLineConnectorPainter(QPointF startPoint, QPointF finishPoint, qreal angle, QRectF bounds)
    : ChartSeriesLabelConnectorPainterBase(startPoint, finishPoint, bounds)
{
    m_angle = angle;
}

void ChartSeriesLabelLineConnectorPainter::drawFillLine(QPainter* p, ChartSeriesLabel* label, const QColor& color)
{
    Q_UNUSED(label);
//    ChartPainter::painter()->setAntialiasing(p);
    ChartPainter::painter()->drawFillLine(p, m_startPoint, m_finishPoint, color, 1);
//    ChartPainter::painter()->unsetAntialiasing(p);
}

/*!
    \class Qtitan::ChartSeriesItem
    \inmodule QtitanChart
    \internal
*/
ChartSeriesItem::ChartSeriesItem(ChartDataTablePrivate* data, ChartAreaItem* areaItem)
    : GraphicsItem(areaItem->seriesItem())
{
    m_data = data;
    m_areaItem = areaItem;
    m_pointsItem = Q_NULL;
    m_labelsItem = Q_NULL;
}

ChartSeriesItem::~ChartSeriesItem()
{
}

ChartSeriesPointItem* ChartSeriesItem::createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem)
{
    Q_UNUSED(point);
    Q_UNUSED(parentItem);
    return Q_NULL;
}

void ChartSeriesItem::createPointsItem()
{
    m_pointsItem = new GraphicsItem(this);
    m_labelsItem = m_areaItem->labelsItem();

    for (int i = 0, count = m_data->count(); i < count; i++)
    {
        ChartDataPoint* point = m_data->at(i);
        if (ChartSeriesPointItem* pointItem = createSeriesPointItem(point, m_pointsItem))
        {
            if (ChartSeriesLabel* label = m_data->series()->label())
            {
                if (label->isVisible())
                    label->qtn_d().createItem(pointItem, m_labelsItem);
            }
        }
    }
}

AbstractSeries* ChartSeriesItem::series() const
{
    return m_data->series();
}

ChartArea* ChartSeriesItem::area() const
{
    return m_areaItem->area();
}

QColor ChartSeriesItem::beginColor() const
{
    if (m_data->series()->color().isValid())
        return m_data->series()->color();

    return ChartTheme::themeTopParent(this)->chartPalette()->lightColor(m_data->paletteIndex());
}

QColor ChartSeriesItem::endColor() const
{
    if (m_data->series()->color().isValid())
        return ChartTheme::lightColor(m_data->series()->color());

    return ChartTheme::themeTopParent(this)->chartPalette()->darkColor(m_data->paletteIndex());
}

void ChartSeriesItem::calculateLayout()
{
}

void ChartSeriesItem::updateRange()
{
}

QString ChartSeriesItem::legendName() const
{
    QString strName = m_data->legendText();
    if (strName.isEmpty())
        strName = m_data->name();

    return strName;
}

void ChartSeriesItem::drawLegendComponent(const QRect& rect)
{
    Q_UNUSED(rect);
}

void ChartSeriesItem::addToLegend(ChartLegendItem* item)
{
    if (m_data->series()->isAlternateColor())
    {
        for (int i = 0, count = m_pointsItem->count(); i < count; i++)
        {
            ChartSeriesPointItem* pointItem = (ChartSeriesPointItem*)m_pointsItem->at(i);
            item->addItem(pointItem);
        }
    }
    else
    {
        item->addItem(this);
    }
}

ChartAreaItem* ChartSeriesItem::areaItem() const 
{
    return m_areaItem;
}

ChartDataTablePrivate* ChartSeriesItem::data() const 
{
    return m_data;
}
