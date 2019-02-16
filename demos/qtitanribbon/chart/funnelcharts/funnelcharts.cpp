#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QDoubleSpinBox>

#include "funnelcharts.h"

/* FunnelCharts */
FunnelCharts::FunnelCharts()
    : ChartWindow(tr("Scatter Charts"), 0)
{
    createSeriesParametrs();
    createFunnelSeries();
    seriesChanged(m_seriesSwitcher->currentIndex());
    updateValueParameters();
}

FunnelCharts::~FunnelCharts()
{
}

void FunnelCharts::createSeriesParametrs()
{
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Funnel", QVariant(FunnelSeries));
    m_seriesSwitcher->addItem("Funnel3D", QVariant(Funnel3DSeries));
    m_seriesSwitcher->setCurrentIndex(Funnel3DSeries);
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));
    localLayout->addRow(m_seriesSwitcher);

    m_transparency = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_transparency->setRange(0, 255);
    m_transparency->setSliderPosition(255);
    m_transparency->setSingleStep(5);
    connect(m_transparency, SIGNAL(valueChanged(int)), this, SLOT(transparencyChanged(int)));
    localLayout->addRow(new QLabel(QObject::tr("Transparency:"), seriesTypeGroup));
    localLayout->addRow(m_transparency);

    m_pointGap = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_pointGap->setRange(0, 20);
    m_pointGap->setSliderPosition(4);
    m_pointGap->setSingleStep(1);
    connect(m_pointGap, SIGNAL(valueChanged(int)), this, SLOT(pointGapChanged(int)));
    localLayout->addRow(new QLabel(QObject::tr("Point Gap:"), seriesTypeGroup));
    localLayout->addRow(m_pointGap);

    m_neckHeightInPercents = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_neckHeightInPercents->setRange(0, 100);
    m_neckHeightInPercents->setSliderPosition(25);
    m_neckHeightInPercents->setSingleStep(1);
    connect(m_neckHeightInPercents, SIGNAL(valueChanged(int)), this, SLOT(neckHeightInPercentsChanged(int)));
    localLayout->addRow(new QLabel(QObject::tr("Neck Height %:"), seriesTypeGroup));
    localLayout->addRow(m_neckHeightInPercents);

    // Option Lables
    m_dataLablesGroup = createGpoupParameters(tr("Show Data Lables"), true);
    localLayout = (QFormLayout*)m_dataLablesGroup->layout();

    connect(m_dataLablesGroup, SIGNAL(toggled(bool)), this, SLOT(showDataLables(bool)));
    m_angleDataLablesSwitcher = new QComboBox(m_dataLablesGroup);
    m_angleDataLablesSwitcher->addItem("Left",    QVariant(LabelLeft));
    m_angleDataLablesSwitcher->addItem("Right",   QVariant(LabelRight));
    m_angleDataLablesSwitcher->addItem("Center",  QVariant(LabelCenter));
    m_angleDataLablesSwitcher->setCurrentIndex(1);
    connect(m_angleDataLablesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(lablesPositionChanged(int)));
    // Layouts Lables
    localLayout->addRow(tr("Position:"), m_angleDataLablesSwitcher);

}

void FunnelCharts::createFunnelSeries()
{
    createTitle(tr("Web Site Visitor Trend"));
    m_chart->legend()->setVisible(true);    

    ChartFunnelSeries* series = new ChartFunnelSeries();

    ChartDataPoint* pnt = series->add(9152, tr("Visited a Web Site: 9152"));
    pnt->setLegendText(tr("Visited a Web Site: 100%"));

    pnt = series->add(6870, tr("Downloaded a Trial: 6870"));
    pnt->setLegendText(tr("Downloaded a Trial: 75%"));

    pnt = series->add(5121, tr("Contacted to Support: 5121"));
    pnt->setLegendText(tr("Contacted to Support: 56%"));

    pnt = series->add(2224,  tr("Subscribed: 2224"));
    pnt->setLegendText(tr("Subscribed: 24%"));

    pnt = series->add(1670,  tr("Renewed: 1670"));
    pnt->setLegendText(tr("Renewed: 18%"));

    series->setNeckHeightInPercents(25.0);

    m_chart->appendSeries(series);
}

void FunnelCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    transparencyChanged(m_transparency->value());
    pointGapChanged(m_pointGap->value());
    lablesPositionChanged(m_angleDataLablesSwitcher->currentIndex());
}

void FunnelCharts::seriesChanged(int index)
{
    bool is3D = Funnel3DSeries == index;
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setSeries3D(is3D);
    }
}

void FunnelCharts::transparencyChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartFunnelSeries* series = qobject_cast<ChartFunnelSeries*>(listSeries.at(i)))
            series->setTransparency(value);
    }
}

void FunnelCharts::pointGapChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartFunnelSeries* series = qobject_cast<ChartFunnelSeries*>(listSeries.at(i)))
            series->setPointGap(value);
    }
}

void FunnelCharts::neckHeightInPercentsChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartFunnelSeries* series = qobject_cast<ChartFunnelSeries*>(listSeries.at(i)))
            series->setNeckHeightInPercents(value);
    }
}

void FunnelCharts::lablesPositionChanged(int index)
{
    QVariant var = m_angleDataLablesSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();
    for (int i = 0, count = listSeries.count(); i < count; i++)
    {
        ChartFunnelSeries* series = (ChartFunnelSeries*)listSeries.at(i);
        if (ChartPyramidSeriesLabel* label = (ChartPyramidSeriesLabel*)series->label())
            label->setPosition((LabelPosition)var.toInt());
    }
}

