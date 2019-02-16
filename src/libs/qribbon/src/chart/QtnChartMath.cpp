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
#include <qnumeric.h>

#include "QtnChartMath.h"

using namespace Qtitan;

qreal ChartMath::QTN_PI   = 3.14159265358979323846;
qreal ChartMath::QTN_PI_2 = 1.57079632679489661923;
qreal ChartMath::QTN_PI2  = (QTN_PI*2.0); 
qreal ChartMath::QTN_DEGRAD = (M_PI/180.0);
qreal ChartMath::QTN_RADDEG = (180.0/M_PI);
qreal ChartMath::QTN_METRIC_ACCURACY = 1E-5;
qreal ChartMath::QTN_EPSILON = 1.192092896e-07F;
qreal ChartMath::QTN_EPSILON_1 = 1e-6;


/*!
    \class Qtitan::ChartMath
    \inmodule QtitanChart
    \internal
*/
qreal ChartMath::normalizeRadian(qreal angleRadian)
{
    qreal x = qFabs(angleRadian) / QTN_PI2;
    x = QTN_PI2 * (x - floor(x));
    return x * sign(angleRadian);
}

qreal ChartMath::normalizeDegree(qreal angle)
{
    qreal result = angle - 360 * ((int)angle / 360);
    if (result < 0)
        result += 360;
    return result;
}

int ChartMath::fuzzyCmp(qreal value1, qreal value2, qreal epsilon)
{
    if (qFabs(value1 - value2) <= epsilon)
        return 0;

    if (value1 > value2)
        return 1;

    return - 1;
}

int ChartMath::fuzzyCmp(qreal value1, qreal value2)
{
    return fuzzyCmp(value1, value2, QTN_METRIC_ACCURACY);
}

int ChartMath::sign(qreal value)
{
    if (qFabs(value) < QTN_METRIC_ACCURACY)
        return 0;

    return value > 0 ? +1 : -1;
}

qreal ChartMath::radian2Degree(qreal angleRadian)
{
//    return angleRadian * 180 / QTN_PI;
    return angleRadian * QTN_RADDEG;
}

qreal ChartMath::degree2Radian(qreal angleDegree)
{
    return angleDegree * QTN_DEGRAD;
}

qreal ChartMath::distance(qreal dx, qreal dy)
{
    return qSqrt(dx * dx + dy * dy);
}

qreal ChartMath::distanceToPoint2(const QPointF& pt1, const QPointF& pt2)
{
    return distance(pt2.x() - pt1.x(), pt2.y() - pt1.y());
}

int ChartMath::snapValue(int value, int low, int high)
{
    if (value < low)
        return low;
    else if (value > high)
        return high;
    return value;
}

int ChartMath::round(qreal value)
{
    int x = (int)qFloor(value);
    return qFabs(x - value) < qFabs(x + 1 - value) ? x : x + 1;
}

int ChartMath::clamp(int value, int low, int high)
{
    return value < low ? low : (value > high ? high : value);
}

qreal ChartMath::angle(qreal dx, qreal dy)
{
    if (dy == 0)
        return (dx < 0.0) ? -QTN_PI : 0.0;
    return qAtan2(dy, dx);
}

qreal ChartMath::angle(const QPointF& pt1, const QPointF& pt2, bool invY)
{
    return ChartMath::angle (pt2.x() - pt1.x(), invY ? pt1.y() - pt2.y() : pt2.y() - pt1.y());
}

/* ChartLineSegment */
ChartGeometry::ChartGeometry()
{
}

ChartGeometry::~ChartGeometry()
{
}

/* ChartLineSegment */
ChartLineSegment::ChartLineSegment(const QPointF& pnt)
{
    m_point = pnt;
}

/* ChartArcSegment */
ChartArcSegment::ChartArcSegment(const QPointF& pnt, const QSizeF& radius, bool isClockwise, bool isLargeArc, qreal rotationAngle)
{
    m_point = pnt;
    m_radius = radius;
    m_isClockwise = isClockwise;
    m_isLargeArc = isLargeArc;
    m_rotationAngle = rotationAngle;
}

