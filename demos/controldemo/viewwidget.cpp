#include "viewwidget.h"
#include "QNavListView.h"
#include "QNavTreeView.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QVBoxLayout>

ViewWidget::ViewWidget(QWidget *parent)
    : QWidget(parent)
{
    createWindow();
}

void ViewWidget::createWindow()
{
    m_pTabWidget = new QTabWidget();

    createNavListView();
    createNavTreeView();

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_pTabWidget);
    setLayout(mainLayout);
}

void ViewWidget::createNavListView()
{
    QNavListView *listView1 = new QNavListView(this);

    QNavListView *listView2 = new QNavListView(this);
    listView2->setIconBgcVisible(false);
    listView2->setLineColor(QColor(255, 255, 255));
    listView2->setBackColor(QColor(238, 238, 238), QColor(255, 69, 0), QColor(255, 160, 122));
    listView2->setTextColor(QColor(10, 10, 10), QColor(254, 255, 239), QColor(10, 10, 10));

    QNavListView *listView3 = new QNavListView(this);
    listView3->setIconBgcVisible(false);
    listView3->setLineColor(QColor(32, 53, 74));
    listView3->setBackColor(QColor(52, 73, 94), QColor(24, 189, 155), QColor(24, 189, 155, 150));
    listView3->setTextColor(QColor(254, 255, 255), QColor(252, 252, 252), QColor(0, 0, 0));

    listView1->setInfoVisible(false);
    listView2->setInfoVisible(false);
    listView3->setInfoVisible(true);

    listView3->setLineVisible(false);
    listView3->setIconStyle(QNavListView::Triangle);

    listView1->readData(":/view/config.xml");
    listView2->readData(":/view/config.xml");
    listView3->readData(":/view/config.xml");

    QWidget *w = new QWidget();
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->addWidget(listView1);
    hboxLayout->addWidget(listView2);
    hboxLayout->addWidget(listView3);
    w->setLayout(hboxLayout);

    m_pTabWidget->addTab(w, tr("ListView"));
}

void ViewWidget::createNavTreeView()
{
    QNavTreeView *treeView = new QNavTreeView(this);

    QString parentText(tr("qtcanpool"));
    treeView->addItem(parentText);
    treeView->setItemInfo(parentText, tr("Qt"));
    parentText = QString(tr("demos"));
    treeView->addItem(parentText);
    treeView->addItem(parentText, tr("controldemo"));
    treeView->addItem(parentText, tr("fancydemo"));
    treeView->addItem(parentText, tr("fancyribbon"));
    treeView->addItem(parentText, tr("fancytitlebar"));
    treeView->addItem(parentText, tr("helloworld"));
    treeView->addItem(parentText, tr("plotdemo"));
    treeView->addItem(parentText, tr("qrcodedemo"));
    treeView->addItem(parentText, tr("ribbondemo"));
    parentText = QString(tr("doc"));
    treeView->addItem(parentText);
    treeView->addItem(parentText, tr("pics"));
    parentText = QString(tr("projects"));
    treeView->addItem(parentText);
    treeView->addItem(parentText, tr("qtcreator"));
    treeView->addItem(parentText, tr("template"));
    treeView->addItem(parentText, tr("template2"));
    parentText = QString(tr("src"));
    treeView->addItem(parentText);
    treeView->addItem(parentText, tr("libs"));
    treeView->addItem(parentText, tr("modules"));
    treeView->addItem(parentText, tr("plugins"));
    treeView->addItem(parentText, tr("shared"));

    QWidget *w = new QWidget();
    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->addWidget(treeView);
    w->setLayout(hboxLayout);

    m_pTabWidget->addTab(w, tr("TreeView"));
}
