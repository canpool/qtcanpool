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
#ifndef QTN_CHARTPRIVATE_H
#define QTN_CHARTPRIVATE_H

#include <QObject>

#include "QtnChartDefines.h"
#include "QtnChart.h"

class QRect;
class QPoint;

namespace Qtitan
{
    class ChartTheme;
    class Chart2DItem;
    class ChartBorder;
    class ChartLegend;

    /* ChartPrivate */   
    class ChartPrivate : public QObject
    {
        Q_OBJECT
    public:
        QTN_DECLARE_PUBLIC(Chart)
    public:
        explicit ChartPrivate();
        virtual ~ChartPrivate();

    public:
        void init();
        Chart2DItem* createItem(const QRect& boundingRect);
        ChartFillDrawHelper* fillHelper() const;
        void updateChart();

    public:
        void mouseReleaseEvent(Chart2DItem* item, const QPoint& pt, Qt::MouseButton button);
        void mouseMoveEvent(Chart2DItem* item, const QPoint& pt, Qt::MouseButton button);

    protected:
        bool hitTestInternal(Chart2DItem* item, const QPoint& pt, ChartHitInfo* hitInfo);

    public:
        Chart2DItem* m_chartItem;

        ChartTheme* m_theme;
        ChartBorder* m_border;
        ChartLegend* m_legend;

        AreaList m_listArea;
        TitleList m_listTitle;
        SeriesList m_listSeries;

        QBrush* m_background;
        ChartFillDrawHelper* m_fillBackground;

        int m_areasLayoutSpacing;
        bool m_updateOptions;

        ChartHitInfo::HitInfoTest m_hitInfoFlags;
        ChartHitInfo m_lastHit;
        Qt::Orientation m_areasOrientation; 

    private:
        Q_DISABLE_COPY(ChartPrivate)
    };
}; //namespace Qtitan


#endif // QTN_CHARTPRIVATE_H
