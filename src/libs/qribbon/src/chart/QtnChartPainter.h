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
#ifndef QTN_CHARTPAINTER_H
#define QTN_CHARTPAINTER_H

#include <QObject>
#include <QBrush>
#include "QtnChartDefines.h"

class QPainter;
class QRectF;
class QColor;
class QPointF;
class QPoints;
class QBrush;
class QFont;
class QDomElement;
class QGradient;
class QLinearGradient;

namespace Qtitan
{
    class ChartFillDrawHelperPrivate;
    /* ChartFillDrawHelper */
    class ChartFillDrawHelper : public QObject
    {
    public:
        enum GradientDirection
        {
            TopToBottom,
            BottomToTop,
            LeftToRight,
            RightToLeft,
            TopLeftToBottomRight,
            BottomRightToTopLeft,
            TopRightToBottomLeft,
            BottomLeftToTopRight,
            ToCenterHorizontal,
            FromCenterHorizontal,
            ToCenterVertical,
            FromCenterVertical,
        };
    public:
        ChartFillDrawHelper(QObject* parent = Q_NULL);
        virtual ~ChartFillDrawHelper();

    public:
        void setFillStyle(Qt::BrushStyle fillMode);
        Qt::BrushStyle fillStyle() const;

        void setGradientDirection(GradientDirection direction);
        GradientDirection getGradientDirection() const;

        QBrush brushGradient(const QRectF& rc, const QColor& color, const QColor& color2)  const;

        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        void drawGradientRect(QPainter* p, const QRectF& boundingRect, int xround = 0, int yround = 0);
        void drawGradientRect(QPainter* p, const QRectF& boundingRect, const QColor& color, const QColor& color2, int xround = 0, int yround = 0);
        void drawGradientPolygon(QPainter* p, const QVector<QPointF>& arrPoints, const QColor& color, const QColor& color2);
        void drawGradientCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, const QColor& color2);
        void drawFillSplinePolygon(QPainter* p, const QVector<QPointF>& arrPoints, const QColor& color, const QColor& color2, bool twoSides = false);

    public:
        ChartFillDrawHelper* createRotatedHelper();

