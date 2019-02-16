/****************************************************************************
**
** Qtitan Library by Developer Machines (Advanced QtitanChart for Qt)
** 
** Copyright (c) 2009-2012 Developer Machines (http://www.devmachines.com)
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

#include "QtnChartMathUtils.h"


using namespace Qtitan;

qreal ChartMathUtils::PI = acos(-1.0);


qreal ChartMathUtils::normalizeRadian(qreal angleRadian)
{
	qreal x = fabs(angleRadian) / (PI * 2);
	x = PI * 2 * (x - floor(x));
	return x * sign(angleRadian);
}

qreal ChartMathUtils::normalizeDegree(qreal angle)
{
	qreal result = angle - 360 * ((int)angle / 360);
	if (result < 0)
		result += 360;
	return result;
}

int ChartMathUtils::compare(qreal value1, qreal value2, qreal epsilon)
{
	if (fabs(value1 - value2) <= epsilon)
		return 0;

	if (value1 > value2)
		return 1;

	return - 1;
}

int ChartMathUtils::compare(qreal value1, qreal value2)
{
	return compare(value1, value2, 1e-5);
}


int ChartMathUtils::sign(qreal value)
{
	if (fabs(value) < 1e-5)
		return 0;

	return value > 0 ? +1 : -1;
}

qreal ChartMathUtils::radian2Degree(qreal angleRadian)
{
	return angleRadian * 180 / PI;
}

qreal ChartMathUtils::degree2Radian(qreal angleDegree)
{
	return angleDegree * PI / 180.0;
}

int ChartMathUtils::round(qreal value)
{
	int x = (int)floor(value);
	return fabs(x - value) < fabs(x + 1 - value) ? x : x + 1;
}

qreal ChartMathUtils::getNaN()
{
	unsigned long nan[2]={0xffffffff, 0x7fffffff};
	qreal g = *( qreal* )nan;

	return g;
}

bool ChartMathUtils::isNan(qreal x)
{
    return !!qIsNaN(x);
}

/* ChartEllipse */
ChartEllipse::ChartEllipse()
{
    m_majorSemiaxis = m_minorSemiaxis = 0;
    m_area = 0;
}

ChartEllipse::ChartEllipse(const QPointF& ptCenter, qreal majorSemiaxis, qreal minorSemiaxis)
{
    m_ptCenter = ptCenter;
    m_majorSemiaxis = majorSemiaxis;
    m_minorSemiaxis = minorSemiaxis;

    m_area = ChartMathUtils::PI * m_majorSemiaxis * m_minorSemiaxis;
}

qreal ChartEllipse::getArea() const 
{
    return m_area;
}

QPointF ChartEllipse::getCenter() const 
{
    return m_ptCenter;
}

qreal ChartEllipse::calcEllipseSectorFinishAngle(qreal areaSector, qreal startAngle) const
{
    qreal val = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, startAngle);
    qreal angle = 2 * areaSector / (m_majorSemiaxis * m_minorSemiaxis) + val;
    return atanMulTan(m_minorSemiaxis / m_majorSemiaxis, angle);
}

