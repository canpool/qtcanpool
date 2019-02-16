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
#ifndef QTN_CHARTPIESERIES_H
#define QTN_CHARTPIESERIES_H

#include "QtnAbstractSeries.h"

namespace Qtitan
{
    class ChartPieSeriesBasePrivate;
    /* ChartPieSeriesBase */
    class QTITAN_EXPORT ChartPieSeriesBase : public AbstractSeries
    {
        Q_OBJECT
        Q_PROPERTY(int rotation READ rotation WRITE setRotation)
        Q_PROPERTY(int holePercent READ holePercent WRITE setHolePercent)
        Q_PROPERTY(bool isExploded READ isExploded)
        Q_PROPERTY(int explodeRelativeSizePercent READ explodeRelativeSizePercent WRITE setExplodeRelativeSizePercent)
    public:
        virtual ~ChartPieSeriesBase();

    public:
        ChartDataPoint* add(qreal value, bool special = false, const QString& label = QString(), const QColor& color = QColor());
        int count() const;

        int rotation() const;
        void setRotation(int rotation);

        int holePercent() const;
        void setHolePercent(int holePercent);

        int explodeRelativeSizePercent() const;
        void setExplodeRelativeSizePercent(int sizePercent);
        bool isExploded() const;

        virtual int depth() const;

    protected:
        explicit ChartPieSeriesBase(ChartPieSeriesBasePrivate& d, QObject* parent);

    private:
        ChartDataTablePrivate* data() const;

    protected:
        QTN_DECLARE_EX_PRIVATE(ChartPieSeriesBase)
        Q_DISABLE_COPY(ChartPieSeriesBase)
    };

    class ChartBorder;
    class ChartPieSeriesPrivate;
    /* ChartPieSeries */
    class QTITAN_EXPORT ChartPieSeries : public ChartPieSeriesBase
    {
        Q_OBJECT
    public:
        explicit ChartPieSeries();
        virtual ~ChartPieSeries();

    public:
        QColor borderColor() const;

    private:
        friend class ChartPieSeriesItem;
        friend class ChartPieSeriesSliceItem;

        QTN_DECLARE_EX_PRIVATE(ChartPieSeries)
        Q_DISABLE_COPY(ChartPieSeries)
    };

    class ChartPieSeriesLabelPrivate;
    /* ChartPieSeriesLabel */
    class QTITAN_EXPORT ChartPieSeriesLabel : public ChartSeriesLabel
    {
        Q_OBJECT
        Q_PROPERTY(ChartPieSeriesLabel::PieLabelPosition position READ position WRITE setPosition)
        Q_PROPERTY(bool isInside READ isInside)
        Q_ENUMS(PieLabelPosition)
    public:
        enum PieLabelPosition
        {
            PieLabelOutside,
            PieLabelInside,
            PieLabelRadial,
        };
    public:
        explicit ChartPieSeriesLabel(QObject* p = Q_NULL);
        virtual ~ChartPieSeriesLabel();

    public:
        PieLabelPosition position() const;
        void setPosition(PieLabelPosition pos);
        bool isInside() const;

    private:
        friend class ChartSliceLabelItem;

        Q_DISABLE_COPY(ChartPieSeriesLabel)
        QTN_DECLARE_EX_PRIVATE(ChartPieSeriesLabel)
    };
};


#endif // QTN_CHARTPIESERIES_H
