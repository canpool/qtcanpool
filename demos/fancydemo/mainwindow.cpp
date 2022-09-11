#include "mainwindow.h"

#include <QLabel>
#include <QIcon>
#include <QActionGroup>
#include <QMenuBar>
#include <QMenu>
#include <QLineEdit>
#include <QTextEdit>

#include <QApplication>
#include <QFile>

#include "qcanpool/fancybar.h"
#include "qcanpool/fancydialog.h"
#include "qcanpool/fancytitlebar.h"
#include "qcanpool/fancytabwidget.h"
#include "qcanpool/fancytabbar.h"
#include "qcanpool/fancytoolbutton.h"
#include "qcanpool/quickaccessbar.h"

MainWindow::MainWindow(QWidget *parent)
    : FancyWindow(parent)
    , m_themeStyle(QString(":/qss/flatdark"))
{
    setWindowIcon(QIcon(":/main/logo"));
    setWindowTitle(tr("Qt canpool fancy demo V2"));
    setMinimumSize(QSize(600, 400));
    setMouseTracking(true);

    m_pTabWidget = new FancyTabWidget();

    createWindow();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createWindow()
{
    createQuickAccessBar();
    createMenuBar();
    createSystemMenu();
    createCentralWidget();
}

void MainWindow::createCentralWidget()
{
    FancyTabBar *tabBar = m_pTabWidget->tabBar();

    QLabel *label = new QLabel("I CAN DO IT");
    // ignore the label's control to continue processEvent
    label->setTextInteractionFlags(Qt::NoTextInteraction);
    label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_pTabWidget->addTab(label, QIcon(":/main/logo"), tr("tab1"));

    QTextEdit *text = new QTextEdit(this);
    m_pTabWidget->addTab(text, QIcon(":/main/logo"), tr("tab2"));

    QAction *action = new QAction(QIcon(":/main/logo"), tr("testAction1"));
    tabBar->addAction(action, FancyTabBar::Middle);
    tabBar->setActionIconOnly(action, false);

    FancyToolButton *button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setMenuArea(FancyToolButton::RightMenuArea);
    button->setMenuArrowType(Qt::RightArrow);
    action = new QAction(QIcon(":/main/logo"), tr("testMenu"));
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    action->setMenu(menu);
    button->setDefaultAction(action);
    tabBar->addActionButton(button, FancyTabBar::Middle);

    tabBar->addAction(QIcon(":/tools/start"), tr("start"));
    tabBar->addAction(QIcon(":/tools/suspend"), tr("suspend"));
    action = tabBar->addAction(QIcon(":/tools/stop"), tr("stop"));
    tabBar->setActionIconOnly(action, true);

    m_pTabWidget->setTabPosition(FancyTabWidget::West);

    setCentralWidget(m_pTabWidget);
}

void MainWindow::createQuickAccessBar()
{
    if (QuickAccessBar *quickAccessBar = fancyBar()->quickAccessBar())
    {
        QAction *action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        action = quickAccessBar->addAction(QIcon(":/tools/new"), tr("New"));
        action->setShortcut(QKeySequence::New);
        action->setToolTip(tr("New File or Project\nCtrl+N"));
        connect(action, SIGNAL(triggered()), this, SLOT(slotNew()));

        action = quickAccessBar->addAction(QIcon(":/tools/open"), tr("Open"));
        action->setShortcut(tr("Ctrl+O"));
        action->setToolTip(tr("Open File or Project\nCtrl+O"));
        quickAccessBar->setActionVisible(action, false);

        QMenu *menu = new QMenu(this);
        menu->addAction(tr("action1"));
        menu->addAction(tr("action2"));
        menu->addAction(tr("action3"));
        menu->addAction(tr("action4"));
        action->setMenu(menu);

        action = quickAccessBar->addAction(QIcon(":/tools/save"), tr("Save"));
        action->setShortcut(tr("Ctrl+S"));
        action->setToolTip(tr("Save the active document\nCtrl+S"));

        action = quickAccessBar->addAction(QIcon(":/tools/save_all"), tr("Save All"));
        action->setShortcut(tr("Ctrl+Shift+S"));
        action->setToolTip(tr("Save all active documents\nCtrl+Shift+S"));

        action = quickAccessBar->addAction(QIcon(":/tools/undo"), tr("&Undo"));
        action->setShortcut(QKeySequence::Undo);
        action->setEnabled(false);

        action = quickAccessBar->addAction(QIcon(":/tools/redo"), tr("&Redo"));
        action->setShortcut(QKeySequence::Redo);
        action->setEnabled(false);

        fancyBar()->showQuickAccess(true);
    }
}

void MainWindow::createMenuBar()
{
    if (QMenuBar *menuBar = fancyBar()->menuBar())
    {
        QAction *action = new QAction(tr("action1"), this);
        QMenu *menu = menuBar->addMenu(tr("&File"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&Edit"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&Debug"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&View"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&Window"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&Tools"));
        menu->addAction(action);
        action = new QAction(tr("action1"), this);
        menu = menuBar->addMenu(tr("&Help"));
        menu->addAction(action);

        fancyBar()->showMenuBar(true);
    }

    QToolBar *toolBar = fancyBar()->toolBar();
    toolBar->addAction(QIcon(":/tools/new"), tr("new"));
    toolBar->addAction(QIcon(":/tools/open"), tr("open"));
    toolBar->addAction(QIcon(":/tools/save"), tr("save"));
}

void MainWindow::createSystemMenu()
{
    QAction *mainAction = new QAction(QIcon(":/tools/menu"), tr("menu"));
    QMenu *mainMenu = new QMenu(mainAction->text());
    mainAction->setMenu(mainMenu);
    fancyBar()->titleBar()->addAction(mainAction);

    QMenu *menu = new QMenu(tr("theme style"));
    mainMenu->addMenu(menu);
    QActionGroup *group = new QActionGroup(this);
    addThemeStyleItem(group, menu->addAction(tr("flatdark")), QString(":/qss/flatdark"));
    addThemeStyleItem(group, menu->addAction(tr("classic")), QString(":/qss/classic"));
    emit group->actions().at(0)->trigger();

    menu = new QMenu(tr("window style"));
    mainMenu->addMenu(menu);
    group = new QActionGroup(this);
    addWindowStyleItem(group, menu->addAction(tr("WindowStyle")), FancyBar::WindowStyle);
    addWindowStyleItem(group, menu->addAction(tr("MergedStyle")), FancyBar::MergedStyle);
    emit group->actions().at(0)->trigger();

    menu = new QMenu(tr("tab position"));
    mainMenu->addMenu(menu);
    group = new QActionGroup(this);
    addTabPositionItem(group, menu->addAction(tr("North")), FancyTabWidget::North);
    addTabPositionItem(group, menu->addAction(tr("South")), FancyTabWidget::South);
    addTabPositionItem(group, menu->addAction(tr("West")), FancyTabWidget::West);
    addTabPositionItem(group, menu->addAction(tr("East")), FancyTabWidget::East);
    emit group->actions().at(2)->trigger();
}

void MainWindow::addThemeStyleItem(QActionGroup *group, QAction *action, const QString &qss)
{
    group->addAction(action);
    action->setCheckable(true);
    action->setData(QVariant(qss));
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(slotChangeThemeStyle()));
}

void MainWindow::addWindowStyleItem(QActionGroup *group, QAction *action, int style)
{
    group->addAction(action);
    action->setCheckable(true);
    action->setData(QVariant(style));
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(slotChangeWindowStyle()));
}

void MainWindow::addTabPositionItem(QActionGroup *group, QAction *action, int position)
{
    group->addAction(action);
    action->setCheckable(true);
    action->setData(QVariant(position));
    QObject::connect(action, SIGNAL(triggered()), this, SLOT(slotSetTabPosition()));
}

void MainWindow::setThemeStyle(const QString &style)
{
    QFile file(style);
    if (file.open(QFile::ReadOnly)) {
        QString qss = QLatin1String(file.readAll());
        qApp->setStyleSheet(qss);
        file.close();
    } else {
        qApp->setStyleSheet(style);
    }
}

void MainWindow::slotNew()
{
    FancyDialog dialog;
    dialog.setWindowIcon(QIcon(":/main/logo"));
    dialog.setWindowTitle(tr("Qt canpool fancy dialog"));
    dialog.setTitleBarHeight(30);
    dialog.addAction(QIcon(":/tools/new"), tr("new"));
    dialog.addAction(QIcon(":/tools/open"), tr("open"));
    dialog.addAction(QIcon(":/tools/save"), tr("save"));
    dialog.exec();
}

void MainWindow::slotChangeThemeStyle()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_themeStyle = action->data().toString();
        setThemeStyle(m_themeStyle);
    }
}

void MainWindow::slotChangeWindowStyle()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        fancyBar()->setFancyStyle(static_cast<FancyBar::FancyStyle>(action->data().toInt()));
    }
}

void MainWindow::slotSetTabPosition()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        m_pTabWidget->setTabPosition(static_cast<FancyTabWidget::TabPosition>(action->data().toInt()));
        setThemeStyle(m_themeStyle);
    }
}
