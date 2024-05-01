#include "mainwindow.h"
#include "aboutdialog.h"
#include "qribbon/ribbon_def.h"
#include "qribbon/ribbonquickaccessbar.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QPaintEvent>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QStyleOption>

#define Font_Default    "System Default"
#define Font_Normal     "Normal"
#define Font_Large      "Large"
#define Font_ExLarge    "Extra Large"
#define DPI_100_percent "DPI 100%"

/* MainWindow */
MainWindow::MainWindow(QWidget *parent) : QRibbon::RibbonMainWindow(parent)
{
    m_ribbonStyle = qobject_cast<QRibbon::RibbonStyle *>(qApp->style());
    m_ribbonStyle->setTheme(OfficeStyle::OfficeWhite);

    m_defaultFont = 8;

    setWindowTitle(tr("Ribbon System Menu Sample"));
    setCentralWidget(new RibbonWorkspace);

    ribbonBar()->setFrameThemeEnabled();
    ribbonBar()->setTitleBarVisible(false);
    createMenuFile();
    createQuickAccessBar();
    createRibbon();
    statusBar();

    QAction *actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), "About", Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program<br />information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    createOptions();

    m_actionRibbonMinimize =
        ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), "Minimize the Ribbon", Qt::ToolButtonIconOnly);
    m_actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    m_actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(m_actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChanged(bool)));

    QRect geom = QApplication::desktop()->availableGeometry();
    move(QPoint(200, 200));
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/shared/res/logo.png"));
    if (QAction *actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) {
        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));

        if (QRibbon::RibbonSystemPopupBar *popupBar =
                qobject_cast<QRibbon::RibbonSystemPopupBar *>(actionFile->menu())) {
            QAction *newFile = popupBar->addAction(QIcon(":/res/new.png"), tr("&New"));
            newFile->setShortcut(tr("Ctrl+N"));
            newFile->setStatusTip(tr("Create a new document"));
            newFile->setToolTip(tr("New"));
            newFile->setEnabled(false);

            QAction *openFile = popupBar->addAction(QIcon(":/res/open.png"), tr("&Open..."));
            openFile->setShortcut(tr("Ctrl+O"));
            openFile->setToolTip(tr("Open"));
            openFile->setStatusTip(tr("Open an existing document"));
            connect(openFile, SIGNAL(triggered()), this, SLOT(open()));

            QAction *saveFile = popupBar->addAction(QIcon(":/res/save.png"), tr("&Save"));
            saveFile->setShortcut(tr("Ctrl+S"));
            saveFile->setToolTip(tr("Save"));
            saveFile->setStatusTip(tr("Save the active document"));
            connect(saveFile, SIGNAL(triggered()), this, SLOT(save()));

            QAction *saveAsFile = popupBar->addAction(tr("Save &As..."));
            saveAsFile->setToolTip(tr("Save As"));
            saveAsFile->setStatusTip(tr("Save the active document with a new name"));
            connect(saveAsFile, SIGNAL(triggered()), this, SLOT(save()));

            popupBar->addSeparator();

            QAction *actionPrint = new QAction(QIcon(":/res/print.png"), tr("&Print..."), this);
            actionPrint->setToolTip(
                tr("Select a printer, number of copies, and other printing options before printing"));

            QAction *actionPagePrint = new QAction(actionPrint->icon(), actionPrint->text(), this);
            popupBar->addAction(actionPagePrint);
            if (RibbonPageSystemPopup *pageSystemPopup =
                    popupBar->addPageSystemPopup(tr("Preview and print the document"), actionPagePrint, true)) {
                connect(actionPagePrint, SIGNAL(triggered()), actionPrint, SIGNAL(triggered()));
                pageSystemPopup->setMinimumWidth(296);
                pageSystemPopup->addAction(actionPrint);

                QAction *actionPrintSetup = new QAction(QIcon(":/res/printSetup.png"), tr("P&rint Setup"), this);
                actionPrintSetup->setToolTip(tr("Change the printer and printing options"));

                pageSystemPopup->addAction(actionPrintSetup);

                QAction *actionPrintPreview =
                    new QAction(QIcon(":/res/printPreview.png"), tr("Print Pre&view..."), this);
                actionPrintPreview->setToolTip(tr("Preview and make changes to pages before printing"));
                pageSystemPopup->addAction(actionPrintPreview);
            }

            QAction *actionPrepare = popupBar->addAction(QIcon(":/res/prepare.png"), tr("Pr&epare"));
            popupBar->addAction(actionPrepare);

            QAction *actionSend = popupBar->addAction(QIcon(":/res/send.png"), tr("Sen&d"));
            popupBar->addAction(actionSend);
            if (RibbonPageSystemPopup *pageSystemPopup =
                    popupBar->addPageSystemPopup(tr("Preview and print the document"), actionSend, false)) {
                pageSystemPopup->setMinimumWidth(296);
                QAction *actionMail = pageSystemPopup->addAction(QIcon(":/res/mail.png"), tr("E-Mail"));
                actionMail->setToolTip(tr("Send the active document by e-mail"));
                pageSystemPopup->addAction(actionMail);

                QAction *actionIntenetFax =
                    pageSystemPopup->addAction(QIcon(":/res/internetfix.png"), tr("Intenet Fax"));
                actionIntenetFax->setToolTip(tr("Use an Internet fax service to fax the document"));
                pageSystemPopup->addAction(actionIntenetFax);
            }

            popupBar->addSeparator();

            QIcon iconClose;
            iconClose.addPixmap(QPixmap(":/res/close.png"));
            iconClose.addPixmap(QPixmap(":/res/smallclose.png"));
            QAction *actClose = popupBar->addAction(iconClose, tr("&Close"));
            actClose->setShortcut(tr("Ctrl+C"));
            actClose->setStatusTip(tr("Exit"));
            connect(actClose, SIGNAL(triggered()), this, SLOT(close()));
            popupBar->addPopupBarAction(actClose, Qt::ToolButtonTextBesideIcon);

            QAction *option = new QAction(QPixmap(":/res/smalloption.png"), tr("Opt&ion"), this);
            popupBar->addPopupBarAction(option, Qt::ToolButtonTextBesideIcon);
            option->setEnabled(false);

            if (RibbonPageSystemRecentFileList *pageRecentFile = popupBar->addPageRecentFile(tr("Recent Documents"))) {
                pageRecentFile->setSize(9);
                QStringList list;
                list.append("Document1.txt");
                list.append("Document2.txt");
                list.append("Document3.txt");
                list.append("Document4.txt");
                list.append("Document5.txt");
                list.append("Document6.txt");
                pageRecentFile->updateRecentFileActions(list);
            }
        }
    }
}

