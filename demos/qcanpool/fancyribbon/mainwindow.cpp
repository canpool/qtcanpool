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
#include <QToolBar>
#include <QRadioButton>
#include <QCheckBox>
#include <QActionGroup>
#include <QComboBox>

#include "qcanpool/ribbonbar.h"
#include "qcanpool/ribbonpage.h"
#include "qcanpool/ribbongroup.h"
#include "qcanpool/ribboncontainers.h"
#include "qcanpool/quickaccessbar.h"
#include "qcanpool/fancytitlebar.h"
#include "qcanpool/fancytoolbutton.h"

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/main/logo");
    RibbonBar *ribbonBar = this->ribbonBar();

    QuickAccessBar *quickAccessBar = ribbonBar->quickAccessBar();
    quickAccessBar->addAction(icon, tr("action 1"));
    quickAccessBar->addAction(icon, tr("action 2"));

    QToolBar *ribbonToolBar = ribbonBar->toolBar(RibbonBar::ToolBarTopPosition);
    ribbonToolBar->addAction(icon, tr("top btn 1"));
    ribbonToolBar->addAction(icon, tr("top btn 2"));
    ribbonToolBar = ribbonBar->toolBar(RibbonBar::ToolBarBottomPosition);
    ribbonToolBar->addAction(icon, tr("bottom btn 1"));
    ribbonToolBar->addAction(icon, tr("bottom btn 2"));

    QAction *action = nullptr;
    QMenu *menu = nullptr;
    QToolBar *toolBar = nullptr;

    menu = new QMenu();
    menu->addAction(icon, tr("test"));
    menu->addAction(icon, tr("test"));

    action = new QAction(icon, tr("test"));
    ribbonBar->addAction(action);

    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    ribbonBar->addAction(action);

    // Inner Container QToolButton
    RibbonPage *page = ribbonBar->addPage(tr("Home"));
    RibbonGroup *group = page->addGroup(tr("InnerContainer(QToolButtons)"));
    group->addAction(icon, tr("testgroup"), RibbonGroup::GroupLarge);
    group->addAction(icon, tr("test\ngroup"), RibbonGroup::GroupLarge); // too long
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    group->addAction(action, RibbonGroup::GroupLarge);
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    // add action that has menu
    group->addAction(action, RibbonGroup::GroupMedium);
    group->addAction(icon, tr("test123"), RibbonGroup::GroupMedium);
    group->addAction(icon, tr("test12345"), RibbonGroup::GroupMedium);
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    group->addAction(action, RibbonGroup::GroupSmall);
    group->addAction(icon, tr("test123"), RibbonGroup::GroupSmall);
    group->addAction(icon, tr("test12345"), RibbonGroup::GroupSmall);
    group->setOptionButtonVisible();

    /* fancy toolbutton */
    group = page->addGroup(tr("InnerContainer(FancyToolButtons)"));
    FancyToolButton *button = new FancyToolButton(icon, tr("test button"));
    button->setPopupMode(QToolButton::DelayedPopup);
    button->setMenu(menu);
    group->addToolButton(button, RibbonGroup::GroupLarge);
    button = new FancyToolButton(icon, tr("test"));
    button->setPopupMode(QToolButton::MenuButtonPopup);
    button->setMenu(menu);
    group->addToolButton(button, RibbonGroup::GroupLarge);
    button = new FancyToolButton(icon, tr("test\nbutton"));
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    group->addToolButton(button, RibbonGroup::GroupLarge);
    button = new FancyToolButton(icon, tr("test"));
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    group->addToolButton(button, RibbonGroup::GroupMedium);
    button = new FancyToolButton(icon, tr("test123"));
    group->addToolButton(button, RibbonGroup::GroupMedium);
    button = new FancyToolButton(icon, tr("test12345"));
    group->addToolButton(button, RibbonGroup::GroupMedium);
    button = new FancyToolButton(icon, tr("test"));
    button->setPopupMode(QToolButton::InstantPopup);
    button->setMenu(menu);
    button->setForceAlignCenter(false);   // disable align center for small
    group->addToolButton(button, RibbonGroup::GroupSmall); // using QToolButton is appropriate
    button = new FancyToolButton(icon, tr("test123"));
    group->addToolButton(button, RibbonGroup::GroupSmall);
    button = new FancyToolButton(icon, tr("test12345"));
    group->addToolButton(button, RibbonGroup::GroupSmall);

    /* custom containers */
    page = ribbonBar->addPage(tr("Custom"));
    group = page->addGroup(tr("GridContainer(Radios & Checks)"));
    RibbonGridContainer *gridContainer = new RibbonGridContainer(this);
    gridContainer->addWidget(new QRadioButton(tr("Radio 1")));
    gridContainer->addWidget(new QRadioButton(tr("Radio 2")));
    gridContainer->addWidget(new QRadioButton(tr("Radio 3")));
    gridContainer->addWidget(new QRadioButton(tr("Radio 4")));
    gridContainer->addWidget(new QRadioButton(tr("Radio 5")));
    gridContainer->addWidget(new QCheckBox(tr("Check 1")));
    gridContainer->addWidget(new QCheckBox(tr("Check 2")));
    gridContainer->addWidget(new QCheckBox(tr("Check 3")));
    gridContainer->addWidget(new QCheckBox(tr("Check 4")));
    group->addWidget(gridContainer);

    group = page->addGroup(tr("GridContainer(ToolButtons)"));
    gridContainer = new RibbonGridContainer(this);
    QToolButton *btn = createToolButton(icon, tr("test 1"));
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    gridContainer->addWidget(btn);
    btn = createToolButton(icon, tr("test 2"));
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    gridContainer->addWidget(btn);
    btn = createToolButton(icon, tr("test 3"));
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    gridContainer->addWidget(btn);
    gridContainer->addWidget(createToolButton(icon, tr("test 4")));
    gridContainer->addWidget(createToolButton(icon, tr("test 5")));
    gridContainer->addWidget(createToolButton(icon, tr("test 6")));
    gridContainer->addWidget(createToolButton(icon, tr("test 7")));
    gridContainer->addWidget(createToolButton(icon, tr("test 8")));
    gridContainer->addWidget(createToolButton(icon, tr("test 9")));
    gridContainer->addWidget(createToolButton(icon, tr("test 10")));
    gridContainer->addWidget(createToolButton(icon, tr("test 11")));
    group->addWidget(gridContainer);

    group = page->addGroup(tr("ActionContainer"));
    RibbonActionContainer *actionContainer = new RibbonActionContainer(this);
    actionContainer->addAction(icon, tr("testgroup"), RibbonGroup::GroupLarge);
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    actionContainer->addAction(action, RibbonGroup::GroupLarge);
    actionContainer->addAction(icon, tr("test\ngroup"), RibbonGroup::GroupLarge); // too long
    actionContainer->addAction(icon, tr("test"), RibbonGroup::GroupMedium);
    actionContainer->addAction(icon, tr("test123"), RibbonGroup::GroupMedium);
    actionContainer->addAction(icon, tr("test12345"), RibbonGroup::GroupMedium);
    actionContainer->addAction(icon, tr("test 1"), RibbonGroup::GroupMedium);
    actionContainer->addAction(icon, tr("test 2"), RibbonGroup::GroupMedium);
    actionContainer->addAction(icon, tr("test 1"), RibbonGroup::GroupSmall);
    actionContainer->addAction(icon, tr("test 2"), RibbonGroup::GroupSmall);
    actionContainer->addAction(icon, tr("test 3"), RibbonGroup::GroupSmall);
    actionContainer->addAction(icon, tr("test 4"), RibbonGroup::GroupSmall);
    actionContainer->addAction(icon, tr("test 5"), RibbonGroup::GroupSmall);
    group->addWidget(actionContainer);

    // loft container
    group = page->addGroup(tr("LoftContainer"));
    RibbonLoftContainer *loftContainer = new RibbonLoftContainer(this);
    toolBar = loftContainer->toolBar(RibbonLoftContainer::Top);
    QComboBox *cb = new QComboBox(this);
    cb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cb->addItems({ "Arial", "Helvetica", "Times" });
    toolBar->addWidget(cb);
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    toolBar->addAction(action);
    toolBar = loftContainer->toolBar(RibbonLoftContainer::Bottom);
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    toolBar->addSeparator();
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    group->addWidget(loftContainer);

    // other
    group = page->addGroup(tr("Defines"));
    toolBar = new QToolBar("bar");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    action = new QAction(icon, tr("test"));
    action->setMenu(menu);
    toolBar->addAction(action);
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    toolBar->addAction(icon, tr("test"));
    group->addWidget(toolBar);

    ribbonBar->addPage(tr("Help"));

    /* options */
    action = new QAction(icon, tr("options"));
    menu = new QMenu(tr("options"));
    action->setMenu(menu);
    ribbonToolBar->addAction(action);

    action = new QAction(tr("Frameless"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(slotSetFrameless(bool)));
    menu->addAction(action);

    menu->addSeparator();

    QActionGroup *actionGroup = new QActionGroup(action);
    QAction *styleAction = menu->addAction(tr("Classic Style"));
    styleAction->setCheckable(true);
    styleAction->setData(QVariant(RibbonBar::ClassicStyle));
    actionGroup->addAction(styleAction);
    styleAction = menu->addAction(tr("Merged Style"));
    styleAction->setCheckable(true);
    styleAction->setData(QVariant(RibbonBar::MergedStyle));
    actionGroup->addAction(styleAction);
    connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotChangeStyle(QAction *)));
    emit actionGroup->actions().at(1)->trigger();


    setWindowIcon(icon);
    setWindowTitle(tr("fancy ribbon demo"));

    QWidget *w = new QWidget();
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(100, 100, 0));
    w->setPalette(palette);
    w->setAutoFillBackground(true);
    setCentralWidget(w);

    resize(800, 300);
}

MainWindow::~MainWindow()
{

}

QToolButton *MainWindow::createToolButton(const QIcon &icon, const QString &text)
{
    QToolButton *button = new QToolButton();
    button->setToolButtonStyle(Qt::ToolButtonIconOnly);
    button->setAutoRaise(true);
    button->setIconSize(QSize(18, 18));
    button->setText(text);
    button->setIcon(icon);
    button->setToolTip(text);
    return button;
}

void MainWindow::slotChangeStyle(QAction *action)
{
    ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(action->data().toUInt()));
}

void MainWindow::slotSetFrameless(bool frameless)
{
    setFrameless(frameless);
    show();
}
