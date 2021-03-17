#include "custommode.h"
#include "qcanpool/minisplitter.h"
#include "qcanpool/ministack.h"
#include "qcanpool/tinytabwidget.h"

#include "cwidgets.h"
#include "ctablewidget.h"

CustomMode::CustomMode(QWidget *parent)
    : FancyMode(parent)
{
    setObjectName(QLatin1String("CustomMode"));
    setDisplayName(tr("Custom"));
    setIcon(QIcon(":/mode/home"));

    createWindow();
}

void CustomMode::createWindow()
{
    MiniStack *stack1 = new MiniStack();
    stack1->setTitle(tr("Mini Stack"));
    stack1->setTitleMargins(5,1,5,1);

    MiniStack *stack2 = new MiniStack();
    stack2->setTitleMargins(5,1,5,1);
    stack2->addTab(tr("Tab1"), new QTextEdit("tab1"));
    stack2->addTab(tr("Tab2"), new QTextEdit("tab2"));
    stack2->addTab(tr("Tab3"), new QTextEdit("tab3"));
    stack2->addTab(tr("Tab4"), new QTextEdit("tab4"));
    stack2->setTitle(tr("Mini Stack"));

    QHBoxLayout *pStackLayout = new QHBoxLayout();
    pStackLayout->setContentsMargins(10,5,10,5);
    pStackLayout->setSpacing(10);
    pStackLayout->addWidget(stack1);
    pStackLayout->addWidget(stack2);
    /////////////////

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addLayout(pStackLayout);
    layout->addStretch();
    QWidget *w = new QWidget();
    w->setLayout(layout);

    TinyTabWidget *tabWidget1 = new TinyTabWidget();
    tabWidget1->addTab(new QWidget(), QIcon(":/mode/edit"), tr("tab2"));
    tabWidget1->addTab(new QWidget(), tr("tab3"));

    tabWidget1->setTextColor(QColor(10,10,10));
    tabWidget1->setSelectedTextColor(QColor(255,255,255));
    tabWidget1->setHoverColor(QColor(180,180,180));
    tabWidget1->setBackgroundColor(QColor(100,200,0,100));
    tabWidget1->setTabPosition(TinyTabWidget::West);

    TinyTabWidget *tabWidget2 = new TinyTabWidget();
    tabWidget2->addTab(new QWidget(), tr("tab2"));
    tabWidget2->addTab(new QWidget(), tr("tab3"));

    tabWidget2->setTextColor(QColor(10,10,10));
    tabWidget2->setSelectedTextColor(QColor(255,255,255));
    tabWidget2->setHoverColor(QColor(180,180,180));
    tabWidget2->setBackgroundColor(QColor(100,0,200,120));
    tabWidget2->setTabPosition(TinyTabWidget::North);

    TinyTabWidget *tabWidget3 = new TinyTabWidget();
    tabWidget3->addTab(new QWidget(), QIcon(":/mode/edit"), tr("tab2"));
    tabWidget3->addTab(new QWidget(), tr("tab3"));

    tabWidget3->setTextColor(QColor(10,10,10));
    tabWidget3->setSelectedTextColor(QColor(255,255,255));
    tabWidget3->setHoverColor(QColor(180,180,180));
    tabWidget3->setBackgroundColor(QColor(0,100,200,120));
    tabWidget3->setTabPosition(TinyTabWidget::South);

    TinyTabWidget *tabWidget = new TinyTabWidget();
    tabWidget->addTab(w, QIcon(":/mode/edit"), tr("tab1"));
    tabWidget->addTab(tabWidget1, tr("tab2"));
    tabWidget->addTab(tabWidget2, tr("tab3"));
    tabWidget->addTab(tabWidget3, tr("tab4"));

    tabWidget->setTextColor(QColor(10,10,10));
    tabWidget->setSelectedTextColor(QColor(255,255,255));
    tabWidget->setHoverColor(QColor(180,180,180));
    tabWidget->setBackgroundColor(QColor(0,0,0,20));
    tabWidget->setTabPosition(TinyTabWidget::East);

    setWidget(tabWidget);
}
