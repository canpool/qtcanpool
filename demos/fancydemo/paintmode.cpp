#include "paintmode.h"
#include "cwidgets.h"

#include "qcanpool/tinytabwidget.h"
#include "qcanpool/centralarea.h"
#include "qcanpool/ministack.h"

#include "stateprogressbar.h"
#include "qroundprogressbar.h"
#include "colorprogressbar.h"
#include "zoomcirclewait.h"
#include "waitprogressbar.h"

#include "dialplot.h"
#include "dashboard.h"

PaintMode::PaintMode(QWidget *parent)
    : IMode(parent)
{
    setObjectName(QLatin1String("PaintMode"));
    setDisplayName(tr("Painter"));
    setIcon(QIcon(":/mode/paint"));
    createWindow();
}

void PaintMode::createWindow()
{
    m_pMainTabWidget = new TinyTabWidget();

    // progress widget
    createProgressWidget();
    CentralArea *progressArea = new CentralArea();
    progressArea->setWidget(m_pProgressWidget);


    m_pMainTabWidget->addTab(progressArea, tr("Progress"));
    m_pMainTabWidget->addTab(new QWidget(), tr("Switch"));
    m_pMainTabWidget->addTab(new QWidget(), tr("tab1"));
    m_pMainTabWidget->addTab(new QWidget(), tr("tab2"));

    m_pMainTabWidget->setTextColor(QColor(10,10,10));
    m_pMainTabWidget->setSelectedTextColor(QColor(255,255,255));
    m_pMainTabWidget->setHoverColor(QColor(180,180,180));
    m_pMainTabWidget->setBackgroundColor(QColor(0,100,200,120));
    m_pMainTabWidget->setTabPosition(TinyTabWidget::East);

    setWidget(m_pMainTabWidget);
}

void PaintMode::createProgressWidget()
{
    m_pProgressWidget = new QWidget();

    createProgressBar();
    createDial();

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setMargin(0);
    layout1->setSpacing(10);
    layout1->addWidget(m_pProgressBarStack);
    layout1->addWidget(m_pDialStack);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(1,1,1,1);
    layout->setSpacing(10);
    layout->addLayout(layout1);
//    layout->addLayout(layout2);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    layout->addWidget(new QTextBrowser);
    m_pProgressWidget->setLayout(layout);
}

void PaintMode::createProgressBar()
{
    m_pProgressBarStack = new MiniStack();
    m_pProgressBarStack->setTitle("ProgressBar");
    /**
     * @brief StateProgressBar
     */
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 5);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(1);

    QVBoxLayout *stateLayout = new QVBoxLayout();
    stateLayout->setMargin(0);
    stateLayout->setSpacing(2);
    StateProgressBar *stateProgressBar = new StateProgressBar();
    stateProgressBar->setType(StateProgressBar::ACK); // NACK, ACK
    QStringList states;
    states<<tr("C")<<tr("C++")<<tr("Linux")<<tr("Qt")<<tr("Java");
    stateProgressBar->setStates(states);
    stateProgressBar->setTip("now");
    stateProgressBar->setValue(4);
//    stateProgressBar->setFixedHeight(140);
    stateProgressBar->setMinimumHeight(140);
    stateLayout->addWidget(stateProgressBar);
    connect(slider, SIGNAL(valueChanged(int)), stateProgressBar, SLOT(setValue(int)));

    stateProgressBar = new StateProgressBar();
    stateProgressBar->setType(StateProgressBar::NACK); // NACK, ACK
    states.clear();
    states<<tr("edit")<<tr("compile")<<tr("link")<<tr("run")<<tr("debug");
    stateProgressBar->setStates(states);
    stateProgressBar->setTip("Qt");
    stateProgressBar->setValue(1);
