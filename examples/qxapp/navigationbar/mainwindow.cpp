#include "mainwindow.h"

#include "qxapp/qxnavigationbar.h"
#include "qxapp/qxnavbar.h"

#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QStackedWidget>
#include <QVBoxLayout>

QX_APP_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(720, 480);
    setWindowTitle(tr("QxNavigationBar / QxNavBar"));

    auto *central = new QWidget(this);
    auto *root = new QHBoxLayout(central);

    auto *nav = new QxNavigationBar(this);
    nav->addItem(QIcon(), tr("Home"));
    nav->addItem(QIcon(), tr("Editor"));
    nav->addItem(QIcon(), tr("Help"));
    root->addWidget(nav);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(new QLabel(tr("Home page"), this));
    m_stack->addWidget(new QLabel(tr("Editor page"), this));
    m_stack->addWidget(new QLabel(tr("Help page"), this));
    root->addWidget(m_stack);
    root->setStretch(1, 1);

    setCentralWidget(central);

    connect(nav, &QxNavigationBar::currentChanged, this, &MainWindow::onNavCurrentChanged);
    nav->setCurrentIndex(0);

    // QxNavBar: a toolbar-style navigation bar whose customize menu is fed by
    // the application through the showCustomizeMenu signal.
    auto *bar = new QxNavBar(this);
    bar->addTab(tr("One"));
    bar->addTab(tr("Two"));
    bar->addTab(tr("Three"));
    addToolBar(bar);
    connect(bar, &QxNavBar::showCustomizeMenu, this, [](QMenu *menu) {
        menu->addAction(tr("Reset layout"));
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::onNavCurrentChanged(int index)
{
    if (index >= 0 && index < m_stack->count())
        m_stack->setCurrentIndex(index);
}
