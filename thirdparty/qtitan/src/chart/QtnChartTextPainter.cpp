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
#include <float.h>

#include <QPainter>

#include "QtnChartPainter.h"
#include "QtnChartTextPainter.h"
#include "QtnChartComponent.h"
#include "QtnChartMath.h"


using namespace Qtitan;

/*!
    \class Qtitan::ChartTextPainterBase
    \inmodule QtitanChart
    \internal
*/
ChartTextPainterBase::ChartTextPainterBase(const QString& text, ChartTextComponent* textProvider)
{
    m_strText = text;
    m_textProvider = textProvider;

    QFontMetricsF fontMetrics(textProvider->font());
    m_szTextSize.setWidth(fontMetrics.width(m_strText) + 2.0);
    m_szTextSize.setHeight(fontMetrics.height());

    m_height = qCeil(m_szTextSize.height());
    m_width = qCeil(m_szTextSize.width());
}

ChartTextPainterBase::~ChartTextPainterBase()
{
}

QSizeF ChartTextPainterBase::size() const 
{
  return m_szTextSize;
}

QRect ChartTextPainterBase::roundedBounds()
{
    if (m_rcRoundedBounds.isNull())
    {
        m_boundingRect = calculateBounds();
        m_rcRoundedBounds = QRect(QPoint((int)m_boundingRect.x(), (int)m_boundingRect.y()), QPoint((int)m_boundingRect.right(), (int)m_boundingRect.bottom()));
    }
    return m_rcRoundedBounds;
}



/*!
    \class Qtitan::ChartTextPainter
    \inmodule QtitanChart
    \internal
*/
ChartTextPainter::ChartTextPainter(const QString& text, ChartTextComponent* textProvider)
    : ChartTextPainterBase(text, textProvider)
{
    m_ptLocation = QPointF(0, 0);
}

void ChartTextPainter::setLocation(const QPointF& location)
{
    m_ptLocation = location;
}

void ChartTextPainter::drawBoundedText(QPainter* p, const QColor& color)
{
    QRectF rect(m_ptLocation.x(), m_ptLocation.y(), m_szTextSize.width(), m_szTextSize.height());
//    p->drawRect(rect);
    p->save();
    ChartPainter::painter()->setTextAntialiasing(p, m_textProvider->antialiasing());
    ChartPainter::painter()->drawBoundedText(p, m_strText, m_textProvider->font(), color, rect);
    ChartPainter::painter()->unsetTextAntialiasing(p);
    p->restore();
}

QRectF ChartTextPainter::calculateBounds()
{
    return QRectF(m_ptLocation.x(), m_ptLocation.y(), m_szTextSize.width(), m_szTextSize.height());
}


/*!
    \class Qtitan::ChartRotatedTextPainterBase
    \inmodule QtitanChart
    \internal
*/
ChartRotatedTextPainterBase::ChartRotatedTextPainterBase(const QString& text, ChartTextComponent* textProvider, QPoint ptBase)
    : ChartTextPainterBase(text, textProvider)
{
    m_ptBasePoint = ptBase;
    m_angle = 0;
}

void ChartRotatedTextPainterBase::setBasePoint(QPoint pt)
{
    m_ptBasePoint = pt;
}

QRectF ChartRotatedTextPainterBase::calculateBounds()
{
    float minX = FLT_MAX, minY = FLT_MAX, maxX = FLT_MIN, maxY = FLT_MIN;

    float points[4][2];
    calculatePoints(calculateRotation(), getInitialTextRect(), m_angle, points);

    for (int i = 0; i < 4; i++)
    {
        minX = qMin(minX, points[i][0]);
        maxX = qMax(maxX, points[i][0]);

        minY = qMin(minY, points[i][1]);
        maxY = qMax(maxY, points[i][1]);
    }

    return QRectF(QPointF(minX, minY), QSizeF(maxX - minX, maxY - minY));
}

QRect ChartRotatedTextPainterBase::getInitialTextRect()
{
    return QRect(calculateLeftTopPoint(), QSize(m_width, m_height));
}

