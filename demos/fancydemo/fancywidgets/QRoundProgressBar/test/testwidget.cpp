#include "testwidget.h"
#include "ui_testwidget.h"

TestWidget::TestWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestWidget)
{
    qApp->setStyle("fusion");

    ui->setupUi(this);

    // donut (default style)
    ui->RoundBar1->setFormat("%v");
    ui->RoundBar1->setDecimals(0);
    connectToSlider(ui->RoundBar1);

    // pie
    ui->RoundBar2->setNullPosition(QRoundProgressBar::PositionRight);
    ui->RoundBar2->setBarStyle(QRoundProgressBar::StylePie);
    connectToSlider(ui->RoundBar2);

    // line
    ui->RoundBar3->setFormat("%m");
    ui->RoundBar3->setBarStyle(QRoundProgressBar::StyleLine);
    connectToSlider(ui->RoundBar3);

    // donut (customized via QPalette + gradient)
    QPalette p1;
    p1.setBrush(QPalette::AlternateBase, Qt::black);
    p1.setColor(QPalette::Text, Qt::yellow);
    ui->RoundBar4->setPalette(p1);

    ui->RoundBar4->setNullPosition(QRoundProgressBar::PositionLeft);
    ui->RoundBar4->setDecimals(0);

    // make a gradient from green over yellow to red
    QGradientStops gradientPoints;
    gradientPoints << QGradientStop(0, Qt::green) << QGradientStop(0.5, Qt::yellow) << QGradientStop(1, Qt::red);
    // and set it
    ui->RoundBar4->setDataColors(gradientPoints);

    connectToSlider(ui->RoundBar4);

    // pie (customized via QPalette + gradient)
    QPalette p2(p1);
    p2.setBrush(QPalette::Base, Qt::lightGray);
    p2.setColor(QPalette::Text, Qt::magenta);
    p2.setColor(QPalette::Shadow, Qt::green);
    ui->RoundBar5->setPalette(p2);
    ui->RoundBar5->setNullPosition(QRoundProgressBar::PositionRight);
    ui->RoundBar5->setBarStyle(QRoundProgressBar::StylePie);
    ui->RoundBar5->setDataColors(gradientPoints);   // here gradient colors is set
    connectToSlider(ui->RoundBar5);

    // line style with custom outline
    ui->RoundBar6->setDecimals(2);
    ui->RoundBar6->setBarStyle(QRoundProgressBar::StyleLine);
    ui->RoundBar6->setOutlinePenWidth(18);
    ui->RoundBar6->setDataPenWidth(10);
    connectToSlider(ui->RoundBar6);

    // big one
    connectToSlider(ui->RoundBar7);
}

TestWidget::~TestWidget()
{
    delete ui;
}

void TestWidget::connectToSlider(QRoundProgressBar *bar)
{
    bar->setRange(ui->Slider->minimum(), ui->Slider->maximum());
    bar->setValue(ui->Slider->value());
    connect(ui->Slider, SIGNAL(valueChanged(int)), bar, SLOT(setValue(int)));
}
