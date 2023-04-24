#include "mainwindow.h"
#include "qcanpool/fancytoolbutton.h"

#include <QToolBar>
#include <QAction>
#include <QMenu>
#include <QLayout>

#include <QDebug>

#include <QFile>
#include <QApplication>

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QToolBar *toolBar = new QToolBar();
    toolBar->setIconSize(QSize(32, 32));
//    toolBar->layout()->setSizeConstraint(QLayout::SetFixedSize);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QMenu *menu = nullptr;
    FancyToolButton *button = nullptr;
    QAction *action = nullptr;

    // QToolButton and FancyToolButton
    QToolButton *btn = new QToolButton();
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setIcon(QIcon(":/logo"));
    btn->setText("test");
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    btn->setMenu(menu);
    btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    toolBar->addWidget(btn);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    action = new QAction(QIcon(":/logo"), "test");
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    action->setMenu(menu);
    button->setDefaultAction(action);
    toolBar->addWidget(button);

    toolBar->addSeparator();

    // FancyToolButton ToolButtonTextUnderIcon hasMenu
    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test button");
    button->setPopupMode(QToolButton::DelayedPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test\nbutton");
    button->setPopupMode(QToolButton::InstantPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenuArea(FancyToolButton::RightMenuArea);
    button->setMenuArrowType(Qt::RightArrow);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    toolBar->addSeparator();

    // FancyToolButton ToolButtonTextUnderIcon hasNotMenu
    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setIcon(QIcon(":/logo"));
    button->setText("test\nbutton");
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setPopupMode(QToolButton::InstantPopup);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setIcon(QIcon(":/logo"));
    button->setText("test button");
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setMenuArea(FancyToolButton::RightMenuArea);
    button->setIcon(QIcon(":/logo"));
    button->setText("testbutton");
    toolBar->addWidget(button);

    toolBar->addSeparator();

    // FancyToolButton ToolButtonTextOnly hasMenu
    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenuArrowType(Qt::RightArrow);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    toolBar->addSeparator();

    // FancyToolButton ToolButtonIconOnly hasMenu
    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenuArrowType(Qt::RightArrow);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setArrowType(Qt::DownArrow);
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->addWidget(button);

    toolBar->addSeparator();

    // FancyToolButton ToolButtonTextBesideIcon hasMenu
    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    button = new FancyToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIcon(QIcon(":/logo"));
    button->setText("test");
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenuArea(FancyToolButton::RightMenuArea);
    menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    button->setMenu(menu);
    toolBar->addWidget(button);

    addToolBar(toolBar);

    resize(1100, 130);
}

MainWindow::~MainWindow()
{
}

