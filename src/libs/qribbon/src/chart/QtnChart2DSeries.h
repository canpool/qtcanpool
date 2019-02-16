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
#ifndef QTN_CHART2DSERIES_H
#define QTN_CHART2DSERIES_H

#include "QtitanDef.h"

#include "QtnAbstractSeries.h"
#include "QtnChartArea.h"
#include "QtnChartDataPoint.h"

namespace Qtitan
{
    class ChartArea;
    class Chart2DSeriesPrivate;
    /* Chart2DSeries */
    class QTITAN_EXPORT Chart2DSeries : public AbstractSeries
    {
        Q_OBJECT
    protected:
        Chart2DSeries(Chart2DSeriesPrivate& d, QObject* parent);

    public:
        virtual ~Chart2DSeries();

    public:
        ChartDataPoint* addAxisPointY(double value, const QString& axisLabel = QString());
        ChartDataPoint* addY(qreal value, const QString& label = QString(), const QColor& color = QColor());
        ChartDataPoint* addY(const QVariant& value, const QString& label = QString(), const QColor& color = QColor());
        ChartDataPoint* addXY(qreal xvalue, qreal yvalue, const QString& label = QString(), const QColor& color = QColor());
        ChartDataPoint* addXYY1(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label = QString(), const QColor& color = QColor());
        ChartDataPoint* addXY(const QVariant& xvalue, const QVariant& yvalue, const QString& label = QString(), const QColor& color = QColor());

        int count() const;
        ChartDataPoint* at(int index) const;
        int indexOf(ChartDataPoint* point) const;
        void clear();

        QColor color() const;

        void setName(const QString& name);
        QString name() const;

        void setLegendText(const QString& legendText);
        QString legendText() const;

        void setPointLegendFormat(const QString& strFormat);
        QString pointLegendFormat() const;

        void setSecondaryAxisX(bool secondaryAxis);
        bool isSecondaryAxisX() const;

        void setSecondaryAxisY(bool secondaryAxis);
        bool isSecondaryAxisY() const;

    private:
        virtual ChartDataTablePrivate* data() const;

    private:
        Q_DISABLE_COPY(Chart2DSeries)
        QTN_DECLARE_EX_PRIVATE(Chart2DSeries)
    };

}; //namespace Qtitan


#endif // QTN_CHART2DSERIES_H
