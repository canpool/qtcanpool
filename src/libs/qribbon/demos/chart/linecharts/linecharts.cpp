#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>

#include "linecharts.h"

/* LineCharts */
LineCharts::LineCharts()
    : ChartWindow(tr("Line Charts"))
{
    m_arYears[0] = "2008";
    m_arYears[1] = "2009";
    m_arYears[2] = "2010";
    m_arYears[3] = "2011";
    m_arYears[4] = "2012";

    createSeriesParametrs();
    createMarkersGroup();
    createLablesGroup();

    seriesChanged(m_seriesSwitcher->currentIndex());
}

LineCharts::~LineCharts()
{
}

void LineCharts::createSeriesParametrs()
{
    // Option Series
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Line", QVariant(LineSeries));
    m_seriesSwitcher->addItem("Spline", QVariant(SplineSeries));
    m_seriesSwitcher->addItem("StepLine", QVariant(StepLineSeries));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));
    // Layouts Series
    localLayout->addRow(m_seriesSwitcher);
}

void LineCharts::createLineSeries()
{
    createTitle(tr("PC Sales"));

    m_chart->legend()->setVisible(true);    

    ChartLineSeries* series1 = new ChartLineSeries();
    m_chart->appendSeries(series1);
    series1->setThickness(4);
    series1->setDashStyle(Qtitan::DashStyleDash);
    series1->setName(QObject::tr("HP"));

    series1->addAxisPointY(1500, m_arYears[0]);
    series1->addAxisPointY(2300, m_arYears[1]);
    series1->addAxisPointY(2500, m_arYears[2]);
    series1->addAxisPointY(2800, m_arYears[3]);
    series1->addAxisPointY(3000, m_arYears[4]);

    ChartLineSeries* series2 = new ChartLineSeries();
    m_chart->appendSeries(series2);
    series2->setThickness(4);
    series2->setDashStyle(Qtitan::DashStyleDashDot);
    series2->setName(QObject::tr("Samsung"));

    series2->addAxisPointY(1200, m_arYears[0]);
    series2->addAxisPointY(1400, m_arYears[1]);
    series2->addAxisPointY(1500, m_arYears[2]);
    series2->addAxisPointY(1700, m_arYears[3]);
    series2->addAxisPointY(1800, m_arYears[4]);

    ChartLineSeries* series3 = new ChartLineSeries();
    m_chart->appendSeries(series3);
    series3->setThickness(4);
    series3->setName(QObject::tr("Acer"));

    series3->addAxisPointY(1300, m_arYears[0]);
    series3->addAxisPointY(1500, m_arYears[1]);
    series3->addAxisPointY(2000, m_arYears[2]);
    series3->addAxisPointY(2600, m_arYears[3]);
    series3->addAxisPointY(3200, m_arYears[4]);
    ///////
//    ChartArea2D* area = (ChartArea2D*)m_chart->chartAreas().at(0);
//    ChartAxis* axisX = area->axisX();
//    axisX->setColor(QColor(255, 0, 0));
}

void LineCharts::createSplineSeries()
{
    createTitle(tr("Population"));
    m_chart->legend()->setVisible(true);    

    ChartSplineSeries* series1 = new ChartSplineSeries();
    m_chart->appendSeries(series1);
    series1->setThickness(4);
    series1->setName(tr("Moscow"));

    series1->addXY(1900, 8268);
    series1->addXY(1950, 24830);
    series1->addXY(1990, 27990);
    series1->addXY(2000, 28976);
    series1->addXY(2008, 29490);

    ChartSplineSeries* series2 = new ChartSplineSeries();
    m_chart->appendSeries(series2);
    series2->setThickness(4);
    series2->setName(tr("Voronezh"));

    series2->addXY(1900, 7485);
    series2->addXY(1950, 20586);
    series2->addXY(1990, 23760);
    series2->addXY(2000, 23871);
    series2->addXY(2008, 26756);

    ChartSplineSeries* series3 = new ChartSplineSeries();
    m_chart->appendSeries(series3);
    series3->setThickness(4);
    series3->setName(tr("Ryazan"));

    series3->addXY(1900, 1028);
    series3->addXY(1950, 1871);
    series3->addXY(1990, 12100);
    series3->addXY(2000, 14982);
    series3->addXY(2008, 15328);


    ChartSplineSeries* series4 = new ChartSplineSeries();
    m_chart->appendSeries(series4);
    series4->setThickness(4);
    series4->setName(tr("Cherepovets"));

    series4->addXY(1900, 2048);
    series4->addXY(1950, 5711);
    series4->addXY(1990, 12986);
    series4->addXY(2000, 10851);
    series4->addXY(2008, 20326);

    const SeriesList& series2D = m_chart->series();
    // Set the X and Y Axis title for the series.
    ChartArea2D* area = dynamic_cast<ChartArea2D*>(((ChartSplineSeries*)series2D.at(0))->area());
    Q_ASSERT(area);

    if (ChartAxis* axisX = area->axisX())
    {
        if (ChartAxisTitle* title = axisX->title())
        {
            title->setText(tr("Decade / Year"));
            title->setVisible(true);
        }
    }

    if (ChartAxis* axisY = area->axisY())
    {
        if (ChartAxisTitle* title = axisY->title())
        {
            title->setText(tr("Population Growth (Thousands)"));
            title->setVisible(true);
        }
    }
}

void LineCharts::createStepLineSeries()
{
    createTitle(tr("Transport expenses"));
    ChartTitle* title = m_chart->titles().at(0);
    title->setTextColor(QColor(255,255,255));

    m_chart->legend()->setVisible(true);    
    m_chart->setBackgroundBrush(QBrush(QPixmap(":/res/cars.png")));

    ChartStepLineSeries* series = new ChartStepLineSeries();
    m_chart->appendSeries(series);
    series->setThickness(4);

    ChartArea2D* chartArea = (ChartArea2D*)((ChartStepLineSeries*)series)->area();
    chartArea->setBackgroundColor(QColor(0,0,0,150));
    chartArea->axisX()->label()->setTextColor(QColor(255,255,255));
    chartArea->axisY()->label()->setTextColor(QColor(255,255,255));

    series->setName(QObject::tr("Series 1"));

    QColor colorPoint = QColor(Qt::blue).light();
    series->setColor(colorPoint);
    series->addAxisPointY(50.0,  m_arYears[0])->setColor(colorPoint);
    series->addAxisPointY(80.0,  m_arYears[1])->setColor(colorPoint);
    series->addAxisPointY(40.0,  m_arYears[2])->setColor(colorPoint);
    series->addAxisPointY(100.0, m_arYears[3])->setColor(colorPoint);
    series->addAxisPointY(200.0, m_arYears[4])->setColor(colorPoint);
}

void LineCharts::seriesChanged(int index)
{
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    switch((SeriesType)var.toUInt())
    {
        case LineSeries :
                createLineSeries();
            break;
        case SplineSeries :
                createSplineSeries();
            break;
        case StepLineSeries :
                createStepLineSeries();
            break;
        default:
            break;
    }
    updateValueParameters();
}
