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

#include "qcanpool/fancyribbon.h"
#include "qcanpool/fancyribbonpage.h"
#include "qcanpool/fancyribbongroup.h"

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


    m_ribbon->addPage(tr("Insert"));
    m_ribbon->addPage(tr("Debug"));
    m_ribbon->addPage(tr("View"));
    m_ribbon->addPage(tr("Help"));

    setMenuWidget(m_ribbon);

    setWindowIcon(QIcon(":/main/logo"));
    setWindowTitle(tr("fancy ribbon demo"));

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(100, 100, 0));
    this->setPalette(palette);
    this->setAutoFillBackground(true);

    resize(800, 600);
}

MainWindow::~MainWindow()
{

}
