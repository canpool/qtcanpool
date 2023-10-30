#include "mainwindow.h"

#include <QDebug>
#include <QToolButton>
#include <QMenu>
#include <QActionGroup>
#include <QLayout>
#include <QTextEdit>
#include <QMenuBar>

#include "qxwidget/minitabbar.h"
#include "qxwidget/minitabwidget.h"

QX_WIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    MiniTabWidget *tw = new MiniTabWidget(this);
    setCentralWidget(tw);

    tw->addTab(new QTextEdit(tr("tab1"), this), icon, tr("tab1"));
    tw->addTab(new QTextEdit(tr("tab22"), this), icon, tr("tab22"));
    tw->addTab(new QTextEdit(tr("tab333"), this), icon, tr("tab333"));
    tw->addTab(new QTextEdit(tr("tab4444"), this), icon, tr("tab4444"));
    tw->setTabEnabled(1, false);
    tw->insertTab(2, new QTextEdit(tr("tabN"), this), icon, tr("tabN"));
    tw->setTabVisible(3, false);
    tw->setCurrentIndex(0);

    QMenuBar *mb = menuBar();
    QMenu *menu = new QMenu(tr("tab position"), this);
    mb->addMenu(menu);
    QActionGroup *group = new QActionGroup(this);

    auto lambdaAddPositionAction = [this](QActionGroup *group, QAction *action, int position) {
        group->addAction(action);
        action->setCheckable(true);
        action->setData(QVariant(position));
    };
    lambdaAddPositionAction(group, menu->addAction(tr("North")), QTabWidget::North);
    lambdaAddPositionAction(group, menu->addAction(tr("South")), QTabWidget::South);
    lambdaAddPositionAction(group, menu->addAction(tr("West")), QTabWidget::West);
    lambdaAddPositionAction(group, menu->addAction(tr("East")), QTabWidget::East);
    connect(group, &QActionGroup::triggered, this, [this, tw](QAction *action) {
        tw->setTabPosition(static_cast<QTabWidget::TabPosition>(action->data().toInt()));
    });
    emit group->actions().at(2)->trigger();


    setWindowTitle(tr("MiniTabWidget Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}
