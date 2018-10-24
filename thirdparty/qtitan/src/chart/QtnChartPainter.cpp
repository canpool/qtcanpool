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
#include <QStack>
#include <QDomElement>
#include <QLinearGradient>

#include "QtnChartPainter.h"
#include "QtnChartMath.h"
#include "QtnChartPieSeries.h"

using namespace Qtitan;


QString lpszGradientDirection[] =
{
    "TopToBottom",
    "BottomToTop",
    "LeftToRight",
    "RightToLeft",
    "TopLeftToBottomRight",
    "BottomRightToTopLeft",
    "TopRightToBottomLeft",
    "BottomLeftToTopRight",
    "ToCenterHorizontal",
    "FromCenterHorizontal",
    "ToCenterVertical",
    "FromCenterVertical"
};


namespace Qtitan
{
    /* ChartFillDrawHelperPrivate */   
    class ChartFillDrawHelperPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartFillDrawHelper)
    public:
        explicit ChartFillDrawHelperPrivate();
        virtual ~ChartFillDrawHelperPrivate();
    public:
        void init();
        ChartComponent* getComponentParent() const;

    public:
        struct ColorInfo
        {
            QColor color;
            qreal position;
        };
        QVector<ColorInfo> m_colors;

        Qt::BrushStyle m_fillStyle;
        ChartFillDrawHelper::GradientDirection m_gradientDirection;
    };
}; //namespace Qtitan

/*!
    \class Qtitan::ChartFillDrawHelperPrivate
    \inmodule QtitanChart
    \internal
*/
ChartFillDrawHelperPrivate::ChartFillDrawHelperPrivate()
{
}

ChartFillDrawHelperPrivate::~ChartFillDrawHelperPrivate()
{
}

void ChartFillDrawHelperPrivate::init()
{
    m_fillStyle = Qt::NoBrush;
//    m_hatchStyle = ChartFillDrawHelper::HatchStyleSolidDiamond;
    m_gradientDirection = ChartFillDrawHelper::LeftToRight;
}

ChartComponent* ChartFillDrawHelperPrivate::getComponentParent() const
{
    QTN_P(const ChartFillDrawHelper);
    ChartComponent* parentComponent = qobject_cast<ChartComponent*>(p.parent());
    return parentComponent;
}


/*!
    \class Qtitan::ChartFillDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartFillDrawHelper::ChartFillDrawHelper(QObject* parent)
    : QObject(parent)
{
    QTN_INIT_PRIVATE(ChartFillDrawHelper);
    QTN_D(ChartFillDrawHelper);
    d.init();
}

ChartFillDrawHelper::~ChartFillDrawHelper()
{
    QTN_FINI_PRIVATE();
}

Qt::BrushStyle ChartFillDrawHelper::fillStyle() const 
{
    QTN_D(const ChartFillDrawHelper);
    return d.m_fillStyle;
}

ChartFillDrawHelper::GradientDirection ChartFillDrawHelper::getGradientDirection() const 
{
    QTN_D(const ChartFillDrawHelper);
    return d.m_gradientDirection;
}

static QBrush linearGradientPattern(const QRectF& rc, const QColor& color, const QColor& color2, ChartPainter::ChartLinearGradientMode mode)
{
    QLinearGradient gradient;
    if (mode == ChartPainter::ChartLinearGradientModeVertical)
        gradient = QLinearGradient(rc.topLeft(), rc.bottomLeft());
    else if (mode == ChartPainter::ChartLinearGradientModeHorizontal)
        gradient = QLinearGradient(rc.topLeft(), rc.topRight());
    else if (mode == ChartPainter::ChartLinearGradientModeForwardDiagonal)
        gradient = QLinearGradient(rc.topLeft(), rc.bottomRight());
    else if (mode == ChartPainter::ChartLinearGradientModeBackwardDiagonal)
        gradient = QLinearGradient(rc.topRight(), rc.bottomLeft());
    else
        Q_ASSERT(false);

    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, color2);
    return gradient;
}

QBrush ChartFillDrawHelper::brushGradient(const QRectF& rc,const QColor& color, const QColor& color2)  const
{
    QTN_D(const ChartFillDrawHelper);
    if (d.m_fillStyle == Qt::LinearGradientPattern)
    {
        switch(d.m_gradientDirection)
        {
            case TopToBottom: //
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeVertical);
            case BottomToTop: //
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeVertical);
            case LeftToRight: //
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeHorizontal);
            case RightToLeft: // 
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeHorizontal);
            case TopLeftToBottomRight: //
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeForwardDiagonal);
            case BottomRightToTopLeft: //
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeForwardDiagonal);
            case TopRightToBottomLeft: //
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
            case BottomLeftToTopRight: //
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
            case ToCenterHorizontal:
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeCenterHorizontal);
            case FromCenterHorizontal:
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeCenterHorizontal);
            case ToCenterVertical:
                    return ::linearGradientPattern(rc, color, color2, ChartPainter::ChartLinearGradientModeCenterVertical);
            case FromCenterVertical:
                    return ::linearGradientPattern(rc, color2, color, ChartPainter::ChartLinearGradientModeCenterVertical);
            default:
                    Q_ASSERT(false);
                break;
        }
    }
    return QBrush();
}

/*
ChartFillDrawHelper::HatchStyle ChartFillDrawHelper::getHatchStyle() const 
{
    QTN_D(const ChartFillDrawHelper);
    return d.m_hatchStyle;
}
*/
void ChartFillDrawHelper::extractXML(const QDomElement& el)
{
    QTN_D(ChartFillDrawHelper);
    if (!el.isNull())
    {
        QString styleBackground = el.attribute("style");
        if (!styleBackground.isEmpty())
        {
            if (styleBackground == "NoBrush")
                d.m_fillStyle = Qt::NoBrush;
            else if (styleBackground == "SolidPattern")
                d.m_fillStyle = Qt::SolidPattern;
            else if (styleBackground == "LinearGradientPattern")
                d.m_fillStyle = Qt::LinearGradientPattern;
            else 
                Q_ASSERT(false);
        }

        QString strGradientDirection = el.attribute("gradientDirection");
        if (!strGradientDirection.isEmpty())
        {
            int count = (sizeof(lpszGradientDirection) / sizeof(lpszGradientDirection[0]));
            for (int i = 0; i < count; i++)
            {
                if (strGradientDirection == lpszGradientDirection[i])
                {
                    d.m_gradientDirection = (GradientDirection)i;
                    break;
                }
            }
        }

        QDomElement elBackgroundColor = el.firstChildElement("Color");
        while(!elBackgroundColor.isNull())
        {
            ChartFillDrawHelperPrivate::ColorInfo info;
            info.color = QColor(elBackgroundColor.text());
            QString strAlpha = elBackgroundColor.attribute("alpha");
            if (!strAlpha.isEmpty())
            {
                int alpha = strAlpha.toInt();
                info.color.setAlpha(alpha);
            }
            info.position = elBackgroundColor.attribute("position").toDouble();
            d.m_colors.append(info);
            elBackgroundColor = elBackgroundColor.nextSiblingElement();
        }
    }
}

void ChartFillDrawHelper::resetAllElements()
{
    QTN_D(ChartFillDrawHelper);
    d.m_colors.clear();
    d.m_fillStyle = Qt::NoBrush;
//    d.m_hatchStyle = ChartFillDrawHelper::HatchStyleSolidDiamond;
    d.m_gradientDirection = ChartFillDrawHelper::LeftToRight;
}

void ChartFillDrawHelper::drawGradientRect(QPainter* p, const QRectF& boundingRect, int xround, int yround)
{
    QTN_D(ChartFillDrawHelper);
    if (d.m_colors.count() > 0)
    {
        if (d.m_colors.count() == 1)
            drawGradientRect(p, boundingRect, d.m_colors.at(0).color, d.m_colors.at(0).color, xround, yround);
        else if (d.m_colors.count() > 1)
            drawGradientRect(p, boundingRect, d.m_colors.at(0).color, d.m_colors.at(1).color, xround, yround);
    }
/*
    QLinearGradient gradient = srcGradient;
    gradient.setStart(bounds.topLeft());
    gradient.setFinalStop(bounds.topRight());

    QBrush brushSave = p->brush();
    QPen penSave = p->pen();
    p->setBrush(gradient);
    p->setPen(Qt::transparent);
    p->drawRoundedRect(bounds, xround, yround, Qt::AbsoluteSize);
    p->setPen(penSave);
    p->setBrush(brushSave);
*/
}

