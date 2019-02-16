#include <QtitanChart.h>
#include "chartwindow.h"


class QComboBox;
class QCheckBox;

/* AdvancedFeatures */
class AdvancedFeatures : public ChartWindow
{
    Q_OBJECT
protected:
    enum SeriesType
    {
        SecondaryAxis,
        Interactive,
    };
public:
    AdvancedFeatures();
    virtual ~AdvancedFeatures();

protected:
    void createSeriesParametrs();
    void createSecondaryAxis();
    void createInteractive();
    virtual void updateValueParameters();

protected slots:
    void mouseTrack(ChartHitInfo* hit);
    void mouseUp(ChartHitInfo* hit);

protected slots:
    void seriesChanged(int);
    void showSecondaryAxisXChanged(int);
    void showAxisXChanged(int);
    void showSecondaryAxisYChanged(int);
    void showAxisYChanged(int);


protected:
    QComboBox* m_seriesSwitcher;
    QCheckBox* m_showSecondaryAxisX;
    QCheckBox* m_showSecondaryAxisY;
    QLabel* m_hitTestResult;

    ChartAxis* m_axisY1;
    ChartAxis* m_axisY2;

private:
    Q_DISABLE_COPY(AdvancedFeatures)
};

