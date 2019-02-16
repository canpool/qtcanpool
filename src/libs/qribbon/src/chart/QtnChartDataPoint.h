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
#ifndef QTN_CHARTDATAPOINT_H
#define QTN_CHARTDATAPOINT_H

#include <QColor>

#include "QtitanDef.h"
#include "QtnChartComponent.h"

namespace Qtitan
{
    class AbstractSeries;
    class ChartDataPointPrivate;

    /* ChartDataPoint */
    class QTITAN_EXPORT ChartDataPoint
    {
    public:
        ChartDataPoint(qreal xvalue, qreal yvalue, const QString& label = QString(), const QColor& color = QColor());
        ChartDataPoint(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label = QString(), const QColor& color = QColor());
        virtual ~ChartDataPoint();

    public:
        qreal valueX() const;
        void setValueX(qreal value);

        qreal valueY() const;
        void setValueY(qreal value);

        qreal valueY1() const;
        void setValueY1(qreal value);

        QColor color() const;
        void setColor(const QColor& clr);

        void setLegendText(const QString& legendText);
        QString legendText() const;

        void setTooltip(const QString& tooltip);
        QString tooltip() const;

        void setLabel(const QString& label);
        QString label() const;

        AbstractSeries* series() const;

    protected:
        friend class ChartDataTablePrivate;
        friend class ChartSeriesLabel;
        friend class ChartSeriesPointItem;
        friend class ChartPieSeriesSliceItem;
        friend class ChartNumericalScaleTypeMap;
        friend class ChartDateTimeScaleTypeMap;
        friend class ChartQualitativeScaleTypeMap;

        QTN_DECLARE_PRIVATE(ChartDataPoint);
    };

}; //namespace Qtitan


#endif // QTN_CHARTDATAPOINT_H
