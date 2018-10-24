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
#ifndef QTN_CHARTTHEMEPRIVATE_H
#define QTN_CHARTTHEMEPRIVATE_H

#include <QObject>
#include <QColor>
#include <QBrush>

#include "QtnChart.h"
#include "QtitanDef.h"
#include "QtnChartPainter.h"

class QDomElement;

namespace Qtitan
{
    class ChartFillDrawHelper;
    /* ChartTitleTheme */
    class ChartTitleTheme : public QObject
    {
    public:
        explicit ChartTitleTheme(QObject* parent = Q_NULL);
        void extractXML(const QDomElement& el);
        void resetAllElements();
    public:
        QColor m_textColor;
    private:
        Q_DISABLE_COPY(ChartTitleTheme)
    };

    /* ChartLegendTheme */
    class ChartLegendTheme : public QObject
    {
    public:
        explicit ChartLegendTheme(QObject* parent = Q_NULL);
        void extractXML(const QDomElement& el);
        void resetAllElements();
    public:
        QColor m_textColor;
        ChartFillDrawHelper* m_fillHelper;
        QColor m_borderColor;
        int m_round;
    private:
        Q_DISABLE_COPY(ChartLegendTheme)
    };

    /* ChartFrameTheme */
    class ChartFrameTheme : public QObject
    {
    public:
        explicit ChartFrameTheme(QObject* parent = Q_NULL);
        virtual ~ChartFrameTheme();
    public:
        ChartTitleTheme* getTitleTheme() const;
        ChartLegendTheme* getLegendTheme() const;

        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        ChartFillDrawHelper* m_fillHelper;
        QColor m_borderColor;
        int m_round;
    protected:
        ChartTitleTheme* m_titleTheme;
        ChartLegendTheme* m_legendTheme;

    private:
        Q_DISABLE_COPY(ChartFrameTheme)
    };

    /* ChartTheme */
    class ChartAxisTheme : public QObject
    {
    public:
      explicit ChartAxisTheme(QObject* parent = Q_NULL);
      virtual ~ChartAxisTheme();
      void extractXML(const QDomElement& el);
      void resetAllElements();

    public:
      QColor m_color;
      QColor m_interlacedColor;
      QColor m_interlacedColor2;
      ChartFillDrawHelper* m_interlacedFillHelper;

      QColor m_gridLinesColor;
      QColor m_gridLinesMinorColor;
      QColor m_axisLabelTextColor;
      QColor m_axisTitleTextColor;

      QColor m_constantLineColor;
      QColor m_constantLineTextColor;

      QColor m_stripColor;
      QColor m_stripColor2;

    private:
        Q_DISABLE_COPY(ChartAxisTheme)
    };

    /* ChartSeriesLabelTheme */
    class ChartSeriesLabelTheme : public QObject
    {
    public:
        explicit ChartSeriesLabelTheme(QObject* parent = Q_NULL);
        virtual ~ChartSeriesLabelTheme();
        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        QColor m_backgroundColor;
        QColor m_textColor;
        QColor m_borderColor;
        QColor m_connectorColor;

    private:
        Q_DISABLE_COPY(ChartSeriesLabelTheme)
    };

    /* ChartSeriesTheme */
    class ChartSeriesTheme : public QObject
    {
    public:
        explicit ChartSeriesTheme(QObject* parent = Q_NULL);
        virtual ~ChartSeriesTheme();
        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        ChartSeriesLabelTheme* labelTheme() const;

    protected:
        ChartSeriesLabelTheme* m_labelTheme;

    private:
        Q_DISABLE_COPY(ChartSeriesTheme)
    };

    /* ChartPieSeriesTheme */
    class ChartPieSeriesTheme : public QObject
    {
    public:
        explicit ChartPieSeriesTheme(QObject* parent = Q_NULL);
        virtual ~ChartPieSeriesTheme();
        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        QColor m_borderColor;

    private:
        Q_DISABLE_COPY(ChartPieSeriesTheme)
    };

    /* ChartAreas2DTheme */
    class ChartAreas2DTheme : public QObject
    {
    public:
        explicit ChartAreas2DTheme(QObject* parent = Q_NULL);
        virtual ~ChartAreas2DTheme();
        void extractXML(const QDomElement& el);
        void resetAllElements();

    public:
        ChartAxisTheme* axisTheme() const { return m_axisTheme; }

    public:
        QColor m_borderColor;
        ChartFillDrawHelper* m_backgroundFillHelper;

    protected:
        ChartAxisTheme* m_axisTheme;
    private:
        Q_DISABLE_COPY(ChartAreas2DTheme)
    };

    class ChartPalette;
    /* ChartTheme */
    class ChartTheme : public QObject
    {
    public:
        explicit ChartTheme(QObject* parent);
        virtual ~ChartTheme();

    public:
        ChartFrameTheme* frameTheme() const;
        ChartAreas2DTheme* area2DTheme() const;
        ChartSeriesTheme* seriesTheme() const;
        ChartPieSeriesTheme* pieTheme() const;
        ChartPalette* chartPalette() const;

        static ChartTheme* themeTopParent(const QObject* element);

    public:
        void setTheme(Chart::Theme theme);
        Chart::Theme theme() const;
        bool loadTheme(const QString& theme);
    
    public:
        static QColor lightColor(const QColor& clr);

    protected:
        void extractXML(const QDomElement& el);
        void resetAllElements();

    protected:
        Chart::Theme m_currentTheme;
        ChartAreas2DTheme* m_area2DTheme;
        ChartFrameTheme* m_frameTheme;
        ChartSeriesTheme* m_seriesTheme;
        ChartPieSeriesTheme* m_pieImplTheme;
        ChartPalette* m_palette;

    private:
        Q_DISABLE_COPY(ChartTheme)
    };

}; //namespace Qtitan


#endif // QTN_CHARTTHEMEPRIVATE_H
