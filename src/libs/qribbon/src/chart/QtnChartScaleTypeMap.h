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
#ifndef QTN_CHARTSCALETYPEMAP_H
#define QTN_CHARTSCALETYPEMAP_H

#include "QtnChartComponent.h"


namespace Qtitan
{
    class ChartAxis;
    class ChartDataTablePrivate;
    class ChartDataPoint;
    class ChartAxisItem;

    /* ChartScaleTypeMap */
    class ChartScaleTypeMap : public ChartComponent
    {
    public:
        explicit ChartScaleTypeMap(QObject* parent = Q_NULL);
        virtual ~ChartScaleTypeMap();

    public:
        ChartAxis* axis() const;

        virtual void updateSeries(ChartAxisItem* item);
        virtual void updateSeries(ChartDataTablePrivate* series);
        virtual void updateSeriesPointArgument(ChartDataPoint* point);

        virtual QString internalToValue(const QString& strFormat, qreal value) const = 0;
        virtual qreal valueToInternal(const QString& strValue) const = 0;

    public:
        static ChartScaleTypeMap* create(ChartAxis* axis);
    };

    /* ChartNumericalScaleTypeMap */
    class  ChartNumericalScaleTypeMap : public ChartScaleTypeMap
    {
    public:
        explicit ChartNumericalScaleTypeMap(ChartAxis* axis);

    public:
        virtual QString internalToValue(const QString& strFormat, qreal value) const;
        virtual qreal valueToInternal(const QString& strValue) const;

    protected:
        void updateSeriesPointArgument(ChartDataPoint* point);

    protected:
        qreal m_minValue;
        qreal m_maxValue;
    };

    /* ChartDateTimeScaleTypeMap */
    class ChartDateTimeScaleTypeMap : public ChartScaleTypeMap
    {
    public:
        explicit ChartDateTimeScaleTypeMap(ChartAxis* axis);
        virtual ~ChartDateTimeScaleTypeMap();

    public:
        virtual QString internalToValue(const QString& strFormat, qreal value) const;
        virtual qreal valueToInternal(const QString& strValue) const;

    protected:
        void updateSeriesPointArgument(ChartDataPoint* point);

    };

    /* ChartQualitativeScaleTypeMap */
    class ChartQualitativeScaleTypeMap : public ChartScaleTypeMap
    {
    public:
        explicit ChartQualitativeScaleTypeMap(ChartAxis* axis);
        virtual ~ChartQualitativeScaleTypeMap();

    public:
        virtual QString internalToValue(const QString& strFormat, qreal value) const;
        virtual qreal valueToInternal(const QString& strValue) const;

    protected:
        void updateSeries(ChartDataTablePrivate* series);
        void updateSeriesPointArgument(ChartDataPoint* point);

        int fillNextsList(ChartDataTablePrivate* series, int i, QStringList& nexts, const QString& argument_i);

    protected:
        qreal m_minValue;
        qreal m_maxValue;
    private:
        QStringList* m_storage;
    };
}

#endif