void ChartRotatedTextPainterBase::calculatePoints(TextRotation rotation, QRect rect, float fDegree, float points[4][2])
{
//    qreal pi = ChartMath::QTN_PI;
    qreal radial = fDegree * ChartMath::QTN_DEGRAD;

    float width = (float)rect.width() * 0.5f;
    float height = (float)rect.height() * 0.5f;
    float fsin = (float)qSin(radial);
    float fcos = (float)qCos(radial);

    points[0][0] = rect.left()  + height * fsin + width * (1 - fcos), points[0][1] = rect.top()    + height * (1 - fcos) - width * fsin;
    points[1][0] = rect.right() + height * fsin - width * (1 - fcos), points[1][1] = rect.top()    + height * (1 - fcos) + width * fsin;
    points[2][0] = rect.right() - height * fsin - width * (1 - fcos), points[2][1] = rect.bottom() - height * (1 - fcos) + width * fsin;
    points[3][0] = rect.left()  - height * fsin + width * (1 - fcos), points[3][1] = rect.bottom() - height * (1 - fcos) - width * fsin;

    float offset[2];
    switch (rotation)
    {
        case TextLeftTop:
                offset[0] = -width * (1.0f - fcos) - height * fsin, offset[1] = width * fsin - height * (1.0f - fcos);
            break;
        case TextCenterTop:
                offset[0] = -height * fsin, offset[1] = -height * (1.0f - fcos);
            break;
        case TextRightTop:
                offset[0] = width * (1.0f - fcos) - height * fsin, offset[1] = -width * fsin - height * (1.0f - fcos);
            break;
        case TextLeftCenter:
                offset[0] = -width * (1.0f - fcos), offset[1] = width * fsin;
            break;
        case TextRightCenter:
                offset[0] = width * (1.0f - fcos), offset[1] = -width * fsin;
            break;
        case TextLeftBottom:
                offset[0] = -width * (1.0f - fcos) + height * fsin, offset[1] = width * fsin + height * (1.0f - fcos);
            break;
        case TextCenterBottom:
                offset[0] = height * fsin, offset[1] = height * (1.0f - fcos);
            break;
        case TextRightBottom:
                offset[0] = width * (1.0f - fcos) + height * fsin, offset[1] = -width * fsin + height * (1.0f - fcos);
            break;
        default:
            return;
    }

    for (int i = 0; i < 4; i++)
    {
        points[i][0] += offset[0];
        points[i][0] += offset[1];
    }

}

void ChartRotatedTextPainterBase::drawBoundedText(QPainter* p, const QColor& color)
{
    QRect rect = getInitialTextRect();
    QPoint offset;

    QPoint center(rect.left() + rect.width() / 2, rect.top() + rect.height() / 2);

    switch (calculateRotation())
    {
        case TextLeftTop:
            offset = QPoint(rect.left(), rect.top());
            break;
        case TextCenterTop:
            offset = QPoint(center.x(), rect.top());
            break;
        case TextRightTop:
            offset = QPoint(rect.right(), rect.top());
            break;
        case TextLeftCenter:
            offset = QPoint(rect.left(), center.y());
            break;
        case TextCenterCenter:
            offset = QPoint(center.x(), center.y());
            break;
        case TextRightCenter:
            offset = QPoint(rect.right(), center.y());
            break;
        case TextLeftBottom:
            offset = QPoint(rect.left(), rect.bottom());
            break;
        case TextCenterBottom:
            offset = QPoint(center.x(), rect.bottom());
            break;
        case TextRightBottom:
            offset = QPoint(rect.right(), rect.bottom());
            break;
        default:
            offset = QPoint(0, 0);
            break;
    }
    rect.translate(-offset.x(), -offset.y());

    p->save();

    ChartPainter::painter()->pushTransform(p, (qreal)offset.x(), (qreal)offset.y(), 0);
    ChartPainter::painter()->pushRotateTransform(p, m_angle);
    ChartPainter::painter()->setTextAntialiasing(p, m_textProvider->antialiasing());

    ChartPainter::painter()->drawBoundedText(p, m_strText, m_textProvider->font(), color, QRectF(rect));

    ChartPainter::painter()->unsetTextAntialiasing(p);
    ChartPainter::painter()->popRotateTransform(p);
    ChartPainter::painter()->popTransform(p);

    p->restore();
}


/*!
    \class Qtitan::ChartRotatedTextPainterNearLine
    \inmodule QtitanChart
    \internal
*/
ChartRotatedTextPainterNearLine::ChartRotatedTextPainterNearLine(const QString& text, ChartTextComponent* textProvider, QPoint ptBase, NearTextPosition position, float angle)
: ChartRotatedTextPainterBase(text, textProvider, ptBase)
{
    m_nearPosition = position;
    m_angle = angle;
}


