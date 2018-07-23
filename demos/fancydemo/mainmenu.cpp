#include "mainmenu.h"
#include "cabout.h"
#include "cwidgets.h"
#include "optiondialog.h"

MainMenu::MainMenu(QWidget *parent)
    : QMenu(parent)
{
    QAction *action = new QAction(QIcon(":/tools/setting"), tr("&Options..."),this);
    addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotOption()));

    action = new QAction(QIcon(":/tools/help"), tr("Context &Help"),this);
    action->setShortcut(tr("F1"));
    addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotHelp()));

    action = new QAction(QIcon(":/main/logo"), tr("About &Us"),this);
    addAction(action);
    connect(action, SIGNAL(triggered(bool)), this, SLOT(slotAbout()));

    addSeparator();

    action = new QAction(QIcon(":/tools/exit"), tr("E&xit"),this);
    action->setShortcut(tr("Ctrl+Q"));
    addAction(action);
    connect(action, SIGNAL(triggered(bool)), qApp, SLOT(quit()));


}

MainMenu::~MainMenu()
{

}

void MainMenu::slotOption()
{
    OptionDialog opDlg;
    opDlg.exec();
}

//slots
void MainMenu::slotAbout()
{
    CAbout about;
    about.exec();

//    CAbout *about = new CAbout;
//    about->setAttribute(Qt::WA_DeleteOnClose);
//    about->open();
}

void MainMenu::slotHelp()
{
    QDesktopServices desktopServices;
    QString strUrl = QCoreApplication::applicationDirPath();
    strUrl = QString("file:///%1/qtproject_help.chm").arg(strUrl);
    QUrl help(strUrl);
    desktopServices.openUrl(help);
}
