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
#ifndef QTN_CHARTAXISPRIVATE_H
#define QTN_CHARTAXISPRIVATE_H

#include <QObject>
#include "QtnChartAxis.h"
#include "QtnChartLegend.h"
#include "QtnChartDataPointPrivate.h"

namespace Qtitan
{
    class ChartScaleTypeMap;
    class ChartFillDrawHelper;
    class ChartAxisGridLines;
    class ChartAxisIntervalStreak;
    class ChartAxisCustomLabel;
    class ChartAxisConstantLine;

    typedef QList<ChartAxisIntervalStreak*> StreakList;
    typedef QList<ChartAxisCustomLabel*> CustomLabelsList;
    typedef QList<ChartAxisConstantLine*> ConstantLinesList;

    /* ChartPrivate */   
    class ChartAxisPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(ChartAxis)
    public:
        explicit ChartAxisPrivate();
        virtual ~ChartAxisPrivate();

    public:
        void init();
        void setSecondary(bool secondary);

        ChartFillDrawHelper* interlacedFillHelper() const;

        QColor interlacedStartColor() const;
        void setInterlacedStartColor(const QColor& color);

        QColor interlacedFinalColor() const;
        void setInterlacedFinalColor(const QColor& color);

    public:
        virtual ChartAxisItem* createItem(GraphicsItem* parentItem);

    public:
        bool m_visible;
        bool m_reversed;
        bool m_interlaced;
        bool m_secondary;

        int m_thickness;
        int m_minorCount;
        int m_axisID;
        ChartAxis::Alignment m_alignment;
        QColor m_color;
        QColor m_clrInterlaced;
        QColor m_clrInterlaced2;
        ChartFillDrawHelper* m_interlacedFillHelper;

        qreal m_gridSpacing;
        bool m_gridSpacingAuto;
        bool m_allowZoom;
        QRect m_boundingRect;

        ChartAxisGridLines* m_gridLines;
        ChartAxisLabel* m_label;
        ChartAxisTitle* m_title;
        ChartAxisTickMarks* m_tickMarks;
        ChartScaleTypeMap* m_scaleTypeMap;
        ChartAxisRange* m_range;
        ConstantLinesList m_constantLines;
        CustomLabelsList m_customLabels;
        StreakList m_streaks;