QPoint ChartRotatedTextPainterNearLine::calculateLeftTopPoint()
{
    qreal pi = acos(-1.0);
    qreal radial = m_angle * pi / 180.0;
    int halfHeight = m_height / 2;

    if (m_nearPosition == TextNearBottom)
    {
        if (m_angle == 0 || m_angle == 180)
            return QPoint(m_ptBasePoint.x() - m_width / 2, m_ptBasePoint.y());

        return m_angle < 180 ? QPoint(m_ptBasePoint.x(), m_ptBasePoint.y() - (int)(m_height / 2 * qSin(radial))) :
            QPoint(m_ptBasePoint.x() - m_width, m_ptBasePoint.y() - (int)(m_height / 2 * qSin(radial - pi)));
    }
    else if (m_nearPosition == TextNearLeft)
    {
        if (m_angle == 90 || m_angle == 270)
            return QPoint(m_ptBasePoint.x() - m_width / 2 - halfHeight, m_ptBasePoint.y() - halfHeight);

        return (m_angle < 90 || m_angle > 270) ? QPoint(m_ptBasePoint.x() - (int)(m_width + fabs(halfHeight * qSin(radial))), m_ptBasePoint.y() - halfHeight) 
            : QPoint(m_ptBasePoint.x() - (int)(fabs(halfHeight * qCos(radial - pi / 2))), m_ptBasePoint.y() - halfHeight);
    }
    else if (m_nearPosition == TextNearRight)
    {
        if (m_angle == 90 || m_angle == 270)
            return QPoint(m_ptBasePoint.x() - m_width / 2 + halfHeight, m_ptBasePoint.y() - halfHeight);

        return (m_angle < 90 || m_angle > 270) ? QPoint(m_ptBasePoint.x() + (int)(fabs(halfHeight * qSin(radial))), m_ptBasePoint.y() - halfHeight) :
            QPoint(m_ptBasePoint.x() - m_width + (int)(fabs(halfHeight * qCos(radial - pi / 2))), m_ptBasePoint.y() - halfHeight);
    }
    else if (m_nearPosition == TextNearTop)
    {
        if (m_angle == 0 || m_angle == 180)
            return QPoint(m_ptBasePoint.x() - m_width / 2, m_ptBasePoint.y() - m_height);

        return m_angle < 180 ? QPoint(m_ptBasePoint.x() - m_width, m_ptBasePoint.y() - (int)(halfHeight + fabs(halfHeight * qCos(radial)))) :
            QPoint(m_ptBasePoint.x(), m_ptBasePoint.y() - (int)(halfHeight + fabs(halfHeight * qCos(radial - pi))));
    }
    return QPoint(0, 0);
}

ChartRotatedTextPainterBase::TextRotation ChartRotatedTextPainterNearLine::calculateRotation()
{
    switch (m_nearPosition)
    {
        case TextNearBottom:
            if (m_angle == 0 || m_angle == 180)
                return TextCenterCenter;
            return m_angle < 180 ? TextLeftCenter : TextRightCenter;

        case TextNearLeft:
            if (m_angle == 90 || m_angle == 270)
                return TextCenterCenter;
            return (m_angle < 90 || m_angle > 270) ? TextRightCenter : TextLeftCenter;

        case TextNearRight:
            if (m_angle == 90 || m_angle == 270)
                return TextCenterCenter;
            return (m_angle < 90 || m_angle > 270) ? TextLeftCenter : TextRightCenter;

        case TextNearTop:
            if (m_angle == 0 || m_angle == 180)
                return TextCenterCenter;
            return m_angle < 180 ? TextRightCenter : TextLeftCenter;

        default:
            return TextCenterCenter;
    }
}


/*!
    \class Qtitan::ChartRotatedTextPainterOnCircleRadial
    \inmodule QtitanChart
    \internal
*/
ChartRotatedTextPainterOnCircleRadial::ChartRotatedTextPainterOnCircleRadial(const QString& text, ChartTextComponent* textProvider, QPoint ptBase, float angle)
    : ChartRotatedTextPainterBase(text, textProvider, ptBase)
{
    angle = (float)ChartMath::normalizeDegree(angle);
    m_angle = angle <= 180 ? angle - 90 : angle + 90;
}

QPoint ChartRotatedTextPainterOnCircleRadial::calculateLeftTopPoint()
{
    return QPoint(m_ptBasePoint.x() - m_width / 2, m_ptBasePoint.y() - m_height / 2);
}

ChartRotatedTextPainterBase::TextRotation ChartRotatedTextPainterOnCircleRadial::calculateRotation()
{
    return TextCenterCenter;
}