void ChartFillDrawHelper::drawGradientRect(QPainter* p, const QRectF& bounds, const QColor& color, 
                                           const QColor& color2, int xround, int yround)
{
    QTN_D(ChartFillDrawHelper);

    if (d.m_fillStyle == Qt::SolidPattern)
        ChartPainter::painter()->drawFillRect(p, bounds, color, xround, yround);

    if (d.m_fillStyle == Qt::LinearGradientPattern)
    {
        switch(d.m_gradientDirection)
        {
            case TopToBottom: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeVertical, xround, yround);
                break;
            case BottomToTop: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeVertical, xround, yround);
                break;
            case LeftToRight: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeHorizontal, xround, yround);
                break;
            case RightToLeft: // 
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeHorizontal, xround, yround);
                break;
            case TopLeftToBottomRight: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeForwardDiagonal, xround, yround);
                break;
            case BottomRightToTopLeft: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeForwardDiagonal, xround, yround);
                break;
            case TopRightToBottomLeft: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeBackwardDiagonal, xround, yround);
                break;
            case BottomLeftToTopRight: //
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeBackwardDiagonal, xround, yround);
                break;
            case ToCenterHorizontal:
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeCenterHorizontal, xround, yround);
                break;
            case FromCenterHorizontal:
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeCenterHorizontal, xround, yround);
                break;
            case ToCenterVertical:
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color, color2, 
                        ChartPainter::ChartLinearGradientModeCenterVertical, xround, yround);
                break;
            case FromCenterVertical:
                    ChartPainter::painter()->drawGradientRectangle(p, bounds, color2, color, 
                        ChartPainter::ChartLinearGradientModeCenterVertical, xround, yround);
                break;
            default:
                break;
        }
    }
}

void ChartFillDrawHelper::drawGradientCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, const QColor& color2)
{
    QTN_D(ChartFillDrawHelper);
/*
    if (m_fillStyle == ChartFillSolid)
    {
        ChartSolidCircle(center, radius, color);
    }

    if (m_fillStyle == ChartFillHatch)
    {
        ChartHatchCircle(center, radius, color, color2, m_hatchStyle);
    }
*/
    if (d.m_fillStyle == Qt::LinearGradientPattern)
    {
        switch(d.m_gradientDirection)
        {

            case TopToBottom:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeVertical);
                break;

            case BottomToTop:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeVertical);
                break;

            case LeftToRight:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeHorizontal);
                break;

            case RightToLeft:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeHorizontal);
                break;

            case TopLeftToBottomRight:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeForwardDiagonal);
                break;

            case BottomRightToTopLeft:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeForwardDiagonal);
                break;

            case TopRightToBottomLeft:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
                break;

            case BottomLeftToTopRight:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
                break;

            case ToCenterHorizontal:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeCenterHorizontal);
                break;

            case FromCenterHorizontal:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeCenterHorizontal);
                break;

            case ToCenterVertical:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color, color2, ChartPainter::ChartLinearGradientModeCenterVertical);
                break;

            case FromCenterVertical:
                    ChartPainter::painter()->drawGradientCircle(p, center, radius, color2, color, ChartPainter::ChartLinearGradientModeCenterVertical);
                break;
            default:
                break;
        }
    }
}

void ChartFillDrawHelper::drawFillSplinePolygon(QPainter* p, const QVector<QPointF>& arrPoints, const QColor& color, const QColor& /*color2*/, bool twoSides)
{
    ChartPainter::painter()->drawFillSplinePolygon(p, arrPoints, color, twoSides);
}

void ChartFillDrawHelper::drawGradientPolygon(QPainter* p, const QVector<QPointF>& arrPoints, const QColor& color, const QColor& color2)
{
    QTN_D(ChartFillDrawHelper);

    if (d.m_fillStyle == Qt::SolidPattern)
        ChartPainter::painter()->drawFillPolygon(p, arrPoints, color);

//    if (m_fillStyle == ChartFillHatch)
//        ChartPainter::painter()->drawHatchPolygon(p, arrPoints, color, color2, m_hatchStyle);

    if (d.m_fillStyle == Qt::LinearGradientPattern)
    {
        switch(d.m_gradientDirection)
        {
            case TopToBottom:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeVertical);
                break;

            case BottomToTop:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeVertical);
                break;
            case LeftToRight:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeHorizontal);
                break;
            case RightToLeft:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeHorizontal);
                break;
            case TopLeftToBottomRight:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeForwardDiagonal);
                break;
            case BottomRightToTopLeft:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeForwardDiagonal);
                break;

            case TopRightToBottomLeft:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
                break;
            case BottomLeftToTopRight:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeBackwardDiagonal);
                break;
            case ToCenterHorizontal:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeCenterHorizontal);
                break;
            case FromCenterHorizontal:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeCenterHorizontal);
                break;
            case ToCenterVertical:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color, color2, ChartPainter::ChartLinearGradientModeCenterVertical);
                break;
            case FromCenterVertical:
                    ChartPainter::painter()->drawGradientPolygon(p, arrPoints, color2, color, ChartPainter::ChartLinearGradientModeCenterVertical);
                break;
            default:
                break;
        }
    }
}

void ChartFillDrawHelper::setFillStyle(Qt::BrushStyle fillMode)
{
    QTN_D(ChartFillDrawHelper);
    if (d.m_fillStyle != fillMode)
    {
        d.m_fillStyle = fillMode;
        if (ChartComponent* comp = d.getComponentParent())
            comp->chartUpdate();
    }
}

void ChartFillDrawHelper::setGradientDirection(GradientDirection direction)
{
    QTN_D(ChartFillDrawHelper);
    d.m_gradientDirection = direction;
    d.getComponentParent()->chartUpdate();
}

ChartFillDrawHelper* ChartFillDrawHelper::createRotatedHelper()
{
    QTN_D(ChartFillDrawHelper);
    if (d.m_fillStyle != Qt::LinearGradientPattern)
    {
        Q_ASSERT(false); 
        ChartFillDrawHelper* style = this;
        return style;
    }

    ChartFillDrawHelper* style = new ChartFillDrawHelper(d.getComponentParent());
    style->setFillStyle(Qt::LinearGradientPattern);

    switch(d.m_gradientDirection)
    {
        case TopToBottom: 
                style->setGradientDirection(RightToLeft);
            break;
        case BottomToTop: 
                style->setGradientDirection(LeftToRight);
            break;
        case LeftToRight: 
                style->setGradientDirection(TopToBottom);
            break;
        case RightToLeft: 
                style->setGradientDirection(BottomToTop);
            break;
        case ToCenterHorizontal: 
                style->setGradientDirection(ToCenterVertical);
            break;
        case FromCenterHorizontal: 
                style->setGradientDirection(FromCenterVertical);
            break;
        case ToCenterVertical: 
                style->setGradientDirection(ToCenterHorizontal);
            break;
        case FromCenterVertical: 
                style->setGradientDirection(FromCenterHorizontal);
            break;
        default:
            style->setGradientDirection(d.m_gradientDirection);
            break;
    }
    return style;
}


namespace Qtitan
{
    /* ChartLineDrawHelperPrivate */   
    class ChartLineDrawHelperPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartLineDrawHelper)
    public:
        explicit ChartLineDrawHelperPrivate();
        virtual ~ChartLineDrawHelperPrivate();

    public:
        void init();
        ChartComponent* getComponentParent();

    public:
        int m_thickness;
        DashStyle dashStyle_;
    };
}; //namespace Qtitan


/*!
    \class Qtitan::ChartLineDrawHelperPrivate
    \inmodule QtitanChart
    \internal
*/
ChartLineDrawHelperPrivate::ChartLineDrawHelperPrivate()
{
}

ChartLineDrawHelperPrivate::~ChartLineDrawHelperPrivate()
{
}

void ChartLineDrawHelperPrivate::init()
{
    dashStyle_ = DashStyleSolid;
    m_thickness = 1;
}

ChartComponent* ChartLineDrawHelperPrivate::getComponentParent()
{
    QTN_P(ChartLineDrawHelper);

    ChartComponent* parent = qobject_cast<ChartComponent*>(p.parent());
    return parent;
}


