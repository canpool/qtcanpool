#include "colorprogressbartest.h"
#include "ui_colorprogressbartest.h"

ColorProgressBarTest::ColorProgressBarTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorProgressBarTest)
{
    ui->setupUi(this);
    ui->colorProgressBar1->setDecimal(0);
    ui->colorProgressBar1->setBarBackGroundColor(QColor(Qt::white));
    ui->colorProgressBar1->setBarColor(QColor(100, 184, 255));
    ui->colorProgressBar1->setShowBarSplitLine(false);
//    ui->slider1->setRange(0, 100);
    connect(ui->slider1, SIGNAL(valueChanged(int)), ui->colorProgressBar1, SLOT(setValue(int)));
    ui->colorProgressBar2->setDecimal(1);
    ui->colorProgressBar2->setBarBackGroundColor(QColor(Qt::white));
    ui->colorProgressBar2->setBarColor(QColor(255, 107, 107));
    ui->colorProgressBar2->setShowBarSplitLine(true);
    ui->colorProgressBar2->setBarSplitLineDelta(3);
    ui->colorProgressBar2->setBarSplitLineColor(QColor(242, 242, 242));
    connect(ui->slider2, SIGNAL(valueChanged(int)), ui->colorProgressBar2, SLOT(setValue(int)));
    ui->colorProgressBar3->setDecimal(1);
    ui->colorProgressBar3->setBarBackGroundColor(QColor(250, 250, 250));
    ui->colorProgressBar3->setBarColor(QColor(24, 189, 155));
    ui->colorProgressBar3->setShowBarSplitLine(true);
    ui->colorProgressBar3->setBarSplitLineDelta(5);
    ui->colorProgressBar3->setBarSplitLineColor(QColor(242, 242, 242));
    connect(ui->slider3, SIGNAL(valueChanged(int)), ui->colorProgressBar3, SLOT(setValue(int)));
}

ColorProgressBarTest::~ColorProgressBarTest()
{
    delete ui;
}
