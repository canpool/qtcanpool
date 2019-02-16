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
#ifndef QTN_CHARTAREASERIESPRIVATE_H
#define QTN_CHARTAREASERIESPRIVATE_H

#include "QtnChartPointSeriesPrivate.h"
#include "QtnChartPointSeries.h"

namespace Qtitan
{
    /* ChartAreaSeriesItem */
    class ChartAreaSeriesItem : public ChartArea2DSeriesItem
    {
    public:
        ChartAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
        QColor startColor() const;
        QColor finalColor() const;
        QColor borderColor() const;
        void drawLegendComponent(QPainter* p, const QRect& boundingRect);
    };

    /* ChartSplineAreaSeriesItem */
    class ChartSplineAreaSeriesItem : public ChartAreaSeriesItem
    {
    public:
        ChartSplineAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);
    };

    /* ChartStackedAreaSeriesItem */
    class ChartStackedAreaSeriesItem : public ChartAreaSeriesItem
    {
    public:
        ChartStackedAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);
    protected:
        ChartSeriesPointItem* createSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
        virtual void paint(QPainter* p);

        void updateRange();
    };

    /* ChartStackedAreaSeriesPointItem */
    class ChartStackedAreaSeriesPointItem : public ChartPointSeriesPointItem
    {
    public:
        ChartStackedAreaSeriesPointItem(ChartDataPoint* point, GraphicsItem* parentItem);
    public:
        qreal m_valueFrom;
        qreal m_valueTo;
        friend class ChartStackedAreaSeriesItem;
    };

    /* ChartStackedSplineAreaSeriesItem */
    class ChartStackedSplineAreaSeriesItem : public ChartStackedAreaSeriesItem
    {
    public:
        ChartStackedSplineAreaSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem);

    protected:
        void paint(QPainter* p);
    };
};

#endif // QTN_CHARTAREASERIESPRIVATE_H