/*!
    \class Qtitan::ChartLineDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartLineDrawHelper::ChartLineDrawHelper(QObject* parent)
    : QObject(parent)
{
    QTN_INIT_PRIVATE(ChartLineDrawHelper);
    QTN_D(ChartLineDrawHelper);
    d.init();
}

ChartLineDrawHelper::~ChartLineDrawHelper()
{
    QTN_FINI_PRIVATE();
}

int ChartLineDrawHelper::thickness() const 
{
    QTN_D(const ChartLineDrawHelper);
    return d.m_thickness;
}

DashStyle ChartLineDrawHelper::dashStyle() const 
{
    QTN_D(const ChartLineDrawHelper);
    return d.dashStyle_;
}

void ChartLineDrawHelper::setDashStyle(DashStyle dashStyle)
{
    QTN_D(ChartLineDrawHelper);
    d.dashStyle_ = dashStyle;
    d.getComponentParent()->chartUpdate();
}

void ChartLineDrawHelper::setThickness(int thickness)
{
    QTN_D(ChartLineDrawHelper);
    d.m_thickness = thickness;
    d.getComponentParent()->chartUpdate();
}

void ChartLineDrawHelper::draw(QPainter* p, const QPointF& point1, const QPointF& point2, const QColor& color)
{
    QTN_D(ChartLineDrawHelper);
    if (d.dashStyle_ == DashStyleEmpty)
        return;

    if (d.dashStyle_ == DashStyleSolid)
    {
        ChartPainter::painter()->drawFillLine(p, point1, point2, color, d.m_thickness);
        return;
    }
    ChartPainter::painter()->drawDashedLine(p, point1, point2, color, d.m_thickness, d.dashStyle_);
}


void ChartLineDrawHelper::draw(QPainter* p, const QVector<QPointF>& points, const QColor& color)
{
    QTN_D(ChartLineDrawHelper);
    if (d.dashStyle_ == DashStyleEmpty)
        return;

    ChartPainter::painter()->drawFillPolyline(p, points, color, d.m_thickness);
}


void ChartLineDrawHelper::drawSpline(QPainter* p, const QVector<QPointF>& points, const QColor& color)
{
    QTN_D(ChartLineDrawHelper);
    if (d.dashStyle_ == DashStyleEmpty)
        return;

    if (d.dashStyle_ == DashStyleSolid)
    {
        ChartPainter::painter()->drawFillSpline(p, points, color, d.m_thickness);
        return;
    }
    ChartPainter::painter()->drawDashedSpline(p, points, color, d.m_thickness, d.dashStyle_);
}


namespace Qtitan
{
    /* ChartPieDrawHelper */
    class ChartPieDrawHelper
    {
    public:
        ChartPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, 
            qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent);
    protected:
        QPainterPath* createPieGraphicsPath(const QPointF& center, qreal majorSemiaxis, qreal minorSemiaxis, 
            qreal holePercent, qreal beginAngle, qreal sweepAngle) const;
        void calculateStartFinishPoints(const QPointF& center, qreal majorSemiaxis, qreal minorSemiaxis, 
            qreal beginAngle, qreal sweepAngle, QPointF& startPoint, QPointF& finishPoint) const;

    protected:
        QPointF m_center;
        qreal m_majorSemiAxis;
        qreal m_minorSemiAxis;
        qreal m_beginAngle;
        qreal m_sweepAngle;
        qreal m_depth;
        qreal m_holePercent;

        qreal m_innerMajorSemiAxis;
        qreal m_innerMinorSemiAxis;

        qreal m_facetSize;
        qreal m_radius;
        qreal m_correctedDepth;
        qreal m_innerRadius;
        bool m_bDoughnut;

    };

    /* ChartGradientPieDrawHelper */
    class ChartGradientPieDrawHelper : public ChartPieDrawHelper
    {
    public:
        ChartGradientPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, qreal sweepAngle, 
            qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2);
    public:
        virtual void drawPie(QPainter* p);
    protected:
        QBrush* createBrush();
    protected:
        QRectF m_rcGradientBounds;
        QColor m_color;
        QColor m_color2;
    };

    /* ChartGradientTorusDrawHelper */
    class ChartGradientTorusDrawHelper : public ChartGradientPieDrawHelper
    {
    public:
        ChartGradientTorusDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, qreal sweepAngle,
            qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2);
    };

    /* ChartBoundedPieDrawHelper */
    class ChartBoundedPieDrawHelper : public ChartPieDrawHelper
    {
    public:
        ChartBoundedPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, 
            qreal sweepAngle, qreal depth, int holePercent, const QColor& color, int width);
    public:
        virtual void drawPie(QPainter* p);

    protected:
        QColor m_color;
        int m_width;
    };
};


/*!
    \class Qtitan::ChartPieDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartPieDrawHelper::ChartPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent)
{
    m_center = center;
    m_majorSemiAxis = majorSemiAxis;
    m_minorSemiAxis = minorSemiAxis;
    m_beginAngle = beginAngle;
    m_sweepAngle = sweepAngle;
    m_depth = depth;
    m_holePercent = holePercent / 100.0;

    m_facetSize = 0;

    m_innerMajorSemiAxis = 0;
    m_innerMinorSemiAxis = 0;

    m_correctedDepth = 0;

    m_innerRadius = 0;

    m_bDoughnut = holePercent > 0;

    if (m_bDoughnut)
    {
        qreal doughnutThickness = minorSemiAxis * (1.0 - m_holePercent);
        m_innerMajorSemiAxis = majorSemiAxis - doughnutThickness;
        m_innerMinorSemiAxis = minorSemiAxis - doughnutThickness;
    }

    if (depth > 0.0f)
    {
        m_facetSize = majorSemiAxis * 0.02f;
        m_facetSize = 0;

        if (m_facetSize > depth / 2.0f)
            m_facetSize = depth / 2.0f;
        m_radius = majorSemiAxis - m_facetSize;
        qreal diff = m_radius - m_innerMajorSemiAxis;
        if (diff < 0.0f) {
            m_radius -= diff;
            m_facetSize += diff;
        }

        m_correctedDepth = depth - m_facetSize * 2;

        if (m_bDoughnut) {
            m_innerRadius = m_innerMajorSemiAxis;
            if (m_innerRadius > m_radius)
                m_innerRadius = m_radius;
        }

    }
}

/*!
    \class Qtitan::ChartGradientPieDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartGradientPieDrawHelper::ChartGradientPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis,
    qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2)
    : ChartPieDrawHelper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent)
{
    m_rcGradientBounds = gradientBounds;
    m_color = color;
    m_color2 = color2;
}

void ChartPieDrawHelper::calculateStartFinishPoints(const QPointF& center, qreal majorSemiaxis, qreal minorSemiaxis, qreal beginAngle, qreal sweepAngle, QPointF& startPoint, QPointF& finishPoint) const
{
    ChartEllipse ellipse(center, majorSemiaxis, minorSemiaxis);
    startPoint = (QPointF)ellipse.calcEllipsePoint(-ChartMath::degree2Radian(beginAngle));
    finishPoint = (QPointF)ellipse.calcEllipsePoint(-ChartMath::degree2Radian(beginAngle + sweepAngle));
}

QPainterPath* ChartPieDrawHelper::createPieGraphicsPath(const QPointF& center, qreal majorSemiaxis, qreal minorSemiaxis, qreal holePercent, qreal beginAngle, qreal sweepAngle)  const
{
    QPainterPath* path = new QPainterPath();

    QRectF rect(QPointF((center.x() - majorSemiaxis), (center.y() - minorSemiaxis)), QPointF((center.x() + majorSemiaxis), (center.y() + minorSemiaxis)));
    rect = rect.normalized();

    bool shouldAddLines = ChartMath::fuzzyCmp(sweepAngle, -360.0) != 0;
    qreal innerMajorSemiaxis = majorSemiaxis * holePercent;
    qreal innerMinorSemiaxis = minorSemiaxis * holePercent;

    if (innerMajorSemiaxis >= 1.0f && innerMinorSemiaxis >= 1.0f)
    {
        QRectF innerRect(QPointF((center.x() - innerMajorSemiaxis),  (center.y() - innerMinorSemiaxis)), 
            QPointF((center.x() + innerMajorSemiaxis), (center.y() + innerMinorSemiaxis)));

        QPointF startPoint, finishPoint, innerStartPoint, innerFinishPoint;

        if (shouldAddLines)
        {
            calculateStartFinishPoints(center, majorSemiaxis, minorSemiaxis, beginAngle, sweepAngle, startPoint, finishPoint);
            calculateStartFinishPoints(center, innerMajorSemiaxis, innerMinorSemiaxis, beginAngle, sweepAngle, innerStartPoint, innerFinishPoint);
        }

        if (shouldAddLines)
        {
            path->moveTo(innerStartPoint);
            path->lineTo(startPoint);
        }

//        path->arcMoveTo(rect, -beginAngle);
        path->arcTo(rect, -beginAngle, -sweepAngle);

        innerRect = innerRect.normalized();
        path->arcTo(innerRect, -(beginAngle + sweepAngle), sweepAngle);

        if (shouldAddLines)
        {
            path->moveTo(finishPoint);
            path->lineTo(innerFinishPoint);
        }

        path->closeSubpath();
    }
    else if (shouldAddLines)
    {
        QPointF point = rect.center();
        path->moveTo(point);
        path->arcTo(rect.left(), rect.top(), rect.width(), rect.height(), -beginAngle, -sweepAngle);
        path->closeSubpath();
    }
    else
    {
        path->addEllipse(rect.left(), rect.top(), rect.width(), rect.height());
    }
    return path;
}

QBrush* ChartGradientPieDrawHelper::createBrush()
{
    QBrush* brush = Q_NULL;

//    QLinearGradient gradient = QLinearGradient(QPointF(m_rcGradientBounds.x(), m_center.y()), 
//        QPointF((m_rcGradientBounds.x() + m_rcGradientBounds.width()), m_center.y()));
    QRadialGradient gradient(m_center.x(),
        m_center.y(),
        qMax(m_rcGradientBounds.width(), m_rcGradientBounds.height()),
        m_center.x(), m_center.y());

    gradient.setColorAt(0.0, m_color.lighter());
    gradient.setColorAt(1.0, m_color2.darker());

//    gradient.setColorAt(1.0, m_color);
//    gradient.setColorAt(0.0, m_color2);

    brush = new QBrush(gradient);

    return brush;
}

void ChartGradientPieDrawHelper::drawPie(QPainter* painter)
{
    QBrush saveBrush = painter->brush();

    QBrush* brush = createBrush();
    painter->setBrush(*brush);


    QPainterPath* path = createPieGraphicsPath(m_center, m_majorSemiAxis, m_minorSemiAxis, m_holePercent, m_beginAngle, m_sweepAngle);
    painter->fillPath(*path, *brush);
    delete path;
/*
    int inner = (6*m_majorSemiAxis)/100; 
    QPainterPath* pathInternal = createPieGraphicsPath(m_center, m_majorSemiAxis - inner, m_minorSemiAxis - inner, m_holePercent, m_beginAngle, m_sweepAngle);
    painter->fillPath(*pathInternal, m_color);
    delete pathInternal;
*/
    delete brush;
    painter->setBrush(saveBrush);
}


