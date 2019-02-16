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
#ifndef QTN_CHARTBARSERIES_H
#define QTN_CHARTBARSERIES_H

#include <QRectF>

#include "QtnChart2DSeries.h"


namespace Qtitan
{
    class ChartFillDrawHelper;
    class ChartBorder;
    class ChartBarSeriesPrivate;

    /* ChartBarSeries */
    class QTITAN_EXPORT ChartBarSeries : public Chart2DSeries
    {
        Q_OBJECT
    public:
        explicit ChartBarSeries();
        virtual ~ChartBarSeries();

    protected:
        ChartBarSeries(ChartBarSeriesPrivate& d, QObject* parent);

    public:
        qreal width() const;
        void setWidth(qreal width);

        bool isSideBySide() const;
        void setSideBySide(bool sideBySide);

        int distanceFixed() const;
        void setDistanceFixed(int barDistanceFixed);

    private:
        friend class ChartBarSeriesItem;
        friend class ChartBarSeriesPointItem;

        Q_DISABLE_COPY(ChartBarSeries)
        QTN_DECLARE_EX_PRIVATE(ChartBarSeries)
    };

    class ChartStackedBarSeriesPrivate;
    /* ChartStackedBarSeries */
    class QTITAN_EXPORT ChartStackedBarSeries : public ChartBarSeries
    {
        Q_OBJECT
    public:
        explicit ChartStackedBarSeries();
        virtual ~ChartStackedBarSeries();

    public:
        void setStackHeight(qreal level = 100.0);
        qreal stackHeight() const;

        void setStackGroup(int stackGroup);
        int stackGroup() const;

    private:
        Q_DISABLE_COPY(ChartStackedBarSeries)
        QTN_DECLARE_EX_PRIVATE(ChartStackedBarSeries)
    };

    class ChartBarSeriesLabelPrivate;
    /* ChartBarSeriesLabel */
    class QTITAN_EXPORT ChartBarSeriesLabel : public ChartSeriesLabel
    {
        Q_OBJECT
        Q_PROPERTY(ChartBarSeriesLabel::BarLabelPosition position READ position WRITE setPosition)
        Q_ENUMS(BarLabelPosition)
    public:
        enum BarLabelPosition
        {
            ChartBarLabelTop,
            ChartBarLabelCenter
        };

    public:
        explicit ChartBarSeriesLabel(QObject* p = Q_NULL);
        virtual ~ChartBarSeriesLabel();

    public:
        void setPosition(BarLabelPosition position);
        BarLabelPosition position() const;

    private:
        Q_DISABLE_COPY(ChartBarSeriesLabel)
        QTN_DECLARE_EX_PRIVATE(ChartBarSeriesLabel)
    };
};

#endif // QTN_CHARTBARSERIES_H
