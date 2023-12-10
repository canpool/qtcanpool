#include "mainwindow.h"

#include <QDebug>
#include <QToolButton>
#include <QMenu>
#include <QActionGroup>
#include <QLayout>
#include <QStatusBar>
#include <QTextEdit>

#include "qcanpool/tinytabbar.h"
#include "qcanpool/tinytabwidget.h"
#include "qcanpool/tinynavbar.h"
#include "qcanpool/menuaccessbutton.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon icon(":/logo");

    TinyTabWidget *tw = new TinyTabWidget(this);
    setCentralWidget(tw);

    tw->addTab(new QTextEdit(tr("tab1"), this), icon, tr("tab1"));
    tw->addTab(new QTextEdit(tr("tab22"), this), icon, tr("tab22"));
    tw->addTab(new QTextEdit(tr("tab333"), this), icon, tr("tab333"));
    tw->addTab(new QTextEdit(tr("tab4444"), this), icon, tr("tab4444"));
    tw->setTabEnabled(1, false);
    tw->insertTab(2, new QTextEdit(tr("tabN"), this), icon, tr("tabN"));
    tw->setTabVisible(3, false);
    tw->setCurrentIndex(0);

    TinyTabBar *tb = tw->tabBar();
    tb->setTabStyle(Qt::ToolButtonTextUnderIcon);
//    tb->layout()->setSizeConstraint(QLayout::SetFixedSize);

    connect(tb, &TinyTabBar::currentChanged, this, [tb](int index) {
        qDebug() << "currentChanged:" << index << tb->isTabChecked(index);
    });

    connect(tb, &TinyTabBar::currentToggled, this, [tw](int index, bool checked) {
        qDebug() << "currentToggled:" << index << checked;
    });

//    QToolButton *button = new QToolButton(this);
//    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
//    button->setPopupMode(QToolButton::InstantPopup);

    MenuAccessButton *button = new MenuAccessButton(this);
    connect(tb, &TinyTabBar::orientationChanged, button, &MenuAccessButton::setOrientation);
    connect(tb, &TinyTabBar::toolButtonStyleChanged, button, &MenuAccessButton::setToolButtonStyle);

    tb->addWidget(button);

    QMenu *menu = new QMenu(tr("tab position"), this);
    button->setMenu(menu);
    QActionGroup *group = new QActionGroup(this);

    auto lambdaAddPositionAction = [this](QActionGroup *group, QAction *action, int position) {
        group->addAction(action);
        action->setCheckable(true);
        action->setData(QVariant(position));
    };
    lambdaAddPositionAction(group, menu->addAction(tr("North")), TinyTabWidget::North);
    lambdaAddPositionAction(group, menu->addAction(tr("South")), TinyTabWidget::South);
    lambdaAddPositionAction(group, menu->addAction(tr("West")), TinyTabWidget::West);
    lambdaAddPositionAction(group, menu->addAction(tr("East")), TinyTabWidget::East);
    connect(group, &QActionGroup::triggered, this, [this, tw](QAction *action) {
        tw->setTabPosition(static_cast<TinyTabWidget::TabPosition>(action->data().toInt()));
    });
    emit group->actions().at(2)->trigger();

    tw->addButton(icon, tr("button1"));
    tw->addButton(icon, tr("button2"));
    tw->addButton(icon, tr("button3"));

    // TinyNavBar
    TinyNavBar *nb = new TinyNavBar(this);
    nb->layout()->setSpacing(2);
    nb->addTab(tr("nav1"));
    nb->addTab(tr("nav2"));
    nb->addTab(tr("nav3"));
    nb->addTab(tr("nav4"));
    nb->addTab(tr("nav5"));
    nb->removeTab(3);
    nb->insertTab(2, tr("navN"));
    nb->setTabVisible(3, false);
//    nb->layout()->setSizeConstraint(QLayout::SetFixedSize);

    QStatusBar *sb = statusBar();
    sb->addWidget(nb);

    setWindowTitle(tr("TinyTabWidget Example"));
    setWindowIcon(icon);
    resize(400, 200);
}

MainWindow::~MainWindow()
{
}
