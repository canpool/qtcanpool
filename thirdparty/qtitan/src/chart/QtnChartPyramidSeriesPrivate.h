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
#ifndef QTN_CHARTPYRAMIDSERIESPRIVATE_H
#define QTN_CHARTPYRAMIDSERIESPRIVATE_H

#include "QtnSeriesPrivate.h"
#include "QtnChartPyramidSeries.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartAreaPrivate.h"


namespace Qtitan
{
    /* ChartPieSeriesBasePrivate */
    class ChartPyramidSeriesPrivate : public AbstractSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPyramidSeries)
    public:
        explicit ChartPyramidSeriesPrivate();
        virtual ~ChartPyramidSeriesPrivate();
        void init();

    public:
        virtual void linkData();
        virtual void unlinkData();
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual ChartArea* createArea(QObject* parent);
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        bool m_isSeries3D;
        bool m_isCircular;
        int m_pointGap;
        int m_depthPercent;
        qreal m_angle;
        ChartBorder* m_border;
        ChartFillDrawHelper* m_fillHelper;
        int m_transparency;

        ChartDataTablePrivate* m_data;
    private:
        Q_DISABLE_COPY(ChartPyramidSeriesPrivate)
    };

    /* ChartPyramidSeriesItem */
    class ChartPyramidSeriesItem : public ChartSeriesItem
    {
        Q_OBJECT
    public:
        ChartPyramidSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual ~ChartPyramidSeriesItem();

    public:
        virtual void calculatePointLayout(const QRect& boundingRect);
        void calculateLabelLayout();
        QRectF internalBoundingRect() const;

    protected:
        virtual ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

        bool calculateSeriesParams(const QRect& rectArea, qreal& sum, int& nonEmptyCount, QRectF& rectShape);
        void sortDataPoints(int screenPointIndex, QVector<ChartDataPoint*>& dataPointsSorted);
        int findDataPointIndex(ChartDataPoint* dataPoint) const;

    public:
        void drawLegendComponent(QPainter* p, QRect boundingRect, const QColor& startColor, 
            const QColor& finalColor, const QColor& clrBorder);

    protected:
        QRectF m_internalRect;
        QRectF m_maxDataLabelSize;
        int m_allowedPointGap;
    private:
        Q_DISABLE_COPY(ChartPyramidSeriesItem)
    };

    /* ChartPyramidSeriesPartItem */
    class ChartPyramidSeriesPartItem : public ChartSeriesPointItem
    {
    public:
        enum Pyramid
        {
            pntHeight,
            pntOffsets,
            pntLeftTop,
            pntRightBottom,
        };
    public:
        ChartPyramidSeriesPartItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual ~ChartPyramidSeriesPartItem();

    public:
        void calculateBoundingRect();
        void calculatePyramidPoints(const QRectF& rectPyramid, qreal depth, qreal xCenterOffset, 
            qreal y1, qreal y2, QPointF& pointLeft, QPointF& pointRight);

        void setDataPointScreenPoint(int screenPointIndex, const QPointF& pt);
        QPointF dataPointScreenPoint(int screenPointIndex) const;

        QColor borderColor() const;

    public:
        virtual QPointF markerPoint();

    protected:
        virtual void paint(QPainter* p);
        void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    public:
        bool m_isValidRect;
        QPointF m_pointHeight;
        QPointF m_pointOffsets;
        QPointF m_pointLeftTop;
        QPointF m_pointRightBottom;
        QRectF m_rectBounds;
    private:
        friend class ChartPyramidSeriesItem;
        Q_DISABLE_COPY(ChartPyramidSeriesPartItem)
    };


    class ChartPyramidAreaPrivate;
    /* ChartPieArea */
    class ChartPyramidArea : public ChartArea
    {
        Q_OBJECT
    public:
        explicit ChartPyramidArea(QObject* parent = Q_NULL);
        virtual ~ChartPyramidArea();
    private:
        Q_DISABLE_COPY(ChartPyramidArea)
        QTN_DECLARE_EX_PRIVATE(ChartPyramidArea)
    };

    /* ChartPyramidAreaPrivate */
    class ChartPyramidAreaPrivate : public ChartAreaPrivate
    {
    public:
        QTN_DECLARE_EX_PUBLIC(ChartPyramidArea)
    public:
        explicit ChartPyramidAreaPrivate(){}
        virtual ~ChartPyramidAreaPrivate(){}

    public:
        virtual ChartAreaItem* createItem(GraphicsItem* parent);
        virtual void calculateSeriesLayout(ChartAreaItem* item);

    private:
        Q_DISABLE_COPY(ChartPyramidAreaPrivate)
    };

    /* ChartPyramidAreaItem */
    class ChartPyramidAreaItem : public ChartAreaItem
    {
    public:
        explicit ChartPyramidAreaItem(ChartArea* area, GraphicsItem* parent);
    private:
        Q_DISABLE_COPY(ChartPyramidAreaItem)
    };


    /* ChartPyramidSeriesLabelPrivate */
    class ChartPyramidSeriesLabelPrivate : public ChartSeriesLabelPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPyramidSeriesLabel)
    public:
        explicit ChartPyramidSeriesLabelPrivate();
        virtual ~ChartPyramidSeriesLabelPrivate();

    public:
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
    public:
        LabelPosition m_nPosition;
    private:
        Q_DISABLE_COPY(ChartPyramidSeriesLabelPrivate)
    };

    /* ChartPyramidSeriesLabelItem */
    class ChartPyramidSeriesLabelItem : public ChartSeriesLabelItem
    {
    public:
        ChartPyramidSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);

    protected:
        virtual void calculateLayout();
        virtual void paint(QPainter* p);

    protected:
        LabelPosition position() const;
    private:
        Q_DISABLE_COPY(ChartPyramidSeriesLabelItem)
    };

};

#endif // QTN_CHARTPYRAMIDSERIESPRIVATE_H
