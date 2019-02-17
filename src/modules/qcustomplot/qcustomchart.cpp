#include "qcustomchart.h"
#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QTextBrowser>

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief QCustomChart::QCustomChart
/// \param parent
///
QCustomChart::QCustomChart(QWidget *parent)
    : QWidget(parent)
{
    createWindow();
}

QCustomChart::~QCustomChart()
{

}

void QCustomChart::setTitle(const QString &text)
{
    m_pTitleLabel->setText(text);
}

void QCustomChart::setXaxisLabel(const QString &text)
{
    m_pCustomPlot->xAxis->setLabel(text);
}

void QCustomChart::setYaxisLabel(const QString &text)
{
    m_pCustomPlot->yAxis->setLabel(text);
}

void QCustomChart::setYaxisRange(double lower, double upper)
{
    // 自动调整轴上值的显示范围
    m_pCustomPlot->rescaleAxes();

    m_yLower = lower;
    m_yLower = upper;
    // 自定义y轴值的显示范围
    m_pCustomPlot->yAxis->setRange(m_yLower,m_yLower);

    // 重绘
    m_pCustomPlot->replot();
}

//void QCustomChart::setData(const QVector<double> &keys, const QVector<double> &values)
//{
//    m_pCustomPlot->graph(0)->setData(keys, values);
//    m_pCustomPlot->rescaleAxes();
//    m_pCustomPlot->replot();
//}

void QCustomChart::createWindow()
{
    QFont font("Segoe UI",10);
    m_pTitleLabel = new QLabel(tr("Title"));
    font.setBold(true);
    font.setPointSize(12);
    m_pTitleLabel->setFont(font);

    // action
    m_pPrimaryAction = new QAction(QIcon(":/chart/data"), tr("Primary Data"), this);
    m_pChartAction = new QAction(QIcon(":/chart/line"), tr("Chart"), this);
    m_pRefreshAction = new QAction(QIcon(":/chart/refresh"), tr("Refresh"), this);
    m_pDownloadAction = new QAction(QIcon(":/chart/download"), tr("Download"), this);

    QActionGroup *actionGroup = new QActionGroup(this);
    m_pPrimaryAction->setCheckable(true);
    m_pChartAction->setCheckable(true);
    actionGroup->addAction(m_pPrimaryAction);
    actionGroup->addAction(m_pChartAction);

    connect(m_pPrimaryAction, SIGNAL(triggered(bool)), this, SLOT(slotPrimaryData()));
    connect(m_pChartAction, SIGNAL(triggered(bool)), this, SLOT(slotChart()));
    connect(m_pRefreshAction, SIGNAL(triggered(bool)), this, SLOT(slotRefresh()));
    connect(m_pDownloadAction, SIGNAL(triggered(bool)), this, SLOT(slotDownload()));

    // toolbar
    m_pToolBar = new QToolBar(this);
    m_pToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pToolBar->setIconSize(QSize(18,18));

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_pToolBar->addWidget(m_pTitleLabel);
    m_pToolBar->addWidget(spacer);
    m_pToolBar->addAction(m_pPrimaryAction);
    m_pToolBar->addAction(m_pChartAction);
    m_pToolBar->addAction(m_pRefreshAction);
    m_pToolBar->addAction(m_pDownloadAction);

    m_pStackedWidget = new QStackedWidget();
    m_pCustomPlot = new CustomPlotZoom();
//    initCustomPlot();
    m_pTextBrowser = new QTextBrowser();
    m_pStackedWidget->addWidget(m_pCustomPlot);
    m_pStackedWidget->addWidget(m_pTextBrowser);

    m_pRubberBand = new QRubberBand(QRubberBand::Rectangle, m_pCustomPlot);

    connect(m_pCustomPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress(QMouseEvent*)));
    connect(m_pCustomPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(mouseMove(QMouseEvent*)));
    connect(m_pCustomPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(mouseRelease(QMouseEvent*)));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_pToolBar);
    layout->addWidget(m_pStackedWidget);

    setLayout(layout);

    m_pChartAction->setChecked(true);
}

//void QCustomChart::initCustomPlot()
//{
//    // 添加一个图表
//    m_pCustomPlot->addGraph();
//    // 设置关键点的显示效果
//    m_pCustomPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle,
//                                                             QPen(QColor(41,138,220), 2), QBrush(QColor(5,189,251)), 10));

//    // 设置x轴显示时间
//    // configure bottom axis to show date instead of number:
//    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//    dateTicker->setDateTimeFormat("MM-dd hh:mm");
//    m_pCustomPlot->xAxis->setTicker(dateTicker);

//    // make range draggable and zoomable:
//    m_pCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