qreal ChartEllipse::atanMulTan(qreal multiplier, qreal tanAngle) const
{
    qreal angle = ChartMathUtils::normalizeRadian(tanAngle);
    qreal pi = ChartMathUtils::PI;

    qreal result = tanAngle;
    if (ChartMathUtils::compare(fabs(angle), pi / 2) != 0 && ChartMathUtils::compare(fabs(angle), pi / 2 * 3) != 0)
    {
        result = atan(multiplier * tan(angle)) + tanAngle - angle;
        if (ChartMathUtils::compare(fabs(angle), pi / 2) > 0 && ChartMathUtils::compare(fabs(angle), pi / 2 * 3) < 0)
            result += ChartMathUtils::sign(tanAngle) * pi;
        else if (ChartMathUtils::compare(fabs(angle), pi / 2 * 3) > 0)
            result += ChartMathUtils::sign(tanAngle) * 2 * pi;
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

qreal ChartEllipse::calcEllipseSectorArea(qreal startAngle, qreal finishAngle) const
{
    qreal val1 = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, finishAngle);
    qreal val2 = atanMulTan(m_majorSemiaxis / m_minorSemiaxis, startAngle);
    return m_majorSemiaxis *  m_minorSemiaxis / 2.0 * (val1 - val2);
}

qreal ChartEllipse::calcEllipseAngleFromCircleAngle(qreal angle) const
{
    return atanMulTan(m_minorSemiaxis / m_majorSemiaxis, angle);
}


/* QtnSpline */
static int lookup( qreal x, const QPolygonF &values )
{
#if 0
#endif
    int i1;
    const int size = ( int )values.size();

    if ( x <= values[0].x() )
        i1 = 0;
    else if ( x >= values[size - 2].x() )
        i1 = size - 2;
    else
    {
        i1 = 0;
        int i2 = size - 2;
        int i3 = 0;

        while ( i2 - i1 > 1 )
        {
            i3 = i1 + ( ( i2 - i1 ) >> 1 );

            if ( values[i3].x() > x )
                i2 = i3;
            else
                i1 = i3;
        }
    }
    return i1;
}

QtnSpline::QtnSpline()
{
    d_data = new PrivateData;
}

QtnSpline::QtnSpline(const QtnSpline& other)
{
    d_data = new PrivateData( *other.d_data );
}

QtnSpline& QtnSpline::operator=( const QtnSpline & other )
{
    *d_data = *other.d_data;
    return *this;
}

QtnSpline::~QtnSpline()
{
    delete d_data;
}

void QtnSpline::setSplineType( SplineType splineType )
{
    d_data->splineType = splineType;
}

QtnSpline::SplineType QtnSpline::splineType() const
{
    return d_data->splineType;
}

bool QtnSpline::setPoints( const QPolygonF& points )
{
    const int size = points.size();
    if ( size <= 2 )
    {
        reset();
        return false;
    }

    d_data->points = points;

    d_data->a.resize( size - 1 );
    d_data->b.resize( size - 1 );
    d_data->c.resize( size - 1 );

    bool ok;
    if ( d_data->splineType == Periodic )
        ok = buildPeriodicSpline( points );
    else
        ok = buildNaturalSpline( points );

    if ( !ok )
        reset();

    return ok;
}

QPolygonF QtnSpline::points() const
{
    return d_data->points;
}

const QVector<qreal> &QtnSpline::coefficientsA() const
{
    return d_data->a;
}

const QVector<qreal> &QtnSpline::coefficientsB() const
{
    return d_data->b;
}

const QVector<qreal> &QtnSpline::coefficientsC() const
{
    return d_data->c;
}

void QtnSpline::reset()
{
    d_data->a.resize( 0 );
    d_data->b.resize( 0 );
    d_data->c.resize( 0 );
    d_data->points.resize( 0 );
}

bool QtnSpline::isValid() const
{
    return d_data->a.size() > 0;
}

qreal QtnSpline::value( qreal x ) const
{
    if ( d_data->a.size() == 0 )
        return 0.0;

    const int i = lookup( x, d_data->points );

    const qreal delta = x - d_data->points[i].x();
    return( ( ( ( d_data->a[i] * delta ) + d_data->b[i] )
        * delta + d_data->c[i] ) * delta + d_data->points[i].y() );
}

bool QtnSpline::buildNaturalSpline( const QPolygonF &points )
{
    int i;

    const QPointF *p = points.data();
    const int size = points.size();

    qreal *a = d_data->a.data();
    qreal *b = d_data->b.data();
    qreal *c = d_data->c.data();

    QVector<qreal> h( size - 1 );
    for ( i = 0; i < size - 1; i++ )
    {
        h[i] = p[i+1].x() - p[i].x();
        if ( h[i] <= 0 )
            return false;
    }

    QVector<qreal> d( size - 1 );
    qreal dy1 = ( p[1].y() - p[0].y() ) / h[0];
    for ( i = 1; i < size - 1; i++ )
    {
        b[i] = c[i] = h[i];
        a[i] = 2.0 * ( h[i-1] + h[i] );

        const qreal dy2 = ( p[i+1].y() - p[i].y() ) / h[i];
        d[i] = 6.0 * ( dy1 - dy2 );
        dy1 = dy2;
    }

    for ( i = 1; i < size - 2; i++ )
    {
        c[i] /= a[i];
        a[i+1] -= b[i] * c[i];
    }

    QVector<qreal> s( size );
    s[1] = d[1];
    for ( i = 2; i < size - 1; i++ )
        s[i] = d[i] - c[i-1] * s[i-1];

    s[size - 2] = - s[size - 2] / a[size - 2];
    for ( i = size - 3; i > 0; i-- )
        s[i] = - ( s[i] + b[i] * s[i+1] ) / a[i];
    s[size - 1] = s[0] = 0.0;

    for ( i = 0; i < size - 1; i++ )
    {
        a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i] );
        b[i] = 0.5 * s[i];
        c[i] = ( p[i+1].y() - p[i].y() ) / h[i]
        - ( s[i+1] + 2.0 * s[i] ) * h[i] / 6.0;
    }

    return true;
}

