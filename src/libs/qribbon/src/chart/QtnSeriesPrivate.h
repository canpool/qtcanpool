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
#ifndef QTN_ABSTRACTSERIESPRIVATE_H
#define QTN_ABSTRACTSERIESPRIVATE_H

#include <QColor>

#include "QtnChartDefines.h"
#include "QtnGraphicsItem.h"
#include "QtnSeriesPrivate.h"
#include "QtnChartLegend.h"
#include "QtnAbstractSeries.h"


namespace Qtitan
{
    class ChartAxis;
    class ChartSeriesLabel;
    class ChartAreaItem;
    class ChartSeriesItem;
    /* AbstractSeriesPrivate */
    class AbstractSeriesPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(AbstractSeries)
    public:
        explicit AbstractSeriesPrivate();
        virtual ~AbstractSeriesPrivate();

    public:
        virtual void linkData() = 0;
        virtual void unlinkData() = 0;
        virtual ChartArea* createArea(QObject* parent);
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual ChartSeriesItem* createItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem) = 0;
        virtual void correctAxisSideMargins(ChartAxis* axis, qreal minValue, qreal maxValue, qreal& correction);
        virtual int seriesPointValueIndex() const;
        virtual void setArea(ChartArea* area);

    public:
        ChartSeriesLabel* m_label;
        QColor m_color;
        bool m_colorAlternate;
        bool m_visible;

    private:
        friend class Chart;

        Q_DISABLE_COPY(AbstractSeriesPrivate)
    };

    class ChartFillDrawHelper;
    class ChartSeriesPointItem;

    /* ChartSeriesLabelPrivate */
    class ChartSeriesLabelPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(ChartSeriesLabel)
    public:
        explicit ChartSeriesLabelPrivate();
        virtual ~ChartSeriesLabelPrivate();

    public:
        void init();

    public:
        virtual GraphicsItem* createItem(ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);

    public:
        QColor m_textColor;
        QColor m_backColor;
        QColor m_lineColor;
        bool m_visible;
        QFont m_font;
        bool m_antialiasing;
        bool m_showLines;
        int m_lineLength;
        ChartBorder* m_border;
        ChartFillDrawHelper* m_fillHelper;
        QString m_strFormat;

    private:
        Q_DISABLE_COPY(ChartSeriesLabelPrivate)
    };

    /* ChartSeriesLabelItem */
    class ChartSeriesLabelItem : public GraphicsItem
    {
    public:
        explicit ChartSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
        virtual ~ChartSeriesLabelItem();

    public:
        virtual void calculateLayout();

        QColor textColor() const;
        QColor borderColor() const;
        QColor linkColor() const;

    protected:
        ChartSeriesLabel* m_label;
        ChartSeriesPointItem* m_pointItem;

    private:
        Q_DISABLE_COPY(ChartSeriesLabelItem)
    };

    /* ChartSeriesLabelConnectorPainterBase */
    class ChartSeriesLabelConnectorPainterBase
    {
    public:
        explicit ChartSeriesLabelConnectorPainterBase(QPointF startPoint, QPointF finishPoint, QRectF bounds);

    public:
        static QRectF calcBorderBoundsForTangentDrawing(const QPointF& point, qreal angle, const QSizeF& textSize, int borderThickness, QRectF& innerBounds);
    protected:
        QPointF m_startPoint;
        QPointF m_finishPoint;
        QRectF m_bounds;
    };

    /* ChartSeriesLabelLineConnectorPainter */
    class ChartSeriesLabelLineConnectorPainter : public ChartSeriesLabelConnectorPainterBase
    {
    public:
        explicit ChartSeriesLabelLineConnectorPainter(QPointF startPoint, QPointF finishPoint, qreal angle, QRectF bounds);

    public:
        void drawFillLine(QPainter* p, ChartSeriesLabel* label, const QColor& color);
    protected:
        qreal m_angle;
    };

    class ChartDataPoint;
    /* ChartSeriesItem */
    class ChartSeriesItem : public GraphicsItem, public ChartLegendInterface
    {
    public:
        explicit ChartSeriesItem(ChartDataTablePrivate* data, ChartAreaItem* areaItem);
        virtual ~ChartSeriesItem();

    public:
        virtual void createPointsItem();

        ChartDataTablePrivate* data() const;
        AbstractSeries* series() const;
        ChartArea* area() const;

        ChartAreaItem* areaItem() const;

        QColor beginColor() const;
        QColor endColor() const;

        GraphicsItem* pointsItem() const;

    public:
        virtual void calculateLayout();
        virtual void updateRange();
        virtual void addToLegend(ChartLegendItem* item);
        virtual void drawLegendComponent(const QRect& boundingRect);

    protected:
        virtual ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual QString legendName() const;

    protected:
        ChartDataTablePrivate* m_data;
        ChartAreaItem* m_areaItem;
        GraphicsItem* m_pointsItem;
        GraphicsItem* m_labelsItem;

    private:
        Q_DISABLE_COPY(ChartSeriesItem)
    };

}; //namespace Qtitan


#endif // QTN_ABSTRACTSERIES_H