//    // make top right axes clones of bottom left axes:
//    m_pCustomPlot->axisRect()->setupFullAxesBox();
//    // connect signals so top and right axes move in sync with bottom and left axes:
//    connect(m_pCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
//    connect(m_pCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_pCustomPlot->yAxis2, SLOT(setRange(QCPRange)));

//    // 设置轴的标题
//    m_pCustomPlot->xAxis->setLabel(tr("- Time -"));
//}

void QCustomChart::slotRefresh()
{
    m_pCustomPlot->rescaleAxes();
    m_pCustomPlot->replot();
}

void QCustomChart::slotDownload()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save picture"),"untitled.png",
                                                    tr("png file(*.png);;jpg file(*.jpg);;pdf file(*.pdf)"));
    if(fileName.isEmpty()){
        return;
    }
    QString suffix = QFileInfo(fileName).suffix().toLower();
    if(suffix == "png"){
        m_pCustomPlot->savePng(fileName,m_pCustomPlot->width(), m_pCustomPlot->height());
    }else if(suffix == "jpg"){
        m_pCustomPlot->saveJpg(fileName,m_pCustomPlot->width(), m_pCustomPlot->height());
    }else if(suffix == "pdf"){
        m_pCustomPlot->savePdf(fileName,m_pCustomPlot->width(), m_pCustomPlot->height());
    }
}

void QCustomChart::slotPrimaryData()
{
    m_pStackedWidget->setCurrentWidget(m_pTextBrowser);
}

void QCustomChart::slotChart()
{
    m_pStackedWidget->setCurrentWidget(m_pCustomPlot);
}

void QCustomChart::mousePress(QMouseEvent *mevent)
{
    if(mevent->button() == Qt::RightButton)
    {
        m_rubberOrigin = mevent->pos();
        m_pRubberBand->setGeometry(QRect(m_rubberOrigin, QSize()));
        m_pRubberBand->show();
    }
}

void QCustomChart::mouseMove(QMouseEvent *mevent)
{
    if(m_pRubberBand->isVisible())
            m_pRubberBand->setGeometry(QRect(m_rubberOrigin, mevent->pos()).normalized());
}

void QCustomChart::mouseRelease(QMouseEvent *mevent)
{
    Q_UNUSED(mevent);
    if (m_pRubberBand->isVisible())
    {
        const QRect zoomRect = m_pRubberBand->geometry();
        int xp1, yp1, xp2, yp2;
        zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
        double x1 = m_pCustomPlot->xAxis->pixelToCoord(xp1);
        double x2 = m_pCustomPlot->xAxis->pixelToCoord(xp2);
        double y1 = m_pCustomPlot->yAxis->pixelToCoord(yp1);
        double y2 = m_pCustomPlot->yAxis->pixelToCoord(yp2);

        m_pCustomPlot->xAxis->setRange(x1, x2);
        m_pCustomPlot->yAxis->setRange(y1, y2);

        m_pRubberBand->hide();
        m_pCustomPlot->replot();
    }
}

//////////////////////////////////////////////////////////////////////////
/// \brief CustomPlotZoom::CustomPlotZoom
/// \param parent
///
CustomPlotZoom::CustomPlotZoom(QWidget *parent)
    : QCustomPlot(parent)
    , m_bZoomMode(false)
    , m_pRubberBand(new QRubberBand(QRubberBand::Rectangle, this))
{

}

CustomPlotZoom::~CustomPlotZoom()
{

}

void CustomPlotZoom::setZoomMode(bool mode)
{
    m_bZoomMode = mode;
}

void CustomPlotZoom::mousePressEvent(QMouseEvent *event)
{
    if (m_bZoomMode)
    {
        if (event->button() == Qt::RightButton)
        {
            m_origin = event->pos();
            m_pRubberBand->setGeometry(QRect(m_origin, QSize()));
            m_pRubberBand->show();
        }
    }
    QCustomPlot::mousePressEvent(event);
}

void CustomPlotZoom::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pRubberBand->isVisible())
    {
        m_pRubberBand->setGeometry(QRect(m_origin, event->pos()).normalized());
    }
    QCustomPlot::mouseMoveEvent(event);
}

void CustomPlotZoom::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_pRubberBand->isVisible())
    {
        const QRect zoomRect = m_pRubberBand->geometry();
        int xp1, yp1, xp2, yp2;
        zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
        double x1 = xAxis->pixelToCoord(xp1);
        double x2 = xAxis->pixelToCoord(xp2);
        double y1 = yAxis->pixelToCoord(yp1);
        double y2 = yAxis->pixelToCoord(yp2);

        xAxis->setRange(x1, x2);
        yAxis->setRange(y1, y2);

        m_pRubberBand->hide();
        replot();
    }
    QCustomPlot::mouseReleaseEvent(event);
}

