#include <QtitanChart.h>
#include "chartwindow.h"


class QComboBox;
class QCheckBox;
class QSlider;
class QSpinBox;

/* PieCharts */
class PieCharts : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        PieSeries,
        DoughnuSeries,
    };
public:
    PieCharts();
    virtual ~PieCharts();

protected:
    void createSeriesParametrs();
    void createPieSeries();
    void createDoughnuSeries();
    void setValueHole(int val);
    virtual void updateValueParameters();

protected slots:
    void seriesChanged(int);
    void lablesPositionChanged(int);
    void explodedSeriesChanged(int);
    void rotateSeriesChanged(int);
    void holeSeriesChanged(int);

protected:
    QComboBox* m_seriesSwitcher;
    QComboBox* m_posDataLablesSwitcher;
    QCheckBox* m_explodedCheckBox;
    QSlider* m_rotateSlider;
    QLabel* m_captionEditorHole;
    QSpinBox* m_editorHole;

private:
    Q_DISABLE_COPY(PieCharts)
};

