#include "dynamicprogressbartest.h"
#include "ui_dynamicprogressbartest.h"

DynamicProgressBarTest::DynamicProgressBarTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DynamicProgressBarTest)
{
    ui->setupUi(this);
    ui->taichiWait1->setFrontColor(QColor(97, 117, 118));
    ui->taichiWait1->setBackColor(QColor(255, 107, 107));
    ui->taichiWait2->setFrontColor(QColor(24, 189, 155));
    ui->taichiWait2->setBackColor(Qt::white);
    ui->taichiWait2->setClockwiseRotate(false);
    ui->circleWait1->setColor(QColor(253, 120, 120));
    ui->circleWait2->setColor(QColor(46, 194, 163));
    ui->circleWait2->setClockwiseRotate(false);
    ui->pieWait1->setFrontColor(QColor(255, 107, 107));
    ui->pieWait1->setBackColor(QColor(97, 117, 118));
    ui->pieWait2->setFrontColor(QColor(24, 189, 155));
    ui->pieWait2->setBackColor(Qt::white);
    ui->pieWait2->setClockwiseRotate(false);
    ui->lineWait1->setColor(QColor(253, 132, 132));
    ui->lineWait2->setColor(QColor(46, 194, 164));
    ui->lineWait2->setClockwiseRotate(false);
    ui->donutWait1->setFrontColor(QColor(255, 107, 107));
    ui->donutWait1->setBackColor(QColor(97, 117, 118));
    ui->donutWait2->setFrontColor(QColor(24, 189, 155));
    ui->donutWait2->setBackColor(Qt::white);
    ui->donutWait2->setClockwiseRotate(false);
    ui->zoomCircleWait1->setColor(QColor(255, 107, 107));
    ui->zoomCircleWait1->setZoomDelta(5);
    ui->zoomCircleWait2_1->setColor(QColor(24, 189, 155));
    ui->zoomCircleWait2_1->setMinRadious(25);
    ui->zoomCircleWait2_1->setZoomDelta(5);
    ui->zoomCircleWait2_1->setZoomOut(true);
    ui->zoomCircleWait2_2->setColor(Qt::white);
    ui->zoomCircleWait2_2->setMinRadious(25);
    ui->zoomCircleWait2_2->setRadious(50);
    ui->zoomCircleWait2_2->setZoomDelta(5);
    ui->zoomCircleWait2_2->setZoomOut(true);

}

DynamicProgressBarTest::~DynamicProgressBarTest()
{
    delete ui;
}
