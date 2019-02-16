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
#ifndef QTN_CHARTAREA_H
#define QTN_CHARTAREA_H

#include <QRectF>
#include <QBrush>

#include "QtnChartDefines.h"
#include "QtnChartComponent.h"


namespace Qtitan
{
    class GraphicsItem;
    class ChartTitle;
    class ChartAreaPrivate;
    class AbstractSeries;

    /* ChartArea */ 
    class QTITAN_EXPORT ChartArea : public ChartComponent
    {
        Q_OBJECT
    public:
        ChartArea(QObject* parent);
    protected:
        ChartArea(ChartAreaPrivate& d, QObject* parent);
        virtual ~ChartArea();
    public:
        void appendSeries(AbstractSeries* series);
        void removeSeries(AbstractSeries* series);

        const QList<ChartTitle*>& titles() const;

        const QBrush& backgroundBrush() const;
        void setBackgroundBrush(const QBrush& brush);

        const QColor& backgroundColor() const;
        void setBackgroundColor(const QColor& color);

        const QColor& borderColor() const;
        void setBorderColor(const QColor& color);

    protected:
        void clear();

    private:
        QTN_DECLARE_PRIVATE(ChartArea)
        Q_DISABLE_COPY(ChartArea)
        friend class Chart;
        friend class ChartArea2D;
        friend class ChartPieArea;
        friend class ChartFunnelArea;
        friend class ChartPyramidArea;
        friend class ChartDataTablePrivate;
        friend class Chart2DItem;
        friend class ChartArea2DItem;
        friend class ChartAreaItem;
        friend class ChartPieSeriesPrivate;
        friend class ChartFunnelSeriesPrivate;
        friend class ChartPyramidSeriesPrivate;
    };

}; //namespace Qtitan


#endif //QTN_CHARTAREA_H
