#include "mainwindow.h"

#include <QTreeWidget>
#include <QStackedWidget>
#include <QHBoxLayout>

#include "progressbarwidget.h"
#include "gaugewidget.h"
#include "viewwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createWindow();

    resize(1000, 600);
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWindow()
{
    m_pTreeWidget = new QTreeWidget();
    m_pTreeWidget->setHeaderLabel(tr("QtCustomControls Demos"));
    m_pTreeWidget->setFixedWidth(200);
    m_pStackedWidget = new QStackedWidget();
    connect(m_pTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(slotItemClicked(QTreeWidgetItem *, int)));

    ViewWidget *viewWidget = new ViewWidget();
    addWidget(tr("View"), viewWidget);
    GaugeWidget *gaugeWidget = new GaugeWidget();
    addWidget(tr("Gauge"), gaugeWidget);
    ProgressBarWidget *progressBarWidget = new ProgressBarWidget();
    addWidget(tr("ProgressBar"), progressBarWidget);

    QWidget *centerWidget = new QWidget();
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addWidget(m_pTreeWidget);
    centerLayout->addWidget(m_pStackedWidget);
    centerWidget->setLayout(centerLayout);
    setCentralWidget(centerWidget);

    m_pTreeWidget->setCurrentItem(m_pTreeWidget->invisibleRootItem()->child(0));
}

void MainWindow::addWidget(const QString &name, QWidget *w)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(m_pTreeWidget->invisibleRootItem());
    item->setText(0, name);
    m_itemMap.insert(item, w);
    m_pStackedWidget->addWidget(w);
}

void MainWindow::slotItemClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    m_pStackedWidget->setCurrentWidget(m_itemMap.value(item));
}