void MainWindow::createQuickAccessBar()
{
    if (QRibbon::RibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar()) {
        QAction *action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction *smallButton = quickAccessBar->addAction(QIcon(":/res/smallNew.png"), tr("New"));
        smallButton->setToolTip(tr("Create a new document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
        smallButton->setToolTip(tr("Open an existing document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallSave.png"), tr("Save"));
        smallButton->setToolTip(tr("Save the active document"));
        connect(smallButton, SIGNAL(triggered()), this, SLOT(pressButton()));

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallUndo.png"), tr("&Undo"));
        smallButton->setShortcut(QKeySequence::Undo);
        smallButton->setEnabled(false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallRedo.png"), tr("&Redo"));
        smallButton->setShortcut(QKeySequence::Redo);
        smallButton->setEnabled(false);

        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
    if (QRibbon::RibbonPage *pageButtons = ribbonBar()->addPage(tr("&Home"))) {
        if (QRibbon::RibbonGroup *groupClipboard = pageButtons->addGroup(tr("Clipboard"))) {
            QMenu *editPaste = new QMenu(this);
            QAction *actionPaste1 = editPaste->addAction(QIcon(":/res/smallClipboard.png"), tr("Paste"));
            actionPaste1->setPriority(QAction::LowPriority);
            actionPaste1->setShortcut(QKeySequence::Paste);

            QAction *actionPaste2 = groupClipboard->addAction(QIcon(":/res/clipboard.png"), tr("&Paste"),
                                                              Qt::ToolButtonTextUnderIcon, editPaste);
            actionPaste2->setPriority(QAction::LowPriority);
            actionPaste2->setShortcut(QKeySequence::Paste);

            QAction *actionCut =
                groupClipboard->addAction(QIcon(":/res/smallCut.png"), tr("&Cut"), Qt::ToolButtonTextBesideIcon);
            actionCut->setShortcut(QKeySequence::Cut);

            QAction *actionCopy =
                groupClipboard->addAction(QIcon(":/res/smallCopy.png"), tr("&Copy"), Qt::ToolButtonTextBesideIcon);
            actionCopy->setShortcut(QKeySequence::Copy);

            groupClipboard->addAction(QIcon(":/res/smallFormatPainter.png"), tr("F&ormat Pointer"),
                                      Qt::ToolButtonTextBesideIcon);

            groupClipboard->setEnabled(false);
        }
    }
    m_defaultFont = ribbonBar()->font().pointSize();

    if (QToolButton *button = ribbonBar()->getSystemButton())
        button->setToolButtonStyle(Qt::ToolButtonTextOnly);
}

void MainWindow::createOptions()
{
    QRibbon::RibbonStyle::Theme themeId = QRibbon::RibbonStyle::OfficeWhite;
    if (m_ribbonStyle)
        themeId = m_ribbonStyle->getTheme();

    QMenu *menu = ribbonBar()->addMenu(tr("Options"));
    QAction *actionStyle = menu->addAction(tr("Style"));

    QMenu *menuStyle = new QMenu(ribbonBar());
    QActionGroup *styleActions = new QActionGroup(this);

    QAction *actionWhite = menuStyle->addAction(tr("White"));
    actionWhite->setObjectName("OfficeWhite");
    actionWhite->setCheckable(true);
    actionWhite->setChecked(themeId == RibbonStyle::OfficeWhite);

    QAction *actionGray = menuStyle->addAction(tr("Gray"));
    actionGray->setObjectName("OfficeGray");
    actionGray->setCheckable(true);
    actionGray->setChecked(themeId == RibbonStyle::OfficeGray);

    QAction *actionDark = menuStyle->addAction(tr("Dark"));
    actionDark->setObjectName("OfficeDark");
    actionDark->setCheckable(true);
    actionDark->setChecked(themeId == RibbonStyle::OfficeDark);

    menuStyle->addSeparator();

    styleActions->addAction(actionWhite);
    styleActions->addAction(actionGray);
    styleActions->addAction(actionDark);

    actionStyle->setMenu(menuStyle);
    connect(styleActions, SIGNAL(triggered(QAction *)), this, SLOT(optionsTheme(QAction *)));

    QAction *actionMenu = menu->addAction(tr("Font"));

    QMenu *menuFont = new QMenu(ribbonBar());
    QActionGroup *fontActions = new QActionGroup(this);

    m_actionDefault = menuFont->addAction(Font_Default);
    m_actionDefault->setCheckable(true);
    m_actionDefault->setChecked(true);
    m_actionDefault->setObjectName(Font_Default);
    fontActions->addAction(m_actionDefault);

    menuFont->addSeparator();

    m_actionNormal = menuFont->addAction(Font_Normal);
    m_actionNormal->setCheckable(true);
    m_actionNormal->setObjectName(Font_Normal);
    fontActions->addAction(m_actionNormal);

    m_actionLarge = menuFont->addAction(Font_Large);
    m_actionLarge->setCheckable(true);
    m_actionLarge->setObjectName(Font_Large);
    fontActions->addAction(m_actionLarge);

    m_actionExLarge = menuFont->addAction(Font_ExLarge);
    m_actionExLarge->setCheckable(true);
    m_actionExLarge->setObjectName(Font_ExLarge);
    fontActions->addAction(m_actionExLarge);
    actionMenu->setMenu(menuFont);
    connect(fontActions, SIGNAL(triggered(QAction *)), this, SLOT(optionsFont(QAction *)));

    menuFont->addSeparator();
    QAction *actionDPI_100_percent = menuFont->addAction(tr("DPI Aware (new)"));
    actionDPI_100_percent->setObjectName(DPI_100_percent);
    actionDPI_100_percent->setCheckable(true);
    actionDPI_100_percent->setChecked(true);
    connect(actionDPI_100_percent, SIGNAL(toggled(bool)), this, SLOT(setDPIToggled(bool)));

    menu->addSeparator();
    QAction *actionCusomize = menu->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if (!fileName.isEmpty()) {
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}

bool MainWindow::save()
{
    QString fileName =
        QFileDialog::getSaveFileName(this, tr("Save As"), tr("Document"), QLatin1Char('*') + QLatin1String(".txt"));
    if (!fileName.isEmpty()) {
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

void MainWindow::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
    }
}

void MainWindow::printSetup()
{
}

void MainWindow::about()
{
    QRibbon::AboutDialog::show(this, tr("About Ribbon System Menu Sample"), tr("QRibbon"),
                               QLatin1String(QRIBBON_VERSION_STR));
}

void MainWindow::optionsTheme(QAction *action)
{
    if (m_ribbonStyle) {
        QRibbon::RibbonStyle::Theme themeId = QRibbon::RibbonStyle::OfficeWhite;
        if (action->objectName() == "OfficeWhite")
            themeId = OfficeStyle::OfficeWhite;
        else if (action->objectName() == "OfficeGray")
            themeId = OfficeStyle::OfficeGray;
        else if (action->objectName() == "OfficeDark")
            themeId = OfficeStyle::OfficeDark;

        if (QToolButton *button = ribbonBar()->getSystemButton()) {
            if (themeId == QRibbon::OfficeStyle::OfficeDark)
                button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            else
                button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }

        m_ribbonStyle->setTheme(themeId);
        m_actionDefault->setChecked(true);
    }
}

void MainWindow::pressButton()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Press button."));
    messageBox.exec();
}

void MainWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void MainWindow::minimizationChanged(bool minimized)
{
    m_actionRibbonMinimize->setChecked(minimized);
    m_actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png")
                                              : QIcon(":/shared/res/ribbonMinimize.png"));
}

void MainWindow::optionsFont(QAction *act)
{
    QFont fnt = ribbonBar()->font();

    if (Font_Default == act->objectName())
        fnt.setPointSize(m_defaultFont);
    else if (Font_Normal == act->objectName())
        fnt.setPointSize(8);
    else if (Font_Large == act->objectName())
        fnt.setPointSize(11);
    else if (Font_ExLarge == act->objectName())
        fnt.setPointSize(13);

    ribbonBar()->setFont(fnt);
}

void MainWindow::setDPIToggled(bool on)
{
    m_actionDefault->setEnabled(on);
    m_actionNormal->setEnabled(on);
    m_actionLarge->setEnabled(on);
    m_actionExLarge->setEnabled(on);
    m_ribbonStyle->setDPIAware(on);
}