//    stateProgressBar->setFixedHeight(140);
    stateProgressBar->setMinimumHeight(140);
    stateLayout->addWidget(stateProgressBar);
    connect(slider, SIGNAL(valueChanged(int)), stateProgressBar, SLOT(setValue(int)));
    QWidget *stateProgressWidget = new QWidget();
    stateProgressWidget->setLayout(stateLayout);
    m_pProgressBarStack->addTab(tr("State"), stateProgressWidget, slider);

    /**
     * @brief Round PorgressBar
     */
    slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(999);
    slider->setValue(150);
    slider->setOrientation(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(25);

    QGridLayout *grid1 = new QGridLayout();
    QLabel *label = new QLabel(tr("Donut Style"));
    label->setAlignment(Qt::AlignCenter);
    grid1->addWidget(label, 0, 0, 1, 1);
    label = new QLabel(tr("Pie Style"));
    label->setAlignment(Qt::AlignCenter);
    grid1->addWidget(label, 0, 1, 1, 1);
    label = new QLabel(tr("Line Style"));
    label->setAlignment(Qt::AlignCenter);
    grid1->addWidget(label, 0, 2, 1, 1);

    // 1. donut (default style)
    QRoundProgressBar *roundProgressBar = new QRoundProgressBar();
    roundProgressBar->setFormat("%v");
    roundProgressBar->setDecimals(0);
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 1, 0, 1, 1);

    // 2. pie
    roundProgressBar = new QRoundProgressBar();
    roundProgressBar->setNullPosition(QRoundProgressBar::PositionRight);
    roundProgressBar->setBarStyle(QRoundProgressBar::StylePie);
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 1, 1, 1, 1);

    // 3. line
    roundProgressBar = new QRoundProgressBar();
    roundProgressBar->setFormat("%m");
    roundProgressBar->setBarStyle(QRoundProgressBar::StyleLine);
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 1, 2, 1, 1);

    // 4. donut (customized via QPalette + gradient)
    roundProgressBar = new QRoundProgressBar();
    QPalette p1;
    p1.setBrush(QPalette::AlternateBase, Qt::black);
    p1.setColor(QPalette::Text, Qt::yellow);
    roundProgressBar->setPalette(p1);

    roundProgressBar->setNullPosition(QRoundProgressBar::PositionLeft);
    roundProgressBar->setDecimals(0);

    // make a gradient from green over yellow to red
    QGradientStops gradientPoints;
    gradientPoints << QGradientStop(0, Qt::green) << QGradientStop(0.5, Qt::yellow) << QGradientStop(1, Qt::red);
    // and set it
    roundProgressBar->setDataColors(gradientPoints);
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 2, 0, 1, 1);

    // 5.pie (customized via QPalette + gradient)
    roundProgressBar = new QRoundProgressBar();
    QPalette p2(p1);
    p2.setBrush(QPalette::Base, Qt::lightGray);
    p2.setColor(QPalette::Text, Qt::magenta);
    p2.setColor(QPalette::Shadow, Qt::green);
    roundProgressBar->setPalette(p2);
    roundProgressBar->setNullPosition(QRoundProgressBar::PositionRight);
    roundProgressBar->setBarStyle(QRoundProgressBar::StylePie);
    roundProgressBar->setDataColors(gradientPoints);   // here gradient colors is set
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 2, 1, 1, 1);

    // 6. line style with custom outline
    roundProgressBar = new QRoundProgressBar();
    roundProgressBar->setDecimals(2);
    roundProgressBar->setBarStyle(QRoundProgressBar::StyleLine);
    roundProgressBar->setOutlinePenWidth(18);
    roundProgressBar->setDataPenWidth(10);
    roundProgressBar->setRange(slider->minimum(), slider->maximum());
    roundProgressBar->setValue(slider->value());
    connect(slider, SIGNAL(valueChanged(int)), roundProgressBar, SLOT(setValue(int)));
    grid1->addWidget(roundProgressBar, 2, 2, 1, 1);

    QHBoxLayout *roundLayout = new QHBoxLayout();
    roundLayout->addLayout(grid1);
    QWidget *roundProgressWidget = new QWidget();
    roundProgressWidget->setLayout(roundLayout);
    m_pProgressBarStack->addTab(tr("Round"), roundProgressWidget, slider);

    /**
     * @brief ColorProgressBar
     */
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(5);
    ColorProgressBar *colorProgressBar1 = new ColorProgressBar();
    colorProgressBar1->setDecimal(0);
    colorProgressBar1->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar1->setBarColor(QColor(100, 184, 255));
    colorProgressBar1->setShowBarSplitLine(false);
    colorProgressBar1->setValue(40);
    colorProgressBar1->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar1, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar2 = new ColorProgressBar();
    colorProgressBar2->setDecimal(1);
    colorProgressBar2->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar2->setBarColor(QColor(255, 107, 107));
    colorProgressBar2->setShowBarSplitLine(true);
    colorProgressBar2->setBarSplitLineDelta(3);
    colorProgressBar2->setBarSplitLineColor(QColor(242, 242, 242));
    colorProgressBar2->setValue(10);
    colorProgressBar2->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar2, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar3 = new ColorProgressBar();
    colorProgressBar3->setDecimal(1);
    colorProgressBar3->setBarBackGroundColor(QColor(250, 250, 250));
    colorProgressBar3->setBarColor(QColor(24, 189, 155));
    colorProgressBar3->setShowBarSplitLine(true);
    colorProgressBar3->setBarSplitLineDelta(5);
    colorProgressBar3->setBarSplitLineColor(QColor(242, 242, 242));
    colorProgressBar3->setValue(30);
    colorProgressBar3->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar3, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar4 = new ColorProgressBar();
    colorProgressBar4->setDecimal(0);
    colorProgressBar4->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar4->setBarColor(QColor(255, 184, 100));
    colorProgressBar4->setShowBarSplitLine(false);
    colorProgressBar4->setValue(70);
    colorProgressBar4->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar4, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar5 = new ColorProgressBar();
    colorProgressBar5->setDecimal(0);
    colorProgressBar5->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar5->setBarColor(QColor(250, 50, 50));
    colorProgressBar5->setShowBarSplitLine(false);
    colorProgressBar5->setValue(100);
    colorProgressBar5->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar5, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar6 = new ColorProgressBar();
    colorProgressBar6->setDecimal(0);
    colorProgressBar6->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar6->setBarColor(QColor(50, 184, 50));
    colorProgressBar6->setShowBarSplitLine(false);
    colorProgressBar6->setValue(20);
    colorProgressBar6->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar6, SLOT(setValue(int)));
    ColorProgressBar *colorProgressBar7 = new ColorProgressBar();
    colorProgressBar7->setDecimal(0);
    colorProgressBar7->setBarBackGroundColor(QColor(Qt::white));
    colorProgressBar7->setBarColor(QColor(10, 10, 255));
    colorProgressBar7->setShowBarSplitLine(false);
    colorProgressBar7->setValue(40);
    colorProgressBar7->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar7, SLOT(setValue(int)));

    QVBoxLayout *colorLayout = new QVBoxLayout();
    colorLayout->setContentsMargins(5, 5, 5, 5);
    colorLayout->setSpacing(5);