    private:
        Q_DISABLE_COPY(ChartFillDrawHelper)
        QTN_DECLARE_PRIVATE(ChartFillDrawHelper)
    };

    class ChartLineDrawHelperPrivate;
    /* ChartLineDrawHelper */
    class ChartLineDrawHelper : public QObject
    {
        Q_OBJECT
    public:
        explicit ChartLineDrawHelper(QObject* parent = Q_NULL);
        ~ChartLineDrawHelper();

    public:
        void setThickness(int thickness);
        int thickness() const;

        void setDashStyle(DashStyle dashStyle);
        DashStyle dashStyle() const;

    public:
        void draw(QPainter* p, const QPointF& point1, const QPointF& point2, const QColor& color);
        void draw(QPainter* p, const QVector<QPointF>& points, const QColor& color);
        void drawSpline(QPainter* p, const QVector<QPointF>& points, const QColor& color);

    private:
        Q_DISABLE_COPY(ChartLineDrawHelper)
        QTN_DECLARE_PRIVATE(ChartLineDrawHelper)
    };


    class ChartGeometry;
    class ChartPainterPrivate;
    /* ChartPainter */
    class ChartPainter : public QObject
    {
        Q_OBJECT
    public:
        static ChartPainter* painter();

        enum ChartLinearGradientMode
        {
            ChartLinearGradientModeHorizontal,
            ChartLinearGradientModeVertical,
            ChartLinearGradientModeForwardDiagonal,
            ChartLinearGradientModeBackwardDiagonal,
            ChartLinearGradientModeCenterHorizontal,
            ChartLinearGradientModeCenterVertical,
        };

    protected:
        explicit ChartPainter(QObject* parent = Q_NULL);
        virtual ~ChartPainter();

    public:
        void setAntialiasing(QPainter* p, bool antiAlias = true);
        void unsetAntialiasing(QPainter* p);

        void setTextAntialiasing(QPainter* p, bool antiAlias = true);
        void unsetTextAntialiasing(QPainter* p);

        void pushTransform(QPainter* p, qreal dx, qreal dy, qreal dz);
        void popTransform(QPainter* p);

        void pushRotateTransform(QPainter* p, qreal angle);
        void popRotateTransform(QPainter* p);

        void drawFillRect(QPainter* p, const QRectF& rect, const QColor& color, int xround = 0, int yround = 0) const;
        void drawBackground(QPainter* p, const QRectF& rect, const QColor& color) const;
        void drawGradientBackground(QPainter* p, const QRectF& rect, const QBrush& brush, int xround = 0, int yround = 0) const;
        void drawBoundedRect(QPainter* p, const QRectF& rect, const QColor& color, int width, int xround = 0, int yround = 0) const;
        void drawInnerBorder(QPainter* p, const QRectF& rect, const QColor& color, int width, int xround = 0, int yround = 0) const;
        void drawGradientRectangle(QPainter* p, const QRectF& rect, const QColor& color, const QColor& color2, 
            ChartLinearGradientMode mode, int xround = 0, int yround = 0) const;

        void drawDashedSpline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width, DashStyle dashStyle) const;
        void drawFillSpline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width) const;
        void drawDashedLine(QPainter* p, const QPointF& p1, const QPointF& p2, const QColor& color, int width, DashStyle dashStyle) const;
        void drawFillLine(QPainter* p, const QPointF& p1, const QPointF& p2, const QColor& color, qreal width) const;
        void drawFillPolyline(QPainter* p, const QVector<QPointF>& pnts, const QColor& color, int width) const;

        void drawBoundedPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, int width) const;
        void drawGradientPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const;
        void drawFillSplinePolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color, bool twoSides) const;
        void drawFillPolygon(QPainter* p, const QVector<QPointF>& points, const QColor& color) const;
        void drawFillPolygon(QPainter* p, const QVector<QPointF>& points, const QBrush& brFill) const;

        void drawBoundedCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, int width) const;
        void drawBoundedEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QColor& color, int width) const;
        void drawGradientCircle(QPainter* p, const QPointF& center, qreal radius, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const;
        void drawGradientEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QColor& color, const QColor& color2, ChartLinearGradientMode mode) const;
        void drawGradientEllipse(QPainter* p, const QPointF& center, qreal radiusX, qreal radiusY, const QBrush& br) const;

        void drawBoundedPie(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, 
            qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QColor& color, int width) const;
        void drawGradientPie(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, 
            qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2) const;
        void drawGradientTorus(QPainter* p, const QPointF& center, qreal majorSemiAxis, qreal minorSemiAxis, 
            qreal beginAngle, qreal sweepAngle, qreal depth, int holePercent, const QRectF& gradientBounds, const QColor& color, const QColor& color2) const;

        void drawBoundedText(QPainter* p, const QString& strText, const QFont& font, const QColor& color, const QRectF& rect) const;

        void drawFunnel(QPainter* p, const QRectF& rectFunnel, qreal neckHeight, qreal neckWidth, const QBrush& brFill, const QColor& brLine, qreal y1, qreal y2) const;
        void drawFunnel3D(QPainter* p, const QRectF& rectFunnel, qreal dblDepth, qreal neckHeight, qreal neckWidth, const QBrush& brFill, const QColor& brLine,
            qreal y1, qreal y2, bool bIsCircularBase = true, const QBrush& brTopFill = QBrush()) const;

        void drawPyramid(QPainter* p, const QRectF& rect, qreal y1, qreal y2, const QBrush& brushfill, const QColor& lineColor, bool isTransparent) const; 
        void drawPyramid3D(QPainter* p, const QRectF& rectPyramid, qreal dblDepth,
            const QBrush& brSideFillLeft, const QBrush& brSideFillRight, const QColor& brLine,
            qreal dblXCenterOffset, qreal y1, qreal y2, bool isTransparent, bool bIsCircularBase = false, const QBrush& brTopFill = QBrush()) const;

        void drawFillPolygon(QPainter* p, const QVector<QPointF>& pts, const QBrush& brFill, const QColor& brLine) const;

        void buildContourPath(QPainterPath& path, const ChartGeometry* geometry) const;
        void drawContour(QPainter* p, const ChartGeometry* geometry, const QColor& color, qreal lineWidth = 1.0) const;
        void drawFillContour(QPainter* p, const ChartGeometry* geometry, const QBrush& brFill) const;
                                            

    private:
        Q_DISABLE_COPY(ChartPainter)
        QTN_DECLARE_PRIVATE(ChartPainter)
    };

}; //namespace Qtitan


#endif // QTN_CHARTPAINTER_H