QPointF ChartArcSegment::arcCenter(const QPointF& ptFrom, bool& isLargeArc, qreal& rX, qreal& rY) const
{
    const qreal x1 = ptFrom.x();
    const qreal y1 = ptFrom.y();

    const qreal x2 = m_point.x();
    const qreal y2 = qFabs(m_point.y() - y1) < ChartMath::QTN_EPSILON ? y1 : m_point.y();

    rX = m_radius.width();
    rY = m_radius.height();

    QPointF pntCenter;

    if (rX <= 0.0 || rY <= 0.0)
    {
        rX = 0.0;
        rY = 0.0;
        return pntCenter;
    }

    const qreal x12  = x1 * x1;
    const qreal x22  = x2 * x2;
    const qreal y12  = y1 * y1;
    const qreal y22  = y2 * y2;

    const qreal rX2  = rX * rX;
    const qreal rY2  = rY * rY;
    const qreal rYX2 = rY2 / rX2;

    const qreal dY = y1 == y2 ? 0.01 : (y1 - y2);
    const qreal K = rYX2 * (x2 - x1) / dY;
    const qreal M = (rYX2 * (x12 - x22) + y12 - y22) / (2.0 * dY);
    const qreal A = rY2 + rX2 * K * K;
    const qreal B = 2.0 * (rX2 * K * (M - y1) - rY2 * x1);
    const qreal C = rY2 * x12 + rX2 * ((M - 2.0 * y1) * M + y12) - rX2 * rY2;
    qreal D = B * B - 4.0 * A * C;

    isLargeArc = m_isLargeArc;

    if (D >= 0)
    {
        if (D > 0)
        {
            D = qSqrt(D);

            if (y2 <= y1)
                D = -D;

            if ((!m_isClockwise || !m_isLargeArc) && (m_isClockwise || m_isLargeArc))
                D = -D;
        }

        pntCenter.setX((-B + D) / (2.0 * A));
        pntCenter.setY(K * pntCenter.x() + M);
    }
    else
    {
        pntCenter.setX((x2 + x1) / 2.0);
        pntCenter.setY((y2 + y1) / 2.0);

        D = rY / rX;
        rX = ChartMath::distance(x1 - pntCenter.x(), (y1 - pntCenter.y()) / D);
        rY = D * rX;

        isLargeArc = false;
    }

    return pntCenter;
}


/*!
    \class Qtitan::ChartContour
    \inmodule QtitanChart
    \internal
*/
void ChartContour::appendLine(const QPointF& pt)
{
    if (m_pntStart == QPointF(-1., -1.))
        return;

    m_segments.append(new ChartLineSegment(pt));
}

void ChartContour::appendBezier(const QPointF& pnt1, const QPointF& pnt2, const QPointF& pnt3)
{
    if (m_pntStart == QPointF(-1.0, -1.0))
    {
        Q_ASSERT(false);
        return;
    }

    m_segments.append(new ChartBezierSegment(pnt1, pnt2, pnt3));
}

void ChartContour::appendArc(const QPointF& pt, const QSizeF szRadiusIn, bool isClockwise, bool isLargeArc, double rotationAngle)
{
    if (m_pntStart == QPointF(-1., -1.))
    {
        Q_ASSERT(false);
        return;
    }

    QSizeF szRadius = szRadiusIn;
    szRadius.setWidth(qMax(0.0, szRadius.width()));
    szRadius.setHeight(qMax(0.0, szRadius.height()));

    m_segments.append(new ChartArcSegment(pt, szRadius, isClockwise, isLargeArc, rotationAngle));
}
/*
void ChartContour::appendPoints(const QVector<QPointF>& points, CurveType curveType)
{
    bool setStart = m_pntStart == QPointF(-1., -1.);

    int count = points.count();
    if (count < 1)
        return;

    if (setStart)
        setStart(points[0]);

    if (curveType == CurveLine)
    {
        for (int i = (setStart ? 1 : 0); i < count; i++)
            m_segments.append(new ChartLineSegment(points[i]));
    }
    else
    {
        if (!setStart)
            m_segments.append(new ChartLineSegment(points[0]));

        if (count == 2)
        {
            m_segments.append(new ChartLineSegment(points[1]));
            return;
        }

        for (int i = 1; i < count; i++)
        {
            const QPointF& pt = points[i];
            ChartBezierSegment* segment = new ChartBezierSegment(pt, pt, pt);

            if (i < count - 1)
                i++;

            segment->m_point2 = points[i];
            if (i < count - 1)
                i++;

            segment->m_point3 = points[i];
            m_segments.append(segment);
        }
    }
}
*/
void ChartContour::clear()
{
    m_pntStart = QPointF(-1., -1.);
    m_isClosed = true;

    for (int i = 0; i < m_segments.count(); i++)
        delete m_segments[i];

    m_segments.clear();
}


