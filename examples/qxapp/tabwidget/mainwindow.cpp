#include "mainwindow.h"

#include "qxapp/qxtabwidget.h"
#include "qxapp/qxtabbar.h"

#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>

QX_APP_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(640, 420);
    setWindowTitle(tr("QxTabWidget / QxTabBar"));

    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);

    auto *tabs = new QxTabWidget(this);
    tabs->addTab(new QLabel(tr("First page"), this), tr("First"));
    tabs->addTab(new QLabel(tr("Second page"), this), tr("Second"));
    tabs->addTab(new QLabel(tr("Third page"), this), QIcon(), tr("Third"));
    root->addWidget(tabs);

    // Cycle the tab position to show the four orientations.
    auto *posButton = new QPushButton(tr("Cycle tab position"), this);
    root->addWidget(posButton);
    connect(posButton, &QPushButton::clicked, this, [tabs]() {
        const QxTabWidget::TabPosition next = static_cast<QxTabWidget::TabPosition>((tabs->tabPosition() + 1) % 4);
        tabs->setTabPosition(next);
    });

    // A standalone, togglable tab bar demonstrating the currentToggled signal.
    auto *bar = new QxTabBar(this);
    bar->addTab(tr("A"));
    bar->addTab(tr("B"));
    bar->addTab(tr("C"));
    bar->setTogglable(true);
    root->addWidget(bar);
    connect(bar, &QxTabBar::currentToggled, this, [this](int index, bool checked) {
        statusBar()->showMessage(tr("Tab %1 %2").arg(index).arg(checked ? tr("checked") : tr("unchecked")));
    });

    setCentralWidget(central);
}

MainWindow::~MainWindow() = default;
