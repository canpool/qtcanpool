#include <QApplication>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QMessageBox>
#include <QPainter>
#include <QFormLayout>

#include <math.h>

#include "advancedfeatures.h"

/* AdvancedChart */
class AdvancedChart : public Qtitan::Chart
{
public:
    AdvancedChart(QWidget* parent = 0);

protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void resizeEvent(QResizeEvent* event);
public:
    qreal m_XLine;
    QVector<QPointF> m_arMarkers;
};

AdvancedChart::AdvancedChart(QWidget* parent)
    : Qtitan::Chart(parent)
{
}

static const QSizeF m_szMarker(5.0, 5.0);
void AdvancedChart::paintEvent(QPaintEvent* event)
{
    Qtitan::Chart::paintEvent(event);
    QPainter p(this);
    // Draw markers:
    for (int i = 0; i < m_arMarkers.size(); i++)
    {
        Chart2DSeries* series2D = (Chart2DSeries*)series().at(i);
        QColor color = series2D->color();

        ChartArea2D* area2D = (ChartArea2D*)series2D->area();
        ChartAxis* axisY = area2D->axisY();
        Q_ASSERT(axisY != Q_NULL);

        QRect rectAxisBounds = axisY->boundingRect();
        rectAxisBounds.setWidth(area2D->boundingRect().width());
        QPointF ptMarker = m_arMarkers[i]; 

        if (rectAxisBounds.contains(ptMarker.toPoint()))
        {
            QPainter::RenderHints hints = p.renderHints();
            p.setRenderHints(hints, false);
            p.setRenderHint(QPainter::Antialiasing);

            QPen savePen = p.pen();
            p.setPen(QPen(color.dark()));

            QBrush saveBrush = p.brush();
            color.setAlpha(140);
            p.setBrush(color);

            p.drawEllipse(ptMarker, m_szMarker.width(), m_szMarker.height());
            p.setBrush(saveBrush);
            p.setPen(savePen);

            p.setRenderHints(p.renderHints(), false);
            p.setRenderHints(hints, true);
        }
    }

    // Draw vertical line:
    if (m_XLine != -1.)
    {
        QPen savePen = p.pen();
        p.setPen(QPen(QBrush(QColor(Qt::red)), 1.0, Qt::DashLine));

        QRect rectDiagram = plotAreas();
        p.drawLine(m_XLine, rectDiagram.top(), m_XLine, rectDiagram.bottom());

        p.setPen(savePen);
    }
}

void AdvancedChart::resizeEvent(QResizeEvent* event)
{
    Qtitan::Chart::resizeEvent(event);
    m_arMarkers.clear();
    m_XLine = -1.;
}


#define DEFAULT_INFO    tr("Move the mouse cursor over the chart and see information on hovered chart component")
/* AdvancedFeatures */
AdvancedFeatures::AdvancedFeatures()
    : ChartWindow(tr("Advanced Features"), new AdvancedChart())
{
    createSeriesParametrs();
    seriesChanged(m_seriesSwitcher->currentIndex());

    showSecondaryAxisXChanged(Qt::Checked);
    showSecondaryAxisYChanged(Qt::Checked);

    connect(m_chart, SIGNAL(chartMouseTrack(ChartHitInfo*)), this, SLOT(mouseTrack(ChartHitInfo*)), Qt::DirectConnection);
    connect(m_chart, SIGNAL(chartMouseUp(ChartHitInfo*)), this, SLOT(mouseUp(ChartHitInfo*)), Qt::DirectConnection);
}

AdvancedFeatures::~AdvancedFeatures()
{
}

