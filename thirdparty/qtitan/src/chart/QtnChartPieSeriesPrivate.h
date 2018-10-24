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
#ifndef QTN_CHARTPIESERIESPRIVATE_H
#define QTN_CHARTPIESERIESPRIVATE_H

#include "QtnChartArea.h"
#include "QtnChartAreaPrivate.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartPieSeries.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartMath.h"

namespace Qtitan
{
    class ChartEllipse;
    /* ChartPieDrawHelper */
    class ChartPieDrawHelper
    {
    public:
        class BoundingRect
        {
        public:
            BoundingRect() { xMin = INT_MAX, xMax = -INT_MAX, yMin = INT_MAX, yMax = -INT_MAX; }
        public:
            void update(const QPointF& point)
            {
                xMax = qMax(xMax, point.x()); yMax = qMax(yMax, point.y());
                xMin = qMin(xMin, point.x()); yMin = qMin(yMin, point.y());
            }
        public:
            qreal xMin, xMax, yMin, yMax;
        };

    public:
        ChartPieDrawHelper(qreal beginAngle, qreal endAngle, const ChartEllipse& ellipse, int depthPercent, int holePercent);

    public:
        QPointF calculateCenter(const QPointF& basePoint) const;

    public:
        void drawPie(QPainter* p, const QColor color, const QColor color2, const QPointF& basePoint);
        void drawBoundedPie(QPainter* p, const QColor color, int thickness, const QPointF& basePoint);
        void drawTorus(QPainter* p, const QColor color, const QColor color2);

        qreal halfAngle() const;
        qreal beginAngle() const;

        QPointF endPoint() const;
        QPointF startPoint() const;

        QPointF centerPoint() const;

        QRectF boundingRect() const;

        int holePercent() const;
        qreal calcAxisPercent(qreal factor);

    protected:
        void updateBoundingRect();
        void updateBoundingRect(ChartEllipse& ellipse, BoundingRect& boundingRect);

    protected:
        ChartEllipse m_ellipse;
        qreal m_beginAngle;
        qreal m_sweepAngle;
        qreal m_depth;
        int m_holePercent;
        QRectF m_boundingRect;
    };

    class ChartPieAreaPrivate;
    /* ChartPieArea */
    class ChartPieArea : public ChartArea
    {
        Q_OBJECT
    public:
        ChartPieArea(QObject* parent = Q_NULL);
        virtual ~ChartPieArea();
    private:
        Q_DISABLE_COPY(ChartPieArea)
        QTN_DECLARE_EX_PRIVATE(ChartPieArea)
    };

    /* ChartPieAreaPrivate */
    class ChartPieAreaPrivate : public ChartAreaPrivate
    {
    public:
        QTN_DECLARE_EX_PUBLIC(ChartPieArea)
    public:
        explicit ChartPieAreaPrivate(){}
        virtual ~ChartPieAreaPrivate(){}

    public:
        virtual ChartAreaItem* createItem(GraphicsItem* parent);
        virtual void calculateSeriesLayout(ChartAreaItem* item);

    private:
        Q_DISABLE_COPY(ChartPieAreaPrivate)
    };

    class GraphicsItem;
    class ChartAreaItem;
    /* ChartPieSeriesItem */
    class ChartPieSeriesItem : public ChartSeriesItem
    {
    public:
        explicit ChartPieSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual ~ChartPieSeriesItem();

    public:
        void calculatePointLayout(const QRect& boundingRect);
        void calculateLabelLayout();

        QRectF internalBoundingRect() const;
        virtual void calcInternalArea(const QRectF& boundingRect);

    protected:
        virtual ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

        qreal calculateAmountValues() const;
        void setupValuesSlices(qreal sum);
        void calculateValues();

    protected:
        QRectF m_internalRect; 

    private:
        Q_DISABLE_COPY(ChartPieSeriesItem)
    };

    /* ChartPieSeriesSliceItem */
    class ChartPieSeriesSliceItem : public ChartSeriesPointItem
    {
    public:
        explicit ChartPieSeriesSliceItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual ~ChartPieSeriesSliceItem();

    public:
        void calculateLayout(qreal& angle);
        ChartPieDrawHelper* getPie() const;
        QPointF basePoint() const;

    protected:
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    public:
        qreal m_value;
        ChartPieDrawHelper* m_pieDrawHelper;
        QPointF m_basePoint;

    private:
        friend class ChartPieSeriesItem;
        Q_DISABLE_COPY(ChartPieSeriesSliceItem)
    };

    /* ChartPieSeriesBasePrivate */
    class ChartPieSeriesBasePrivate : public AbstractSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPieSeriesBase)
    public:
        explicit ChartPieSeriesBasePrivate();
        virtual ~ChartPieSeriesBasePrivate();
    public:
        virtual void linkData();
        virtual void unlinkData();
    public:
        int m_holePercent;
        int m_explodedRelativeSize;
        int m_rotation;
        ChartDataTablePrivate* m_data;

    private:
        Q_DISABLE_COPY(ChartPieSeriesBasePrivate)
    };

    /* ChartPieSeriesPrivate */
    class ChartPieSeriesPrivate : public ChartPieSeriesBasePrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPieSeries)
    public:
        explicit ChartPieSeriesPrivate();
        virtual ~ChartPieSeriesPrivate();
    public:
        void init();
    public:
        virtual ChartArea* createArea(QObject* parent);
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
    public:
        ChartBorder* m_border;

    private:
        Q_DISABLE_COPY(ChartPieSeriesPrivate)
    };

    /* ChartPieAreaItem */
    class ChartPieAreaItem : public ChartAreaItem
    {
    public:
        explicit ChartPieAreaItem(ChartArea* area, GraphicsItem* parent);
    private:
        Q_DISABLE_COPY(ChartPieAreaItem)
    };

    /* ChartSliceLabelItem */
    class ChartSliceLabelItem : public ChartSeriesLabelItem
    {
    public:
        explicit ChartSliceLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);

    protected:
        virtual void calculateLayout();
        virtual void paint(QPainter* p);

    protected:
        ChartPieSeriesLabel::PieLabelPosition position() const;

    private:
        Q_DISABLE_COPY(ChartSliceLabelItem)
    };

    /* ChartPieSeriesLabelPrivate */
    class ChartPieSeriesLabelPrivate : public ChartSeriesLabelPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPieSeriesLabel)
    public:
        explicit ChartPieSeriesLabelPrivate();
        virtual ~ChartPieSeriesLabelPrivate();
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
        virtual QPointF calculateLocationPointAndAngles(ChartPieSeriesSliceItem* pointItem, int borderThickness, qreal& lineAngle);
    public:
        ChartPieSeriesLabel::PieLabelPosition m_position;
    private:
        Q_DISABLE_COPY(ChartPieSeriesLabelPrivate)
    };

    /* ChartPieLabelsItem */
    class ChartPieLabelsItem : public GraphicsItem
    {
    public:
        explicit ChartPieLabelsItem(GraphicsItem* parentItem);
    public:
        void addLabelRectangle(QRectF& rc);
        QPointF calculateOffset(qreal lineAngle, const QRectF& rc);
        virtual void paint(QPainter* p);

    protected:
        QVector<QRectF> m_arrLabels;
    private:
        Q_DISABLE_COPY(ChartPieLabelsItem)
    };
};


#endif // QTN_CHARTPIESERIESPRIVATE_H
