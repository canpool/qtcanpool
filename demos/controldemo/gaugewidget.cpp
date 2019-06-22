#include "gaugewidget.h"

#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSlider>

#include "QtGaugeProgress.h"

GaugeWidget::GaugeWidget(QWidget *parent)
    : QWidget(parent)
{
    createWindow();
}

void GaugeWidget::createWindow()
{
    m_pTabWidget = new QTabWidget();

    createGaugeProgress();

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_pTabWidget);
    setLayout(mainLayout);
}

void GaugeWidget::createGaugeProgress()
{
    QtGaugeProgress *gaugeProgress1 = new QtGaugeProgress();

    QtGaugeProgress *gaugeProgress2 = new QtGaugeProgress();
    gaugeProgress2->setProgressColor(QColor(71, 164, 233));
    gaugeProgress2->setPointerStyle(QtGaugeProgress::Triangle);
    gaugeProgress2->setShowValue(true);

    QtGaugeProgress *gaugeProgress3 = new QtGaugeProgress();
    gaugeProgress3->setProgressColor(QColor(0, 177, 125));
    gaugeProgress3->setPointerStyle(QtGaugeProgress::Indicator);

    QtGaugeProgress *gaugeProgress4 = new QtGaugeProgress();
    gaugeProgress4->setProgressColor(QColor(222, 175, 57));
    gaugeProgress4->setPointerStyle(QtGaugeProgress::IndicatorR);

    QtGaugeProgress *gaugeProgress5 = new QtGaugeProgress();
    gaugeProgress5->setProgressColor(QColor(162, 121, 197));
    gaugeProgress5->setShowPointer(false);

    QtGaugeProgress *gaugeProgress6 = new QtGaugeProgress();
    gaugeProgress6->setProgressColor(QColor(120, 150, 10));
    gaugeProgress6->setStartAngle(30);
    gaugeProgress6->setEndAngle(30);
    gaugeProgress6->setDecimals(1);
    gaugeProgress6->setTextColor(QColor(255, 0, 128));

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setMargin(0);
    gridLayout->addWidget(gaugeProgress1, 0, 1);
    gridLayout->addWidget(gaugeProgress2, 0, 2);
    gridLayout->addWidget(gaugeProgress3, 0, 3);
    gridLayout->addWidget(gaugeProgress4, 1, 1);
    gridLayout->addWidget(gaugeProgress5, 1, 2);
    gridLayout->addWidget(gaugeProgress6, 1, 3);

    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(1);
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress1, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress2, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress3, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress4, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress5, SLOT(setValue(int)));
    connect(slider, SIGNAL(valueChanged(int)), gaugeProgress6, SLOT(setValue(int)));
    QWidget *w = new QWidget();
    QVBoxLayout *vboxLayout = new QVBoxLayout();
    vboxLayout->addWidget(slider);
    vboxLayout->addLayout(gridLayout);
    w->setLayout(vboxLayout);

    m_pTabWidget->addTab(w, tr("Progress"));
}