void AdvancedFeatures::createSeriesParametrs()
{
    // Option Series
    QGroupBox* seriesTypeGroup = createGpoupParameters(tr("Series"));
    QFormLayout* localLayout = (QFormLayout*)seriesTypeGroup->layout();
    m_seriesSwitcher = new QComboBox(seriesTypeGroup);
    m_seriesSwitcher->addItem(tr("Secondary Axis"), QVariant(SecondaryAxis));
    m_seriesSwitcher->addItem(tr("Interactive"), QVariant(Interactive));
    connect(m_seriesSwitcher, SIGNAL(currentIndexChanged(int)), this, SLOT(seriesChanged(int)));

    // SecondaryAxis
    m_showSecondaryAxisX = new QCheckBox(tr("Secondary X Axis"), seriesTypeGroup);
    m_showSecondaryAxisX->setCheckState(Qt::Checked);
    m_showSecondaryAxisX->setVisible(false);
    connect(m_showSecondaryAxisX, SIGNAL(stateChanged(int)), this, SLOT(showSecondaryAxisXChanged(int)));

    m_showSecondaryAxisY = new QCheckBox(tr("Secondary Y Axis"), seriesTypeGroup);
    m_showSecondaryAxisY->setCheckState(Qt::Checked);
    m_showSecondaryAxisY->setVisible(false);
    connect(m_showSecondaryAxisY, SIGNAL(stateChanged(int)), this, SLOT(showSecondaryAxisYChanged(int)));

    m_hitTestResult = new QLabel(DEFAULT_INFO, seriesTypeGroup);
    m_hitTestResult->setWordWrap(true);
    m_hitTestResult->setVisible(false);
    m_hitTestResult->setMinimumWidth(180);
    m_hitTestResult->setMaximumWidth(180);
    m_hitTestResult->setMinimumHeight(100);
    m_hitTestResult->setMaximumHeight(100);

    localLayout->addRow(m_seriesSwitcher);
    localLayout->addRow(m_showSecondaryAxisX);
    localLayout->addRow(m_showSecondaryAxisY);
    localLayout->addRow(m_hitTestResult);
}

void AdvancedFeatures::createSecondaryAxis()
{
    m_chart->legend()->setVisible(true);    
    m_chart->legend()->setVerticalAlignment(ChartLegend::LegendCenter);

    ChartSplineSeries* series1 = new ChartSplineSeries();
    series1->setName(tr("Series1"));
    m_chart->appendSeries(series1);
    series1->setMarkerType(Qtitan::MarkerTriangle);

    series1->addXY(1, 0.0);
    series1->addXY(2, 2.0);
    series1->addXY(3, 5.0);
    series1->addXY(4, 3.0);
    series1->addXY(5, 3.5);
    series1->addXY(6, 5.0);
    series1->addXY(7, 8.0);
    series1->addXY(8, 7.0);

    ChartSplineSeries* series2 = new ChartSplineSeries();
    series2->setName(tr("Series2"));
    m_chart->appendSeries(series2);
    series2->setMarkerType(Qtitan::MarkerStar);

    series2->addXY(1, 2.0);
    series2->addXY(2, 6.0);
    series2->addXY(3, 12.0);
    series2->addXY(4, 16.0);
    series2->addXY(5, 14.0);
    series2->addXY(6, 9.0);
    series2->addXY(7, 6.0);
    series2->addXY(8, 2.0);
}