/*!
    \class Qtitan::ChartEllipse
    \inmodule QtitanChart
    \internal
*/
ChartEllipse::ChartEllipse()
{
    m_majorSemiaxis = m_minorSemiaxis = 0;
    m_area = 0;
}

ChartEllipse::ChartEllipse(const QPointF& pntCenter, qreal majorSemiaxis, qreal minorSemiaxis)
{
    m_ptCenter = pntCenter;
    m_majorSemiaxis = majorSemiaxis;
    m_minorSemiaxis = minorSemiaxis;

    m_area = ChartMath::QTN_PI * m_majorSemiaxis * m_minorSemiaxis;
}

qreal ChartEllipse::area() const 
{
    return m_area;
}

QPointF ChartEllipse::center() const 
{
    return m_ptCenter;
}

qreal ChartEllipse::calcEllipseSectorFinishAngle(qreal areaSector, qreal beginAngle) const
{
    qreal val = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, beginAngle);
    qreal angle = 2 * areaSector / (m_majorSemiaxis * m_minorSemiaxis) + val;
    return atanMulTan(m_minorSemiaxis / m_majorSemiaxis, angle);
}

qreal ChartEllipse::atanMulTan(qreal multiplier, qreal tanAngle) const
{
    qreal angle = ChartMath::normalizeRadian(tanAngle);
    qreal result = tanAngle;
    if (ChartMath::fuzzyCmp(qFabs(angle), ChartMath::QTN_PI_2) != 0 && 
        ChartMath::fuzzyCmp(qFabs(angle), ChartMath::QTN_PI_2 * 3) != 0)
    {
        result = atan(multiplier * tan(angle)) + tanAngle - angle;
        if (ChartMath::fuzzyCmp(qFabs(angle), ChartMath::QTN_PI_2) > 0 && ChartMath::fuzzyCmp(qFabs(angle), ChartMath::QTN_PI_2 * 3) < 0)
            result += ChartMath::sign(tanAngle) * ChartMath::QTN_PI;
        else if (ChartMath::fuzzyCmp(qFabs(angle), ChartMath::QTN_PI_2 * 3) > 0)
            result += ChartMath::sign(tanAngle) * ChartMath::QTN_PI2;
    }
    return result;
}

QPointF ChartEllipse::polar2Cartesian(qreal angle, qreal radius) const
{
    return QPointF(radius * cos(angle), -radius * sin(angle));
}


QPointF ChartEllipse::applyCenterPoint(const QPointF& point) const
{
    QPointF result = point;
    result += m_ptCenter;
    return result;
}

qreal ChartEllipse::calcEllipseRadius(qreal angle) const
{
    return m_majorSemiaxis * m_minorSemiaxis / sqrt(pow(m_majorSemiaxis* sin(angle), 2.0) + pow(m_minorSemiaxis * cos(angle), 2.0));
}

QPointF ChartEllipse::calcEllipsePoint(qreal angle) const
{
    QPointF point = polar2Cartesian(angle, calcEllipseRadius(angle));
    return applyCenterPoint(point);
}

qreal ChartEllipse::calcEllipseSectorArea(qreal beginAngle, qreal endAngle) const
{
    qreal val1 = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, endAngle);
    qreal val2 = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, beginAngle);
    return m_majorSemiaxis *  m_minorSemiaxis / 2.0 * (val1 - val2);
}

qreal ChartEllipse::calcEllipseAngleFromCircleAngle(qreal angle) const
{
    return atanMulTan(m_minorSemiaxis / m_majorSemiaxis, angle);
}

