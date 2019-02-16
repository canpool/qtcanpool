#include <QApplication>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QSettings>
#include <QPainter>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QFontComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>

#include "chartwindow.h"


/* ChartWindow */
ChartWindow::ChartWindow(const QString& series, Qtitan::Chart* chart)
    : DemoMainWindow(tr("QtitanChart"), QTN_VERSION_CHART_STR)
{
    Q_UNUSED(series);
    m_mainGroup = Q_NULL;
    m_dataMarkersGroup = Q_NULL;
    m_dataLablesGroup = Q_NULL;
    m_themeSwitcher = Q_NULL;
    m_markerShapeSwitcher = Q_NULL;
    m_markerShapeSizeSwitcher = Q_NULL;
    m_angleDataLablesSwitcher = Q_NULL;

    setWindowTitle(tr("Qtitan Charts Simple"));

    if (chart)
        chart->setParent(this);
    m_chart = chart ? chart : new Chart(this);

    m_mainGroup = new QWidget(this);
    setDemoWidget(m_chart, m_mainGroup, 0);
    QFormLayout* formLayout = new QFormLayout;
    m_mainGroup->setLayout(formLayout);

    // Option Theme
    createChartGroup();
    readSettings();
    m_themeSwitcher->setCurrentIndex(0);
}

ChartWindow::~ChartWindow()
{
}

void ChartWindow::createTitle(const QString& title)
{
    ChartTitle* chartTitle = new ChartTitle;
    chartTitle->setText(title);
    m_chart->appendTitle(chartTitle);
}

void ChartWindow::createChartGroup()
{
    // Option Theme
    QGroupBox* chartGroup = createGpoupParameters(tr("Chart"));
    QFormLayout* localLayout = (QFormLayout*)chartGroup->layout();
    
    m_themeSwitcher = new QComboBox(chartGroup);
    m_themeSwitcher->addItem("Default",            QVariant(Chart::Default));
    m_themeSwitcher->addItem("Fire",               QVariant(Chart::Fire));
    m_themeSwitcher->addItem("Office",             QVariant(Chart::Office));
    m_themeSwitcher->addItem("Sunny",              QVariant(Chart::Sunny));
    m_themeSwitcher->addItem("Blue",               QVariant(Chart::Blue));
    m_themeSwitcher->addItem("Gray",               QVariant(Chart::Gray));
    m_themeSwitcher->addItem("Green",              QVariant(Chart::Green));
    m_themeSwitcher->addItem("Azure and Office",   QVariant(Chart::AzureOffice));
    m_themeSwitcher->addItem("Blue and Sunny",     QVariant(Chart::BlueSunny));
    m_themeSwitcher->addItem("Green and Green",    QVariant(Chart::GreenGreen));
    m_themeSwitcher->addItem("Gray and Gray",      QVariant(Chart::GrayGray));
    m_themeSwitcher->addItem("Black and Blue",     QVariant(Chart::BlackBlue));
    connect(m_themeSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(themeChanged(int)));

    QPushButton* exportButton = new QPushButton(tr("Export"), chartGroup);
    connect(exportButton, SIGNAL(released()), this, SLOT(chartExport()));

    localLayout->addRow(tr("Theme:"), m_themeSwitcher);
    localLayout->addRow(tr("File:"), exportButton);
}

void ChartWindow::createMarkersGroup()
{
    m_dataMarkersGroup = createGpoupParameters(tr("Show Data Markers"), true);
    QFormLayout* localLayout = (QFormLayout*)m_dataMarkersGroup->layout();

    connect(m_dataMarkersGroup, SIGNAL(toggled(bool)), this, SLOT(showDataMarkers(bool)));

    m_markerShapeSizeSwitcher = new QComboBox(m_dataMarkersGroup);
    m_markerShapeSizeSwitcher->addItem("8",  QVariant(8));
    m_markerShapeSizeSwitcher->addItem("10", QVariant(10));
    m_markerShapeSizeSwitcher->addItem("12", QVariant(12));
    m_markerShapeSizeSwitcher->addItem("14", QVariant(14));
    m_markerShapeSizeSwitcher->addItem("16", QVariant(16));
    m_markerShapeSizeSwitcher->addItem("18", QVariant(18));
    m_markerShapeSizeSwitcher->addItem("20", QVariant(20));
    m_markerShapeSizeSwitcher->setCurrentIndex(0);
    connect(m_markerShapeSizeSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(markerShapeSizeChanged(int)));

    m_markerShapeSwitcher = new QComboBox(m_dataMarkersGroup);
    m_markerShapeSwitcher->addItem("Circle",   QVariant(Qtitan::MarkerCircle));
    m_markerShapeSwitcher->addItem("Square",   QVariant(Qtitan::MarkerSquare));
    m_markerShapeSwitcher->addItem("Diamond",  QVariant(Qtitan::MarkerDiamond));
    m_markerShapeSwitcher->addItem("Triangle", QVariant(Qtitan::MarkerTriangle));
    m_markerShapeSwitcher->addItem("Pentagon", QVariant(Qtitan::MarkerPentagon));
    m_markerShapeSwitcher->addItem("Hexagon",  QVariant(Qtitan::MarkerHexagon));
    m_markerShapeSwitcher->addItem("Star",     QVariant(Qtitan::MarkerStar));
    m_markerShapeSwitcher->setCurrentIndex(0);
    connect(m_markerShapeSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(markerShapeChanged(int)));

    // Layouts Markers
    m_markerShapeSizeTitleSwitcher = new QLabel(tr("Size:"), m_dataMarkersGroup);
    localLayout->addRow(m_markerShapeSizeTitleSwitcher, m_markerShapeSizeSwitcher);
    localLayout->addRow(tr("Type:"), m_markerShapeSwitcher);
}

