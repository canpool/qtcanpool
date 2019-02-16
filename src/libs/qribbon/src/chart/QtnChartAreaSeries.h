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
#ifndef QTN_CHARTAREASERIES_H
#define QTN_CHARTAREASERIES_H

#include "QtnChartPointSeries.h"

namespace Qtitan
{
    class ChartFillDrawHelper;
    class ChartAreaSeriesPrivate;
    /* ChartAreaSeries */
    class QTITAN_EXPORT ChartAreaSeries : public ChartPointSeries
    {
        Q_OBJECT
    public:
        explicit ChartAreaSeries();
        virtual ~ChartAreaSeries();

    protected:
        ChartAreaSeries(ChartAreaSeriesPrivate& d, QObject* parent);

    public:
        ChartFillDrawHelper* fillHelper() const;
        ChartBorder* border() const;

        void setThickness(int thickness);
        int thickness() const;

    public:
        int transparency() const;
        void setTransparency(int transparency);

    private:
        QTN_DECLARE_EX_PRIVATE(ChartAreaSeries)
        Q_DISABLE_COPY(ChartAreaSeries)
    };

    /* ChartSplineAreaSeries */
    class QTITAN_EXPORT ChartSplineAreaSeries : public ChartAreaSeries
    {
        Q_OBJECT
    public:
        explicit ChartSplineAreaSeries();
        virtual ~ChartSplineAreaSeries();

    private:
        Q_DISABLE_COPY(ChartSplineAreaSeries)
    };

    class ChartStackedAreaSeriesPrivate;
    /* ChartStackedAreaSeries */
    class QTITAN_EXPORT ChartStackedAreaSeries : public ChartAreaSeries
    {
        Q_OBJECT
    public:
        explicit ChartStackedAreaSeries();
        virtual ~ChartStackedAreaSeries();
    public:
        void setStackHeight(qreal level = 100.0);
        qreal stackHeight() const;

    private:
        QTN_DECLARE_EX_PRIVATE(ChartStackedAreaSeries)
        Q_DISABLE_COPY(ChartStackedAreaSeries)
    };

    /* ChartStackedSplineAreaSeries */
    class QTITAN_EXPORT ChartStackedSplineAreaSeries : public ChartStackedAreaSeries
    {
        Q_OBJECT
    public:
        explicit ChartStackedSplineAreaSeries();
        virtual ~ChartStackedSplineAreaSeries();

    private:
        Q_DISABLE_COPY(ChartStackedSplineAreaSeries)
    };
};

#endif // QTN_CHARTAREASERIES_H
