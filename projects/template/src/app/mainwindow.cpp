#include "mainwindow.h"

#include <QAction>
#include <QIcon>
#include <QKeySequence>
#include <QMenu>
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 3.1 retired QCanpool::QuickAccessBar (A2); its replacement,
    // QxRibbon::RibbonQuickAccessBar, only makes sense inside a ribbon bar.
    // This legacy qmake skeleton therefore uses a plain toolbar. New projects
    // should start from the CMake template beside it instead - see
    // projects/template/README.md.
    QToolBar *toolBar = addToolBar(tr("Quick Access"));
    toolBar->setObjectName(QLatin1String("quickAccessBar"));

    QAction *action = toolBar->addAction(tr("Customize"));
    action->setToolTip(tr("Customize Quick Access Bar"));

    QAction *smallButton = toolBar->addAction(QIcon(":/resource/image/logo.png"), tr("New"));
    smallButton->setShortcut(QKeySequence::New);
    smallButton->setToolTip(tr("New File or Project\nCtrl+N"));

    QAction *menuAction = new QAction(QIcon(":/resource/image/logo.png"), tr("Open"));
    menuAction->setShortcut(tr("Ctrl+O"));
    menuAction->setToolTip(tr("Open File or Project\nCtrl+O"));
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    menuAction->setMenu(menu);
    toolBar->addAction(menuAction);

    smallButton = toolBar->addAction(QIcon(":/resource/image/logo.png"), tr("&Undo"));
    smallButton->setShortcut(QKeySequence::Undo);
    smallButton->setEnabled(false);

    setWindowIcon(QIcon(":/resource/image/logo.png"));
    setWindowTitle(tr("template"));
}

MainWindow::~MainWindow()
{
}