inline qreal qwtSqr( qreal x )
{
    return x * x;
}

bool QtnSpline::buildPeriodicSpline( const QPolygonF &points )
{
    int i;

    const QPointF *p = points.data();
    const int size = points.size();

    qreal *a = d_data->a.data();
    qreal *b = d_data->b.data();
    qreal *c = d_data->c.data();

    QVector<qreal> d( size - 1 );
    QVector<qreal> h( size - 1 );
    QVector<qreal> s( size );

    for ( i = 0; i < size - 1; i++ )
    {
        h[i] = p[i+1].x() - p[i].x();
        if ( h[i] <= 0.0 )
            return false;
    }

    const int imax = size - 2;
    qreal htmp = h[imax];
    qreal dy1 = ( p[0].y() - p[imax].y() ) / htmp;
    for ( i = 0; i <= imax; i++ )
    {
        b[i] = c[i] = h[i];
        a[i] = 2.0 * ( htmp + h[i] );
        const qreal dy2 = ( p[i+1].y() - p[i].y() ) / h[i];
        d[i] = 6.0 * ( dy1 - dy2 );
        dy1 = dy2;
        htmp = h[i];
    }

    a[0] = qSqrt( a[0] );
    c[0] = h[imax] / a[0];
    qreal sum = 0;

    for ( i = 0; i < imax - 1; i++ )
    {
        b[i] /= a[i];
        if ( i > 0 )
            c[i] = - c[i-1] * b[i-1] / a[i];
        a[i+1] = qSqrt( a[i+1] - qwtSqr( b[i] ) );
        sum += qwtSqr( c[i] );
    }
    b[imax-1] = ( b[imax-1] - c[imax-2] * b[imax-2] ) / a[imax-1];
    a[imax] = qSqrt( a[imax] - qwtSqr( b[imax-1] ) - sum );


    s[0] = d[0] / a[0];
    sum = 0;
    for ( i = 1; i < imax; i++ )
    {
        s[i] = ( d[i] - b[i-1] * s[i-1] ) / a[i];
        sum += c[i-1] * s[i-1];
    }
    s[imax] = ( d[imax] - b[imax-1] * s[imax-1] - sum ) / a[imax];


    s[imax] = - s[imax] / a[imax];
    s[imax-1] = -( s[imax-1] + b[imax-1] * s[imax] ) / a[imax-1];
    for ( i = imax - 2; i >= 0; i-- )
        s[i] = - ( s[i] + b[i] * s[i+1] + c[i] * s[imax] ) / a[i];

    s[size-1] = s[0];
    for ( i = 0; i < size - 1; i++ )
    {
        a[i] = ( s[i+1] - s[i] ) / ( 6.0 * h[i] );
        b[i] = 0.5 * s[i];
        c[i] = ( p[i+1].y() - p[i].y() )
            / h[i] - ( s[i+1] + 2.0 * s[i] ) * h[i] / 6.0;
    }
    return true;
}


/* QtnSplineCurveFitter */
QtnSplineCurveFitter::QtnSplineCurveFitter()
{
    d_data = new PrivateData;
}

