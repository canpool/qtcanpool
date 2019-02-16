
#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;

#define YEARS_NUM 5

/* LineCharts */
class LineCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        LineSeries,
        SplineSeries,
        StepLineSeries,
    };
public:
    LineCharts();
    virtual ~LineCharts();

protected:
    void createSeriesParametrs();
    void createLineSeries();
    void createSplineSeries();
    void createStepLineSeries();

protected slots:
    void seriesChanged(int);

protected:
    QComboBox* m_seriesSwitcher;

protected:
    QString m_arYears[YEARS_NUM];

private:
    Q_DISABLE_COPY(LineCharts)
};

