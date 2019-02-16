
#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;
class QDoubleSpinBox;
class QSlider;


/* FunnelCharts */
class FunnelCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        FunnelSeries,
        Funnel3DSeries,
    };
public:
    FunnelCharts();
    virtual ~FunnelCharts();

protected:
    void createSeriesParametrs();
    void createFunnelSeries();
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int index);
    void transparencyChanged(int value);
    void pointGapChanged(int value);
    void neckHeightInPercentsChanged(int value);
    void lablesPositionChanged(int);
protected:
    QComboBox* m_seriesSwitcher;
    QSlider* m_transparency;
    QSlider* m_pointGap;
    QSlider* m_neckHeightInPercents;

private:
    Q_DISABLE_COPY(FunnelCharts)
};

