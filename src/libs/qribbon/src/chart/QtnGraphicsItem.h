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
#ifndef QTN_CHARTPRIMITIVE_H
#define QTN_CHARTPRIMITIVE_H

#include <QObject>
#include <QVector>
#include <QRect>

#include "QtnChartDefines.h"

class QPainter;

namespace Qtitan
{
    class ChartContainer;
    struct ChartHitInfo;

    /* GraphicsItem */
    class GraphicsItem : public QObject
    {
        Q_OBJECT
    public:
        explicit GraphicsItem(QObject* parent);
        virtual ~GraphicsItem();

    public:
        int count() const;
        GraphicsItem* at(int index) const;
        
        GraphicsItem* parentItem() const;
        void appendItem(GraphicsItem* childItem);
        GraphicsItem** children();
        
        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

    protected:
        QVector<GraphicsItem*> m_listItem;

    private:
        Q_DISABLE_COPY(GraphicsItem)
    };

    class Chart;
    class ChartLegendItem;
    /* Chart2DItem */
    class Chart2DItem : public GraphicsItem 
    {
    public:
        explicit Chart2DItem(Chart* chart);
        virtual ~Chart2DItem();

    public:
        ChartLegendItem* legendItem() const;

        void createItem();
        void calculateItem(const QRect& rect);

    public:
        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

    protected:
        QRect m_boundingRect;
        Chart* m_chart;
        ChartLegendItem* m_legendItem;
        GraphicsItem* m_titlesItem;
        GraphicsItem* m_areaItem;

    private:
        Q_DISABLE_COPY(Chart2DItem)
    };

}; //namespace Qtitan


#endif // QTN_CHARTPRIMITIVE_H