        ChartScaleType m_scaleType;
    };

    /* ChartAxisLabelPrivate */   
    class ChartAxisLabelPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartAxisLabel)
    public:
        explicit ChartAxisLabelPrivate();
        virtual ~ChartAxisLabelPrivate();

    public:
        void init();
        ChartAxis& axis() const;

    public:
        QColor m_textColor;
        QFont m_font;
        bool m_visible;
        bool m_antialiasing;
        int m_angle;

        QString m_strFormat;
    };

    /* ChartAxisLabelPrivate */   
    class ChartAxisTitlePrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartAxisTitle)
    public:
        explicit ChartAxisTitlePrivate();

    public:
        QString m_strText;
        QColor m_textColor;
        bool m_visible;
        QFont m_font;
        bool m_antialiasing;
        Qt::Alignment m_alignment;
    };

    /* ChartAxisTickMarksPrivate */   
    class ChartAxisTickMarksPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartAxisTickMarks)
    public:
        explicit ChartAxisTickMarksPrivate();
        virtual ~ChartAxisTickMarksPrivate();
    public:
        void init();

    public:
        bool m_visible : 1;
        bool m_minorVisible : 1;
        bool m_crossAxis : 1;
        int m_thickness;
        int m_minorThickness;
        int m_length;
        int m_minorLength;
    };

    class ChartLineDrawHelper;
    /* ChartAxisGridLines */
    class ChartAxisGridLines : public ChartComponent
    {
    public:
        ChartAxisGridLines(QObject* parent = Q_NULL);
        virtual ~ChartAxisGridLines();

    public:
        bool isVisible() const;
        void setVisible(bool visible);

        bool isMinorVisible() const;
        void setMinorVisible(bool minorVisible);

        QColor color() const;
        QColor minorColor() const;

        ChartLineDrawHelper* lineHelper() const;
        ChartLineDrawHelper* minorLineHelper() const;

    protected:
        bool m_visible;
        bool m_minorVisible;
        QColor m_color;
        QColor m_minorColor;

        ChartLineDrawHelper* m_line;
        ChartLineDrawHelper* m_minorLine;

    private:
        Q_DISABLE_COPY(ChartAxisGridLines)
    };


    /* ChartAxisIntervalStreak */
    class ChartAxisIntervalStreak : public ChartComponent, public ChartLegendInterface
    {
    public:
        explicit ChartAxisIntervalStreak();
        virtual ~ChartAxisIntervalStreak();

    public:
        void setVisible(bool visible);
        bool isVisible() const;

        void setLegendVisible(bool visible);
        bool isLegendVisible() const;

        void setName(const QString& name);
        QString getName() const;

        void setLegendName(const QString& name);
        QString getLegendName() const;

        ChartFillDrawHelper* fillHelper() const;

        QString getAxisMinValue() const;
        qreal getAxisMinValueInternal() const;

        void setAxisMinValue(const QString& value);
        void setAxisMinValue(qreal value);

        QString getAxisMaxValue() const;
        qreal getAxisMaxValueInternal() const;

        void setAxisMaxValue(const QString& value);
        void setAxisMaxValue(qreal value);

        QColor getActualColor() const;
        QColor getActualColor2() const;

        QColor color() const;
        void setColor(const QColor& color);

        QColor getColor2() const;
        void setColor2(const QColor& color);

    protected:
        virtual void drawLegendComponent(QPainter* p, const QRect& boundingRect);

    protected:
        bool m_legendVisible;
        bool m_visible;

        QString m_strName;
        QString m_strLegendName;

        QString m_strAxisMinValue;
        QString m_strAxisMaxValue;

        qreal m_axisMinValue;
        qreal m_axisMaxValue;

        QColor m_color;
        QColor m_clrColor2;

        ChartFillDrawHelper* m_fillHelper;

    private:
        Q_DISABLE_COPY(ChartAxisIntervalStreak)
    };

    /* ChartAxisRange */
    class ChartAxisRange : public ChartComponent
    {
    public:
        explicit ChartAxisRange(QObject* parent = Q_NULL);
        virtual ~ChartAxisRange();

    public:
        void updateRange(ChartAxisItem* item);

        qreal maxValue() const;
        qreal minValue() const;

        void setMaxValue(qreal maxValue);
        void setMinValue(qreal minValue);

        void setAutoRange(bool autoRange);
        bool isAutoRange() const;

        qreal viewMaxValue() const;
        qreal viewMinValue() const;

        void setViewMaxValue(qreal maxValue);
        void setViewMinValue(qreal minValue);

        void setViewAutoRange(bool autoRange);
        bool isViewAutoRange() const;

    public:
        void setSideMargins(bool sideMargins);
        bool sideMargins() const;

        void setShowZeroLevel(bool showZeroLevel);
        bool showZeroLevel() const;

    public:
        qreal zoomLimit() const;
        void setZoomLimit(qreal zoomLimit);

    protected:
        void correct(ChartAxisItem* item);

    protected:
        ChartAxis* m_axis;

        qreal m_minValue;
        qreal m_maxValue;

        bool m_sideMargins;
        bool m_showZeroLevel;

        bool m_autoRange;

        qreal m_viewMinValue;
        qreal m_viewMaxValue;

        bool m_viewAutoRange;

        qreal m_zoomLimit;
        friend class ChartAxis;
    };

    /* ChartAxisCustomLabel */
    class ChartAxisCustomLabel : public ChartComponent
    {
    public:
        ChartAxisCustomLabel();
        virtual ~ChartAxisCustomLabel();

    public:
        void setText(const QString& title);
        QString text() const;

        QString axisValue() const;
        qreal axisValueInternal() const;

        void setAxisValue(const QString& value);
        void setAxisValue(qreal value);

    protected:
        QString m_strText;
        QString m_strAxisValue;
        qreal m_axisValue;
    private:
        Q_DISABLE_COPY(ChartAxisCustomLabel)
    };

    /* ChartAxisConstantLine */
    class ChartAxisConstantLine : public ChartTextComponent, 
                                  public ChartLegendInterface
    {
    public:
        ChartAxisConstantLine();
        virtual ~ChartAxisConstantLine();

    public:
        void setVisible(bool visible);
        bool isVisible() const;

        void setLegendVisible(bool visible);
        bool isLegendVisible() const;

        void setText(const QString& title);
        const QString& text() const;

        void setTextColor(const QColor& txtColor);
        QColor textColor() const;

        QColor getActualTextColor() const;

        const QFont& font() const;
        void setFont(const QFont& font);

        void setAlignment(Qt::Alignment alignment);
        Qt::Alignment alignment() const;

        void setAntialiasing(bool antialiasing);
        bool antialiasing() const;

        QString legendName() const;
        void setLegendName(const QString& strName);

        ChartLineDrawHelper* lineHelper() const;

        QString axisValue() const;
        qreal axisValueInternal() const;

        void setAxisValue(const QString& value);
        void setAxisValue(qreal value);

        QColor getActualColor() const;

        bool isShowBehind() const;
        void setShowBehind(bool showBehind);

        void showTextBelow(bool showTextBelow);
        bool isTextBelow() const;

        const QColor& color() const;
        void setColor(const QColor& color);

    protected:
        virtual void drawLegendComponent(QPainter* p, QRect boundingRect);

    protected:
        bool m_legendVisible;
        bool m_visible;

        QString m_strText;
        QString m_strLegendName;
        QString m_strAxisValue;
        qreal m_axisValue;

        QColor m_textColor;
        QColor m_color;

        Qt::Alignment m_alignment;
        bool m_antialiasing;
        QFont m_font;
        bool m_showTextBelow;
        bool m_showBehind;

        ChartLineDrawHelper* m_line;
    private:
        Q_DISABLE_COPY(ChartAxisConstantLine)
    };

}; //namespace Qtitan



#endif // QTN_CHARTAXISPRIVATE_H
