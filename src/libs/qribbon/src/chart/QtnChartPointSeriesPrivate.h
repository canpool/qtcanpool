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
#ifndef QTN_CHARTPOINTSERIESPRIVATE_H
#define QTN_CHARTPOINTSERIESPRIVATE_H

#include <QRect>
#include <QColor>
#include <QVector>

#include "QtitanDef.h"

#include "Qtn2DSeriesPrivate.h"
#include "QtnChartPointSeries.h"
#include "QtnChartBubbleSeries.h"

namespace Qtitan
{
    class ChartMarker;
    /* ChartPointSeries */
    class ChartPointSeriesPrivate : public Chart2DSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartPointSeries)
    public:
        explicit ChartPointSeriesPrivate();
        virtual ~ChartPointSeriesPrivate();

    public:
        void init();

    public:
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        ChartMarker* m_marker;
    private:
        Q_DISABLE_COPY(ChartPointSeriesPrivate)
    };

    class GraphicsItem;
    /* ChartPointSeriesItem */
    class ChartPointSeriesItem : public ChartArea2DSeriesItem
    {
    public:
        explicit ChartPointSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);

    public:
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);
    private:
        Q_DISABLE_COPY(ChartPointSeriesItem)
    };

    /* ChartSeriesPointItem */
    class ChartSeriesPointItem : public GraphicsItem, public ChartLegendInterface
    {
    public:
        explicit ChartSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual ~ChartSeriesPointItem();

    public:
        ChartDataPoint* point() const;
        ChartSeriesItem* seriesItem() const;
        QColor startColor() const;
        QColor finalColor() const;
        qreal internalValue() const;

    public:
        virtual QString legendName() const;
        virtual void drawLegendComponent(const QRect& boundingRect);

        virtual void updateMinMaxRange(qreal& minValue, qreal& maxValue) const;
    public:
        qreal m_internalValue;

    protected:
        ChartDataPoint* m_point;

    private:
        friend class ChartSeriesItem;
        Q_DISABLE_COPY(ChartSeriesPointItem)
    };

    /* ChartPointSeriesPointItem */
    class ChartPointSeriesPointItem : public ChartSeriesPointItem
    {
    public:
        explicit ChartPointSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual ~ChartPointSeriesPointItem();

    public:
        virtual void paint(QPainter* p);
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

    public:
        QPointF screenPoint() const;

    private:
        Q_DISABLE_COPY(ChartPointSeriesPointItem)
    };


    /* ChartBubbleSeriesPrivate */
    class ChartBubbleSeriesPrivate : public ChartPointSeriesPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartBubbleSeries)
    public:
        explicit ChartBubbleSeriesPrivate();
        virtual ~ChartBubbleSeriesPrivate();
    public:
        void init_1();
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
        virtual void correctAxisSideMargins(ChartAxis* axis, double /*minValue*/, double /*maxValue*/, double& correction);
        virtual int seriesPointValueIndex() const;
    public:
        int m_transparency;
        qreal m_minSize;
        qreal m_maxSize;
    private:
        Q_DISABLE_COPY(ChartBubbleSeriesPrivate)
    };

    /* ChartBubbleSeriesItem */
    class ChartBubbleSeriesItem : public ChartPointSeriesItem
    {
    public:
        explicit ChartBubbleSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    public:
        virtual void paint(QPainter* p);

    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        void createPointsItem();

    protected:
        qreal m_minValue;
        qreal m_maxValue;

    private:
        friend class ChartBubbleSeriesPointItem;
        Q_DISABLE_COPY(ChartBubbleSeriesItem)
    };

    /* ChartBubbleSeriesPointItem */
    class ChartBubbleSeriesPointItem : public ChartPointSeriesPointItem
    {
    public:
        explicit ChartBubbleSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);

    public:
        virtual void paint(QPainter* p);

    public:
        QColor startColorPoint() const;
        QColor finalColorPoint() const;

    private:
        Q_DISABLE_COPY(ChartBubbleSeriesPointItem)
    };

    class ChartBubbleSeriesLabelPrivate;
    /* ChartBubbleSeriesLabel */
    class ChartBubbleSeriesLabel : public ChartSeriesLabel
    {
    public:
        explicit ChartBubbleSeriesLabel(QObject* p = Q_NULL);
        virtual ~ChartBubbleSeriesLabel();
    private:
        QTN_DECLARE_EX_PRIVATE(ChartBubbleSeriesLabel)
        Q_DISABLE_COPY(ChartBubbleSeriesLabel)
    };

    /* ChartBubbleSeriesLabelPrivate */
    class ChartBubbleSeriesLabelPrivate : public ChartSeriesLabelPrivate
    {
        QTN_DECLARE_EX_PUBLIC(ChartBubbleSeriesLabel)
    public:
        explicit ChartBubbleSeriesLabelPrivate();
        virtual ~ChartBubbleSeriesLabelPrivate();
    public:
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
    private:
        Q_DISABLE_COPY(ChartBubbleSeriesLabelPrivate)
    };

    /* ChartBubbleSeriesLabelItem */
    class ChartBubbleSeriesLabelItem : public ChartArea2DSeriesLabelItem
    {
    public:
        explicit ChartBubbleSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
        virtual ~ChartBubbleSeriesLabelItem();

    public:
        virtual void paint(QPainter* p);
        virtual void calculateLayout();
    private:
        Q_DISABLE_COPY(ChartBubbleSeriesLabelItem)
    };

}; //namespace Qtitan


#endif // QTN_CHARTPOINTSERIESPRIVATE_H
