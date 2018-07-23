#include "widget.h"
#include "ui_widget.h"
#include "smoothcurvecreator.h"

#include <QPainter>
#include <QtGlobal>
#include <QDateTime>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1. 随机生成曲线上的点: 横坐标为 [-200, 200]，纵坐标为 [-100, 100]
    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    int x = -300;
    while (x < 300) {
        knots << QPointF(x, qrand() % 300 - 100);
        x += qMin(qrand() % 30 + 5, 300);
    }
qDebug()<<QTime::currentTime().toString("HH:mm:ss zzz");
    // [2] 根据曲线上的点创建平滑曲线
    smoothCurve = SmoothCurveCreator::createSmoothCurve(knots);
qDebug()<<QTime::currentTime().toString("HH:mm:ss zzz");
    // 直接连接点的创建非平滑曲线曲线
    nonSmoothCurve.moveTo(knots[0]);
    for (int i = 1; i < knots.size(); ++i) {
        nonSmoothCurve.lineTo(knots[i]);
    }

    connect(ui->showKnotsCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
    connect(ui->showSmoothCurveCheckBox, SIGNAL(clicked(bool)), this, SLOT(update()));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(width() / 2 , height() / 2);
    painter.scale(1, -1);

    // 画坐标轴
    painter.setPen(QColor(180, 180, 180));
    painter.drawLine(-250, 0, 250, 0);
    painter.drawLine(0, 150, 0, -150);

    // [3] showSmoothCurveCheckBox 被选中时显示平滑曲线，否则显示非平滑曲线
    painter.setPen(QPen(QColor(80, 80, 80), 2));
    if (ui->showSmoothCurveCheckBox->isChecked()) {
        painter.drawPath(smoothCurve);
    } else {
        painter.drawPath(nonSmoothCurve);
    }

    // 如果曲线上的点可见，则显示出来
    if (ui->showKnotsCheckBox->isChecked()) {
        painter.setPen(Qt::black);
        painter.setBrush(Qt::gray);

        foreach(QPointF p, knots) {
            painter.drawEllipse(p, 3, 3);
        }
    }
}
