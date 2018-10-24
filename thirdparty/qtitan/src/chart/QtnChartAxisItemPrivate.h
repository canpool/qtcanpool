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
#ifndef QTN_CHARTAXISITEMPRIVATE_H
#define QTN_CHARTAXISITEMPRIVATE_H


#include "QtnGraphicsItem.h"

namespace Qtitan
{
    class ChartAxis;
    class ChartSeriesItem;
    class ChartAxisViewTickEnumerator;

    /* ChartAxisItem */
    class ChartAxisItem : public GraphicsItem
    {
        class ChartAxisViewTick
        {
        public:
            qreal m_value;
            QString m_strLabel;
            QSize m_szBounds;
            QSizeF m_szLabel;
        };

    public:
        explicit ChartAxisItem(ChartAxis* axis, GraphicsItem* parentItem);
        ~ChartAxisItem();

    public:
        QRect bounds() const;

    public:
        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

        void drawGridLines(QPainter* p, const QRect& rcPane);
        void drawInterlaced(QPainter* p, const QRect& rcPane);

        void drawTickMarks(QPainter* p);

        void drawLabels(QPainter* p);
        void drawTitle(QPainter* p);

        void drawConstantLines(QPainter* p, const QRect& rcPane, bool behind);
        void drawStrips(QPainter* p, const QRect& rcPane);

        ChartAxisViewTickEnumerator* createTickMarksEnumerator(bool minor);

        void createItem();
        void addLegendItems();

        int calcLabelSize();
        void calcSize(const QRect& rectArea, int labelSize = -1);
        void setBoundingRect(const QRect& boundingRect);

        ChartAxis* axis() const;

        qreal valueToPoint(qreal value) const;

        double distanceToPoint(double x) const;

        qreal scale() const;

        bool isScollBarVisible() const;

        int size() const;

        bool isVertical() const;

        qreal rangeMinValue() const;
        qreal rangeMaxValue() const;
        qreal rangeMinValueItem() const;
        qreal rangeMaxValueItem() const;

        qreal gridSpacing() const;

    protected:
        qreal calculateGridSpacing(qreal axisRangeDelta, qreal screenDelta, qreal gridSpacingFactor);
        void createTickMarks(QRect rectArea);

    public:
        QVector<ChartAxisViewTick> m_arrTicks;
        QVector<ChartSeriesItem*> m_listDataTables;

    protected:
        ChartAxis* m_axis;
        QRect m_boundingRect;
        int m_size;
        qreal m_gridSpacing;
    };
}; //namespace Qtitan


#endif // QTN_CHARTAXISITEMPRIVATE_H
