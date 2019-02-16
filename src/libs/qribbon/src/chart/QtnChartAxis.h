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
#ifndef QTN_CHARTAXIS_H
#define QTN_CHARTAXIS_H

#include <QColor>
#include <QRect>
#include "QtitanDef.h"

#include "QtnChartDataPoint.h"
#include "QtnChartComponent.h"
#include "QtnChartTitle.h"

namespace Qtitan
{
    class ChartAxisLabel;
    class ChartArea2D;
    class ChartAxisItem;
    class ChartAxisRange;
    class ChartAxisTickMarks;
    class ChartAxisTitle;
    class ChartAxisPrivate;

    /* ChartAxis */
    class QTITAN_EXPORT ChartAxis : public ChartComponent
    {
        Q_OBJECT
    public:
        enum Alignment
        {
            AxisNear,
            AxisFar
        };
    public:
        explicit ChartAxis(ChartArea2D* area, int axisID);
        virtual ~ChartAxis();

    public:
        void updateRange(ChartAxisItem* item);

        virtual bool isVertical() const = 0;
        virtual bool isValuesAxis() const = 0;

        ChartArea2D* area() const;

        virtual qreal calcSideMarginsValue(qreal minValue, qreal maxValue) const = 0;
        virtual ChartAxis* perpendecularAxis() const;
        virtual void updateAxisPos(const QRect& boundingRect);

        ChartAxisLabel* label() const;

        ChartAxisTickMarks* tickMarks() const;

        ChartAxisTitle* title() const;

        const QRect& boundingRect() const;

        QColor color() const;
        void setColor(const QColor& color);

        ChartAxis::Alignment alignment() const;
        void setAlignment(ChartAxis::Alignment alignment);

        bool isInterlaced() const;
        void setInterlaced(bool interlaced);

        bool isVisible() const;
        void setVisible(bool visible);

        int thickness() const;
        void setThickness(int thicknes);

        void setMinorCount(int minorCount);
        int minorCount() const;

        void setAxisID(int axisID);
        int axisID() const;

        void setReversed(bool revered);
        bool isReversed() const;

        bool gridSpacingAuto() const;
        void setGridSpacingAuto(bool gridSpacingAuto);

        qreal gridSpacing() const;
        void setGridSpacing(qreal gridSpacing);

        bool isSecondary() const;

        bool isAllowZoom() const;
        void setAllowZoom(bool allowZoom);

        qreal valueFromPoint(const QPoint& pt);
        qreal pointFromValue(qreal value);
        qreal valueToPoint(qreal value) const;

    public: // axisRange
        qreal maxValue() const;
        void setMaxValue(qreal max);
        qreal minValue() const;
        void setMinValue(qreal min);

        void setFixedRange(qreal minValue, qreal maxValue);
        void setAutoRange(bool autoRange);

        qreal viewMaxValue() const;
        qreal viewMinValue() const;

        void setFixedViewRange(qreal minValue, qreal maxValue);

        void setViewAutoRange(bool autoRange);
        bool isViewAutoRange() const;

        void setSideMargins(bool sideMargins);
        bool sideMargins() const;

        void setShowZeroLevel(bool showZeroLevel);
        bool showZeroLevel() const;

    public: // AxisGridLines
        bool isGridLinesVisible() const;
        void setGridLinesVisible(bool visible);

        bool isMinorGridLinesVisible() const;
        void setMinorGridLinesVisible(bool minorVisible);

        QColor gridLinesColor() const;
        QColor minorGridLinesColor() const;

    private:
        friend class ChartAxisX;
        friend class ChartAxisY;
        friend class ChartAxisItem;
        friend class ChartArea2DPrivate;
        friend class ChartArea2DItem;
        friend class ChartScaleTypeMap;
        friend class ChartAxisViewTickEnumerator;

        Q_DISABLE_COPY(ChartAxis);
        QTN_DECLARE_PRIVATE(ChartAxis)
    };

    /* ChartAxisX */
    class QTITAN_EXPORT ChartAxisX : public ChartAxis
    {
    public:
        ChartAxisX(ChartArea2D* area, int axisID);

    public:
        virtual bool isVertical() const;
        virtual bool isValuesAxis() const;
        virtual qreal calcSideMarginsValue(qreal minValue, qreal maxValue) const;
        virtual ChartAxis* perpendecularAxis() const;

    private:
        Q_DISABLE_COPY(ChartAxisX);
    };

    /* ChartAxisY */
    class QTITAN_EXPORT ChartAxisY : public ChartAxis
    {
    public:
        ChartAxisY(ChartArea2D* area, int axisID);

    public:
        virtual bool isVertical() const;
        virtual bool isValuesAxis() const;
        virtual qreal calcSideMarginsValue(qreal minValue, qreal maxValue) const;
        virtual ChartAxis* perpendecularAxis() const;
    private:
        Q_DISABLE_COPY(ChartAxisY);
    };


    class ChartAxisLabelPrivate;
    /* ChartAxisLabel */
    class QTITAN_EXPORT ChartAxisLabel : public ChartTextComponent
    {
    public:
        ChartAxisLabel(QObject* parent = Q_NULL);
        virtual ~ChartAxisLabel();

    public:
        void setTextColor(const QColor& txtColor);
        QColor textColor() const;

        void setFont(const QFont& font);
        const QFont& font() const;

        int angle() const;
        void setAngle(int agle);

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

        void setVisible(bool visible);
        bool isVisible() const;

        QString format() const;
        void setFormat(const QString& strFormat);
    private:
        Q_DISABLE_COPY(ChartAxisLabel)
        QTN_DECLARE_PRIVATE(ChartAxisLabel)
    };

    class ChartAxisTitlePrivate;
    /* ChartAxisTitle */
    class QTITAN_EXPORT ChartAxisTitle : public ChartTextComponent
    {
    public:
        explicit ChartAxisTitle(QObject* parent = Q_NULL);
        virtual ~ChartAxisTitle();

    public:
        void setText(const QString& title);
        const QString& text() const;

        void setTextColor(const QColor& txtColor);
        const QColor& textColor() const;

        const QFont& font() const;
        void setFont(const QFont& font);

        void setAlignment(Qt::Alignment alignment);
        Qt::Alignment alignment() const;

        bool isVisible() const;
        void setVisible(bool visible);

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

    private:
        Q_DISABLE_COPY(ChartAxisTitle)
        QTN_DECLARE_PRIVATE(ChartAxisTitle)
    };

    class ChartAxisTickMarksPrivate;
    /* ChartAxisTickMarks */
    class QTITAN_EXPORT ChartAxisTickMarks : public ChartComponent
    {
    public:
        explicit ChartAxisTickMarks(QObject* parent = Q_NULL);
        virtual ~ChartAxisTickMarks();

    public:
        void setVisible(bool visible);
        bool isVisible() const;

        void setMinorVisible(bool visible);
        bool isMinorVisible() const;

        void setThickness(int thickness);
        int thickness() const;

        void setMinorThickness(int thickness);
        int minorThickness() const;

        void setLength(int length);
        int length() const;

        void setMinorLength(int length);
        int minorLength() const;

        void setCrossAxis(bool crossAxis);
        bool isCrossAxis() const;

    private:
        Q_DISABLE_COPY(ChartAxisTickMarks)
        QTN_DECLARE_PRIVATE(ChartAxisTickMarks)
    };

}; //namespace Qtitan


#endif // QTN_CHARTAXIS_H
