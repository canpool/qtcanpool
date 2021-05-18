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

#include "qcanpool/fancyribbon.h"
#include "qcanpool/fancyribbonpage.h"
#include "qcanpool/fancyribbongroup.h"
#include "qcanpool/fancyribboncontainers.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ribbon = new FancyRibbon(this);

    m_ribbon->setBackgroundColor(QColor(0, 100, 100));
    m_ribbon->setHoverColor(QColor(0, 255, 255));
    m_ribbon->setPressColor(QColor(240, 240, 240));
    m_ribbon->setSelectedTextColor(QColor(0, 0, 0));

    QAction *action = nullptr;
    QMenu *menu = nullptr;

    action = new QAction(QIcon(":/main/logo"), tr("test"));
    m_ribbon->addAction(action);

    action = new QAction(QIcon(":/main/logo"), tr("test"));
    menu = new QMenu();
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    action->setMenu(menu);
    m_ribbon->addAction(action);

    FancyRibbonPage *page = m_ribbon->addPage(tr("Home"));
    FancyRibbonGroup *group = page->addGroup(tr("Font"));
    group->addAction(QIcon(":/main/logo"), tr("testgroup"), FancyRibbonGroup::GroupLarge);
    action = new QAction(QIcon(":/main/logo"), tr("test"));
    menu = new QMenu();
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    action->setMenu(menu);
    group->addAction(action, FancyRibbonGroup::GroupLarge);
    group->addAction(QIcon(":/main/logo"), tr("test"), FancyRibbonGroup::GroupSmall);
    group->addAction(QIcon(":/main/logo"), tr("test"), FancyRibbonGroup::GroupSmall);
    group->addAction(QIcon(":/main/logo"), tr("test"), FancyRibbonGroup::GroupSmall);

    group = page->addGroup(tr("Clipboard"));
    group->addAction(QIcon(":/main/logo"), tr("test\ngroup"), FancyRibbonGroup::GroupLarge); // too long
    group->addAction(QIcon(":/main/logo"), tr("test"), FancyRibbonGroup::GroupMedium);
    group->addAction(QIcon(":/main/logo"), tr("test123"), FancyRibbonGroup::GroupMedium);
    group->addAction(QIcon(":/main/logo"), tr("test12345"), FancyRibbonGroup::GroupMedium);
    group->setOptionButtonVisible();

    group = page->addGroup(tr("Links"));
    group->addAction(QIcon(":/main/logo"), tr("Hyperlink"), FancyRibbonGroup::GroupMedium);
    group->addAction(QIcon(":/main/logo"), tr("Bookmark"), FancyRibbonGroup::GroupMedium);
    group->addAction(QIcon(":/main/logo"), tr("Cross-reference"), FancyRibbonGroup::GroupMedium);

    group = page->addGroup(tr("Themes"));
    group->addAction(QIcon(":/main/logo"), tr("test\n12"), FancyRibbonGroup::GroupLarge); // it's right
    action = new QAction(QIcon(":/main/logo"), tr("Colors"));
    menu = new QMenu();
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    action->setMenu(menu);
    // add action that has menu
    group->addAction(action, FancyRibbonGroup::GroupMedium);
    group->addAction(QIcon(":/main/logo"), tr("Fonts"), FancyRibbonGroup::GroupMedium);

    group = page->addGroup(tr("Defines"));
    QToolBar *toolBar = new QToolBar("bar");
    action = new QAction(QIcon(":/main/logo"), tr("test"));
    menu = new QMenu();
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    action->setMenu(menu);
    toolBar->addAction(action);
    toolBar->addAction(QIcon(":/main/logo"), tr("test"));
    toolBar->addAction(QIcon(":/main/logo"), tr("test"));
    toolBar->addAction(QIcon(":/main/logo"), tr("test"));
    toolBar->addAction(QIcon(":/main/logo"), tr("test"));
    toolBar->addAction(QIcon(":/main/logo"), tr("test"));
    group->addWidget(toolBar);

    /* custom containers */
    page = m_ribbon->addPage(tr("Insert"));
    group = page->addGroup(tr("Radios&Checks"));
    FancyRibbonGridContainer *gridContainer = new FancyRibbonGridContainer(this);
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

    group = page->addGroup(tr("ToolButtons"));
    gridContainer = new FancyRibbonGridContainer(this);
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 1")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 2")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 3")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 4")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 5")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 6")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 7")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 8")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 9")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 10")));
    gridContainer->addWidget(createToolButton(QIcon(":/main/logo"), tr("test 11")));
    group->addWidget(gridContainer);

    group = page->addGroup(tr("Actions"));
    FancyRibbonActionContainer *actionContainer = new FancyRibbonActionContainer(this);
    actionContainer->addAction(QIcon(":/main/logo"), tr("testgroup"), FancyRibbonGroup::GroupLarge);
    action = new QAction(QIcon(":/main/logo"), tr("test"));
    menu = new QMenu();
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    menu->addAction(QIcon(":/main/logo"), tr("test"));
    action->setMenu(menu);
    actionContainer->addAction(action, FancyRibbonGroup::GroupLarge);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test\ngroup"), FancyRibbonGroup::GroupLarge); // too long
    actionContainer->addAction(QIcon(":/main/logo"), tr("test"), FancyRibbonGroup::GroupMedium);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test123"), FancyRibbonGroup::GroupMedium);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test12345"), FancyRibbonGroup::GroupMedium);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test 1"), FancyRibbonGroup::GroupSmall);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test 2"), FancyRibbonGroup::GroupSmall);
    actionContainer->addAction(QIcon(":/main/logo"), tr("test 3"), FancyRibbonGroup::GroupSmall);
    group->addWidget(actionContainer);


    m_ribbon->addPage(tr("Debug"));
    m_ribbon->addPage(tr("View"));
    m_ribbon->addPage(tr("Help"));

    setMenuWidget(m_ribbon);

    setWindowIcon(QIcon(":/main/logo"));
    setWindowTitle(tr("fancy ribbon demo"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(100, 100, 0));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

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