void ChartWindow::createLablesGroup()
{
    // Option Lables
    m_dataLablesGroup = createGpoupParameters(tr("Show Data Lables"), true);
    QFormLayout* localLayout = (QFormLayout*)m_dataLablesGroup->layout();

    connect(m_dataLablesGroup, SIGNAL(toggled(bool)), this, SLOT(showDataLables(bool)));
    m_angleDataLablesSwitcher = new QComboBox(m_dataLablesGroup);
    m_angleDataLablesSwitcher->addItem("0",   QVariant(0));
    m_angleDataLablesSwitcher->addItem("45",  QVariant(45));
    m_angleDataLablesSwitcher->addItem("90",  QVariant(90));
    m_angleDataLablesSwitcher->addItem("180", QVariant(180));
    m_angleDataLablesSwitcher->addItem("270", QVariant(270));
    m_angleDataLablesSwitcher->setCurrentIndex(1);
    connect(m_angleDataLablesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(lablesAngleChanged(int)));
    // Layouts Lables
    localLayout->addRow(tr("Angle:"), m_angleDataLablesSwitcher);
}

void ChartWindow::updateValueParameters()
{
    if (m_dataMarkersGroup)
    {
        showDataMarkers(m_dataMarkersGroup->isChecked());
        markerShapeSizeChanged(m_markerShapeSizeSwitcher->currentIndex());
        markerShapeChanged(m_markerShapeSwitcher->currentIndex());
    }
    if (m_dataLablesGroup)
    {
        showDataLables(m_dataLablesGroup->isChecked());
        if (m_angleDataLablesSwitcher && m_angleDataLablesSwitcher->isVisible())
            lablesAngleChanged(m_angleDataLablesSwitcher->currentIndex());
    }
}

QGroupBox* ChartWindow::createGpoupParameters(const QString& nameGroup, bool checkable)
{
    if (nameGroup.isEmpty())
        return Q_NULL;

    QGroupBox* group = new QGroupBox();
    group->setMinimumWidth(160);
    QFormLayout* localLayout = new QFormLayout;
    localLayout->setSpacing(3);
    localLayout->setMargin(3);
    group->setLayout(localLayout);
    group->setTitle(nameGroup);
    group->setCheckable(checkable);

    formLayout()->addRow(group);
    return group;
}

QFormLayout* ChartWindow::formLayout() const
{
    if (!m_mainGroup)
        return Q_NULL;
    return qobject_cast<QFormLayout*>(m_mainGroup->layout());
}

void ChartWindow::themeChanged(int index)
{
    QVariant var = m_themeSwitcher->itemData(index);
    m_chart->setTheme((Chart::Theme)var.toUInt());;
}

void ChartWindow::showDataMarkers(bool state)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPointSeries* series = (ChartPointSeries*)listSeries.at(i);
        series->setMarkerVisible(state);
    }
}

void ChartWindow::markerShapeSizeChanged(int index)
{
    QVariant var = m_markerShapeSizeSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();
    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPointSeries* series = (ChartPointSeries*)listSeries.at(i);
        series->setMarkerSize(var.toInt());
    }
}

void ChartWindow::showDataLables(bool state)
{
    const SeriesList& listSeries = m_chart->series();

    for (int i = 0, count = listSeries.count(); i < count; i++)
    {
        ChartPointSeries* series = (ChartPointSeries*)listSeries.at(i);
        if (ChartSeriesLabel* label = series->label())
            label->setVisible(state);
    }
}

void ChartWindow::lablesAngleChanged(int index)
{
    if (m_angleDataLablesSwitcher && m_angleDataLablesSwitcher->isVisible())
    {
        QVariant var = m_angleDataLablesSwitcher->itemData(index);

        const SeriesList& listSeries = m_chart->series();
        for (int i = 0, count = listSeries.count(); i < count; i++)
        {
            ChartPointSeries* series = (ChartPointSeries*)listSeries.at(i);
            if (ChartPointSeriesLabel* label = (ChartPointSeriesLabel*)series->label())
                label->setAngle(var.toInt());
        }
    }
}

void ChartWindow::chartExport()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        tr("Chart"), QLatin1Char('*') + QLatin1String(".png"));
    if (!fileName.isEmpty())
        m_chart->exportToFile(fileName);
}

void ChartWindow::markerShapeChanged(int index)
{
    QVariant var = m_markerShapeSwitcher->itemData(index);

    const SeriesList& listSeries = m_chart->series();
    for (int i = 0; i < listSeries.count(); i++)
    {
        ChartPointSeries* series = (ChartPointSeries*)listSeries.at(i);
        series->setMarkerType((Qtitan::MarkerType)var.toUInt());
    }
}

void ChartWindow::readSettings()
{
    QRect geom = QApplication::desktop()->availableGeometry();
    QSettings settings("Developer Machines", "Qtitan ChartWindow Simple");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(2 * geom.width() / 3, 2 * geom.height() / 3)).toSize();
    move(pos);
    resize(size);
}

void ChartWindow::writeSettings()
{
    QSettings settings("Developer Machines", "Qtitan ChartWindow Simple");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void ChartWindow::closeEvent(QCloseEvent* event)
{
    QWidget::closeEvent(event);
    writeSettings();
}

void ChartWindow::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
}