QtnSplineCurveFitter::~QtnSplineCurveFitter()
{
    delete d_data;
}

void QtnSplineCurveFitter::setFitMode( FitMode mode )
{
    d_data->fitMode = mode;
}

QtnSplineCurveFitter::FitMode QtnSplineCurveFitter::fitMode() const
{
    return d_data->fitMode;
}

void QtnSplineCurveFitter::setSpline( const QtnSpline &spline )
{
    d_data->spline = spline;
    d_data->spline.reset();
}

const QtnSpline &QtnSplineCurveFitter::spline() const
{
    return d_data->spline;
}

QtnSpline& QtnSplineCurveFitter::spline()
{
    return d_data->spline;
}

void QtnSplineCurveFitter::setSplineSize( int splineSize )
{
    d_data->splineSize = qMax( splineSize, 10 );
}

int QtnSplineCurveFitter::splineSize() const
{
    return d_data->splineSize;
}

QPolygonF QtnSplineCurveFitter::fitCurve( const QPolygonF &points ) const
{
    const int size = ( int )points.size();
    if ( size <= 2 )
        return points;

    FitMode fitMode = d_data->fitMode;
    if ( fitMode == Auto )
    {
        fitMode = Spline;

        const QPointF *p = points.data();
        for ( int i = 1; i < size; i++ )
        {
            if ( p[i].x() <= p[i-1].x() )
            {
                fitMode = ParametricSpline;
                break;
            }
        };
    }

    if ( fitMode == ParametricSpline )
        return fitParametric( points );
    else
        return fitSpline( points );
}

QPolygonF QtnSplineCurveFitter::fitSpline(const QPolygonF& points) const
{
    d_data->spline.setPoints( points );
    if ( !d_data->spline.isValid() )
        return points;

    QPolygonF fittedPoints( d_data->splineSize );

    const qreal x1 = points[0].x();
    const qreal x2 = points[int( points.size() - 1 )].x();
    const qreal dx = x2 - x1;
    const qreal delta = dx / ( d_data->splineSize - 1 );

    for ( int i = 0; i < d_data->splineSize; i++ )
    {
        QPointF &p = fittedPoints[i];

        const qreal v = x1 + i * delta;
        const qreal sv = d_data->spline.value( v );

        p.setX( v );
        p.setY( sv );
    }
    d_data->spline.reset();

    return fittedPoints;
}

QPolygonF QtnSplineCurveFitter::fitParametric(const QPolygonF& points) const
{
    int i;
    const int size = points.size();

    QPolygonF fittedPoints( d_data->splineSize );
    QPolygonF splinePointsX( size );
    QPolygonF splinePointsY( size );

    const QPointF *p = points.data();
    QPointF *spX = splinePointsX.data();
    QPointF *spY = splinePointsY.data();

    qreal param = 0.0;
    for ( i = 0; i < size; i++ )
    {
        const qreal x = p[i].x();
        const qreal y = p[i].y();
        if ( i > 0 )
        {
            const qreal delta = qSqrt( qwtSqr( x - spX[i-1].y() )
                + qwtSqr( y - spY[i-1].y() ) );
            param += qMax( delta, 1.0 );
        }
        spX[i].setX( param );
        spX[i].setY( x );
        spY[i].setX( param );
        spY[i].setY( y );
    }

    d_data->spline.setPoints( splinePointsX );
    if ( !d_data->spline.isValid() )
        return points;

    const qreal deltaX =
        splinePointsX[size - 1].x() / ( d_data->splineSize - 1 );
    for ( i = 0; i < d_data->splineSize; i++ )
    {
        const qreal dtmp = i * deltaX;
        fittedPoints[i].setX( d_data->spline.value( dtmp ) );
    }

    d_data->spline.setPoints( splinePointsY );
    if ( !d_data->spline.isValid() )
        return points;

    const qreal deltaY =
        splinePointsY[size - 1].x() / ( d_data->splineSize - 1 );
    for ( i = 0; i < d_data->splineSize; i++ )
    {
        const qreal dtmp = i * deltaY;
        fittedPoints[i].setY( d_data->spline.value( dtmp ) );
    }

    return fittedPoints;
}
