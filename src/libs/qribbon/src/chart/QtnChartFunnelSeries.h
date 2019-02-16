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
#ifndef QTN_CHARTFUNNELSERIES_H
#define QTN_CHARTFUNNELSERIES_H

#include "QtnChartPyramidSeries.h"

namespace Qtitan
{
    class ChartFunnelSeriesPrivate;
    /* ChartFunnelSeries */
    class QTITAN_EXPORT ChartFunnelSeries : public ChartPyramidSeries
    {
        Q_OBJECT
        Q_PROPERTY(int neckWidth READ neckWidth WRITE setNeckWidth)
    public:
        explicit ChartFunnelSeries();
        virtual ~ChartFunnelSeries();

    public:
        void setNeckHeightInPercents(qreal height);
        void setNeckHeightInValues(qreal value);

        bool isNeckHeightValue() const;
        qreal neckHeight() const;

        void setNeckWidth(int width);
        int neckWidth() const;

    private:
        friend class ChartFunnelSeriesItem;
        friend class ChartFunnelSeriesPartItem;
        QTN_DECLARE_EX_PRIVATE(ChartFunnelSeries)
        Q_DISABLE_COPY(ChartFunnelSeries)
    };
};

#endif // QTN_CHARTFUNNELSERIES_H