/*!
    \class Qtitan::ChartGradientTorusDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartGradientTorusDrawHelper::ChartGradientTorusDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2)
    : ChartGradientPieDrawHelper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent, gradientBounds, color, color2)
{
}

/*!
    \class Qtitan::ChartBoundedPieDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartBoundedPieDrawHelper::ChartBoundedPieDrawHelper(const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, 
    qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QColor& color, int width)
    : ChartPieDrawHelper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent)
{
    m_color = color;
    m_width = width;
}

void ChartBoundedPieDrawHelper::drawPie(QPainter* painter)
{
    QPen penSave = painter->pen();
    QColor col = m_color;
    QPen newPen(QBrush(col), (qreal)m_width);

    QPainterPath* path = createPieGraphicsPath(m_center, m_majorSemiAxis, m_minorSemiAxis, m_holePercent, m_beginAngle, m_sweepAngle);
    painter->strokePath(*path, newPen);
    delete path;

    painter->setPen(penSave);
}

namespace Qtitan
{
    /* ChartSplineDrawHelper */
    class ChartSplineDrawHelper
    {
    public:
        ChartSplineDrawHelper(const QVector<QPointF>& points, ChartSpline::SplineType type, bool closed = false);

    public:
        void drawSpline(QPainter* painter);
        void drawFillOneSidedSpline(QPainter* painter, const QColor& color);
        void drawFillDoubleSidedSpline(QPainter* painter, const QColor& color);

    protected:
        ChartSpline spline;
        QVector<QPointF> m_points;
        ChartSpline::SplineType m_type;
        bool m_closed;
    };
};

/*!
    \class Qtitan::ChartSplineDrawHelper
    \inmodule QtitanChart
    \internal
*/
ChartSplineDrawHelper::ChartSplineDrawHelper(const QVector<QPointF>& points, ChartSpline::SplineType type, bool closed)
{
    m_type = type;
    m_closed = closed;
    m_points = points;
}

void ChartSplineDrawHelper::drawSpline(QPainter* painter)
{
    spline.setPoints(m_points, m_type, m_closed);

    const QVector<QPointF>& pnts = spline.points();
    int count = pnts.size();
    if (count > 1)
        count = (count - 1) / 3;

    QPointF startPoint = count > 0 ? QPointF(pnts[0] + QPointF(0.5, 0.5)) : QPointF(0.0, 0.0);
    QPainterPath splinePath(startPoint);

    int index = 1;
    for (int i = 0; i < count; i++)
    {
        splinePath.cubicTo(pnts[index], pnts[index + 1], pnts[index + 2]);
        index += 3;
    }
    painter->drawPath(splinePath);
}

void ChartSplineDrawHelper::drawFillOneSidedSpline(QPainter* painter, const QColor& color)
{
    QVector<QPointF> locPoints(m_points);

    locPoints.remove(0);
    locPoints.remove(locPoints.size() - 1);

    spline.setPoints(locPoints, m_type, m_closed);

    const QVector<QPointF>& pnts = spline.points();
    int count = pnts.size();
    if (count > 1)
        count = (count - 1) / 3;

    QPainterPath splinePath(m_points.at(0));
    splinePath.lineTo(locPoints.at(0));

    int index = 1;
    for (int i = 0; i < count; i++)
    {
        splinePath.cubicTo(pnts[index], pnts[index + 1], pnts[index + 2]);
        index += 3;
    }

    splinePath.lineTo(m_points.at(m_points.size()-1));
    painter->fillPath(splinePath, color);
}

void ChartSplineDrawHelper::drawFillDoubleSidedSpline(QPainter* painter, const QColor& color)
{
    QVector<QPointF> locPoints;
    int count = m_points.count();
    for (int i = 0; count/2 > i; i++)
        locPoints.append(m_points.at(i));

    spline.setPoints(locPoints, m_type, m_closed);

    const QVector<QPointF>& pntsOne = spline.points();
    int countOne = pntsOne.size();
    if (countOne > 1)
        countOne = (countOne - 1) / 3;

    QPointF startPoint = count > 0 ? QPointF(pntsOne[0] + QPointF(0.5, 0.5)) : QPointF(0.0, 0.0);
    QPainterPath splinePath(startPoint);

    int index = 1;
    for (int i = 0; i < countOne; i++)
    {
        splinePath.cubicTo(pntsOne[index], pntsOne[index + 1], pntsOne[index + 2]);
        index += 3;
    }
    splinePath.lineTo(m_points[count/2]);
    splinePath.lineTo(m_points[count/2 + 1]);

    locPoints.clear();
    for (int i = count/2; count > i; i++)
        locPoints.append(m_points.at(i));

    spline.setPoints(locPoints, m_type, m_closed);

    const QVector<QPointF>& pntsTwo = spline.points();
    int countTwo = pntsTwo.size();
    if (countTwo > 1)
        countTwo = (countTwo - 1) / 3;

    index = 1;
    for (int i = 0; i < countTwo; i++)
    {
        splinePath.cubicTo(pntsTwo[index], pntsTwo[index + 1], pntsTwo[index + 2]);
        index += 3;
    }

    splinePath.lineTo(m_points[count-1]);
    splinePath.lineTo(m_points[0]);

    painter->fillPath(splinePath, color);
}


namespace Qtitan
{
    static ChartPainter* m_painter = Q_NULL;

    /* ChartPainterPrivate */   
    class ChartPainterPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartPainter)
    public:
        explicit ChartPainterPrivate();
        virtual ~ChartPainterPrivate();

    public:
        void init();
    public:
        QStack<QTransform> m_stackMatrix;
        QStack<QTransform> m_stackRotateMatrix;
        QStack<QPainter::RenderHints> m_stackAntiAlias;
        QStack<QPainter::RenderHints> m_stackTextAntiAlias;
    };
}; //namespace Qtitan


/*!
    \class Qtitan::ChartPainterPrivate
    \inmodule QtitanChart
    \internal
*/
ChartPainterPrivate::ChartPainterPrivate()
{
}

