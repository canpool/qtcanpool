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
#ifndef QTN_CHART_H
#define QTN_CHART_H

#include <QWidget>

#include "QtitanDef.h"
#include "QtnAbstractSeries.h"
#include "QtnChartComponent.h"

namespace Qtitan
{
    class ChartTitle;

    typedef QList<AbstractSeries*> SeriesList;
    typedef QList<ChartArea*> AreaList;
    typedef QList<ChartTitle*> TitleList;

    class ChartLegend;
    class ChartPrivate;
    /* Chart */
    class QTITAN_EXPORT Chart : public QWidget
    {
        Q_OBJECT
        Q_PROPERTY(Qt::Orientation areasOrientation READ areasOrientation WRITE setAreasOrientation)
        Q_PROPERTY(int areasSpacing READ areasSpacing WRITE setAreasSpacing)
        Q_PROPERTY(Chart::Theme theme READ theme WRITE setTheme)
        Q_ENUMS(ChartTheme)
    public:
        enum Theme
        {
            Default,
            Office,
            Fire,
            Sunny,
            Blue,
            Gray,
            Green,
            AzureOffice,
            BlueSunny,
            GreenGreen,
            GrayGray,
            BlackBlue,
        };

    public:
        explicit Chart(QWidget* parent = Q_NULL);
        virtual ~Chart();

    public:
        const SeriesList& series() const;
        void appendSeries(AbstractSeries* series);
        bool removeSeries(AbstractSeries* series);
        void clearSeries();

        const TitleList& titles() const;
        void appendTitle(ChartTitle* title);
        void clearTitles();

        const AreaList& areas() const;
        void appendArea(ChartArea* area);
        void clearAreas();

        ChartLegend* legend() const;
        ChartAxis* axis(int axisID) const;

        void setTheme(Chart::Theme theme);
        Chart::Theme theme() const;

        bool isBorderVisible() const;
        const QColor& borderColor() const;
        void setBorderColor(const QColor& clr);

        QBrush backgroundBrush() const;
        void setBackgroundBrush(const QBrush& br);

        int borderThickness() const;
        int borderRounded() const;

        void setAreasOrientation(Qt::Orientation orientation);
        Qt::Orientation areasOrientation() const;
        void setAreasSpacing(int spacing);
        int areasSpacing() const;
        QRect plotAreas() const;
        
    public:
        void exportToFile(const QString& fileName);
        void enableMouseTrackingMode(unsigned long hitInfoFlags = ChartHitInfo::HitAllElements);
        void chartUpdate(ChartComponent::UpdateOptions opt = ChartComponent::UpdateView);

    private:
        ChartAxis* axis(int axisID, ChartComponent* owner) const;
        void seriesImplChanged(ChartDataTablePrivate* data);

    public:// Q_SLOTS:
        void updateView();
        void updateLayout();
        void updateRedraw();

    public Q_SLOTS:
        void setBorderVisible(bool visible);
        void setBorderThickness(int thickness);
        void setBorderRounded(int round);

    Q_SIGNALS:
        void chartMouseTrack(ChartHitInfo* hit);
        void chartMouseUp(ChartHitInfo* hit);

    protected:
        virtual void paintEvent(QPaintEvent* event);
        virtual void mousePressEvent(QMouseEvent* event);
        virtual void mouseReleaseEvent(QMouseEvent* event);
        virtual void mouseMoveEvent(QMouseEvent* event);
        virtual void leaveEvent(QEvent* event);
        virtual void resizeEvent(QResizeEvent* event);

    private:
        friend class ChartAxisX;
        friend class ChartAxisY;
        friend class ChartDataTablePrivate;
        friend class ChartTheme;
        friend class Chart2DItem;
        Q_DISABLE_COPY(Chart)
        QTN_DECLARE_PRIVATE(Chart)
    };

}; //namespace Qtitan


#endif // QTN_CHART_H
