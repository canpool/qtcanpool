#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QDoubleSpinBox>

#include "scattercharts.h"

/* ScatterCharts */
ScatterCharts::ScatterCharts()
    : ChartWindow(tr("Scatter Charts"), 0)
{
    createSeriesParametrs();
    createMarkersGroup();
    createLablesGroup();

    seriesChanged(m_seriesSwitcher->currentIndex());
}

ScatterCharts::~ScatterCharts()
{
}

void ScatterCharts::createSeriesParametrs()
{
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Points", QVariant(PointsSeries));
    m_seriesSwitcher->addItem("Bubble", QVariant(BubbleSeries));
//    m_seriesSwitcher->addItem("Bubble 3D Effect", QVariant(Bubble3DSeries));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));

    m_transparency = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_transparency->setRange(0, 255);
    m_transparency->setSliderPosition(255-175);
    m_transparency->setSingleStep(5);
    connect(m_transparency, SIGNAL(valueChanged(int)), this, SLOT(transparencyChanged(int)));

    m_minLabelBox = new QLabel(tr("MinSize"), seriesTypeGroup);
    m_minBox = new QDoubleSpinBox(seriesTypeGroup);
    m_minBox->setValue(0.7);
    m_minBox->setSingleStep(0.1);
    m_minBox->setRange(0.1, 1.5);
    connect(m_minBox, SIGNAL(valueChanged(double)), this, SLOT(valueMinChanged(double)));

    m_maxLabelBox = new QLabel(tr("MaxSize"), seriesTypeGroup);
    m_maxBox = new QDoubleSpinBox(seriesTypeGroup);
    m_maxBox->setValue(1.9);
    m_maxBox->setSingleStep(0.1);
    m_maxBox->setRange(0.1, 3.5);
    connect(m_maxBox, SIGNAL(valueChanged(double)), this, SLOT(valueMaxChanged(double)));

    localLayout->addRow(m_seriesSwitcher);
    localLayout->addRow(new QLabel(QObject::tr("Transparency:"), seriesTypeGroup));
    localLayout->addRow(m_transparency);
    localLayout->addRow(m_minLabelBox, m_minBox);
    localLayout->addRow(m_maxLabelBox, m_maxBox);
}

void ScatterCharts::createPointsSeries()
{
    createTitle(tr("Points (Scatter)"));
    m_chart->legend()->setVisible(true);    

    ChartPointSeries* series1 = new ChartPointSeries();
    series1->setName(tr("Points 1"));

    for (int i = 0; i < 20; i++)
        series1->addXY(qrand() % 40, 50 + (qrand() % 50));

    m_chart->appendSeries(series1);

    ChartPointSeries* series2 = new ChartPointSeries();
    series2->setName(tr("Points 2"));

    for (int i = 0; i < 20; i++)
        series2->addXY(40 + (qrand() % 40), qrand() % 50);

    m_chart->appendSeries(series2);
}

void ScatterCharts::createBubbleSeries()
{
    createTitle(tr("Site visitors from US:"));
    m_chart->legend()->setVisible(true);    

    ChartBubbleSeries* series = new ChartBubbleSeries();
    ChartDataPoint* p = series->addXYY1(2, 3, 7700);
    p->setLegendText(tr("California"));
    p = series->addXYY1(3, 4, 6500);
    p->setLegendText(tr("Virginia"));
    p = series->addXYY1(4, 5, 4200);
    p->setLegendText(tr("New York"));
    p = series->addXYY1(5, 6, 3500);
    p->setLegendText(tr("Pennsylvania"));
    p = series->addXYY1(6, 5, 2400);
    p->setLegendText(tr("Georgia"));
    p = series->addXYY1(7, 4, 1550);
    p->setLegendText(tr("Colorado"));
    p = series->addXYY1(7.5, 8, 1800);
    p->setLegendText(tr("Other States"));

    series->setMinSize(0.7);
    series->setMaxSize(1.9);
    series->setTransparency(135);

    m_chart->appendSeries(series);

    ChartArea2D* area = (ChartArea2D*)m_chart->areas().at(0);
    area->axisY()->setAutoRange(false);

    area->axisY()->setMaxValue(10);
    area->axisY()->setMinValue(0);

//    area->axisX()->setMaxValue(8);
//    area->axisX()->setMinValue(0);

//    area->axisX()->setShowZeroLevel(false);

}

void ScatterCharts::createBubble3DSeries()
{
}

void ScatterCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    transparencyChanged(m_transparency->value());
    valueMinChanged(m_minBox->value());
    valueMaxChanged(m_maxBox->value());
}

void ScatterCharts::seriesChanged(int index)
{
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    switch((SeriesType)var.toUInt())
    {
        case PointsSeries :
            {
                m_dataLablesGroup->show();
                m_markerShapeSizeTitleSwitcher->show();
                m_markerShapeSizeSwitcher->show();
                m_transparency->hide();

                m_minLabelBox->hide();
                m_minBox->hide();
                m_maxLabelBox->hide();
                m_maxBox->hide();

                createPointsSeries();
            }
            break;
        case BubbleSeries :
            {
                m_markerShapeSizeTitleSwitcher->hide();
                m_markerShapeSizeSwitcher->hide();
                m_dataLablesGroup->hide();
                m_transparency->show();

                m_minLabelBox->show();
                m_minBox->show();
                m_maxLabelBox->show();
                m_maxBox->show();

                createBubbleSeries();
            }
            break;
        case Bubble3DSeries :
            {
                createBubble3DSeries();
            }
            break;
        default:
            break;
    }
    updateValueParameters();
}

void ScatterCharts::transparencyChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartBubbleSeries* series = qobject_cast<ChartBubbleSeries*>(listSeries.at(i)))
            series->setTransparency(255 - value);
    }
}

void ScatterCharts::valueMinChanged(double val)
{
    const SeriesList& listSeries = m_chart->series();
    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartBubbleSeries* series = qobject_cast<ChartBubbleSeries*>(listSeries.at(i)))
            series->setMinSize(val);
    }
}

void ScatterCharts::valueMaxChanged(double val)
{
    const SeriesList& listSeries = m_chart->series();
    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartBubbleSeries* series = qobject_cast<ChartBubbleSeries*>(listSeries.at(i)))
            series->setMaxSize(val);
    }
}