ChartPainterPrivate::~ChartPainterPrivate()
{
    delete m_painter;
    m_painter = Q_NULL;
}

void ChartPainterPrivate::init()
{
}


ChartPainter* ChartPainter::painter()
{
    if (!m_painter)
        m_painter = new ChartPainter();
    return m_painter;
}

/*!
    \class Qtitan::ChartPainter
    \inmodule QtitanChart
    \internal
*/
ChartPainter::ChartPainter(QObject* parent)
    : QObject(parent)
{
    QTN_INIT_PRIVATE(ChartPainter);
    QTN_D(ChartPainter);
    d.init();
}

ChartPainter::~ChartPainter()
{
    QTN_FINI_PRIVATE();
}

void ChartPainter::setAntialiasing(QPainter* p, bool antiAlias)
{
    QTN_D(ChartPainter);
    QPainter::RenderHints hints = p->renderHints();
    d.m_stackAntiAlias.push(hints);
    p->setRenderHints(hints, false);
    p->setRenderHint(antiAlias ? QPainter::Antialiasing : QPainter::HighQualityAntialiasing);
}

void ChartPainter::unsetAntialiasing(QPainter* p)
{
    QTN_D(ChartPainter);
    p->setRenderHints(p->renderHints(), false);
    p->setRenderHints(d.m_stackAntiAlias.pop(), true);
}

void ChartPainter::setTextAntialiasing(QPainter* p, bool antiAlias)
{
    Q_UNUSED(antiAlias);

    QTN_D(ChartPainter);
    QPainter::RenderHints saveHints = p->renderHints();
    d.m_stackTextAntiAlias.push(saveHints);
    p->setRenderHints(saveHints, false);

    p->setRenderHint(QPainter::TextAntialiasing/*QPainter::HighQualityAntialiasing*/);
}

void ChartPainter::unsetTextAntialiasing(QPainter* p)
{
    QTN_D(ChartPainter);
    p->setRenderHints(p->renderHints(), false);
    p->setRenderHints(d.m_stackTextAntiAlias.pop(), true);
}

void ChartPainter::pushTransform(QPainter* p, qreal dx, qreal dy, qreal dz)
{
    Q_UNUSED(dz);
    QTN_D(ChartPainter);
    d.m_stackMatrix.push(p->worldTransform());
    p->setWorldTransform(QTransform().translate(dx, dy), true);
}

void ChartPainter::popTransform(QPainter* p)
{
    QTN_D(ChartPainter);
    // throw away the old version of the current entry
    QTransform matrix = d.m_stackMatrix.pop();
    p->setWorldTransform(matrix, true);
}

void ChartPainter::pushRotateTransform(QPainter* p, qreal angle)
{
    QTN_D(ChartPainter);
    d.m_stackRotateMatrix.push(p->worldTransform());
    p->setWorldTransform(QTransform().rotate(angle), true);
}

void ChartPainter::popRotateTransform(QPainter* p)
{
    QTN_D(ChartPainter);
    // throw away the old version of the current entry
    QTransform matrix = d.m_stackRotateMatrix.pop();
    p->setWorldTransform(matrix, true);
}

void ChartPainter::drawFillRect(QPainter* p, const QRectF& rect, const QColor& color, int xround, int yround) const
{
    if (xround > 0 && yround > 0)
    {
        QBrush brushSave = p->brush();
        QPen penSave = p->pen();
        p->setBrush(QBrush(color));
        p->setPen(Qt::transparent);
        p->drawRoundedRect(rect, xround, yround, Qt::AbsoluteSize);
        p->setPen(penSave);
        p->setBrush(brushSave);
    }
    else
        p->fillRect(rect, color);
}

void ChartPainter::drawBackground(QPainter* p, const QRectF& rect, const QColor& color) const
{
    p->fillRect(rect, color);
}

void ChartPainter::drawGradientBackground(QPainter* p, const QRectF& rect, const QBrush& brush, int xround, int yround) const
{
    QBrush br;
    if (const QGradient* gradient = brush.gradient())
    {
        if (gradient->type() == QGradient::LinearGradient)
        {
            QLinearGradient gr(rect.bottomLeft(), rect.topLeft());
            gr.setStops(gradient->stops());
            br = QBrush(gr);
        }
    }
    else
    {
        br = brush;

        QPixmap pix = br.texture();
        if (!pix.isNull())
        {
            QTransform tr = br.transform();
            tr = tr.scale(rect.size().width()/pix.size().width(), rect.size().height()/pix.size().height());
            br.setTransform(tr);
        }
    }

    QBrush brushSave = p->brush();
    QPen penSave = p->pen();
    p->setBrush(br);
    p->setPen(Qt::transparent);
//    p->drawRect(rect);
    p->drawRoundedRect(rect, xround, yround, Qt::AbsoluteSize);
    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawBoundedRect(QPainter* p, const QRectF& rect, const QColor& color, int width, int xround, int yround) const
{
    QPen savePen(p->pen());
    QPen newPen(QBrush(color), (qreal)width);
    p->setPen(newPen);

    if (xround > 0 && yround > 0)
        p->drawRoundedRect(rect, (qreal)xround, (qreal)yround, Qt::AbsoluteSize);
    else
        p->drawRect(rect);

    p->setPen(savePen);
}

void ChartPainter::drawInnerBorder(QPainter* p, const QRectF& rect, const QColor& color, int width, int xround, int yround) const
{
    QRectF rc = rect;
    qreal indent = ((qreal)width / 2.0 - 0.5);
    rc.setLeft(rc.left() + indent);
    rc.setTop(rc.top() + indent);

    indent = ((qreal)width - 0.5);
    rc.setWidth(rc.width() - indent);
    rc.setHeight(rc.height() - indent);

    drawBoundedRect(p, rect, color, width, xround, yround);
}

void ChartPainter::drawGradientRectangle(QPainter* p, const QRectF& rect, const QColor& color, 
                                         const QColor& color2, ChartLinearGradientMode mode, int xround, int yround) const
{
    QRectF rc = rect;
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();

    QLinearGradient gradient;
    if (mode == ChartLinearGradientModeVertical)
        gradient = QLinearGradient(rc.topLeft(), rc.bottomLeft());
    else if (mode == ChartLinearGradientModeHorizontal)
        gradient = QLinearGradient(rc.topLeft(), rc.topRight());
    else if (mode == ChartLinearGradientModeForwardDiagonal)
        gradient = QLinearGradient(rc.topLeft(), rc.bottomRight());
    else if (mode == ChartLinearGradientModeBackwardDiagonal)
        gradient = QLinearGradient(rc.topRight(), rc.bottomLeft());
    else
        Q_ASSERT(false);

    gradient.setColorAt(0.0, color);
    gradient.setColorAt(1.0, color2);

    p->setBrush(gradient);
    p->setPen(Qt::transparent);
    if (xround > 0 || yround > 0)
        p->drawRoundedRect(rc, xround, yround, Qt::AbsoluteSize);
    else
        p->drawRect(rc);
    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawDashedSpline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width, DashStyle dashStyle) const
{
    QPen penSave = p->pen();

    Qt::PenStyle s = dashStyle == DashStyleDash ? Qt::DashLine :
                     dashStyle == DashStyleDashDot ? Qt::DashDotLine :
                     dashStyle == DashStyleDot ? Qt::DotLine :
                     dashStyle == DashStyleDashDotDot ? Qt::DashDotDotLine : Qt::SolidLine;

    QPen newPen(QBrush(color), (qreal)width, s);
    p->setPen(newPen);

    p->drawLines(pnts);

    p->setPen(penSave);
}

void ChartPainter::drawFillSpline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width) const
{
    QPen penSave = p->pen();
    QPen newPen(QBrush(color), (qreal)width);
    p->setPen(newPen);
/*
    ChartSpline spline;
    ChartSplineCurveFitter filter;
    filter.setSplineSize((pnts.size() - 1) * 10 + 1);
    filter.setSpline(spline);
    filter.setFitMode(ChartSplineCurveFitter::Spline);
    p->drawPolyline(filter.fitCurve(pnts));
*/
    ChartSplineDrawHelper spline(pnts, ChartSpline::SplineCubic);
    spline.drawSpline(p);
    p->setPen(penSave);
}

