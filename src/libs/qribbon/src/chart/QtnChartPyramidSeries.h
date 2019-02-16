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
#ifndef QTN_CHARTPYRAMIDSERIES_H
#define QTN_CHARTPYRAMIDSERIES_H

#include "QtnAbstractSeries.h"


namespace Qtitan
{
    class ChartDataTablePrivate;
    class ChartPyramidSeriesPrivate;
    /* ChartPyramidSeries */
    class QTITAN_EXPORT ChartPyramidSeries : public AbstractSeries
    {
        Q_OBJECT
        Q_PROPERTY(bool isSeries3D READ isSeries3D WRITE setSeries3D)
        Q_PROPERTY(int depthPercent READ depthPercent WRITE setDepthPercent)
        Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
        Q_PROPERTY(bool isCircular READ isCircular WRITE setCircular)
        Q_PROPERTY(int transparency READ transparency WRITE setTransparency)
        Q_PROPERTY(int pointGap READ pointGap WRITE setPointGap)
    public:
        explicit ChartPyramidSeries();
        virtual ~ChartPyramidSeries();

    public:
        ChartDataPoint* add(qreal value, const QString& label);
        int count() const;

        void setSeries3D(bool series3D);
        bool isSeries3D() const;

        void setDepthPercent(int depthPercent = 20);
        int depthPercent() const;

        void setRotation(qreal angle);
        qreal rotation() const;

        void setCircular(bool circular = false);
        bool isCircular() const;

        int transparency() const;
        void setTransparency(int transparency);

        int pointGap() const;
        void setPointGap(int pointGap);

    protected:
        ChartPyramidSeries(ChartPyramidSeriesPrivate& d, QObject* parent);

    private:
        ChartDataTablePrivate* data() const;

    protected:
        friend class ChartPyramidSeriesItem;
        friend class ChartPyramidSeriesPartItem;
        QTN_DECLARE_EX_PRIVATE(ChartPyramidSeries)
        Q_DISABLE_COPY(ChartPyramidSeries)
    };

    enum LabelPosition
    {
        LabelLeft,
        LabelRight,
        LabelCenter,
    };

    class ChartPyramidSeriesLabelPrivate;
    /* ChartPyramidSeriesLabel */
    class QTITAN_EXPORT ChartPyramidSeriesLabel : public ChartSeriesLabel
    {
        Q_OBJECT
        Q_PROPERTY(LabelPosition position READ position WRITE setPosition)
    public:
        explicit ChartPyramidSeriesLabel();
        virtual ~ChartPyramidSeriesLabel();

    public:
        LabelPosition position() const;
        void setPosition(LabelPosition pos);

    public:
        bool isInside() const;

    protected:
        ChartPyramidSeriesLabel(ChartPyramidSeriesLabelPrivate& d, QObject* parent);

    private:
        QTN_DECLARE_EX_PRIVATE(ChartPyramidSeriesLabel)
        Q_DISABLE_COPY(ChartPyramidSeriesLabel)
    };

};

#endif // QTN_CHARTPYRAMIDSERIES_H