void AdvancedFeatures::createInteractive()
{
    createTitle(tr("Weather Report"));

    m_chart->setAreasOrientation(Qt::Vertical);
    m_chart->legend()->setVisible(true);    
    m_chart->legend()->setVerticalAlignment(ChartLegend::LegendCenter);

    ChartArea2D* area2 = new ChartArea2D();
    m_chart->appendArea(area2);
    m_axisY1 = area2->axisY();
    m_axisY1->setAxisID(QTN_CHART_FIRST_CUSTOM_ID+1);
    ChartAxisTitle* titleY = m_axisY1->title();
    titleY->setText(tr("Pressure"));
    titleY->setVisible(true);
    area2->axisY()->setAutoRange(false);
    area2->axisY()->setFixedRange(1012, 1025);
    area2->axisX()->setVisible(false);

    ChartArea2D* area1 = new ChartArea2D();
    m_chart->appendArea(area1);
    titleY = area1->axisY()->title();
    titleY->setText(tr("Temperature"));
    titleY->setVisible(true);
    area1->axisY()->setAutoRange(false);
    area1->axisY()->setFixedRange(15, 25);
    area1->axisX()->setVisible(false);

    ChartArea2D* area3 = new ChartArea2D();
    m_chart->appendArea(area3);
    m_axisY2 = area3->axisY();
    m_axisY2->setAxisID(QTN_CHART_FIRST_CUSTOM_ID+2);
    titleY = m_axisY2->title();
    titleY->setText(tr("Humidity,%"));
    titleY->setVisible(true);
    area3->axisX()->title()->setText(tr("Days"));
    area3->axisX()->title()->setVisible(true);

    // Add series and data points:
    ChartLineSeries* series1 = new ChartLineSeries();
    series1->setName(tr("Temperature"));
    m_chart->appendSeries(series1);
    series1->setMarkerVisible(false);
    series1->label()->setVisible(true);
    ((ChartPointSeriesLabel*)series1->label())->setAngle(-270);
    area1->appendSeries(series1);

    ChartAreaSeries* series2 = new ChartAreaSeries();
    series2->setName(tr("Pressure"));
    m_chart->appendSeries(series2);
    series2->setMarkerVisible(false);
    series2->label()->setVisible(false);
    area2->appendSeries(series2);

    ChartBarSeries* series3 = new ChartBarSeries();
    series3->setName(tr("Humidity"));
    m_chart->appendSeries(series3);
    series3->label()->setVisible(false);
    area3->appendSeries(series3);

    for (int nDay = 1; nDay <= 31; nDay++)
    {
        series1->addXY(nDay, (int)(20.0 + ::qtnRand(-5.0, 5.0)));
        series2->addXY(nDay, 1020.0 + ::qtnRand(-4.0, 4.0));
        series3->addXY(nDay, 50.0 + ::qtnRand(-40.0, 40.0));
    }
}

void AdvancedFeatures::updateValueParameters()
{
    ChartWindow::updateValueParameters();

    if (m_showSecondaryAxisX->isVisible() && m_showSecondaryAxisY->isVisible())
    {
        showSecondaryAxisXChanged(m_showSecondaryAxisX->checkState());
        showSecondaryAxisYChanged(m_showSecondaryAxisY->checkState());
    }
}

void AdvancedFeatures::seriesChanged(int index)
{
    ((AdvancedChart*)m_chart)->m_XLine = -1.;
    ((AdvancedChart*)m_chart)->m_arMarkers.clear();
    m_chart->clearSeries();
    m_chart->clearTitles();
    m_chart->setBackgroundBrush(QBrush());

    QVariant var = m_seriesSwitcher->itemData(index);
    switch((SeriesType)var.toUInt())
    {
        case SecondaryAxis :
            {
                m_chart->enableMouseTrackingMode(ChartHitInfo::HitNone);

                m_hitTestResult->setVisible(false);
                m_showSecondaryAxisX->setVisible(true);
                m_showSecondaryAxisY->setVisible(true);
                createSecondaryAxis();
            }
            break;
        case Interactive :
                m_chart->enableMouseTrackingMode(ChartHitInfo::HitChartArea | ChartHitInfo::HitTitle | ChartHitInfo::HitDataPoint |
                    ChartHitInfo::HitLegend | ChartHitInfo::HitAxis | ChartHitInfo::HitAxisName | ChartHitInfo::HitArea);

                m_hitTestResult->setVisible(true);
                m_showSecondaryAxisX->setVisible(false);
                m_showSecondaryAxisY->setVisible(false);
                createInteractive();
            break;
        default:
            break;
    }
    updateValueParameters();
}

void AdvancedFeatures::showSecondaryAxisXChanged(int state)
{
    ChartArea2D* area2D = (ChartArea2D*)m_chart->areas().at(0);

    if (Qt::Checked != state)
        area2D->secondaryAxisX()->setVisible(false);
    else
        showAxisXChanged(Qt::Checked);

    ChartAxisTitle* titleX = area2D->axisX()->title();
    Q_ASSERT(titleX != Q_NULL);

    if (Qt::Checked == state)
    {
        area2D->secondaryAxisX()->title()->setText(tr("Series 2 X"));
        area2D->secondaryAxisX()->title()->setVisible(true);

        titleX->setText(tr("Series 1 X"));
        titleX->setVisible(true);
    }
    else
    {
        titleX->setText(tr("Series 1 and 2 X"));
        titleX->setVisible(true);
    }

    Chart2DSeries* impl = (Chart2DSeries*)m_chart->series().at(1);
    impl->setSecondaryAxisX(Qt::Checked == state);
}