void ChartPainter::drawDashedLine(QPainter* p, const QPointF& p1, const QPointF& p2, const QColor& color, int width, DashStyle dashStyle) const
{
    QPen penSave = p->pen();

    Qt::PenStyle s = dashStyle == DashStyleDash ? Qt::DashLine :
        dashStyle == DashStyleDashDot ? Qt::DashDotLine :
        dashStyle == DashStyleDot ? Qt::DotLine :
        dashStyle == DashStyleDashDotDot ? Qt::DashDotDotLine : Qt::SolidLine;

    QPen newPen(QBrush(color), (qreal)width, s, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(newPen);
    p->drawLine(p1, p2);
    p->setPen(penSave);
}

void ChartPainter::drawFillLine(QPainter* p, const QPointF& p1, const QPointF& p2, const QColor& color, qreal width) const
{
    QPen penSave = p->pen();

    QPen newPen(QBrush(color), (qreal)width);
    p->setPen(newPen);
    p->drawLine(p1, p2);
    p->setPen(penSave);
}

void ChartPainter::drawFillPolyline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width) const
{
    QPen penSave = p->pen();
    QPen newPen(QBrush(color), (qreal)width);
    p->setPen(newPen);

    QPolygonF polyline;
    for (int i = 0, count = pnts.count(); count > i; i++)
        polyline << pnts.at(i);

    p->drawPolyline(polyline);
    p->setPen(penSave);
}

void ChartPainter::drawBoundedPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, int width) const
{
    QPen savePen(p->pen());

    QPen newPen(QBrush(color), (qreal)width);
    p->setPen(newPen);

    p->drawPolygon(points);

    p->setPen(savePen);
}

static void calcRectByPoints(const QVector<QPointF>& points, QRectF& rect)
{
    if (points.size() != 0)
    {
        qreal xMax = points[0].x(), xMin = points[0].x();
        qreal yMax = points[0].y(), yMin = points[0].y();

        for (int i = 1; i < points.size(); i++)
        {
            xMax = qMax(xMax, points[i].x());
            yMax = qMax(yMax, points[i].y());
            xMin = qMin(xMin, points[i].x());
            yMin = qMin(yMin, points[i].y());
        }

        rect.setX(xMin);
        rect.setY(yMin);
        rect.setWidth(xMax - xMin);
        rect.setHeight(yMax - yMin);
    }

}

void ChartPainter::drawGradientPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const
{
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();

    QLinearGradient gradient;
    if (mode == ChartLinearGradientModeCenterHorizontal)
    {
    }
    else if (mode == ChartLinearGradientModeCenterVertical)
    {
    }
    else
    {
        QRectF rect;
        ::calcRectByPoints(points, rect);
        //        gradient = QRadialGradient(m_center, m_radius);
        gradient = QLinearGradient(rect.topLeft(), rect.topRight());
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, color2);
    }
    p->setBrush(gradient);
    p->setPen(Qt::transparent);

    p->drawPolygon(points);

    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawFillSplinePolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, bool twoSides) const
{
    ChartSplineDrawHelper splineDraw(points, ChartSpline::SplineCubic);
    if (!twoSides)
        splineDraw.drawFillOneSidedSpline(p, color);
    else
        splineDraw.drawFillDoubleSidedSpline(p, color);
}

void ChartPainter::drawFillPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color) const
{
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();
    p->setPen(Qt::NoPen);
    p->setBrush(color);

    p->drawPolygon(points);

    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawFillPolygon(QPainter* p, const QVector<QPointF>& points, const QBrush& brFill) const
{
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();
    p->setPen(Qt::NoPen);
    p->setBrush(brFill);

    p->drawPolygon(points);

    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawBoundedCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, int width) const
{
    drawBoundedEllipse(p, center, radius, radius, color, width);
}

void ChartPainter::drawBoundedEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QColor& color, int width) const
{
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();

    p->setPen(QPen(QBrush(color), width));
    p->drawEllipse(center, radiusX, radiusY);
    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawGradientCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const
{
    drawGradientEllipse(p, center, radius, radius, color, color2, mode);
}

void ChartPainter::drawGradientEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const
{
    QRectF rect((center.x() - radiusX), (center.x() - radiusY), (radiusX * 2), (radiusY * 2));
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();

    QRadialGradient gradient;
    if (mode == ChartLinearGradientModeCenterHorizontal)
    {
        Q_ASSERT(false);
    }
    else if (mode == ChartLinearGradientModeCenterVertical)
    {
        Q_ASSERT(false);
    }
    else
    {
        gradient = QRadialGradient(center, radiusX);
        gradient.setColorAt(0.0, color);
        gradient.setColorAt(1.0, color2);
    }
    p->setBrush(gradient);
    p->setPen(Qt::transparent);

    p->drawEllipse(center, radiusX, radiusY);

    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawGradientEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QBrush& br) const
{
    QRectF rect((center.x() - radiusX), (center.x() - radiusY), (radiusX * 2), (radiusY * 2));
    QBrush brushSave = p->brush();
    QPen penSave = p->pen();

    p->setBrush(br);
    p->setPen(Qt::transparent);

    p->drawEllipse(center, radiusX, radiusY);

    p->setPen(penSave);
    p->setBrush(brushSave);
}

void ChartPainter::drawBoundedPie(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, qreal sweepAngle, 
    qreal depth, int holePercent, const QColor& color, int width) const
{
    ChartBoundedPieDrawHelper helper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent, color, width);
    helper.drawPie(p);
}

void ChartPainter::drawGradientPie(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, 
    qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2) const
{
    ChartGradientPieDrawHelper helper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent, gradientBounds, color, color2);
    helper.drawPie(p);
}

void ChartPainter::drawGradientTorus(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, qreal beginAngle, 
    qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2) const
{
    ChartGradientTorusDrawHelper helper(center, majorSemiAxis, minorSemiAxis, beginAngle, sweepAngle, depth, holePercent, gradientBounds, color, color2);
    helper.drawPie(p);
}

void ChartPainter::drawBoundedText(QPainter* p, const QString& strText, const QFont& font, const QColor& color, const QRectF& rect) const
{
    QPen savePen = p->pen();
    QFont saveFont(p->font());
    p->setFont(font);
    p->setPen(QPen(color));

    int flags = Qt::AlignCenter;
    p->drawText(rect, flags, strText);

    p->setFont(saveFont);
    p->setPen(savePen);
}

void ChartPainter::drawFunnel(QPainter* p, const QRectF& rectFunnel, qreal neckHeight, qreal neckWidth, const QBrush& brFill, const QColor& brLine, qreal y1, qreal y2) const
{
    drawFunnel3D(p, rectFunnel, 0.0, neckHeight, neckWidth, brFill, brLine, y1, y2);
}

