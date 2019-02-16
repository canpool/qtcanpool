#include <QTimer>
#include "chartwindow.h"


class QPushButton;

/* LargeValueCharts */
class LargeValueCharts : public ChartWindow
{
    Q_OBJECT
public:
    LargeValueCharts();
    virtual ~LargeValueCharts();

protected:
    void createSeriesParametrs();
    void createChart();

protected slots:
    void start();
    void stop();
    void updateFrequency(int);
    void animation();

protected:
    QVector<qreal> m_values;
    int m_updateFr;
    QTimer m_timer;
    QPushButton* m_startButton;
    QPushButton* m_stopButton;

private:
    Q_DISABLE_COPY(LargeValueCharts)
};