void AdvancedFeatures::showAxisXChanged(int state)
{
    ChartArea2D* area2D = (ChartArea2D*)m_chart->areas().at(0);
    area2D->secondaryAxisX()->setVisible(Qt::Checked == state);
}

void AdvancedFeatures::showSecondaryAxisYChanged(int state)
{
    ChartArea2D* area2D = (ChartArea2D*)m_chart->areas().at(0);

    if (Qt::Checked != state)
        area2D->secondaryAxisY()->setVisible(false);
    else
        showAxisYChanged(Qt::Checked);

    ChartAxisTitle* titleY = area2D->axisY()->title();
    Q_ASSERT(titleY != Q_NULL);

    if (Qt::Checked == state)
    {
        area2D->secondaryAxisY()->title()->setText(tr("Series 2 Y"));
        area2D->secondaryAxisY()->title()->setVisible(true);

        titleY->setText(tr("Series 1 Y"));
        titleY->setVisible(true);
    }
    else
    {
        titleY->setText(tr("Series 1 and 2 Y"));
        titleY->setVisible(true);
    }

    Chart2DSeries* impl = (Chart2DSeries*)m_chart->series().at(1);
    impl->setSecondaryAxisX(Qt::Checked == state);
}

void AdvancedFeatures::showAxisYChanged(int state)
{
    ChartArea2D* area2D = (ChartArea2D*)m_chart->areas().at(0);
    area2D->secondaryAxisY()->setVisible(Qt::Checked == state);
}

