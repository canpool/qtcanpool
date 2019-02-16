#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QDateTime>
#include <QFormLayout>

#include "areacharts.h"

/* AreaCharts */
AreaCharts::AreaCharts()
    : ChartWindow(tr("Area Charts"))
{
    createSeriesParametrs();
    createMarkersGroup();
    createLablesGroup();
    seriesChanged(m_seriesSwitcher->currentIndex());
}

AreaCharts::~AreaCharts()
{
}

static ChartAreaSeries* createImplChart(const QString& format, AreaCharts::SeriesType type)
{
    ChartAreaSeries* impl = NULL;
    if (type == AreaCharts::Stacked100AreaSplineSeries)
        impl = new ChartStackedSplineAreaSeries();
    else if (type == AreaCharts::StackedAreaSplineSeries)
        impl = new ChartStackedSplineAreaSeries();
    else if (type == AreaCharts::AreaSplineSeries)
        impl = new ChartSplineAreaSeries();
    else if (type == AreaCharts::Stacked100AreaLineSeries)
        impl = new ChartStackedAreaSeries();
    else if (type == AreaCharts::StackedAreaLineSeries)
        impl = new ChartStackedAreaSeries();
    else if (type == AreaCharts::AreaLineSeries)
        impl = new ChartAreaSeries();
    else 
        impl = new ChartAreaSeries();

    if (impl)
        impl->label()->setFormat(format);

    impl->setThickness(3);

    return impl;
}

void AreaCharts::createSeriesParametrs()
{
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    // Option Series
    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Area", QVariant(AreaLineSeries));
    m_seriesSwitcher->addItem("Stacked Area", QVariant(StackedAreaLineSeries));
    m_seriesSwitcher->addItem("100% Stacked Area", QVariant(Stacked100AreaLineSeries));
    m_seriesSwitcher->addItem("Area (Spline)", QVariant(AreaSplineSeries));
    m_seriesSwitcher->addItem("Stacked Area (Spline)", QVariant(StackedAreaSplineSeries));
    m_seriesSwitcher->addItem("100% Stacked Area (Spline)", QVariant(Stacked100AreaSplineSeries));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));

    m_transparency = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_transparency->setRange(0, 255);
    m_transparency->setSliderPosition(190);
    m_transparency->setSingleStep(5);
    connect(m_transparency, SIGNAL(valueChanged(int)), this, SLOT(transparencyChanged(int)));

    localLayout->addRow(m_seriesSwitcher);
    localLayout->addRow(new QLabel(QObject::tr("Transparency:"), seriesTypeGroup));
    localLayout->addRow(m_transparency);
}

void AreaCharts::createAreaSeries(SeriesType type)
{
    createTitle(tr("Average Temperatures by Month/Year:"));

    m_chart->legend()->setVisible(true);

    ChartAreaSeries* series1 = createImplChart("%.00f", type);
    m_chart->appendSeries(series1);
    series1->setName(QObject::tr("2010"));

    ChartAreaSeries* series2 = createImplChart("%.00f", type);
    m_chart->appendSeries(series2);
    series2->setName(QObject::tr("2011"));

    ChartAreaSeries* series3 = createImplChart("%.00f", type);
    m_chart->appendSeries(series3);
    series3->setName(QObject::tr("2012"));

    qreal arTemp[] = { 5., 12., 19, 22, 19, 24, 22, 24, 18, 14, 10, 7 };
    for (int month = 1; month <= 12; month++)
    {
        QString strMonth = QDate::shortMonthName(month);
        series1->addAxisPointY(arTemp[month - 1], strMonth);
        series2->addAxisPointY(arTemp[month - 1] + qtnRand(-4, 4), strMonth);
        series3->addAxisPointY(arTemp[month - 1] + qtnRand(-4, 4), strMonth);
    }
    ChartArea2D* area = dynamic_cast<ChartArea2D*>(m_chart->areas().at(0));
    Q_ASSERT(area);
    area->axisX()->setSideMargins(false);
}

void AreaCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    transparencyChanged(m_transparency->value());
}

void AreaCharts::seriesChanged(int index)
{
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    createAreaSeries((SeriesType)var.toUInt());
    updateValueParameters();
}

void AreaCharts::transparencyChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartAreaSeries* series = (ChartAreaSeries*)listSeries.at(i);
        series->setTransparency(255 - value);
    }
}
