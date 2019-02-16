#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;

/* BarCharts */
class BarCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        ClusteredSeries,
        StackedSeries,
        Stacked100Series,
        SideBySideSeries,
    };
public:
    BarCharts();
    virtual ~BarCharts();

protected:
    void createSeriesParametrs();
    ChartBarSeries* createSeries(SeriesType type);
    void createBardSeries(SeriesType type);
    void createSideBySideSeries();
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int);
    void lablesPositionChanged(int);
    void typeSeriesChanged(int);

protected:
    QComboBox* m_seriesSwitcher;
    QComboBox* m_posDataLablesSwitcher;

private:
    Q_DISABLE_COPY(BarCharts)
};

