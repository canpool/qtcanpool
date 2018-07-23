#include "SmoothCurveCreator.h"

QPainterPath SmoothCurveCreator::createSmoothCurve(const QList<QPointF> &points) {
    QPainterPath path;
    int len = points.size();

    if (len < 2) {
        return path;
    }

    QList<QPointF> firstControlPoints;
    QList<QPointF> secondControlPoints;
    calculateControlPoints(points, &firstControlPoints, &secondControlPoints);

    path.moveTo(points[0].x(), points[0].y());

    // Using bezier curve to generate a smooth curve.
    for (int i = 0; i < len - 1; ++i) {
        path.cubicTo(firstControlPoints[i], secondControlPoints[i], points[i+1]);
    }

    return path;
}

void SmoothCurveCreator::calculateFirstControlPoints(double *&result,
                                                     const double *rhs,
                                                     int n) {
    result = new double[n];
    double *tmp = new double[n];
    double b = 2.0;
    result[0] = rhs[0] / b;

    // Decomposition and forward substitution.
    for (int i = 1; i < n; i++) {
        tmp[i] = 1 / b;
        b = (i < n - 1 ? 4.0 : 3.5) - tmp[i];
        result[i] = (rhs[i] - result[i - 1]) / b;
    }

    for (int i = 1; i < n; i++) {
        result[n - i - 1] -= tmp[n - i] * result[n - i]; // Backsubstitution.
    }

    delete tmp;
}

void SmoothCurveCreator::calculateControlPoints(const QList<QPointF> &knots,
                                                QList<QPointF> *firstControlPoints,
                                                QList<QPointF> *secondControlPoints) {
    int n = knots.size() - 1;

    for (int i = 0; i < n; ++i) {
        firstControlPoints->append(QPointF());
        secondControlPoints->append(QPointF());
    }

    if (n == 1) {
        // Special case: Bezier curve should be a straight line.
        // P1 = (2P0 + P3) / 3
        (*firstControlPoints)[0].rx() = (2 * knots[0].x() + knots[1].x()) / 3;
        (*firstControlPoints)[0].ry() = (2 * knots[0].y() + knots[1].y()) / 3;

        // P2 = 2P1 – P0
        (*secondControlPoints)[0].rx() = 2 * (*firstControlPoints)[0].x() - knots[0].x();
        (*secondControlPoints)[0].ry() = 2 * (*firstControlPoints)[0].y() - knots[0].y();

        return;
    }

    // Calculate first Bezier control points
    double *xs = 0;
    double *ys = 0;
    double *rhsx = new double[n]; // Right hand side vector
    double *rhsy = new double[n]; // Right hand side vector

    // Set right hand side values
    for (int i = 1; i < n - 1; ++i) {
        rhsx[i] = 4 * knots[i].x() + 2 * knots[i + 1].x();
        rhsy[i] = 4 * knots[i].y() + 2 * knots[i + 1].y();
    }
    rhsx[0] = knots[0].x() + 2 * knots[1].x();
    rhsx[n - 1] = (8 * knots[n - 1].x() + knots[n].x()) / 2.0;
    rhsy[0] = knots[0].y() + 2 * knots[1].y();
    rhsy[n - 1] = (8 * knots[n - 1].y() + knots[n].y()) / 2.0;

    // Calculate first control points coordinates
    calculateFirstControlPoints(xs, rhsx, n);
    calculateFirstControlPoints(ys, rhsy, n);

    // Fill output control points.
    for (int i = 0; i < n; ++i) {
        (*firstControlPoints)[i].rx() = xs[i];
        (*firstControlPoints)[i].ry() = ys[i];

        if (i < n - 1) {
            (*secondControlPoints)[i].rx() = 2 * knots[i + 1].x() - xs[i + 1];
            (*secondControlPoints)[i].ry() = 2 * knots[i + 1].y() - ys[i + 1];
        } else {
            (*secondControlPoints)[i].rx() = (knots[n].x() + xs[n - 1]) / 2;
            (*secondControlPoints)[i].ry() = (knots[n].y() + ys[n - 1]) / 2;
        }
    }

    delete xs;
    delete ys;
    delete rhsx;
    delete rhsy;
}