#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <QStackedWidget>
#include <QTableWidget>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>

#include <QDebug>

#include "qlite/qlite.h"
#include "qlite/litemodebar.h"
#include "qlite/litetabwidget.h"
#include "qlite/litetabbar.h"
#include "qlite/litedialog.h"
#include "qlite/litebar.h"

using namespace QLite;

MainWindow::MainWindow(QWidget *parent)
    : LiteWindow(parent)
{
    m_pStackedWidget = new QStackedWidget(this);
    m_pModeBar = new LiteModeBar(m_pStackedWidget, this);

    m_pTabWidget = new LiteTabWidget(this);
    m_pTabBar = m_pTabWidget->tabBar();

    createWindow();

//    QString styleData = QLite::loadStyle(QString(":/qss/flatdark"));
//    QLite::setStyle(styleData);

    setWindowIcon(QIcon(":/main/logo"));
    setWindowTitle(tr("qlite window demo"));
//    showMaximized();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createWindow()
{
    QTableWidget *w = new QTableWidget(this);
    m_pTabWidget->addTab(w, QIcon(":/main/logo"), tr("tab"));

    QAction *action = new QAction(QIcon(":/main/logo"), tr("testAction"));
    m_pTabBar->addAction(action);
//    m_pTabBar->setActionStyle(action, Qt::ToolButtonTextBesideIcon);

    m_pModeBar->addMode(m_pTabWidget, QIcon(":/main/logo"), tr("LiteTabWidget"));

    m_pTabWidget->setTabPosition(LiteTabWidget::East);
//    m_pTabWidget->setTabPosition(LiteTabWidget::North);
//    m_pTabWidget->setTabPosition(LiteTabWidget::South);
//    m_pTabWidget->setTabPosition(LiteTabWidget::West);
//    m_pTabWidget->setTabEnabled(widgets.at(0), false);
//    m_pTabWidget->setTabVisible(widgets.at(0), false);


    QList<QTableWidget *> widgets;
    for (int i = 0; i < 2; ++i) {
        QTableWidget *w = new QTableWidget(this);
        widgets.append(w);
        m_pModeBar->addMode(w, QIcon(":/main/logo"), tr("tab %1").arg(i));
    }

    for (int i = 0; i < 2; i++) {
        QAction *action = new QAction(QIcon(":/main/logo"), tr("action %1").arg(i));
        m_pModeBar->addAction(action);
    }
    action = new QAction(QIcon(":/main/logo"), tr("LiteDialog"));
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(slotLiteDialog()));
    m_pModeBar->addAction(action, LiteTabBar::Back);

    m_pModeBar->setModeEnabled(widgets.at(1), false);
    m_pModeBar->setTabSpacing(0);

    QWidget *pCenterWidget = new QWidget();

//    QVBoxLayout *centerLayout = new QVBoxLayout();
//    m_pModeBar->setDirection(LiteTabBar::Horizontal);
    QHBoxLayout *centerLayout = new QHBoxLayout();
    m_pModeBar->setDirection(LiteTabBar::Vertical);

    centerLayout->setMargin(0);
    centerLayout->setSpacing(0);
    centerLayout->addWidget(m_pModeBar);
    centerLayout->addWidget(m_pStackedWidget);
    pCenterWidget->setLayout(centerLayout);

    setCentralWidget(pCenterWidget);
//    setMenuWidget(m_pTabBar);
//    m_pTabWidget->setTabButtonStyle(Qt::ToolButtonTextBesideIcon);

    QMenuBar *pMenuBar = menuBar();
    QMenu *menu = pMenuBar->addMenu(tr("File"));
    menu->addAction(tr("test"));
    menu = pMenuBar->addMenu(tr("Edit"));
    menu->addAction(tr("test"));
    menu = pMenuBar->addMenu(tr("Tools"));
    menu->addAction(tr("test"));
    menu = pMenuBar->addMenu(tr("Help"));
    menu->addAction(tr("test"));
    statusBar();

    action = new QAction(QIcon(":/main/skin"), tr("skin"));
    menu = new QMenu(tr("skin"));
    action->setMenu(menu);
    liteBar()->addAction(action);
    m_pActionGroup = new QActionGroup(this);
    addSkinItem(menu->addAction(tr("flatdark")), QString(":/qss/flatdark"));
    addSkinItem(menu->addAction(tr("blue")), QString(":/qss/blue"));
    addSkinItem(menu->addAction(tr("green")), QString(":/qss/green"));
    addSkinItem(menu->addAction(tr("darkgreen")), QString(":/qss/darkgreen"));
    addSkinItem(menu->addAction(tr("red")), QString(":/qss/red"));
    emit m_pActionGroup->actions().at(0)->trigger();
}

void MainWindow::addSkinItem(QAction *action, const QString &qss)
{
    m_pActionGroup->addAction(action);
    action->setCheckable(true);
    action->setData(QVariant(qss));
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(slotChangeSkin()));
}

void MainWindow::slotLiteDialog()
{
    LiteDialog dlg;
//    dlg.setFixedSize(400, 200);
    dlg.exec();
}

void MainWindow::slotChangeSkin()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QLite::setStyle(action->data().toString());
    }
}