void AdvancedFeatures::mouseTrack(ChartHitInfo* hit)
{
    QPoint pt = hit->m_ptHit;

    ChartAxis* axisXX = m_chart->axis(QTN_CHART_X_PRIMARY_AXIS);
    Q_ASSERT(axisXX != Q_NULL);

    // Get X Position on the chart
    qreal realXMarker = axisXX->valueFromPoint(pt);

    // left marker index
    int xValue = (int)floor(realXMarker);

    bool redraw = false;
    const int seriesCount = m_chart->series().count();
    ((AdvancedChart*)m_chart)->m_arMarkers.resize(seriesCount);

    for (int i = 0; i < seriesCount; i++)
    {
        Chart2DSeries* series2D = (Chart2DSeries*)m_chart->series().at(i);
        Q_ASSERT(series2D != Q_NULL);

        const ChartDataPoint* dp1 = series2D->at(xValue);
        const ChartDataPoint* dp2 = series2D->at(xValue + 1);

        if (dp1 != Q_NULL && dp2 != Q_NULL)
        {
            ChartArea2D* area2D = (ChartArea2D*)series2D->area();
            Q_ASSERT(area2D != Q_NULL);
            ChartAxis* axisX = area2D->axisX();
            Q_ASSERT(axisX != Q_NULL);
            ChartAxis* axisY = area2D->axisY();
            Q_ASSERT(axisY != Q_NULL);

            qreal x1 = axisX->valueToPoint(dp1->valueX());
            qreal x2 = axisX->valueToPoint(dp2->valueX());

            qreal y1 = axisY->valueToPoint(dp1->valueY());
            qreal y2 = axisY->valueToPoint(dp2->valueY());

            QPoint pt1(x1, y1);
            QPoint pt2(x2, y2);

            QPoint ptMarker;
            if (::fabs((qreal)pt.x() - pt1.x()) < ::fabs((qreal)pt.x() - pt2.x()))
                ptMarker = pt1;
            else
                ptMarker = pt2;

            if (((AdvancedChart*)m_chart)->m_arMarkers.size() < seriesCount || ((AdvancedChart*)m_chart)->m_arMarkers[i] != ptMarker)
            {
                ((AdvancedChart*)m_chart)->m_arMarkers[i] = ptMarker;
                if (i == 0)
                    ((AdvancedChart*)m_chart)->m_XLine = ptMarker.x();
                redraw = true;
            }
        }
    }

    if (redraw)
        m_chart->repaint();

    QCursor cursor =  Qt::ArrowCursor;
    QString strInfo;
    switch (hit->m_hitInfo)
    {
    case ChartHitInfo::HitDataPoint:
        {
            strInfo = tr("Hovered Element: Data Point\nClick left mouse button to get additional information");
            cursor = Qt::PointingHandCursor;
        }
        break;
    case ChartHitInfo::HitChartArea:
        strInfo = tr("Hovered Element: Chart Area");
        break;
    case ChartHitInfo::HitAxis:
    case ChartHitInfo::HitAxisName:
        {
            ChartArea2D* area2D = (ChartArea2D*)m_chart->areas().at(0);
            ChartAxis* axisX = area2D->axisX();
            Q_ASSERT(axisX != Q_NULL);

            ChartAxis* axisY = area2D->axisY();
            Q_ASSERT(axisY != Q_NULL);

            QString strName = tr("NoName");
            qreal value = 0.0;

            ChartAxis* axis = Q_NULL;

            if (axisX->axisID() == hit->m_index1)
                axis = axisX;
            else if (axisY->axisID() == hit->m_index1)
                axis = axisY;
            else if (m_axisY1->axisID() == hit->m_index1)
                axis = m_axisY1;
            else if (m_axisY2->axisID() == hit->m_index1)
                axis = m_axisY2;

            if (axis != Q_NULL)
            {
                strName = axis->title()->text();
                value = axis->valueFromPoint(pt);
            }

            if (hit->m_hitInfo == ChartHitInfo::HitAxisName)
                strInfo = QString::fromLatin1("Hovered Element: Axis Name\r\nAxis Name: %1").arg(strName);
            else
                strInfo = QString::fromLatin1("Hovered Element: Axis\r\nAxis Name: %1\r\nValue = %2").arg(strName).arg(value);
        }
        break;
    case ChartHitInfo::HitTitle:
        strInfo = tr("Hovered Element: Chart Title");
        break;
    case ChartHitInfo::HitArea:
        {
            ChartAxis* axisX = m_chart->axis(hit->m_index1);
            ChartAxis* axisY = m_chart->axis(hit->m_index2);

            if (axisX != Q_NULL && axisY != Q_NULL)
            {
                Q_ASSERT(axisX != Q_NULL);
                Q_ASSERT(axisY != Q_NULL);
                int valueX = (int)(axisX->valueFromPoint(pt) + 1.5);
                qreal valueY = axisY->valueFromPoint(pt);
                strInfo = QString::fromLatin1("Hovered Element: Chart Area\r\nX Value: %1\r\nY Value: %2").arg(valueX).arg(valueY);
            }
            else
                strInfo = tr("Hovered Element: Chart Area");
        }
        break;

    case ChartHitInfo::HitLegend:
        strInfo = tr("Hovered Element: Legend");
        break;
    default:
        strInfo = DEFAULT_INFO;
        break;
    }

    if (!strInfo.isEmpty())
        m_hitTestResult->setText(strInfo);

    setCursor(cursor);
}

void AdvancedFeatures::mouseUp(ChartHitInfo* hit)
{
    if (hit->m_mouseButton == Qt::NoButton || hit->m_hitInfo != ChartHitInfo::HitDataPoint)
        return;

    Chart2DSeries* dataTable = (Chart2DSeries*)m_chart->series().at(hit->m_index1);
    Q_ASSERT(dataTable != Q_NULL);

    QString strInfo;
    strInfo = QString::fromLatin1("Clicked on the Data Point:\r\n\nSeries Index: %1\r\nSeries Name: %2\r\nData Point Index: %3\r\nData Point Value: %4").
        arg(hit->m_index1).arg(dataTable->name()).arg(hit->m_index2).arg(dataTable->at(hit->m_index2)->valueY());

    QMessageBox messageBox(QMessageBox::Information, qApp->applicationName(), QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(strInfo);
    messageBox.exec();
}

