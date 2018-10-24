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
#ifndef QTN_CHARTFUNNELSERIESPRIVATE_H
#define QTN_CHARTFUNNELSERIESPRIVATE_H

#include "QtnChartArea.h"
#include "QtnChartAreaPrivate.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartFunnelSeries.h"
#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartPyramidSeriesPrivate.h"
#include "QtnChartMath.h"

namespace Qtitan
{
    class ChartBorder;
    class ChartFillDrawHelper;
    /* ChartFunnelSeriesPrivate */
    class ChartFunnelSeriesPrivate : public ChartPyramidSeriesPrivate
    {
    public:
        QTN_DECLARE_EX_PUBLIC(ChartFunnelSeries)
    public:
        explicit ChartFunnelSeriesPrivate();
        virtual ~ChartFunnelSeriesPrivate();
        void init();
    public:
        virtual void linkData();
        virtual void unlinkData();

        virtual ChartArea* createArea(QObject* parent);
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        double m_neckHeight;
        bool m_neckHeightValue;
        int m_neckWidth;
        int m_interval;
        ChartBorder* m_border;
        ChartFillDrawHelper* m_fillHelper;

    private:
        Q_DISABLE_COPY(ChartFunnelSeriesPrivate)
    };

    /* ChartFunnelSeriesItem */
    class ChartFunnelSeriesItem : public ChartPyramidSeriesItem
    {
        Q_OBJECT
    public:
        explicit ChartFunnelSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual ~ChartFunnelSeriesItem();

    public:
        void calculatePointLayout(const QRect& boundingRect);
        void calculateLabelLayout();
        QRectF internalBoundingRect() const;

    protected:
        virtual void createPointsItem();
        virtual ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    public:
        void drawLegendComponent(QPainter* p, QRect boundingRect, const QColor& startColor, 
            const QColor& finalColor, const QColor& clrBorder);
    public:
        QRectF m_internalRect;
        QSizeF m_neckSize;

    private:
        Q_DISABLE_COPY(ChartFunnelSeriesItem)
    };

    /* ChartFunnelSeriesPartItem */
    class ChartFunnelSeriesPartItem : public ChartPyramidSeriesPartItem
    {
    public:
        explicit ChartFunnelSeriesPartItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual ~ChartFunnelSeriesPartItem();

    public:
        virtual QPointF markerPoint();
        void calculateFunnelPoints(const QRectF& rectFunnel, qreal depth, const QSizeF& szNeckSize, 
            qreal y1, qreal y2, QPointF& pointLeft, QPointF& pointRight);

    protected:
        virtual void paint(QPainter* p);
        void drawLegendComponent(QPainter* p, const QRect& boundingRect);
    private:
        Q_DISABLE_COPY(ChartFunnelSeriesPartItem)
        friend class ChartFunnelSeriesItem;
    };

    class ChartFunnelAreaPrivate;
    /* ChartPieArea */
    class ChartFunnelArea : public ChartArea
    {
        Q_OBJECT
    public:
        explicit ChartFunnelArea(QObject* parent = Q_NULL);
        virtual ~ChartFunnelArea();
    private:
        Q_DISABLE_COPY(ChartFunnelArea)
        QTN_DECLARE_EX_PRIVATE(ChartFunnelArea)
    };

    /* ChartFunnelAreaPrivate */
    class ChartFunnelAreaPrivate : public ChartAreaPrivate
    {
    public:
        QTN_DECLARE_EX_PUBLIC(ChartFunnelArea)
    public:
        explicit ChartFunnelAreaPrivate(){}
        virtual ~ChartFunnelAreaPrivate(){}

    public:
        virtual ChartAreaItem* createItem(GraphicsItem* parent);
        virtual void calculateSeriesLayout(ChartAreaItem* item);

    private:
        Q_DISABLE_COPY(ChartFunnelAreaPrivate)
    };

    /* ChartFunnelAreaItem */
    class ChartFunnelAreaItem : public ChartAreaItem
    {
    public:
        explicit ChartFunnelAreaItem(ChartArea* area, GraphicsItem* parent);
    private:
        Q_DISABLE_COPY(ChartFunnelAreaItem)
    };

    /* ChartFunnelSeriesLabel */
    class ChartFunnelSeriesLabel : public ChartPyramidSeriesLabel
    {
        Q_OBJECT
    public:
        explicit ChartFunnelSeriesLabel();
        virtual ~ChartFunnelSeriesLabel();

    private:
        Q_DISABLE_COPY(ChartFunnelSeriesLabel)
    };

};

#endif // QTN_CHARTFUNNELSERIESPRIVATE_H
