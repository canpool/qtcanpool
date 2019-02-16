
#include <QtitanChart.h>
#include "chartwindow.h"

class QComboBox;
class QDoubleSpinBox;
class QSlider;
class QCheckBox;


/* PyramidCharts */
class PyramidCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        PyramidSeries,
        Pyramid3DSeries,
    };
public:
    PyramidCharts();
    virtual ~PyramidCharts();

protected:
    void createSeriesParametrs();
    void createFunnelSeries();
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int index);
    void transparencyChanged(int value);
    void pointGapChanged(int value);
    void rotationChanged(int value);
    void depthPercentChanged(int value);
    void lablesPositionChanged(int);
    void circularSwitcher(int);

protected:
    QComboBox* m_seriesSwitcher;
    QCheckBox* m_circularSwitcher;
    QSlider* m_transparency;
    QSlider* m_pointGap;
    QLabel* m_labelRotation;
    QSlider* m_rotation;
    QLabel* m_labeldepthPercen;
    QSlider* m_depthPercent;

private:
    Q_DISABLE_COPY(PyramidCharts)
};

