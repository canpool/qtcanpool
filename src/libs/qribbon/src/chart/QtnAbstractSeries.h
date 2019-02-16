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
#ifndef QTN_ABSTRACTSERIES_H
#define QTN_ABSTRACTSERIES_H

#include <QColor>
#include <QFont>

#include "QtnChartDefines.h"
#include "QtnChartComponent.h"


namespace Qtitan
{
    class ChartAxis;
    class ChartArea;
    class ChartDataTablePrivate;
    class ChartSeriesLabel;
    class AbstractSeriesPrivate;

    /* AbstractSeries */
    class QTITAN_EXPORT AbstractSeries : public ChartComponent
    {
        Q_OBJECT
        Q_PROPERTY(QColor color READ color WRITE setColor)
        Q_PROPERTY(bool isAlternateColor READ isAlternateColor WRITE setAlternateColor)
        Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible)
    public:
        virtual ~AbstractSeries();

    public:
        ChartSeriesLabel* label() const;
        void setLabel(ChartSeriesLabel* label);

    public:
        QColor color() const;
        void setColor(const QColor& color);
        
        void setAlternateColor(bool alternate);
        bool isAlternateColor() const;

        void setVisible(bool visible);
        bool isVisible() const;

        ChartArea* area();

    private:
        virtual ChartDataTablePrivate* data() const = 0;

    protected:
        AbstractSeries(AbstractSeriesPrivate& d, QObject* parent);

    protected:
        QTN_DECLARE_PRIVATE(AbstractSeries)

    private:
        friend class Chart;
        friend class ChartPrivate;
        friend class ChartArea;
        friend class Chart2DItem;
        friend class ChartAxisRange;
        friend class ChartDataTablePrivate;
        friend class ChartSeriesLabel;

        Q_DISABLE_COPY(AbstractSeries)
    };

    class ChartDataPoint;
    class ChartBorder;
    class ChartFillDrawHelper;
    class GraphicsItem;
    class ChartSeriesPointItem;
    class ChartSeriesLabelPrivate;

    /* ChartSeriesLabel */
    class QTITAN_EXPORT ChartSeriesLabel : public ChartTextComponent
    {
        Q_OBJECT
        Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
        Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
        Q_PROPERTY(bool showLines READ showLines WRITE setShowLines)
        Q_PROPERTY(bool lineLength READ lineLength WRITE setLineLength)
        Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    public:
        explicit ChartSeriesLabel(QObject* p);
        virtual ~ChartSeriesLabel();
    public:
        void setTextColor(const QColor& color);
        const QColor& textColor() const;

        const QFont& font() const;
        void setFont(const QFont& font);

        void setShowLines(bool showLines);
        bool showLines() const;

        void setLineLength(int lineLength);
        int lineLength() const;

        void setVisible(bool visible);
        bool isVisible() const;

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

        QString format() const;
        void setFormat(const QString& format);

        virtual QString pointLabel(ChartDataPoint* point, int valueIndex = -1) const;

        ChartBorder* border() const;

        void setBackgroundColor(const QColor& color);
        QColor backgroundColor() const;

        void setLineColor(const QColor& color);
        QColor lineColor() const;

    Q_SIGNALS:
        void fontChanged(QFont font);
        void visibleChanged(bool visible);

    protected:
        ChartSeriesLabel(ChartSeriesLabelPrivate& d, QObject* p);
    protected:
        QTN_DECLARE_PRIVATE(ChartSeriesLabel)
    private:
        friend class AbstractSeries;
        friend class ChartSeriesItem;
        friend class ChartSliceLabelItem;
        friend class ChartArea2DSeriesLabelItem;
        friend class ChartFunnelSeriesLabelItem;
        friend class ChartPyramidSeriesLabelItem;
        friend class ChartFunnelSeriesItem;
        Q_DISABLE_COPY(ChartSeriesLabel)
    };

}; //namespace Qtitan


#endif // QTN_ABSTRACTSERIES_H