/*!
    \class Qtitan::QtnEllipse
    \inmodule QtitanChart
    \internal
*/
QtnEllipse::QtnEllipse()
{
    radiusX = 0.0;
    radiusY = 0.0;
}

QtnEllipse::QtnEllipse(const QPointF& pt, qreal radX, qreal radY)
{
    point = pt;
    radiusX = radX;
    radiusY = radY;
}

QtnEllipse::QtnEllipse(const QPointF& pt, const QSizeF& sz)
{
    point = pt;
    radiusX = sz.width() * 0.5;
    radiusY = sz.height() * 0.5;
}

QtnEllipse::QtnEllipse(const QRectF& rect)
{
    point = rect.center();
    radiusX = rect.width() * 0.5;
    radiusY = rect.height() * 0.5;
}

/*!
    \class Qtitan::ChartSpline
    \inmodule QtitanChart
    \internal
*/
ChartSpline::ChartSpline()
{
    m_closed = true;
}

ChartSpline::~ChartSpline()
{
}

void ChartSpline::setPoints(const QVector<QPointF>& points, SplineType type, bool closed, qreal strain, qreal offset, qreal regularity)
{
    clear();

    m_closed = closed;

    int count = points.size ();

    if (count < 3)
        return;

    m_points.resize((count - 1) * 3 + 1);

    const QPointF* srcPoints = points.data ();
    QPointF* dstPoints = m_points.data ();

    *dstPoints++ = *srcPoints;

    if (type == SplineCubic)
    {
        strain     = ChartMath::snapValue(strain, -1.0, 1.0);
        offset     = ChartMath::snapValue(offset, -1.0, 1.0);
        regularity = ChartMath::snapValue(regularity, -1.0, 1.0);

        for (int i = 0; i < count - 1; i++)
        {
            const QPointF& pt0 = srcPoints[i == 0 ? (m_closed ? count - 2 : 0) : i - 1];
            const QPointF& pt1 = srcPoints[i];
            const QPointF& pt2 = srcPoints[i + 1];
            const QPointF& pt3 = srcPoints[i == count - 2 ? (m_closed ? 1 : count - 1) : i + 2];

            *dstPoints++ = pt1 + ((pt2 - pt1) * (1.0 - offset) * (1.0 - regularity) + (pt1 - pt0) * (1.0 + offset) * (1.0 + regularity)) * (1.0 - strain) / 6.0;
            *dstPoints++ = pt2 - ((pt3 - pt2) * (1.0 - offset) * (1.0 + regularity) + (pt2 - pt1) * (1.0 + offset) * (1.0 - regularity)) * (1.0 - strain) / 6.0;
            *dstPoints++ = pt2;
        }
    }
    else
    {
        for (int i = 0; i < count - 1; i++)
        {
            const QPointF& pt0 = srcPoints[i == 0 ? (m_closed ? count - 2 : 0) : i - 1];
            const QPointF& pt1 = srcPoints[i];
            const QPointF& pt2 = srcPoints[i + 1];
            const QPointF& pt3 = srcPoints[i == count - 2 ? (m_closed ? 1 : count - 1) : i + 2];

            qreal x   = ChartMath::distanceToPoint2(pt2, pt1);
            qreal x01 = ChartMath::distanceToPoint2(pt1, pt0);
            qreal x10 = ChartMath::distanceToPoint2(pt3, pt2);

            if (x <= ChartMath::QTN_EPSILON)
                x = 1.0;

            if (x01 <= ChartMath::QTN_EPSILON)
                x01 = 1.0;

            if (x10 <= ChartMath::QTN_EPSILON)
                x10 = 1.0;

            *dstPoints++ = pt1 + ((pt2 - pt1) + (pt1 - pt0) * x / x01) / 6.0;
            *dstPoints++ = pt2 - ((pt3 - pt2) * x / x10 + (pt2 - pt1)) / 6.0;
            *dstPoints++ = pt2;
        }
    }
}

const QVector<QPointF>& ChartSpline::points() const
{
    return m_points;
}

void ChartSpline::clear()
{
    m_closed = true;
    m_points.clear();
}

bool ChartSpline::isClosed() const
{
    return m_closed;
}

