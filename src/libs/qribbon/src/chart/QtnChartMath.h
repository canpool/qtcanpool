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
#ifndef QTN_CHARTMATH_H
#define QTN_CHARTMATH_H

#include <QPolygonF>
#include <qmath.h>

class QPainterPath;

namespace Qtitan
{
    class ChartMath
    {
    public:
        static qreal normalizeRadian(qreal angleRadian);
        static qreal normalizeDegree(qreal angle);
        static int sign(qreal value);
        static int fuzzyCmp(qreal value1, qreal value2);
        static int fuzzyCmp(qreal value1, qreal value2, qreal epsilon);
        static qreal radian2Degree(qreal angleRadian);
        static qreal degree2Radian(qreal angleDegree);
        static qreal distance(qreal dx, qreal dy);
        static qreal distanceToPoint2(const QPointF& pt1, const QPointF& pt2);
        static int snapValue(int value, int low, int high);
        static int round(qreal value);
        static int clamp(int value, int low, int high);
        static qreal angle(qreal dx, qreal dy);
        static qreal angle(const QPointF& pt1, const QPointF& pt2, bool invY = false);
        
    public:
        static qreal QTN_PI;      // 3.14159265358979323846
        static qreal QTN_PI_2;    // M_PI / 2.0
        static qreal QTN_PI2;     // 2.0 * M_PI, 6.28318530717958647692
        static qreal QTN_DEGRAD;  // (M_PI/180.0)
        static qreal QTN_RADDEG;  // (180.0/M_PI)
        static qreal QTN_METRIC_ACCURACY; // 1E-5
        static qreal QTN_EPSILON; // smallest such that 1.0+FLT_EPSILON != 1.0
        static qreal QTN_EPSILON_1; //
    };

    /* ChartGeometry */
    class ChartGeometry
    {
    public:
        ChartGeometry();
        virtual ~ChartGeometry();
    };

    /* ChartBezierSegment */
    class ChartLineSegment : public ChartGeometry
    {
    public:
        ChartLineSegment(const QPointF& pnt);
        QPointF m_point;
    };

    /* ChartBezierSegment */
    class ChartBezierSegment : public ChartGeometry
    {
    public:
        ChartBezierSegment(const QPointF& pnt1, const QPointF& pnt2, const QPointF& pnt3)
        {
            m_point1 = pnt1;
            m_point2 = pnt2;
            m_point3 = pnt3;
        }

        QPointF m_point1;
        QPointF m_point2;
        QPointF m_point3;
    };

    /* ChartArcSegment */
    class ChartArcSegment : public ChartGeometry
    {
    public:
        ChartArcSegment(const QPointF& pnt, const QSizeF& radius, bool isClockwise, bool isLargeArc, qreal rotationAngle);
        QPointF arcCenter(const QPointF& ptFrom, bool& isLargeArc, qreal& rX, qreal& rY) const;

        QPointF m_point;
        QSizeF  m_radius;
        bool    m_isClockwise;
        bool    m_isLargeArc;
        qreal   m_rotationAngle;
    };

    /* ChartContour */
    class ChartContour : public ChartGeometry
    {
    public:
        ChartContour(const QPointF& pntStart = QPointF(-1., -1.), bool isClosed = true)
        {
            m_pntStart = pntStart;
            m_isClosed = isClosed;
        }

        virtual ~ChartContour()
        {
            clear();
        }

        void setClosed(bool isClosed = true)
        {
            m_isClosed = isClosed;
        }

        bool isClosed() const
        {
            return m_isClosed;
        }

        void setStart(const QPointF& pntStart)
        {
            m_pntStart = pntStart;
        }

        void appendLine(const QPointF& pt);
        void appendBezier(const QPointF& pt1, const QPointF& pt2, const QPointF& pt3);
        void appendArc(const QPointF& pt, const QSizeF szRadius, bool isClockwise = true, bool isLargeArc = false, double rotationAngle = 0.);
//        void appendPoints(const QVector<QPointF>& points, CurveType curveType);

        void clear();

        const QPointF& startPoint() const
        {
            return m_pntStart;
        }

        const QList<ChartGeometry*>& segments() const
        {
            return m_segments;
        }

        virtual bool isEmpty() const
        {
            return m_segments.count() == 0;
        }

        virtual bool isNull() const
        {
            return m_segments.count() == 0;
        }

    protected:
        QPointF m_pntStart;
        QList<ChartGeometry*> m_segments;
        bool    m_isClosed;
    };


    /* ChartEllipse */
    class ChartEllipse
    {
    public:
        ChartEllipse();
        ChartEllipse(const QPointF& ptCenter, qreal majorSemiaxis, qreal minorSemiaxis);

    public:
        qreal area() const;
        QPointF center() const;
        qreal calcEllipseSectorFinishAngle(qreal areaSector, qreal beginAngle) const;

    public:
        QPointF polar2Cartesian(qreal angle, qreal radius) const;
        qreal calcEllipseRadius(qreal angle) const;
        QPointF calcEllipsePoint(qreal angle) const;
        qreal calcEllipseSectorArea(qreal beginAngle, qreal endAngle) const;
        qreal calcEllipseAngleFromCircleAngle(qreal angle) const;
        QPointF applyCenterPoint(const QPointF& point) const;
        qreal atanMulTan(qreal multiplier, qreal tanAngle) const;

    public:
        QPointF m_ptCenter;
        qreal m_majorSemiaxis;
        qreal m_minorSemiaxis;

        qreal m_area;
    };

    /* QtnEllipse */
    class QtnEllipse
    {
    public:
        QtnEllipse();
        QtnEllipse(const QPointF& pt, qreal radX, qreal radY);
        QtnEllipse(const QPointF& pt, const QSizeF& sz);
        QtnEllipse(const QRectF& rect);

        bool isNull() const
        {
            return radiusX <= 0.0 || radiusY <= 0.0;
        }

    public:
        QPointF point;
        qreal radiusX;
        qreal radiusY;
    };

    /* ChartSpline */ 
    class ChartSpline
    {
    public:
        enum SplineType
        {
            SplineCubic,    // Cubic Hermite spline with strain
            SplineHermite   // Hermite spline
        };

    public:
        ChartSpline();
        ~ChartSpline();

    public:
        void setPoints(const QVector<QPointF>& points, SplineType type, 
            bool closed = true, qreal strain = 0.0, qreal offset = 0.0, qreal regularity = 0.0);
        const QVector<QPointF>& points() const;

        void clear();
        bool isClosed() const;

    protected:
        QVector<QPointF> m_points;
        bool m_closed;
    };
};

#endif // QTN_CHARTMATH_H
