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
#ifndef QTN_CHARTLEGENDPRIVATE_H
#define QTN_CHARTLEGENDPRIVATE_H

#include "QtitanDef.h"
#include "QtnGraphicsItem.h"
#include "QtnChartLegend.h"

class QLinearGradient;


namespace Qtitan
{
    class ChartLegend;
    class ChartLegendInterface;
    /* ChartLegendItem */
    class ChartLegendItem : public GraphicsItem
    {
        Q_OBJECT
    public:
        explicit ChartLegendItem(ChartLegend* legend, GraphicsItem* parentItem);
        virtual ~ChartLegendItem();

    public:
        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

        void calculateItem(QRect& rcChart);
        QSize size();
        void addItem(ChartLegendInterface* pItem);

    protected:
        ChartLegend* m_legend;
        QRect m_boundingRect;
        QVector<ChartLegendInterface*> m_arrItems;

    private:
        Q_DISABLE_COPY(ChartLegendItem)
    };

    /* ChartBorder */
    class ChartBorder : public QObject
    {
        Q_OBJECT
    public:
        explicit ChartBorder(QObject* parent);
        virtual ~ChartBorder();

    public:
        bool isVisible() const;
        void setVisible(bool visible);

        const QColor& color() const;
        void setColor(const QColor& clr);

        int thickness() const;
        void setThickness(int thickness);

        int borderRounded() const;
        void setBorderRounded(int round);

    public:
        void drawBorder(QPainter* p, const QRectF& boundingRect, const QColor& color);
        void drawInnerBorder(QPainter* p, const QRectF& boundingRect, const QColor& color, int xround = 0, int yround = 0);

    protected:
        QColor m_color;
        int m_thickness;
        int m_round;
        bool m_visible;

    private:
        Q_DISABLE_COPY(ChartBorder)
    };

    class ChartFillDrawHelper;
    /* ChartLegendPrivate */   
    class ChartLegendPrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartLegend)
    public:
        explicit ChartLegendPrivate();
        virtual ~ChartLegendPrivate();

    public:
        void init();
        ChartFillDrawHelper* fillHelper() const;
        virtual ChartLegendItem* createItem(GraphicsItem* parentItem);

    public:
        bool m_visible       : 1;
        bool m_antialiasing  : 1;
        bool m_markerVisible : 1;

        ChartBorder* m_border;
        ChartFillDrawHelper* m_fillHelper;
        QFont m_font;
        QSize m_markerSize;

        QLinearGradient* m_clrBackground;
        QColor m_colorText;

        int m_columnCount;

        ChartLegend::LegendAlignment m_horizontalAlignment;
        ChartLegend::LegendAlignment m_verticalAlignment;
        ChartLegend::LegendDirection m_direction;
    };

}; //namespace Qtitan


#endif // QTN_CHARTLEGENDPRIVATE_H
