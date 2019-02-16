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
#ifndef QTN_CHARTBUBBLESERIES_H
#define QTN_CHARTBUBBLESERIES_H

#include "QtnChartPointSeries.h"


namespace Qtitan
{
    class ChartBubbleSeriesPrivate;
    /* ChartBubbleSeries */
    class QTITAN_EXPORT ChartBubbleSeries : public ChartPointSeries
    {
        Q_OBJECT
        Q_PROPERTY(int transparency READ transparency WRITE setTransparency)
        Q_PROPERTY(qreal minSize READ minSize WRITE setMinSize)
        Q_PROPERTY(qreal maxSize READ maxSize WRITE setMaxSize)
    public:
        explicit ChartBubbleSeries();
        virtual ~ChartBubbleSeries();

    public:
        int transparency() const;
        void setTransparency(int transparency);

        qreal minSize() const;
        void setMinSize(qreal minSize);

        qreal maxSize() const;
        void setMaxSize(qreal minSize);

    private:
        Q_DISABLE_COPY(ChartBubbleSeries)
        QTN_DECLARE_EX_PRIVATE(ChartBubbleSeries)
    };

};

#endif //QTN_CHARTBUBBLESERIES_H