//    colorLayout->addStretch();
    colorLayout->addWidget(colorProgressBar1);
    colorLayout->addWidget(colorProgressBar2);
    colorLayout->addWidget(colorProgressBar3);
    colorLayout->addWidget(colorProgressBar4);
    colorLayout->addWidget(colorProgressBar5);
    colorLayout->addWidget(colorProgressBar6);
    colorLayout->addWidget(colorProgressBar7);
//    colorLayout->addStretch();
    QWidget *colorProgressWidget = new QWidget();
    colorProgressWidget->setLayout(colorLayout);
    m_pProgressBarStack->addTab(tr("Color"), colorProgressWidget, slider);

    /**
     * @brief WaitProgressBar
     */
    WaitProgressBar *taichiWait1 = new WaitProgressBar(WaitProgressBar::Taichi);
    taichiWait1->setFrontColor(QColor(97, 117, 118));
    taichiWait1->setBackColor(QColor(255, 107, 107));
    WaitProgressBar *taichiWait2 = new WaitProgressBar(WaitProgressBar::Taichi);
    taichiWait2->setFrontColor(QColor(24, 189, 155));
    taichiWait2->setBackColor(Qt::darkGray);
    taichiWait2->setClockwiseRotate(false);
    WaitProgressBar *circleWait1 = new WaitProgressBar(WaitProgressBar::Circle);
    circleWait1->setFrontColor(QColor(253, 120, 120));
    WaitProgressBar *circleWait2 = new WaitProgressBar(WaitProgressBar::Circle);
    circleWait2->setFrontColor(QColor(46, 194, 163));
    circleWait2->setClockwiseRotate(false);
    WaitProgressBar *pieWait1 = new WaitProgressBar(WaitProgressBar::Pie);
    pieWait1->setFrontColor(QColor(255, 107, 107));
    pieWait1->setBackColor(QColor(97, 117, 118));
    WaitProgressBar *pieWait2 = new WaitProgressBar(WaitProgressBar::Pie);
    pieWait2->setFrontColor(QColor(24, 189, 155));
    pieWait2->setBackColor(Qt::gray);
    pieWait2->setClockwiseRotate(false);
    WaitProgressBar *lineWait1 = new WaitProgressBar(WaitProgressBar::Line);
    lineWait1->setFrontColor(QColor(253, 132, 132));
    WaitProgressBar *lineWait2 = new WaitProgressBar(WaitProgressBar::Line);
    lineWait2->setFrontColor(QColor(46, 194, 164));
    lineWait2->setClockwiseRotate(false);
    WaitProgressBar *donutWait1 = new WaitProgressBar(WaitProgressBar::Donut);
    donutWait1->setFrontColor(QColor(255, 107, 107));
    donutWait1->setBackColor(QColor(97, 117, 118));
    WaitProgressBar *donutWait2 = new WaitProgressBar(WaitProgressBar::Donut);
    donutWait2->setFrontColor(QColor(24, 189, 155));
    donutWait2->setBackColor(Qt::lightGray);
    donutWait2->setClockwiseRotate(false);
    ZoomCircleWait *zoomCircleWait1 = new ZoomCircleWait();
    zoomCircleWait1->setColor(QColor(255, 107, 107));
    zoomCircleWait1->setZoomDelta(5);
    ZoomCircleWait *zoomCircleWait2 = new ZoomCircleWait();
    zoomCircleWait2->setColor(QColor(24, 189, 155));
    zoomCircleWait2->setMinRadious(25);
    zoomCircleWait2->setZoomDelta(5);
    zoomCircleWait2->setZoomOut(true);
    ZoomCircleWait *zoomCircleWait3 = new ZoomCircleWait();
    zoomCircleWait3->setColor(Qt::red);
    zoomCircleWait3->setMinRadious(25);
    zoomCircleWait3->setRadious(50);
    zoomCircleWait3->setZoomDelta(5);
    zoomCircleWait3->setZoomOut(true);

    QHBoxLayout *zoomLayout = new QHBoxLayout();
    zoomLayout->addWidget(zoomCircleWait2);
    zoomLayout->addWidget(zoomCircleWait3);
    QWidget *zoomWidget = new QWidget();
    zoomWidget->setLayout(zoomLayout);

    QGridLayout *waitLayout = new QGridLayout();
    waitLayout->addWidget(taichiWait1, 0, 0, 1, 1);
    waitLayout->addWidget(taichiWait2, 1, 0, 1, 1);
    waitLayout->addWidget(circleWait1, 0, 1, 1, 1);
    waitLayout->addWidget(circleWait2, 1, 1, 1, 1);
    waitLayout->addWidget(pieWait1, 0, 2, 1, 1);
    waitLayout->addWidget(pieWait2, 1, 2, 1, 1);
    waitLayout->addWidget(lineWait1, 0, 3, 1, 1);
    waitLayout->addWidget(lineWait2, 1, 3, 1, 1);
    waitLayout->addWidget(donutWait1, 0, 4, 1, 1);
    waitLayout->addWidget(donutWait2, 1, 4, 1, 1);
    waitLayout->addWidget(zoomCircleWait1, 0, 5, 1, 1);
    waitLayout->addWidget(zoomWidget, 1, 5, 1, 1);
    QWidget *waitProgressWidget = new QWidget();
    waitProgressWidget->setLayout(waitLayout);
    m_pProgressBarStack->addTab(tr("Wait"), waitProgressWidget);
}