void ChartPainter::drawFunnel3D(QPainter* p, const QRectF& rectFunnel, qreal depth, qreal neckHeight, qreal neckWidth, const QBrush& brFill, const QColor& brLine,
    qreal y1, qreal y2, bool /*isCircularBase*/, const QBrush& brTopFill) const
{
    if (rectFunnel.isEmpty())
        return;

    ChartContour contourSide;
    QtnEllipse ellipseTop;

    QRectF rect = rectFunnel;

    neckWidth = qMin(neckWidth, rect.width());
    neckHeight = qMin(neckHeight, rect.height());

    qreal radiusY1 = 5.0;
    qreal radiusY2 = 5.0;

    if (depth >= 1.)
    {
        qreal depth2 = depth * 0.5;
        qreal height = rect.height();

        rect.adjust(0.0, depth2, 0.0, -depth2);

        qreal ratio = rect.height() / height;
        neckHeight *= ratio;
        y1 *= ratio;
        y2 *= ratio;

        height = rect.height();

        radiusY1 = qMax(radiusY1, depth2 * y1 / height);
        radiusY2 = qMax(radiusY2, depth2 * y2 / height);
    }

    y1 += rect.top();
    y2 += rect.top();

    qreal xCenter = rect.center().x();

    rect.setBottom(rect.bottom() - neckHeight);

    if (y1 >= rect.bottom())
    {
        qreal xRadius = neckWidth * 0.5;
        if (depth < 1.0)
        {
            contourSide.setStart(QPointF(xCenter - xRadius, y1));
            contourSide.appendLine(QPointF(xCenter + xRadius, y1));
            contourSide.appendLine(QPointF(xCenter + xRadius, y2));
            contourSide.appendLine(QPointF(xCenter - xRadius, y2));
        }
        else
        {
            QSizeF szRadiusTop(xRadius, radiusY1);
            QSizeF szRadiusBottom(xRadius, radiusY2);

            qreal delta = /*(m_Type == MANAGER_GDI)*/false ? 0.0 : 2.0;

            contourSide.setStart(QPointF(xCenter - xRadius, y1 + delta));
            contourSide.appendArc(QPointF(xCenter + xRadius, y1), szRadiusTop, false);
            contourSide.appendLine(QPointF(xCenter + xRadius, y2));
            contourSide.appendArc(QPointF(xCenter - xRadius, y2), szRadiusBottom, true);

            ellipseTop.point = QPointF(xCenter, y1 + 1);
            ellipseTop.radiusX = szRadiusTop.width();
            ellipseTop.radiusY = szRadiusTop.height();
        }
    }
    else
    {
        qreal neckWidth2 = neckWidth * 0.5;
        qreal angle = ChartMath::angle(rect.width()  * 0.5 - neckWidth2, rect.height());

        qreal yBottom = y2 > rect.bottom() ? rect.bottom() : y2;

        qreal dy = (yBottom - y1);
        qreal dx = dy / qTan(angle);
        qreal dyBottom = y2 > rect.bottom() ? 0 : rect.bottom() - y2;
        qreal dxBottom = neckWidth2 + (dx * dyBottom / dy);
        qreal dxTop    = dxBottom + dx;

        QPointF pointTopLeft(xCenter + dxTop, y1);
        QPointF pointTopRight(xCenter - dxTop, y1);

        QPointF pointBottomLeft(xCenter + dxBottom, yBottom);
        QPointF pointBottomRight(xCenter - dxBottom, yBottom);

        if (y2 > rect.bottom())
        {
            QPointF pointBottomLeftNeck(xCenter + dxBottom, y2);
            QPointF pointBottomRightNeck(xCenter - dxBottom, y2);

            if (depth < 1.)
            {
                contourSide.setStart(pointTopLeft);
                contourSide.appendLine(pointBottomLeft);
                contourSide.appendLine(pointBottomLeftNeck);
                contourSide.appendLine(pointBottomRightNeck);
                contourSide.appendLine(pointBottomRight);
                contourSide.appendLine(pointTopRight);
            }
            else
            {
                contourSide.setStart(pointBottomLeftNeck);
                contourSide.appendArc(pointBottomRightNeck, QSizeF(qFabs(xCenter - pointBottomLeft.x()), radiusY2), true);
                contourSide.appendLine(pointBottomRight);
                contourSide.appendLine(pointTopRight);
                contourSide.appendArc(pointTopLeft, QSizeF(qFabs(xCenter - pointTopLeft.x()), radiusY1), false);
                contourSide.appendLine(pointBottomLeft);

                ellipseTop.point = QPointF(xCenter, pointTopLeft.y() + 1);
                ellipseTop.radiusX = qFabs(xCenter - pointTopLeft.x());
                ellipseTop.radiusY = radiusY1;
            }
        }
        else
        {
            if (depth < 1.0)
            {
                contourSide.setStart(pointTopLeft);
                contourSide.appendLine(pointBottomLeft);
                contourSide.appendLine(pointBottomRight);
                contourSide.appendLine(pointTopRight);
            }
            else
            {
                contourSide.setStart(pointBottomLeft);
                contourSide.appendArc(pointBottomRight, QSizeF(qFabs(xCenter - pointBottomLeft.x()), radiusY2), true);
                contourSide.appendLine(pointTopRight);
                contourSide.appendArc(pointTopLeft, QSizeF(qFabs(xCenter - pointTopLeft.x()), radiusY1), false);

                ellipseTop.point = QPointF(xCenter, pointTopLeft.y() + 1);
                ellipseTop.radiusX = qFabs(xCenter - pointTopLeft.x());
                ellipseTop.radiusY = radiusY1;
            }
        }
    }

    drawFillContour(p, &contourSide, brFill);
    drawContour(p, &contourSide, brLine);

    if (!ellipseTop.isNull())
    {
        QBrush br = !brTopFill.color().isValid() ? brFill : brTopFill;
        drawGradientEllipse(p, ellipseTop.point, ellipseTop.radiusX, ellipseTop.radiusY, br);
        drawBoundedEllipse(p, ellipseTop.point, ellipseTop.radiusX, ellipseTop.radiusY, brLine, 1.0);
    }
}

void ChartPainter::drawPyramid(QPainter* p, const QRectF& rect, qreal y1, qreal y2, const QBrush& brushfill, const QColor& lineColor, bool isTransparent) const
{
    drawPyramid3D(p, rect, 0.0, brushfill, /*QBrush()*/brushfill, lineColor, 0.0, y1, y2, isTransparent);
}

