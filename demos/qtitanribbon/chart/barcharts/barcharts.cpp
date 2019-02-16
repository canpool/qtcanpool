#include <QHBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QFormLayout>

#include "barcharts.h"


/* BarCharts */
BarCharts::BarCharts()
    : ChartWindow(tr("Bar Charts"), 0)
{
    createSeriesParametrs();
/*
    QCheckBox* columnSwitcher = new QCheckBox("Column", seriesTypeGroup);
    columnSwitcher->setChecked(true);
    connect(columnSwitcher, SIGNAL(stateChanged(int)), this, SLOT(typeSeriesChanged(int)));
*/
    seriesChanged(m_seriesSwitcher->currentIndex());
}

BarCharts::~BarCharts()
{
}

void BarCharts::createSeriesParametrs()
{
    // Option Series
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Clustered",            QVariant(ClusteredSeries));
    m_seriesSwitcher->addItem("Stacked",              QVariant(StackedSeries));
    m_seriesSwitcher->addItem("100% Stacked",         QVariant(Stacked100Series));
    m_seriesSwitcher->addItem("Side by Side Stacked", QVariant(SideBySideSeries));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));
    localLayout->addRow(m_seriesSwitcher);

    // Option Lables
    m_dataLablesGroup = createGpoupParameters(tr("Show Data Lables"), true);
    localLayout = (QFormLayout*)m_dataLablesGroup->layout();

    connect(m_dataLablesGroup, SIGNAL(toggled(bool)), this, SLOT(showDataLables(bool)));
    m_posDataLablesSwitcher = new QComboBox(m_dataLablesGroup);
    m_posDataLablesSwitcher->addItem(tr("Top"),    QVariant(ChartBarSeriesLabel::ChartBarLabelTop));
    m_posDataLablesSwitcher->addItem(tr("Center"), QVariant(ChartBarSeriesLabel::ChartBarLabelCenter));
    m_posDataLablesSwitcher->setCurrentIndex(1);
    connect(m_posDataLablesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(lablesPositionChanged(int)));
    localLayout->addRow(m_posDataLablesSwitcher);
}

ChartBarSeries* BarCharts::createSeries(SeriesType type)
{
    ChartBarSeries* series = (type == StackedSeries || 
        type == Stacked100Series) ? new ChartStackedBarSeries : new ChartBarSeries;

    if (type == Stacked100Series)
        ((ChartStackedBarSeries*)series)->setStackHeight(100.0);

    return series;
}

void BarCharts::createBardSeries(SeriesType type)
{
    createTitle(tr("Results"));

    m_chart->legend()->setVisible(true);
    m_chart->legend()->setVerticalAlignment(ChartLegend::LegendCenter);

    ChartBarSeries* series1 = createSeries(type);
    m_chart->appendSeries(series1);

    series1->setName(tr("Atom 330"));
    series1->addAxisPointY(490, "Double-Precision 2");
    series1->addAxisPointY(50,  "Double-Precision 3");
    series1->addAxisPointY(100, "Double-Precision 4");
    series1->addAxisPointY(400, "Double-Precision 5");
    series1->addAxisPointY(60,  "Double-Precision 6");
    series1->addAxisPointY(530, "Double-Precision 7");
    series1->addAxisPointY(230, "Double-Precision 8");
    series1->addAxisPointY(510, "Double-Precision 9");
    series1->addAxisPointY(505, "Double-Precision 10");
    series1->addAxisPointY(490, "Double-Precision 11");

    ChartAxis* axisX = m_chart->axis(QTN_CHART_X_PRIMARY_AXIS);
    axisX->label()->setAngle(45);

    ChartBarSeries* series2 = createSeries(type);

    m_chart->appendSeries(series2);

    series2->setName(tr("Athlon 64 x2"));
    series2->addAxisPointY(1000, "Double-Precision 2" );
    series2->addAxisPointY(500,  "Double-Precision 3" );
    series2->addAxisPointY(100,  "Double-Precision 4" );
    series2->addAxisPointY(900,  "Double-Precision 5" );
    series2->addAxisPointY(100,  "Double-Precision 6" );
    series2->addAxisPointY(730,  "Double-Precision 7" );
    series2->addAxisPointY(30,   "Double-Precision 8" );
    series2->addAxisPointY(100,  "Double-Precision 9" );
    series2->addAxisPointY(1600, "Double-Precision 10");
    series2->addAxisPointY(890 , "Double-Precision 11");

    ChartBarSeries* series3 = createSeries(type);

    m_chart->appendSeries(series3);

    series3->setName(tr("2x Xeon 5500"));
    series3->addAxisPointY(1900, "Double-Precision 2" );
    series3->addAxisPointY(600,  "Double-Precision 3" );
    series3->addAxisPointY(500,  "Double-Precision 4" );
    series3->addAxisPointY(2500, "Double-Precision 5" );
    series3->addAxisPointY(1500, "Double-Precision 6" );
    series3->addAxisPointY(1500, "Double-Precision 7" );
    series3->addAxisPointY(3000, "Double-Precision 8" );
    series3->addAxisPointY(2400, "Double-Precision 9" );
    series3->addAxisPointY(1600, "Double-Precision 10");
    series3->addAxisPointY(890,  "Double-Precision 11");
}

void BarCharts::createSideBySideSeries()
{
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setVerticalAlignment(ChartLegend::LegendNear);

    for (int i = 0; i < 4; i++)
    {
        ChartStackedBarSeries* series = new ChartStackedBarSeries();
        m_chart->appendSeries(series);
        series->setName(QString::fromLatin1("Qtn %1 Project 1").arg(i + 1));
        series->setStackGroup(i);

        series->addAxisPointY(8 + rand() % 20,  tr("USA"));
        series->addAxisPointY(12 + rand() % 20, tr("Europe"));
        series->addAxisPointY(7 + rand() % 20,  tr("Asia"));

        series = new ChartStackedBarSeries();
        m_chart->appendSeries(series);
        series->setName(QString::fromLatin1("Qtn %1 Project 2").arg(i + 1));
        series->setStackGroup(i);

        series->addAxisPointY(10 + rand() % 20, tr("USA"));
        series->addAxisPointY(8 + rand() % 20,  tr("Europe"));
        series->addAxisPointY(12 + rand() % 20, tr("Asia"));
    }
}

void BarCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    lablesPositionChanged(m_posDataLablesSwitcher->currentIndex());
}

void BarCharts::seriesChanged(int index)
{
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    switch((SeriesType)var.toUInt())
    {
        case ClusteredSeries :
        case StackedSeries :
        case Stacked100Series :
                createBardSeries((SeriesType)var.toUInt());
            break;
        case SideBySideSeries :
                createSideBySideSeries();
            break;
        default:
            break;
    }
    updateValueParameters();
}

void BarCharts::lablesPositionChanged(int index)
{
    QVariant var = m_posDataLablesSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();
    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartBarSeries* series= (ChartBarSeries*)listSeries.at(i);
        ChartBarSeriesLabel* lable = (ChartBarSeriesLabel*)series->label();
        lable->setPosition((ChartBarSeriesLabel::BarLabelPosition)(var.toUInt()));
    }
}

void BarCharts::typeSeriesChanged(int state)
{
    Q_UNUSED(state)
}
