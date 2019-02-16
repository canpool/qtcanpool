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
#ifndef QTN_CHARTLEGEND_H
#define QTN_CHARTLEGEND_H

#include <QFont>
#include <QBrush>

#include "QtitanDef.h"
#include "QtnChartComponent.h"


namespace Qtitan
{
    class ChartBorder;
    /* ChartLegendInterface */
    class ChartLegendInterface
    {
    public:
        virtual QString legendName() const = 0;
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect) = 0;
    };


    class ChartLegendPrivate;
    /* ChartLegend */
    class QTITAN_EXPORT ChartLegend : public ChartTextComponent
    {
    public:
        enum LegendDirection
        {
            LegendTopToBottom,
            LegendLeftToRight,
        };

        enum LegendAlignment
        {
            LegendNearOutside,
            LegendNear,
            LegendCenter,
            LegendFar,
            LegendFarOutside,
        };

    public:
        explicit ChartLegend(QObject* parent);
        virtual ~ChartLegend();

    public:
        bool isVisible() const;
        void setVisible(bool visible);

        const QFont& font() const;
        void setFont(const QFont& font);

        bool isMarkerVisible() const;
        void setMarkerVisible(bool visible);

        const QSize& markerSize() const;
        void setMarkerSize(const QSize& sz);

        void setTextColor(const QColor& txtColor);
        const QColor& textColor() const;

        const QColor& borderColor() const;
        int borderRounded() const;

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

        ChartBorder* border() const;

        void setHorizontalAlignment(LegendAlignment alignment);
        LegendAlignment horizontalAlignment() const;

        void setVerticalAlignment(LegendAlignment alignment);
        LegendAlignment verticalAlignment() const;

        void setDirection(LegendDirection direction);
        LegendDirection direction() const;

        void setColumnCount(int columnCount);
        int columnCount() const;

    private:
        friend class Chart;
        friend class ChartLegendItem;
        friend class Chart2DItem;

        Q_DISABLE_COPY(ChartLegend)
        QTN_DECLARE_PRIVATE(ChartLegend)
    };
}; //namespace Qtitan


#endif // QTN_CHARTLEGEND_H
