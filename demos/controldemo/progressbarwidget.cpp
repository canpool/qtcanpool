#include "progressbarwidget.h"
#include "QColorProgressBar.h"
#include "QRoundProgressBar.h"
#include "QStateProgressBar.h"
#include "QWaitProgressBar.h"
#include "QZoomCircleWait.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QTabWidget>
#include <QVBoxLayout>

ProgressBarWidget::ProgressBarWidget(QWidget *parent)
    : QWidget(parent)
{
    createWindow();
}

void ProgressBarWidget::createWindow()
{
    QTabWidget *pTabWidget = new QTabWidget();

    /**
     * @brief StateProgressBar
     */

    QVBoxLayout *stateLayout = new QVBoxLayout();
    stateLayout->setContentsMargins(0, 0, 0, 0);
    stateLayout->setSpacing(2);

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 5);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(1);
    stateLayout->addWidget(slider);

    QStateProgressBar *stateProgressBar = new QStateProgressBar();
    stateProgressBar->setType(QStateProgressBar::ACK);   // NACK, ACK
    QStringList states;
    states << tr("C") << tr("C++") << tr("Linux") << tr("Qt") << tr("Java");
    stateProgressBar->setStates(states);
    stateProgressBar->setMaximum(5);
    stateProgressBar->setTip("now");
    stateProgressBar->setValue(4);
    stateLayout->addWidget(stateProgressBar);
    connect(slider, SIGNAL(valueChanged(int)), stateProgressBar, SLOT(setValue(int)));

    stateProgressBar = new QStateProgressBar();
    stateProgressBar->setType(QStateProgressBar::NACK);   // NACK, ACK
    states.clear();
    states << tr("edit") << tr("compile") << tr("link") << tr("run") << tr("debug");
    stateProgressBar->setStates(states);
    stateProgressBar->setMaximum(5);
    stateProgressBar->setBarColor(QColor(0, 128, 255));
    stateProgressBar->setTip("Qt");
    stateProgressBar->setValue(1);
    stateLayout->addWidget(stateProgressBar);
    connect(slider, SIGNAL(valueChanged(int)), stateProgressBar, SLOT(setValue(int)));

    stateProgressBar = new QStateProgressBar();
    stateProgressBar->setType(QStateProgressBar::NACK);   // NACK, ACK
    QStringList notes;
    notes << tr("a") << tr("b") << tr("c") << tr("d") << tr("e");
    states.clear();
    states << tr("A") << tr("B") << tr("C") << tr("D") << tr("E");
    stateProgressBar->setStates(states);
    stateProgressBar->setNotes(notes);
    stateProgressBar->setNoteColor(Qt::black);
    stateProgressBar->setMaximum(5);
    stateProgressBar->setValue(1);
    stateLayout->addWidget(stateProgressBar);
    connect(slider, SIGNAL(valueChanged(int)), stateProgressBar, SLOT(setValue(int)));

    QWidget *stateProgressWidget = new QWidget();
    stateProgressWidget->setLayout(stateLayout);
    pTabWidget->addTab(stateProgressWidget, tr("State"));

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
    QVBoxLayout *roundMainLayout = new QVBoxLayout();
    roundMainLayout->addWidget(slider);
    roundMainLayout->addLayout(roundLayout);
    roundMainLayout->addStretch();
    roundProgressWidget->setLayout(roundMainLayout);
    pTabWidget->addTab(roundProgressWidget, tr("Round"));

    /**
     * @brief ColorProgressBar
     */
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(5);

    QColorProgressBar *colorProgressBar1 = new QColorProgressBar();
    colorProgressBar1->setTextPosition(QColorProgressBar::TextInBar);
    colorProgressBar1->setDecimals(0);
    colorProgressBar1->setBarBackColor(QColor(Qt::gray));
    colorProgressBar1->setBarForeColor(QColor(100, 184, 255));
    colorProgressBar1->setTextForeColor(QColor(Qt::white));
    colorProgressBar1->setValue(40);
    colorProgressBar1->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar1, SLOT(setValue(int)));

    QColorProgressBar *colorProgressBar2 = new QColorProgressBar(QColorProgressBar::StyleSegmented);
    colorProgressBar2->setDecimals(1);
    colorProgressBar2->setBarBackColor(QColor(Qt::white));
    colorProgressBar2->setBarForeColor(QColor(200, 10, 150));
    colorProgressBar2->setBarForeAlternateColor(QColor(255, 107, 107));
    colorProgressBar2->setBarStep(20, -20, 20);
    colorProgressBar2->setValue(50);
    colorProgressBar2->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar2, SLOT(setValue(int)));

    QColorProgressBar *colorProgressBar3 = new QColorProgressBar(QColorProgressBar::StyleSegmented);
    colorProgressBar3->setTextPosition(QColorProgressBar::TextOutBar);
    colorProgressBar3->setDecimals(1);
    colorProgressBar3->setBarBackColor(QColor(250, 250, 250));
    colorProgressBar3->setBarForeColor(QColor(24, 189, 155));
    colorProgressBar3->setBarForeAlternateColor(QColor(242, 242, 242));
    colorProgressBar3->setTextForeColor(Qt::white);
    colorProgressBar3->setTextBackColor(QColor(38, 40, 42));
    colorProgressBar3->setBarStep(5, -10);
    colorProgressBar3->setValue(30);
    colorProgressBar3->setFixedHeight(20);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar3, SLOT(setValue(int)));

    QGradientStops colorGradientPoints;
    colorGradientPoints << QGradientStop(0, Qt::green) << QGradientStop(0.5, Qt::yellow) << QGradientStop(1, Qt::red);
    QColorProgressBar *colorProgressBar4 = new QColorProgressBar(QColorProgressBar::StyleGradual);
    colorProgressBar4->setTextPosition(QColorProgressBar::TextOnBar);
    colorProgressBar4->setDecimals(1);
    colorProgressBar4->setBarBackColor(QColor(250, 250, 250));
    colorProgressBar4->setTextForeColor(Qt::white);
    colorProgressBar4->setTextBackColor(QColor(24, 189, 155));
    colorProgressBar4->setBarForeDataColors(colorGradientPoints);
    colorProgressBar4->setValue(30);
    colorProgressBar4->setFixedHeight(60);
    connect(slider, SIGNAL(valueChanged(int)), colorProgressBar4, SLOT(setValue(int)));

    QVBoxLayout *colorLayout = new QVBoxLayout();
    colorLayout->setContentsMargins(5, 5, 5, 5);
    colorLayout->setSpacing(5);
    colorLayout->addWidget(slider);
    colorLayout->addWidget(colorProgressBar1);
    colorLayout->addWidget(colorProgressBar2);
    colorLayout->addWidget(colorProgressBar3);
    colorLayout->addWidget(colorProgressBar4);
    colorLayout->addStretch();
    QWidget *colorProgressWidget = new QWidget();
    colorProgressWidget->setLayout(colorLayout);
    pTabWidget->addTab(colorProgressWidget, tr("Color"));

    /**
     * @brief QWaitProgressBar
     */
    QWaitProgressBar *taichiWait = new QWaitProgressBar(QWaitProgressBar::Taichi);
    taichiWait->setForeColor(QColor(97, 117, 118));
    taichiWait->setBackColor(QColor(255, 107, 107));
    QWaitProgressBar *petalWait = new QWaitProgressBar(QWaitProgressBar::Petal);
    petalWait->setForeColor(QColor(255, 0, 155));
    petalWait->setClockwiseRotate(false);

    QWaitProgressBar *circleWait = new QWaitProgressBar(QWaitProgressBar::Circle);
    circleWait->setForeColor(QColor(253, 120, 120));
    QWaitProgressBar *bubbleWait = new QWaitProgressBar(QWaitProgressBar::Bubble);
    bubbleWait->setForeColor(QColor(24, 189, 155));

    QWaitProgressBar *pieWait = new QWaitProgressBar(QWaitProgressBar::Pie);
    pieWait->setForeColor(QColor(255, 107, 107));
    pieWait->setBackColor(QColor(97, 117, 118));
    QWaitProgressBar *bearingWait = new QWaitProgressBar(QWaitProgressBar::Bearing);
    bearingWait->setForeColor(QColor(46, 194, 163));
    bearingWait->setBackColor(QColor(255, 107, 107));
    bearingWait->setClockwiseRotate(false);

    QWaitProgressBar *lineWait = new QWaitProgressBar(QWaitProgressBar::Line);
    lineWait->setForeColor(QColor(253, 132, 132));
    QWaitProgressBar *infiniteWait = new QWaitProgressBar(QWaitProgressBar::Infinite);
    infiniteWait->setForeColor(QColor(46, 194, 164));
    infiniteWait->setBackColor(QColor(255, 107, 107));
    infiniteWait->setClockwiseRotate(false);

    QWaitProgressBar *donutWait1 = new QWaitProgressBar(QWaitProgressBar::Donut);
    donutWait1->setForeColor(QColor(255, 107, 107));
    donutWait1->setBackColor(QColor(97, 117, 118));
    QWaitProgressBar *donutWait2 = new QWaitProgressBar(QWaitProgressBar::Donut);
    donutWait2->setForeColor(QColor(24, 189, 155));
    donutWait2->setBackColor(Qt::lightGray);
    donutWait2->setClockwiseRotate(false);

    QZoomCircleWait *zoomCircleWait1 = new QZoomCircleWait();
    zoomCircleWait1->setColor(QColor(255, 107, 107));
    zoomCircleWait1->setZoomDelta(5);
    QZoomCircleWait *zoomCircleWait2 = new QZoomCircleWait();
    zoomCircleWait2->setColor(QColor(24, 189, 155));
    zoomCircleWait2->setMinRadius(25);
    zoomCircleWait2->setZoomDelta(5);
    zoomCircleWait2->setZoomOut(true);
    QZoomCircleWait *zoomCircleWait3 = new QZoomCircleWait();
    zoomCircleWait3->setColor(Qt::red);
    zoomCircleWait3->setMinRadius(25);
    zoomCircleWait3->setRadius(50);
    zoomCircleWait3->setZoomDelta(5);
    zoomCircleWait3->setZoomOut(true);

    QHBoxLayout *zoomLayout = new QHBoxLayout();
    zoomLayout->addWidget(zoomCircleWait2);
    zoomLayout->addWidget(zoomCircleWait3);
    QWidget *zoomWidget = new QWidget();
    zoomWidget->setLayout(zoomLayout);

    QGridLayout *waitLayout = new QGridLayout();
    waitLayout->addWidget(taichiWait, 0, 0, 1, 1);
    waitLayout->addWidget(petalWait, 1, 0, 1, 1);
    waitLayout->addWidget(circleWait, 0, 1, 1, 1);
    waitLayout->addWidget(bubbleWait, 1, 1, 1, 1);
    waitLayout->addWidget(pieWait, 0, 2, 1, 1);
    waitLayout->addWidget(bearingWait, 1, 2, 1, 1);
    waitLayout->addWidget(lineWait, 0, 3, 1, 1);
    waitLayout->addWidget(infiniteWait, 1, 3, 1, 1);
    waitLayout->addWidget(donutWait1, 0, 4, 1, 1);
    waitLayout->addWidget(donutWait2, 1, 4, 1, 1);
    waitLayout->addWidget(zoomCircleWait1, 0, 5, 1, 1);
    waitLayout->addWidget(zoomWidget, 1, 5, 1, 1);
    QWidget *waitProgressWidget = new QWidget();
    QVBoxLayout *waitMainLayout = new QVBoxLayout();
    waitMainLayout->addLayout(waitLayout);
    waitProgressWidget->setLayout(waitMainLayout);
    pTabWidget->addTab(waitProgressWidget, tr("Wait"));

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(pTabWidget);
    setLayout(mainLayout);
}