void ChartPainter::drawPyramid3D(QPainter* p, const QRectF& rectPyramid, qreal depth,
    const QBrush& brSideFillLeft, const QBrush& brSideFillRight, const QColor& brLine,
    qreal xCenterOffset, qreal y1, qreal y2, bool isTransparent, bool isCircularBase, const QBrush& brTopFill) const
{
    if (rectPyramid.isEmpty())
        return;

    isTransparent = isTransparent && depth > 0.0;

    QVector<QPointF> pointsLeft;
    QVector<QPointF> pointsRight;
    QVector<QPointF> pointsTop;
    QVector<QPointF> pontsHidden;

    ChartContour contourSide;
    QtnEllipse ellipseTop;
    QtnEllipse ellipseBottom;

    depth = qMin(depth, rectPyramid.height() * 0.5 - 1);
    depth = qMin(depth, rectPyramid.width() * 0.5 - 1);

    if (depth <= 4.)
        depth = 0.;

    QRectF rect = rectPyramid;
    rect.adjust(depth, 0.0, -depth, -depth);

    qreal maxXOffset = rect.width() * 0.5;

    if (qFabs(xCenterOffset) > maxXOffset)
    {
        if (xCenterOffset < 0.0)
            xCenterOffset = -maxXOffset; 
        else
            xCenterOffset = maxXOffset;
    }

    qreal xCenter = rect.center().x();

    if (xCenterOffset != 0.0 && !isCircularBase)
    {
        qreal halfWidth = rect.width() * 0.5;
        depth *= (halfWidth - qFabs(xCenterOffset)) / halfWidth;
    }

    if (y1 <= 0.0)
    {
        qreal bottom = y2 < 0. ? rect.bottom() : rect.top() + y2;

        qreal ratio = (rect.bottom() - bottom) / rect.height();

        depth *= (1. - ratio);
        xCenterOffset *= (1. - ratio);

        qreal dx = 0.5 * ratio * rect.width();

        QPointF pointTop(xCenter, rect.top());
        QPointF pointBottom(xCenter + xCenterOffset, bottom);

        QPointF pointBottomLeft(rect.left() + dx, bottom - 0.5 * depth);
        QPointF pointBottomRight(rect.right() - dx, bottom - 0.5 * depth);

        if (isCircularBase)
        {
            contourSide.setStart(pointTop);
            contourSide.appendLine(pointBottomLeft);

            if (depth > 0.)
                contourSide.appendArc(pointBottomRight, QSizeF(qFabs(xCenter - pointBottomLeft.x()), depth * 0.5), false);
            else
                contourSide.appendLine(pointBottomRight);

            if (isTransparent)
            {
                ellipseBottom.point = QPointF(xCenter, pointBottomLeft.y() + 0.5);
                ellipseBottom.radiusX = qFabs(xCenter - pointBottomLeft.x()) - 0.5;
                ellipseBottom.radiusY = depth * 0.5 - 0.5;
            }
        }
        else if (!brSideFillRight.color().isValid())
        {
            pointsLeft.append(pointTop);
            pointsLeft.append(pointBottomLeft);
            pointsLeft.append(pointBottomRight);
        }
        else
        {
            pointsLeft.append(pointTop);
            pointsLeft.append(pointBottomLeft);
            pointsLeft.append(pointBottom);

            pointsRight.append(pointTop);
            pointsRight.append(pointBottomRight);
            pointsRight.append(pointBottom);
        }

        if (isTransparent && !isCircularBase)
        {
            pontsHidden.append(pointBottomLeft);
            pontsHidden.append(QPointF(pointTop.x(), bottom - depth));
            pontsHidden.append(pointTop);
            pontsHidden.append(pointBottomRight);
        }

    }
    else
    {
        y1 = rect.top() + y1;
        y2 = (y2 < 0.) ? rect.bottom() : rect.top() + y2;

        qreal ratioTop = (rect.bottom() - y1) / rect.height();
        qreal depthTop = depth * (1. - ratioTop);

        qreal ratioBottom = (rect.bottom() - y2) / rect.height();
        qreal depthBottom = depth * (1. - ratioBottom);

        qreal xCenterTop = xCenter + xCenterOffset * (1.0 - ratioTop);
        qreal xCenterBottom = xCenter + xCenterOffset * (1.0 - ratioBottom);

        qreal dxTop = 0.5 * ratioTop * rect.width();
        qreal dxBottom = 0.5 * ratioBottom * rect.width();

        QPointF ptTopCenter1(xCenter - xCenterOffset * (1.0 - ratioTop), y1 - depthTop);
        QPoint pointTopCenter2(xCenterTop, y1);

        QPointF pointTopLeft(rect.left() + dxTop, y1 - 0.5 * depthTop);
        QPointF pointTopRight(rect.right() - dxTop, y1 - 0.5 * depthTop);

        QPoint pointBottomCenter(xCenterBottom, y2);

        QPointF pointBottomLeft(rect.left() + dxBottom, y2 - 0.5 * depthBottom);
        QPointF pointBottomRight(rect.right() - dxBottom, y2 - 0.5 * depthBottom);

        if (depth > 0.0)
        {
            if (isCircularBase)
            {
                ellipseTop.point = QPointF(xCenter, pointTopLeft.y() + 0.5);
                ellipseTop.radiusX = qFabs(xCenter - pointTopLeft.x()) - 0.5;
                ellipseTop.radiusY = depthTop * 0.5 - 0.5;

                if (isTransparent)
                {
                    ellipseBottom.point = QPointF(xCenter, pointBottomLeft.y() + 0.5);
                    ellipseBottom.radiusX = qFabs(xCenter - pointBottomLeft.x()) - 0.5;
                    ellipseBottom.radiusY = depthBottom / 2.0 - 0.5;
                }
            }
            else
            {
                pointsTop.append(ptTopCenter1);
                pointsTop.append(pointTopLeft);
                pointsTop.append(pointTopCenter2);
                pointsTop.append(pointTopRight);
            }
        }

        if (isCircularBase)
        {
            qreal delta = /*(m_Type == MANAGER_GDI)*/false ? 1.0 : 0.0;
            contourSide.setStart(pointBottomLeft);

            if (depth > 0.)
            {
                contourSide.appendArc(pointBottomRight, QSizeF(qFabs(xCenter - pointBottomLeft.x()), depthBottom * 0.5 + delta), false);
                contourSide.appendLine(pointTopRight);
                contourSide.appendArc(pointTopLeft, QSizeF(qFabs(xCenter - pointTopLeft.x()), depthTop * 0.5 - delta), true);
            }
            else
            {
                contourSide.appendLine(pointBottomRight);
                contourSide.appendLine(pointTopRight);
                contourSide.appendLine(pointTopLeft);
            }
        }
        else if (!brSideFillRight.color().isValid())
        {
            pointsLeft.append(pointTopLeft);
            pointsLeft.append(pointBottomLeft);
            pointsLeft.append(pointBottomRight);
            pointsLeft.append(pointTopRight);
        }
        else
        {
            pointsLeft.append(pointTopCenter2);
            pointsLeft.append(pointTopLeft);
            pointsLeft.append(pointBottomLeft);
            pointsLeft.append(pointBottomCenter);

            pointsRight.append(pointTopCenter2);
            pointsRight.append(pointTopRight);
            pointsRight.append(pointBottomRight);
            pointsRight.append(pointBottomCenter);
        }

        if (isTransparent && !isCircularBase)
        {
            pontsHidden.append(pointBottomLeft);
            pontsHidden.append(QPointF(xCenter - xCenterOffset * (1.0 - ratioBottom), y2 - depthBottom));
            pontsHidden.append(ptTopCenter1);
            pontsHidden.append(pointBottomRight);
        }
    }

    if (!contourSide.isEmpty())
    {
        drawFillContour(p, &contourSide, brSideFillLeft);
        drawContour(p, &contourSide, brLine);

        if (!ellipseTop.isNull())
        {
            QBrush br = !brTopFill.color().isValid() ? brSideFillLeft : brTopFill;
            drawGradientEllipse(p, ellipseTop.point, ellipseTop.radiusX, ellipseTop.radiusY, br);
            drawBoundedEllipse(p, ellipseTop.point, ellipseTop.radiusX, ellipseTop.radiusY, brLine, 1.0);
        }
        if (!ellipseBottom.isNull())
            drawBoundedEllipse(p, ellipseBottom.point, ellipseBottom.radiusX, ellipseBottom.radiusY, brLine, 1.0);
    }
    else
    {
        drawFillPolygon(p, pointsLeft, brSideFillLeft, brLine);
        drawFillPolygon(p, pointsRight, brSideFillRight, brLine);
        drawFillPolygon(p, pointsTop, !brTopFill.color().isValid() ? brSideFillLeft : brTopFill, brLine);
    }

    if (pontsHidden.size() == 4)
    {
        QColor clrHidden = brLine;
        clrHidden.setAlpha(clrHidden.alpha()/5);
        drawDashedLine(p, pontsHidden[0], pontsHidden[1], clrHidden, 1, DashStyleSolid);
        drawDashedLine(p, pontsHidden[1], pontsHidden[2], clrHidden, 1, DashStyleSolid);
        drawDashedLine(p, pontsHidden[1], pontsHidden[3], clrHidden, 1, DashStyleSolid);
    }
}

void ChartPainter::drawFillPolygon(QPainter* p, const QVector<QPointF>& pts, const QBrush& brFill, const QColor& brLine) const
{
    if (pts.size() < 3)
        return;

    drawFillPolygon(p, pts, brFill);
    drawBoundedPolygon(p, pts, brLine, 1);
}

void ChartPainter::buildContourPath(QPainterPath& path, const ChartGeometry* geometry) const
{
    if (const ChartContour* contour = dynamic_cast<const ChartContour*>(geometry))
    {
        path.moveTo(contour->startPoint());
        const QList<ChartGeometry*>& segments = contour->segments();
        for (int i = 0, count = segments.count(); i < count; i++)
        {
            ChartGeometry* segment = segments.at(i);

            if (const ChartLineSegment* lineSegment = dynamic_cast<const ChartLineSegment*>(segment))
            {
                path.lineTo(lineSegment->m_point);
            }
            else if (const ChartArcSegment* arcSegment = dynamic_cast<const ChartArcSegment*>(segment))
            {
                QPointF p1, p2;

                if (arcSegment->m_isClockwise)
                {
                    p1 = QPointF(arcSegment->m_point.x(), arcSegment->m_point.y() - (arcSegment->m_radius.height()));
                    p2 = QPointF(arcSegment->m_point.x() + (arcSegment->m_radius.width()*2.0), arcSegment->m_point.y() + arcSegment->m_radius.height());
                }
                else
                {
                    p1 = QPointF(arcSegment->m_point.x() - (arcSegment->m_radius.width()*2.0), arcSegment->m_point.y() - (arcSegment->m_radius.height()));
                    p2 = QPointF(arcSegment->m_point.x(), arcSegment->m_point.y() + arcSegment->m_radius.height());
                }

                QRectF rect(p1, p2);
                rect = rect.normalized();

                qreal angle = arcSegment->m_isClockwise ? arcSegment->m_rotationAngle : arcSegment->m_rotationAngle  + 180.0;
                path.arcTo(rect, angle, arcSegment->m_isClockwise ? -180.0 : 180.0);
            }

        }

        if (contour->isClosed())
            path.closeSubpath();
    }
}

void ChartPainter::drawContour(QPainter* p, const ChartGeometry* geometry, const QColor& color, qreal lineWidth) const
{
    QPainterPath path;
    buildContourPath(path, geometry);

    QPen penSave = p->pen();

    QPen newPen(QBrush(color), lineWidth);
    p->setPen(newPen);
    p->drawPath(path);
    p->setPen(penSave);
}

void ChartPainter::drawFillContour(QPainter* p, const ChartGeometry* geometry, const QBrush& brFill) const
{
    QPainterPath path;
    buildContourPath(path, geometry);

    if (brFill.style() == Qt::LinearGradientPattern)
    {
        QRectF pointRect = path.controlPointRect();
        QLinearGradient* linearGradient = dynamic_cast<QLinearGradient*>((QLinearGradient*)brFill.gradient());

        linearGradient->setStart(pointRect.bottomLeft());
        linearGradient->setFinalStop(pointRect.bottomRight());
    }

    p->fillPath(path, brFill);
}
