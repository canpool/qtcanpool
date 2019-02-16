
#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;
class QDoubleSpinBox;
class QSlider;

#define YEARS_NUM 5

/* ScatterCharts */
class ScatterCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        PointsSeries,
        BubbleSeries,
        Bubble3DSeries,
    };
public:
    ScatterCharts();
    virtual ~ScatterCharts();

protected:
    void createSeriesParametrs();
    void createPointsSeries();
    void createBubbleSeries();
    void createBubble3DSeries();
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int);
    void transparencyChanged(int);
    void valueMinChanged(double);
    void valueMaxChanged(double);

protected:
    QComboBox* m_seriesSwitcher;
    QGroupBox* m_pointsSeriesBox;
    QLabel* m_minLabelBox;
    QDoubleSpinBox* m_minBox;
    QLabel* m_maxLabelBox;
    QDoubleSpinBox* m_maxBox;
    QSlider* m_transparency;

private:
    Q_DISABLE_COPY(ScatterCharts)
};

