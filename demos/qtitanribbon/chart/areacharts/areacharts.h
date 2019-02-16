#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;
class QSlider;

/* AreaCharts */
class AreaCharts : public ChartWindow
{
    Q_OBJECT
public:
    enum SeriesType
    {
        AreaLineSeries,
        StackedAreaLineSeries,
        Stacked100AreaLineSeries,
        AreaSplineSeries,
        StackedAreaSplineSeries,
        Stacked100AreaSplineSeries, 
    };
public:
    AreaCharts();
    virtual ~AreaCharts();

protected:
    void createSeriesParametrs();
    void createAreaSeries(SeriesType type);
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int);
    void transparencyChanged(int);

protected:
    QComboBox* m_seriesSwitcher;
    QSlider* m_transparency;

private:
    Q_DISABLE_COPY(AreaCharts)
};

