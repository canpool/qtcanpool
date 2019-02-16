#include <QVariant>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QFormLayout>

#include "piecharts.h"

/* PieCharts */
PieCharts::PieCharts()
    : ChartWindow(tr("Pie Charts"), 0)
{
    createSeriesParametrs();
    seriesChanged(m_seriesSwitcher->currentIndex());
}

PieCharts::~PieCharts()
{
}

void PieCharts::createSeriesParametrs()
{
    // Option Series
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Pie",      QVariant(PieSeries));
    m_seriesSwitcher->addItem("Doughnut", QVariant(DoughnuSeries));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));

    m_explodedCheckBox = new QCheckBox(tr("Exploded"), seriesTypeGroup);
    connect(m_explodedCheckBox, SIGNAL(stateChanged(int)), this, SLOT(explodedSeriesChanged(int)));

    m_rotateSlider = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_rotateSlider->setRange(-180, 180);
    m_rotateSlider->setSliderPosition(0);
    m_rotateSlider->setSingleStep(20);
    connect(m_rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(rotateSeriesChanged(int)));

    m_editorHole = new QSpinBox(seriesTypeGroup);
    m_editorHole->setMinimumWidth(60);
    m_editorHole->setRange(0, 100);
    m_editorHole->setValue(60);
    connect(m_editorHole, SIGNAL(valueChanged(int)), this, SLOT(holeSeriesChanged(int)));

    // Layouts Series
    localLayout->addRow(m_seriesSwitcher);
    localLayout->addRow(m_explodedCheckBox);
    localLayout->addRow(new QLabel(tr("Pie Rotation Angle:"), seriesTypeGroup));
    localLayout->addRow(m_rotateSlider);
    m_captionEditorHole = new QLabel(tr("Hole Radius:"), seriesTypeGroup);
    localLayout->addRow(m_captionEditorHole);
    localLayout->addRow(m_editorHole);


    // Option Lables
    m_dataLablesGroup = createGpoupParameters(tr("Show Data Lables"), true);
    localLayout = (QFormLayout*)m_dataLablesGroup->layout();
    
    connect(m_dataLablesGroup, SIGNAL(toggled(bool)), this, SLOT(showDataLables(bool)));
    m_posDataLablesSwitcher = new QComboBox(m_dataLablesGroup);

    m_posDataLablesSwitcher->addItem(tr("Label Outside"), QVariant(ChartPieSeriesLabel::PieLabelOutside));
    m_posDataLablesSwitcher->addItem(tr("Label Inside"),  QVariant(ChartPieSeriesLabel::PieLabelInside));
    m_posDataLablesSwitcher->addItem(tr("Label Radial"),  QVariant(ChartPieSeriesLabel::PieLabelRadial));
    m_posDataLablesSwitcher->setCurrentIndex(0);
    connect(m_posDataLablesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(lablesPositionChanged(int)));
    // Layouts Series
    localLayout->addRow(m_posDataLablesSwitcher);
}

static void createPiePoint(ChartPieSeries* series, const QString& legendText, qreal value, bool special = false)
{
    QString str = legendText;
    QString strValue;
    strValue = strValue.sprintf(", %.01f", value);
    strValue += "%";
    str += strValue;

    ChartDataPoint* point = series->add(value, special);
    point->setLabel(str);
    point->setLegendText(legendText);
}


void PieCharts::createPieSeries()
{
    createTitle(tr("Travel"));

    m_chart->legend()->setVisible(true);    
    m_chart->legend()->setVerticalAlignment(ChartLegend::LegendFar);

    ChartPieSeries* series = new ChartPieSeries();
    m_chart->appendSeries(series);

    ::createPiePoint(series, QObject::tr("Other"), 31, true);
    ::createPiePoint(series, QObject::tr("USA"), 39, true);
    ::createPiePoint(series, QObject::tr("Taiwan"), 3);
    ::createPiePoint(series, QObject::tr("France"), 3);
    ::createPiePoint(series, QObject::tr("China"), 3);
    ::createPiePoint(series, QObject::tr("Australia"), 3);
    ::createPiePoint(series, QObject::tr("Germany"), 4);
    ::createPiePoint(series, QObject::tr("Italy"), 5);
    ::createPiePoint(series, QObject::tr("Russia"), 7);
}

void PieCharts::createDoughnuSeries()
{
    createPieSeries();
    setValueHole(60);
}

void PieCharts::setValueHole(int val)
{
    ChartPieSeries* series2D = (ChartPieSeries*)m_chart->series().at(0);
    Q_ASSERT(series2D != Q_NULL);
    series2D->setHolePercent(val);
}

void PieCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    lablesPositionChanged(m_posDataLablesSwitcher->currentIndex());
    explodedSeriesChanged(m_explodedCheckBox->checkState());
    rotateSeriesChanged(m_rotateSlider->value());
    if (m_editorHole->isEnabled())
        holeSeriesChanged(m_editorHole->value());
}

void PieCharts::seriesChanged(int index)
{
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    switch((SeriesType)var.toUInt())
    {
        case PieSeries :
            {
                m_chart->setBackgroundBrush(QBrush(QPixmap(":/res/plane.png")));
                createPieSeries();
                m_editorHole->setEnabled(false);
                m_captionEditorHole->setEnabled(false);
            }
            break;
        case DoughnuSeries :
            {
                m_chart->setBackgroundBrush(QBrush(QPixmap(":/res/plane.png")));
                createDoughnuSeries();
                m_editorHole->setEnabled(true);
                m_captionEditorHole->setEnabled(true);
            }
            break;
        default:
            break;
    }
    updateValueParameters();
}

void PieCharts::lablesPositionChanged(int index)
{
    QVariant var = m_posDataLablesSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPieSeries* series = (ChartPieSeries*)listSeries.at(i);
        ChartPieSeriesLabel* lable = (ChartPieSeriesLabel*)series->label();
        lable->setPosition((ChartPieSeriesLabel::PieLabelPosition)(var.toUInt()));
    }
}

void PieCharts::explodedSeriesChanged(int state)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPieSeries* series = (ChartPieSeries*)listSeries.at(i);
        series->setExplodeRelativeSizePercent(state == Qt::Checked ? 10 : 0);
    }
}

void PieCharts::rotateSeriesChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPieSeries* series = (ChartPieSeries*)listSeries.at(i);
        series->setRotation(value);
    }
}

void PieCharts::holeSeriesChanged(int val)
{
    ChartPieSeries* series2D = (ChartPieSeries*)m_chart->series().at(0);
    Q_ASSERT(series2D != Q_NULL);
    series2D->setHolePercent(val);
}
