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
#ifndef QTN_CHARTLINESERIES_H
#define QTN_CHARTLINESERIES_H

#include "QtnChartDefines.h"
#include "QtnChartPointSeries.h"


namespace Qtitan
{
    class ChartLineSeriesPrivate;
    /* ChartLineSeries */
    class QTITAN_EXPORT ChartLineSeries : public ChartPointSeries
    {
        Q_OBJECT
    public:
        explicit ChartLineSeries();
        virtual ~ChartLineSeries();

    protected:
        ChartLineSeries(ChartLineSeriesPrivate& d, QObject* parent);

    public:
        void setShadow(bool shadow);
        bool shadow() const;

        void setThickness(int thickness);
        int thickness() const;

        void setDashStyle(DashStyle dashStyle);
        DashStyle dashStyle() const;

    private:
        friend class ChartLineSeriesItem;
        friend class ChartSplineSeriesItem;
        QTN_DECLARE_EX_PRIVATE(ChartLineSeries)
        Q_DISABLE_COPY(ChartLineSeries)
    };

    class ChartStepLineSeriesPrivate;
    /* ChartStepLineSeries */
    class QTITAN_EXPORT ChartStepLineSeries : public ChartLineSeries
    {
        Q_OBJECT
    public:
        explicit ChartStepLineSeries();
        virtual ~ChartStepLineSeries();

    public:
        bool invertedStep() const;
        void setInvertedStep(bool bInvertedStep);

    private:
        friend class ChartStepLineSeriesItem;
        QTN_DECLARE_EX_PRIVATE(ChartStepLineSeries)
        Q_DISABLE_COPY(ChartStepLineSeries)
    };

    /* ChartSplineSeries */
    class QTITAN_EXPORT ChartSplineSeries : public ChartLineSeries
    {
        Q_OBJECT
    public:
        explicit ChartSplineSeries();
        virtual ~ChartSplineSeries();

    private:
        Q_DISABLE_COPY(ChartSplineSeries)
    };
};

#endif // QTN_CHARTLINESERIES_H