void PaintMode::createDial()
{
    m_pDialStack = new MiniStack();
    m_pDialStack->setTitle(tr("Dial"));

    /**
     * @brief dialplot
     */
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(5);

    DialPlot *dialPlot1 = new DialPlot();
    dialPlot1->setDesc(tr("dial"));
    dialPlot1->setUnit(" unit");
    dialPlot1->setRange(0,100);
    dialPlot1->setUpper(70);
    dialPlot1->setLower(30);
    dialPlot1->setValue(10);
    dialPlot1->setTextColor(QColor(0, 0, 0));
    dialPlot1->setBgColor(QColor(255, 255, 255));
    dialPlot1->setFirstColor(QColor(240,0,0));
    dialPlot1->setSecondColor(QColor(0, 205, 0));
    dialPlot1->setThirdColor(QColor(255,255,10));
    dialPlot1->setCenterColor(QColor(51, 51, 51));
    connect(slider, SIGNAL(valueChanged(int)), dialPlot1, SLOT(setValue(int)));

    DialPlot *dialPlot2 = new DialPlot();
    dialPlot2 = new DialPlot();
    dialPlot2->setDesc(tr("O2(vol)"));
    dialPlot2->setUnit(" %");
    dialPlot2->setRange(0,100);
    dialPlot2->setUpper(80);
    dialPlot2->setLower(20);
    dialPlot2->setValue(10);
    dialPlot2->setTextColor(QColor(0, 0, 0));
    dialPlot2->setBgColor(QColor(255, 255, 255));
    dialPlot2->setFirstColor(QColor(240,50,50));
    dialPlot2->setSecondColor(QColor(102, 205, 0));
    dialPlot2->setThirdColor(QColor(240,50,50));
    dialPlot2->setCenterColor(QColor(51, 51, 51));
    connect(slider, SIGNAL(valueChanged(int)), dialPlot2, SLOT(setValue(int)));

//    DialPlot *dialPlot3 = new DialPlot();
//    dialPlot3 = new DialPlot();
//    dialPlot3->setDesc(tr("CO2"));
//    dialPlot3->setUnit(" ppm");
//    dialPlot3->setRange(0,100);
//    dialPlot3->setUpper(80);
//    dialPlot3->setLower(20);
//    dialPlot3->setValue(10);
//    dialPlot3->setTextColor(QColor(0, 0, 0));
//    dialPlot3->setBgColor(QColor(255, 255, 255));
//    dialPlot3->setFirstColor(QColor(240,50,50));
//    dialPlot3->setSecondColor(QColor(102, 205, 0));
//    dialPlot3->setThirdColor(QColor(240,50,50));
//    dialPlot3->setCenterColor(QColor(51, 51, 51));
//    connect(slider, SIGNAL(valueChanged(int)), dialPlot3, SLOT(setValue(int)));

    Dashboard *dashBoard = new Dashboard();
    connect(slider, SIGNAL(valueChanged(int)), dashBoard, SLOT(setValue(int)));


    QHBoxLayout *dialLayout = new QHBoxLayout();
    dialLayout->addWidget(dialPlot1);
    dialLayout->addWidget(dialPlot2);
    dialLayout->addWidget(dashBoard);
    QWidget *dialWidget = new QWidget();
    dialWidget->setLayout(dialLayout);
    m_pDialStack->addTab("Dial", dialWidget, slider);
}


//void PaintMode::slotStateSlider(int value)
//{
//    QList<StateProgressBar*> stateProgressBars = m_pProgressWidget->findChildren<StateProgressBar*>();
//    foreach (StateProgressBar *state, stateProgressBars) {
//        state->setValue(value);
//    }
//}
