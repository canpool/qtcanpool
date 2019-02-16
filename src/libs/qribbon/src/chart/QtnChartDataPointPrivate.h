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
#ifndef QTN_CHARTDATAPOINTPRIVATE_H
#define QTN_CHARTDATAPOINTPRIVATE_H

#include "QtnChartDataPoint.h"

namespace Qtitan
{
    class ChartDataTablePrivate;

    /* ChartDataPointPrivate */   
    class ChartDataPointPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartDataPoint)
    public:
        explicit ChartDataPointPrivate();
        virtual ~ChartDataPointPrivate();

    public:
        void init();

    public:
        ChartDataTablePrivate* m_parent;
        QString m_strArgX;
        qreal m_argX;
        qreal m_values[4];
        int m_length;
        int m_palIndex;
        int m_index;
        bool m_special;

        QString m_legentText;
        QString m_tooltipText;
        QString m_labelText;
        QColor m_colorPoint;
    };

    class ChartArea;
    class AbstractSeries;

    enum ChartScaleType
    {
        ChartScaleQualitative,
        ChartScaleNumerical,
        ChartScaleDateTime
    };

    /* ChartDataTablePrivate */
    class ChartDataTablePrivate : public ChartComponent
    {
        Q_OBJECT
    public:
        explicit ChartDataTablePrivate(QObject* parent = Q_NULL);
        virtual ~ChartDataTablePrivate();

    public:
        ChartDataPoint* addAxisPointY(double value, const QString& axisLabel = "");
        ChartDataPoint* addY(qreal value, bool special = false, const QString& label= "", const QColor& color = QColor());
        ChartDataPoint* addXY(qreal xvalue, qreal yvalue, const QString& label= "", const QColor& color = QColor());
        ChartDataPoint* addXYY1(qreal xvalue, qreal yvalue, qreal yvalue1, const QString& label= "", const QColor& color = QColor());
        ChartDataPoint* addY(const QVariant& value, const QString& label = "", const QColor& color = QColor());
        ChartDataPoint* addXY(const QVariant& xvalue, const QVariant& yvalue, const QString& label = "", const QColor& color = QColor());

        int count() const;
        void append(ChartDataPoint* data);
        ChartDataPoint* at(int index) const;
        int indexOf(ChartDataPoint* point) const;
        void clear();

        AbstractSeries* series() const;
        AbstractSeries* setSeries(AbstractSeries* impl);

        QColor color() const;

        bool setArea(ChartArea* area);
        ChartArea* area() const;

        void setArgScaleType(ChartScaleType scaleType);
        ChartScaleType argScaleType() const;

        void setValueScaleType(ChartScaleType scaleType);
        ChartScaleType valueScaleType() const;

        void setPaletteIndex(int index);
        int paletteIndex() const;

        void setName(const QString& name);
        QString name() const;

        void setLegendText(const QString& legendText);
        QString legendText() const;

        void setPointLegendFormat(const QString& strFormat);
        QString pointLegendFormat() const;

        void setPointPaletteIndex(int indexPnt, int indexPal);

    protected:
        AbstractSeries* m_impl;
        QVector<ChartDataPoint*> m_data;
        ChartScaleType m_argumentScaleType;
        ChartScaleType m_valueScaleType;

        ChartArea* m_area;

        QString m_strName;
        QString m_strLegendText;
        QString m_strPointLegendFormat;

        //        bool m_visible;
        int m_palIndex;

    protected:
        friend class Chart;
        friend class ChartPrivate;

        Q_DISABLE_COPY(ChartDataTablePrivate);
    };

}; //namespace Qtitan

#endif // QTN_CHARTDATAPOINTPRIVATE_H
