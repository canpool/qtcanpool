#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QFormLayout>
#include <qmath.h>

#include "largevaluecharts.h"

#define MAX_POINTS_IN_RANGE     750
#define MIN_VAL                 -15.0
#define MAX_VAL                 15.0
#define MIN_FREQUENCY           16

/* LargeValueCharts */
LargeValueCharts::LargeValueCharts()
    : ChartWindow(tr("Large Value"), 0)
{
    m_updateFr = 100;
    createSeriesParametrs();
    createChart();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(animation()));
}

LargeValueCharts::~LargeValueCharts()
{
}

void LargeValueCharts::createSeriesParametrs()
{
    // Option Series
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Real Time"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_startButton = new QPushButton(tr("Start"), seriesTypeGroup);
    connect(m_startButton, SIGNAL(pressed()), this, SLOT(start()));

    m_stopButton = new QPushButton(tr("Stop"), seriesTypeGroup);
    connect(m_stopButton, SIGNAL(pressed()), this, SLOT(stop()));
    m_stopButton->setEnabled(false);

    QLabel* updateFrequencyText = new QLabel(tr("Update Frequency:"), seriesTypeGroup);

    QSlider* frequencySlider = new QSlider(Qt::Horizontal, seriesTypeGroup);
    frequencySlider->setRange(50, 300);
    frequencySlider->setSliderPosition(m_updateFr);
    frequencySlider->setSingleStep(5);
    connect(frequencySlider, SIGNAL(valueChanged(int)), this, SLOT(updateFrequency(int)));
    // Layouts Series
    localLayout->addRow(m_startButton);
    localLayout->addRow(m_stopButton);
    localLayout->addRow(updateFrequencyText);
    localLayout->addRow(frequencySlider);
}

void LargeValueCharts::createChart()
{
    int totalPoints = 16000 / MAX_POINTS_IN_RANGE + 1;

    ChartBarSeries* series = new ChartBarSeries();
    series->label()->setVisible(false);
    series->setDistanceFixed(25);
    m_chart->appendSeries(series);

    ChartArea2D* area = (ChartArea2D*)series->area();
    ChartAxis* axisX = area->axisX();
    axisX->setGridSpacing(2.1);
    axisX->setGridSpacingAuto(false);
    axisX->setAllowZoom(true);

    if (ChartAxis* axisY = area->axisY())
    {
        if (ChartAxisTitle* title = axisY->title())
        {
            title->setText(tr("dB"));
            title->setVisible(true);
        }
        axisY->setAutoRange(false);
        axisY->setFixedRange(MIN_VAL - 1, MAX_VAL + 1);
    }

    QString strCategory;

    m_values.resize(totalPoints);

    qreal dblArg = 0.0;

    for (int i = 0; i < totalPoints; i++)
    {
        m_values[i] = qCos(dblArg) * MAX_VAL;
        dblArg += 0.2;

        int frequency = i * MAX_POINTS_IN_RANGE + MIN_FREQUENCY;

        if (frequency < 1000)
        {
            strCategory = QString::number(frequency);
            if (i == 0)
                strCategory += tr("Hz");
        }
        else
        {
            strCategory = QString::number((qreal)frequency/1000., 'f', 1);
            if (i == totalPoints - 1)
                strCategory += tr("kHz");
        }
        series->addAxisPointY(m_values[i], strCategory);
    }
}

void LargeValueCharts::start()
{
    m_timer.start(m_updateFr);
    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);
}

void LargeValueCharts::stop()
{
    m_timer.stop();
    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}

void LargeValueCharts::updateFrequency(int updateFr)
{
    m_updateFr = updateFr;
    m_timer.setInterval(m_updateFr);
}

void LargeValueCharts::animation()
{
    Chart2DSeries* series2D = (Chart2DSeries*)m_chart->series().at(0);
    Q_ASSERT(series2D != Q_NULL);

    for (int i = 0; i < m_values.size(); i++)
    {
        qreal val = m_values[i];
        qreal delta = qtnRand(-2.0, 2.0);

        val += delta;
        val = qMax(MIN_VAL, qMin(MAX_VAL, val));

        m_values[i] = val;

        ChartDataPoint* point = series2D->at(i);
        Q_ASSERT(point != Q_NULL);
        point->setValueY(val);
    }
    m_chart->updateView();
}


