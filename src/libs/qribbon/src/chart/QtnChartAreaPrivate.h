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
#ifndef QTN_CHARTAREAPRIVATE_H
#define QTN_CHARTAREAPRIVATE_H

#include <QRectF>

#include "QtnChartDefines.h"
#include "QtnChartArea2D.h"
#include "QtnGraphicsItem.h"


namespace Qtitan
{
    class GraphicsItem;
    class ChartAreaItem;
    class ChartAreaForm;
    class ChartDataTablePrivate;

    /* ChartAreaPrivate */
    class ChartAreaPrivate : public QObject
    {
        Q_OBJECT
    public:
        QTN_DECLARE_PUBLIC(ChartArea)
    public:
        explicit ChartAreaPrivate();
        virtual ~ChartAreaPrivate();
        void init();
    public:
        void createTitleItem(GraphicsItem* parentItem);
        void calculateTitleItem(QRect& rcChart, GraphicsItem* parentItem);

        virtual bool seriesAdded(ChartDataTablePrivate* series);
        int seriesRemoved(ChartDataTablePrivate* series);

    public:
        virtual void calculateSeriesLayout(ChartAreaItem* item);
        virtual ChartAreaItem* createItem(GraphicsItem* parent);

    public:
        QList<ChartDataTablePrivate*> m_listDataTables;
        QList<ChartTitle*> m_listTitles;
        ChartAreaForm* m_form;

    private:
        Q_DISABLE_COPY(ChartAreaPrivate)
    };

    /* ChartAreaItem */
    class ChartAreaItem : public GraphicsItem
    {
        Q_OBJECT
    public:
        explicit ChartAreaItem(ChartArea* area, GraphicsItem* parentItem);

    public:
        QRect boundingRect() const;
        ChartArea* area() const;
        GraphicsItem* labelsItem() const;
        GraphicsItem* seriesItem() const;

        virtual int calculateAxisLabelSize();
        virtual void calculateItem(QRect& boundingRect, int labelSize = -1);
        virtual void updateRange();
        virtual void createItem();

    protected:
        virtual void paint(QPainter* p);

    protected:
        ChartArea* m_area;
        QRect m_boundingRect;
        GraphicsItem* m_labelsItem;
        GraphicsItem* m_seriesItem;
        GraphicsItem* m_titlesItem;
    };

    class ChartAxis;
    /* ChartArea2DPrivate */
    class ChartArea2DPrivate : public ChartAreaPrivate
    {
        Q_OBJECT
    public:
        QTN_DECLARE_EX_PUBLIC(ChartArea2D)
    public:
        explicit ChartArea2DPrivate();
        virtual ~ChartArea2DPrivate();

    public:
        void init();

    public:
        virtual ChartAreaItem* createItem(GraphicsItem* parent);

    public:
        QRect m_boundingRect;

        ChartAxis* m_axisX;
        ChartAxis* m_axisY;

        ChartAxis* m_secondaryAxisX;
        ChartAxis* m_secondaryAxisY;

        bool m_rotated : 1;
        bool m_allowZoom : 1;
        bool m_allowScroll : 1;

    private:
        Q_DISABLE_COPY(ChartArea2DPrivate)
    };

    class ChartAreaFormItem;
    /* ChartArea2DItem */
    class ChartArea2DItem : public ChartAreaItem
    {
        Q_OBJECT
    public:
        explicit ChartArea2DItem(ChartArea* area, GraphicsItem* parent);
        virtual ~ChartArea2DItem();

    public:
        ChartAreaFormItem* areaFormItem() const;

    public:
        void createItem();
        int calculateAxisLabelSize();
        void calculateItem(QRect& boundingRect, int labelSize = -1);
        void updateRange();
        void addAxisItem(GraphicsItem* parentItem, ChartAxis* axis);
        GraphicsItem* axisItem() const;

        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

    public:
        void checkLabelBounds(const QRectF& boundingRect);

    public:
        GraphicsItem* m_axisItem;
        ChartAreaFormItem* m_areaForm;
        QRect m_rcLabelPadding;
    };

    class ChartFillDrawHelper;
    /* ChartAreaForm */
    class ChartAreaForm : public ChartComponent
    {
        Q_OBJECT
    public:
        explicit ChartAreaForm(ChartArea* area);
        virtual ~ChartAreaForm();

    public:
        virtual GraphicsItem* createItem(GraphicsItem* parent);

    public:
        const QBrush& backgroundBrush() const;
        void setBackgroundBrush(const QBrush& brush);

        const QColor& backgroundColor() const;
        void setBackgroundColor(const QColor& color);

        const QColor& borderColor() const;
        void setBorderColor(const QColor& color);

        ChartFillDrawHelper* fillHelper() const;
        ChartFillDrawHelper* currentFillHelper() const;

    public:
        ChartAreas2DTheme* theme() const;

    protected:
        QBrush m_backgroundBrush;
        QColor m_backgroundColor;
        QColor m_borderColor;
        ChartFillDrawHelper* m_backgroundFillHelper;
    };

    class ChartSeriesItem;
    class ChartAxisItem;
    class ChartAxis;
    /* ChartAreaFormItem */
    class ChartAreaFormItem : public GraphicsItem
    {
        Q_OBJECT
    public:
        explicit ChartAreaFormItem(ChartAreaForm* pane, GraphicsItem* parent);
        ~ChartAreaFormItem();

    public:
        QRect bounds() const;

        QPointF screenPoint(const ChartSeriesItem* item, qreal x, qreal y) const;

        void calculateItem(const QRect& boundingRect);

    public:
        ChartAxisItem* axisItem(ChartAxis* axis) const;

        GraphicsItem* createSeriesItem();

    protected:
        virtual void paint(QPainter* p);

        virtual void drawGridLines(QPainter* p, ChartAxisItem* axis);
        virtual void drawInterlaced(QPainter* p, ChartAxisItem* axis);
        virtual void drawConstantLines(QPainter* p, ChartAxisItem* axis, bool behind);
        virtual void drawStrips(QPainter* p, ChartAxisItem* axis);

    public:
        ChartAreaForm* m_form;
        GraphicsItem* m_seriesItem;
        QRect m_boundingRect;
        QPoint m_ptOldPosition;
    };

}; //namespace Qtitan


#endif //QTN_CHARTAREAPRIVATE_H
