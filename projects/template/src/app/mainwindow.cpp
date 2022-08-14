#include "mainwindow.h"
#include "qcanpool/quickaccessbar.h"

QCANPOOL_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QuickAccessBar *quickAccessBar = new QuickAccessBar();
    QAction* action = quickAccessBar->actionCustomizeButton();
    action->setToolTip(tr("Customize Quick Access Bar"));

    QAction* smallButton = quickAccessBar->addAction(QIcon(":/resource/image/logo.png"), tr("New"));
    smallButton->setShortcut(QKeySequence::New);
    smallButton->setToolTip(tr("New File or Project\nCtrl+N"));

    QAction *menuAction = new QAction(QIcon(":/resource/image/logo.png"), tr("Open"));
    menuAction->setShortcut(tr("Ctrl+O"));
    menuAction->setToolTip(tr("Open File or Project\nCtrl+O"));
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("action1"));
    menu->addAction(tr("action2"));
    menuAction->setMenu(menu);
    quickAccessBar->addAction(menuAction);

    smallButton = quickAccessBar->addAction(QIcon(":/resource/image/logo.png"), tr("&Undo"));
    smallButton->setShortcut(QKeySequence::Undo);
    smallButton->setEnabled(false);

    addToolBar(quickAccessBar);

    setWindowIcon(QIcon(":/resource/image/logo.png"));
    setWindowTitle(tr("template"));
}

MainWindow::~MainWindow()
{

}
