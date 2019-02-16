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
#ifndef QTN_CHARTPOINTSERIES_H
#define QTN_CHARTPOINTSERIES_H

#include "QtitanDef.h"
#include "QtnChart2DSeries.h"

namespace Qtitan
{
    class ChartMarker;
    class ChartPointSeriesPrivate;

    /* ChartPointSeries */
    class QTITAN_EXPORT ChartPointSeries : public Chart2DSeries
    {
        Q_OBJECT
    public:
        explicit ChartPointSeries();
        virtual ~ChartPointSeries();

    protected:
        ChartPointSeries(ChartPointSeriesPrivate& d, QObject* parent);

    public:
        int markerSize() const;
        void setMarkerSize(int size);

        void setMarkerBorderVisible(bool borderVisible);
        bool markerBorderVisible() const;

        MarkerType markerType() const;
        void setMarkerType(MarkerType type);

        bool isMarkerVisible() const;
        void setMarkerVisible(bool visible);

    private:
        friend class ChartPointSeriesItem;
        friend class ChartAreaSeriesItem;
        friend class ChartLineSeriesItem;
        friend class ChartPointSeriesPointItem;
        friend class ChartStepLineSeriesItem;
        friend class ChartStepLineSeriesPointItem;
        friend class ChartBubbleSeriesPointItem;

        QTN_DECLARE_EX_PRIVATE(ChartPointSeries)
        Q_DISABLE_COPY(ChartPointSeries)
    };

    class ChartPointSeriesLabelPrivate;
    /* ChartPointSeriesLabel */
    class QTITAN_EXPORT ChartPointSeriesLabel : public ChartSeriesLabel
    {
        Q_OBJECT
        Q_PROPERTY(int angle READ angle WRITE setAngle)
    public:
        explicit ChartPointSeriesLabel(QObject* p = Q_NULL);
        virtual ~ChartPointSeriesLabel();

    public:
        void setAngle(int angle);
        int angle() const;

    private:
        QTN_DECLARE_EX_PRIVATE(ChartPointSeriesLabel)
        Q_DISABLE_COPY(ChartPointSeriesLabel)
    };

}; //namespace Qtitan


#endif // QTN_CHARTPOINTSERIES_H
