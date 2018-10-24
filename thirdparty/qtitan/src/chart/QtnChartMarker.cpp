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

#include "QtnAbstractSeries.h"
#include "QtnChartPainter.h"
#include "QtnChartMarker.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartMarker
    \inmodule QtitanChart
    \internal
*/
ChartMarker::ChartMarker(AbstractSeries* owner)
    : ChartComponent(owner)
{
    m_size = 8;
    m_visible = true;

    m_type = MarkerCircle;
    m_color = QColor();
    m_borderColor = QColor();

    m_borderVisible = true;

    m_fillHelper = new ChartFillDrawHelper(this);
    m_fillHelper->setFillStyle(Qt::LinearGradientPattern);
    m_fillHelper->setGradientDirection(ChartFillDrawHelper::BottomRightToTopLeft);
}

ChartMarker::~ChartMarker()
{
}

int ChartMarker::size() const 
{
    return m_size;
}

void ChartMarker::setSize(int size) 
{
    m_size = size;
    chartUpdate();
}

void ChartMarker::setBorderVisible(bool borderVisible) 
{
    m_borderVisible = borderVisible;
    chartUpdate();
}

bool ChartMarker::borderVisible() const 
{
    return m_borderVisible;
}

MarkerType ChartMarker::type() const 
{
    return m_type;
}

void ChartMarker::setType(MarkerType type) 
{
    m_type = type;
    chartUpdate();
}

ChartFillDrawHelper* ChartMarker::fillHelper() const 
{
    return m_fillHelper;
}

bool ChartMarker::isVisible() const 
{
    return m_visible;
}

void ChartMarker::setVisible(bool visible) 
{
    m_visible = visible;
    chartUpdate();
}

void ChartMarker::paint(QPainter* p, const QPointF& point, QColor color, QColor color2, QColor colorBorder)
{
    paint(p, point, m_size, color, color2, colorBorder);
}

void ChartMarker::paint(QPainter* p, const QPointF& point, int width, QColor color, QColor color2, QColor colorBorder)
{
    if (!m_visible)
        return;
    
    ChartPainter::painter()->setAntialiasing(p, m_type == MarkerSquare);

    if (m_type == MarkerCircle)
    {
        ChartPainter::painter()->setAntialiasing(p);

        m_fillHelper->drawGradientCircle(p, point, width / 2, color, color2);

        if (m_borderVisible)
            ChartPainter::painter()->drawBoundedCircle(p, point, width / 2, colorBorder, 1);
    }
    else if (m_type == MarkerSquare)
    {
        QVector<QPointF> arrPoints;

        arrPoints.append(QPointF(point.x() - width / 2, point.y() - width / 2));
        arrPoints.append(QPointF(point.x() + width / 2, point.y() - width / 2));
        arrPoints.append(QPointF(point.x() + width / 2, point.y() + width / 2));
        arrPoints.append(QPointF(point.x() - width / 2, point.y() + width / 2));

        m_fillHelper->drawGradientPolygon(p, arrPoints, color, color2);

        if (m_borderVisible)
            ChartPainter::painter()->drawBoundedPolygon(p, arrPoints, colorBorder, 1);
    }
    else if (m_type == MarkerDiamond)
    {
        QVector<QPointF> arrPoints;

        float d = width / (float)sqrt(2.0) - 1;

        arrPoints.append(QPointF(point.x() - d, point.y()));
        arrPoints.append(QPointF(point.x(), point.y() - d));
        arrPoints.append(QPointF(point.x() + d, point.y()));
        arrPoints.append(QPointF(point.x(), point.y() + d));

        m_fillHelper->drawGradientPolygon(p, arrPoints, color, color2);

        if (m_borderVisible)
            ChartPainter::painter()->drawBoundedPolygon(p, arrPoints, colorBorder, 1);
    }
    else if (m_type == MarkerTriangle || m_type == MarkerPentagon || m_type == MarkerHexagon)
    {
        int side = m_type == MarkerTriangle ? 3 : m_type == MarkerPentagon ? 5 : 6;

        QVector<QPointF> arrPoints;
        qreal d = width / (float)sqrt(2.0) - 1;
        const qreal PI = acos(-1.0);

        for (int i = 0; i < side; i++)
            arrPoints.append(QPointF(point.x() + d * cos(PI / 2 + i * 2 * PI / side), point.y() - d * sin(PI / 2 + i * 2 * PI / side)));

        m_fillHelper->drawGradientPolygon(p, arrPoints, color, color2);

        if (m_borderVisible)
            ChartPainter::painter()->drawBoundedPolygon(p, arrPoints, colorBorder, 1);
    }
    else if (m_type == MarkerStar)
    {
        int side = 5;

        QVector<QPointF> arrPoints;
        qreal d = width / (float)sqrt(2.0) - 1;
        const qreal PI = acos(-1.0);

        qreal angle = 2 * PI / side;

        for (int i = 0; i < side; i++)
        {
            arrPoints.append(QPointF(point.x() + d * cos(PI / 2 + i * angle), point.y() - d * sin(PI / 2 + i * angle)));

            arrPoints.append(QPointF(point.x() + d / 2 * cos(PI / 2 + i * angle + angle/ 2), point.y() - d / 2 * sin(PI / 2 + i * angle + angle / 2)));
        }

        m_fillHelper->drawGradientPolygon(p, arrPoints, color, color2);

        if (m_borderVisible)
            ChartPainter::painter()->drawBoundedPolygon(p, arrPoints, colorBorder, 1);
    }

    ChartPainter::painter()->unsetAntialiasing(p);
}


