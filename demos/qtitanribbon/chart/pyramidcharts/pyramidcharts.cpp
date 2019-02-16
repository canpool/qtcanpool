#include <QGroupBox>
#include <QComboBox>
#include <QFormLayout>
#include <QCheckBox>

#include "pyramidcharts.h"


/* PyramidCharts */
PyramidCharts::PyramidCharts()
    : ChartWindow(tr("Scatter Charts"))
{
    createSeriesParametrs();
    createFunnelSeries();

    seriesChanged(m_seriesSwitcher->currentIndex());
    updateValueParameters();
}

PyramidCharts::~PyramidCharts()
{
}

void PyramidCharts::createSeriesParametrs()
{
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();

    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem("Pyramid",   QVariant(PyramidSeries));
    m_seriesSwitcher->addItem("Pyramid3D", QVariant(Pyramid3DSeries));
    m_seriesSwitcher->setCurrentIndex(Pyramid3DSeries);
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));
    localLayout->addRow(m_seriesSwitcher);

    m_circularSwitcher = new QCheckBox(tr("Circular"), seriesTypeGroup);
    connect(m_circularSwitcher, SIGNAL(stateChanged(int)), this, SLOT(circularSwitcher(int)));
    localLayout->addRow(m_circularSwitcher);

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

    m_rotation = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_rotation->setRange(-180, 180);
    m_rotation->setSliderPosition(1);
    m_rotation->setSingleStep(1);
    connect(m_rotation, SIGNAL(valueChanged(int)), this, SLOT(rotationChanged(int)));
    m_labelRotation = new QLabel(QObject::tr("Rotation:"));
    localLayout->addRow(m_labelRotation);
    localLayout->addRow(m_rotation);

    m_depthPercent = new QSlider(Qt::Horizontal, seriesTypeGroup);
    m_depthPercent->setRange(0, 45);
    m_depthPercent->setSliderPosition(10);
    m_depthPercent->setSingleStep(1);
    connect(m_depthPercent, SIGNAL(valueChanged(int)), this, SLOT(depthPercentChanged(int)));
    m_labeldepthPercen = new QLabel(QObject::tr("Depth:"));
    localLayout->addRow(m_labeldepthPercen);
    localLayout->addRow(m_depthPercent);

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

void PyramidCharts::createFunnelSeries()
{
    createTitle(tr("Web Site Visitor Trend"));
    m_chart->legend()->setVisible(true);    

    ChartPyramidSeries* series = new ChartPyramidSeries();

    ChartDataPoint* pnt = series->add(9152, tr("Visited a Web Site: 9152"));
    pnt->setLegendText(tr("Visited a Web Site: 100%"));

    pnt = series->add(6870.0, tr("Downloaded a Trial: 6870"));
    pnt->setLegendText(tr("Downloaded a Trial: 75%"));

    pnt = series->add(5121.0, tr("Contacted to Support: 5121"));
    pnt->setLegendText(tr("Contacted to Support: 56%"));

    pnt = series->add(2224.0,  tr("Subscribed: 2224"));
    pnt->setLegendText(tr("Subscribed: 24%"));

    pnt = series->add(1670.0,  tr("Renewed: 1670"));
    pnt->setLegendText(tr("Renewed: 18%"));

//    series->setEqualHeight(true);
    m_chart->appendSeries(series);
}

void PyramidCharts::updateValueParameters()
{
    ChartWindow::updateValueParameters();
    transparencyChanged(m_transparency->value());
    pointGapChanged(m_pointGap->value());
    lablesPositionChanged(m_angleDataLablesSwitcher->currentIndex());
    depthPercentChanged(m_depthPercent->value());
    rotationChanged(m_rotation->value());
}

void PyramidCharts::seriesChanged(int index)
{
    bool is3D = Pyramid3DSeries == index;
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setSeries3D(is3D);
    }

    m_labeldepthPercen->setEnabled(is3D);
    m_depthPercent->setEnabled(is3D);
    m_labelRotation->setEnabled(is3D);
    m_rotation->setEnabled(is3D);
}

void PyramidCharts::transparencyChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setTransparency(value);
    }
}

void PyramidCharts::pointGapChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setPointGap(value);
    }
}

void PyramidCharts::rotationChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setRotation(value);
    }
}

void PyramidCharts::depthPercentChanged(int value)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
            series->setDepthPercent(value);
    }
}

void PyramidCharts::lablesPositionChanged(int index)
{
    QVariant var = m_angleDataLablesSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();
    for (int i = 0, count = listSeries.count(); i < count; i++)
    {
        ChartPyramidSeries* series = (ChartPyramidSeries*)listSeries.at(i);
        if (ChartPyramidSeriesLabel* label = (ChartPyramidSeriesLabel*)series->label())
            label->setPosition((LabelPosition)var.toInt());
    }
}

void PyramidCharts::circularSwitcher(int check)
{
    bool circular = check == Qt::Checked;
    const SeriesList& listSeries = m_chart->series();
    for (int i = 0, count = listSeries.count(); i < count; i++)
    {
        if (ChartPyramidSeries* series = qobject_cast<ChartPyramidSeries*>(listSeries.at(i)))
        {
            series->setCircular(circular);

            bool enable = !circular && series->isSeries3D();
            if (!enable)
                m_depthPercent->setSliderPosition(10);

            m_labeldepthPercen->setEnabled(enable);
            m_depthPercent->setEnabled(enable);
            m_labelRotation->setEnabled(enable);
            m_rotation->setEnabled(enable);
        }
    }
}
