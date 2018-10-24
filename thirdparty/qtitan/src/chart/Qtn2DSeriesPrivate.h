/****************************************************************************
**
** Qtitan Library by Developer Machines (Charts for Qt.C++)
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
#ifndef QTN_2DSERIESPRIVATE_H
#define QTN_2DSERIESPRIVATE_H

#include <QColor>

#include "QtnChartDefines.h"
#include "QtnChart2DSeries.h"
#include "QtnSeriesPrivate.h"


namespace Qtitan
{
    class ChartDataTablePrivate;
    /* ChartPrivate */   
    class Chart2DSeriesPrivate : public AbstractSeriesPrivate
    {
    public:
        QTN_DECLARE_EX_PUBLIC(Chart2DSeries)
    public:
        explicit Chart2DSeriesPrivate();
        virtual ~Chart2DSeriesPrivate();

        virtual void linkData();
        virtual void unlinkData();
        virtual ChartArea* createArea(QObject* parent);
        virtual bool isSeriesArea(ChartArea* area) const;
        virtual void setArea(ChartArea* area);

    public:
        ChartDataTablePrivate* m_data;
        bool m_secondaryAxisX;
        bool m_secondaryAxisY;

    private:
        Q_DISABLE_COPY(Chart2DSeriesPrivate)
    };

    class ChartAxisItem;
    /* ChartArea2DSeriesItem */
    class ChartArea2DSeriesItem : public ChartSeriesItem
    {
    protected:
        explicit ChartArea2DSeriesItem(ChartDataTablePrivate* series, ChartAreaItem* areaItem, bool sortPoints = false);

    public:
        ChartAxisItem* axisItemX() const;
        ChartAxisItem* axisItemY() const;

        QPointF screenPoint(qreal x, qreal y) const;

        virtual void afterUpdateRange();
        virtual void beforeUpdateRange();

    protected:
        ChartAxisItem* m_axisItemX;
        ChartAxisItem* m_axisItemY;

        bool m_sortPoints;

        friend class ChartArea2DItem;
        friend class ChartAreaFormItem;
    };

    /* ChartArea2DSeriesLabelItem */
    class ChartArea2DSeriesLabelItem : public ChartSeriesLabelItem
    {
    public:
        explicit ChartArea2DSeriesLabelItem(ChartSeriesLabel* label, ChartSeriesPointItem* pointItem, GraphicsItem* parentItem);
        virtual ~ChartArea2DSeriesLabelItem();

    public:
        virtual void paint(QPainter* p);

    public:
        void paint(QPainter* p, const QPointF& centerPoint);
        void paint(QPainter* p, const QPointF& centerPoint, const QString& text);
        void paint(QPainter* p, const QPointF& locationPoint, qreal locationAngle, const QString& text);

        virtual QPointF locationPoint() const;
        virtual qreal locationAngle() const;

        virtual void calculateLayout();
        void calculateLayout(QPointF centerPoint);

        void calculateLayout(const QPointF& locationPoint, qreal locationAngle, const QString& text);
        void calculateLayout(QPointF centerPoint, const QString& text);

    protected:
        QRect formRect() const;
    };

}; //namespace Qtitan


#endif // QTN_2DSERIESPRIVATE_H
