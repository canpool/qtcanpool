#include <QApplication>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSettings>
#include <QPainter>
#include <QStyleOption>
#include <QDockWidget>
#include <QListWidget>
#include <QGroupBox>

#include <QtnRibbonQuickAccessBar.h>
#include <QtnRibbonBackstageView.h>

#include "mainwindow.h"
#include "backstagepages.h"


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
    , m_ribbonStyle(qobject_cast<Qtitan::RibbonStyle*>(qApp->style()))
{
    Q_ASSERT(m_ribbonStyle != Q_NULL);

    setWindowTitle(tr("QtitanRibbon Backstage Sample"));

    setCentralWidget(new RibbonWorkspace);
    createBackstage();
    createQuickAccessBar();
    createRibbon();
    createDockWindows();
    statusBar();

    ribbonBar()->setTitleBackground(QPixmap(":/res/background.png") );

    move(QPoint(200, 200));
    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    settings.setValue("recentFileList", files);
    emit updateRecentFileActions(files);
    setButtonStyleTheme(OfficeStyle::Office2016Colorful);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createBackstage()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/res/file.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitanlogo32x32.png"));
    if (QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File")))
    {
        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));

        RibbonBackstageView* backstage = new RibbonBackstageView(ribbonBar());
        connect(backstage, SIGNAL(aboutToShow()), this, SLOT(showBackstage()));
        connect(backstage, SIGNAL(aboutToHide()), this, SLOT(hideBackstage()));

        QAction* saveFile = backstage->addAction(QIcon(":/res/smallSave.png"), tr("&Save"));
        saveFile->setShortcut(tr("Ctrl+S"));
        saveFile->setToolTip(tr("Save"));
        saveFile->setStatusTip(tr("Save the active document"));
        connect(saveFile, SIGNAL(triggered()), this, SLOT(save()));

        QAction* saveAsFile = backstage->addAction(QIcon(":/res/smallSaveAs.png"), tr("Save &As"));
        saveAsFile->setToolTip(tr("Save As"));
        saveAsFile->setStatusTip(tr("Save the active document with a new name"));
        connect(saveAsFile, SIGNAL(triggered()), this, SLOT(save()));

        QAction* openFile = backstage->addAction(QIcon(":/res/smallOpen.png"), tr("&Open"));
        openFile->setShortcut(tr("Ctrl+O"));
        openFile->setToolTip(tr("Open"));
        openFile->setStatusTip(tr("Open an existing document"));
        connect(openFile, SIGNAL(triggered()), this, SLOT(open()));

        BackstagePageInfo* infoPage = new BackstagePageInfo(this);
        backstage->addPage(infoPage);

        BackstagePageSave* savePage = new BackstagePageSave(this);
        backstage->addPage(savePage);

        BackstagePageAbout* aboutPage = new BackstagePageAbout(this);
        backstage->addPage(aboutPage);

        QAction* actOptions = backstage->addAction(QIcon(":/res/smallOptions.png"), tr("Options"));
        connect(actOptions, SIGNAL(triggered()), this, SLOT(pressButton()));

        backstage->addSeparator();

        QAction* actExit = backstage->addAction(QIcon(":/res/smallClose.png"), tr("Exit"));
        connect(actExit, SIGNAL(triggered()), this, SLOT(close()));

        if (RibbonSystemButton* sysButton = ribbonBar()->getSystemButton())
            sysButton->setBackstage(backstage);

        backstage->open();
    }
}

void MainWindow::createQuickAccessBar()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction* smallButton = quickAccessBar->addAction(QIcon(":/res/smallNew.png"), tr("New"));
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
    if (Qtitan::RibbonPage* pageButtons = ribbonBar()->addPage(tr("&Buttons")))
    {
        QMenu* splitPopup = new QMenu(this);
        splitPopup->addAction(tr("Option 1"));
        splitPopup->addAction(tr("Option 2"));
        splitPopup->addAction(tr("Option 3"));

        if (Qtitan::RibbonGroup* groupClipboard = pageButtons->addGroup(tr("Clipboard"))) 
        {
            QMenu* editPaste = new QMenu(this);
            QAction* actionPaste1 = editPaste->addAction(QIcon(":/res/smallClipboard.png"), tr("Paste"));
            actionPaste1->setPriority(QAction::LowPriority);
            actionPaste1->setShortcut(QKeySequence::Paste);

            QAction* actionPaste2 = groupClipboard->addAction(QIcon(":/res/clipboard.png"), 
                tr("&Paste"), Qt::ToolButtonTextUnderIcon, editPaste);
            actionPaste2->setPriority(QAction::LowPriority);
            actionPaste2->setShortcut(QKeySequence::Paste);

            QAction* actionCut = groupClipboard->addAction(QIcon(":/res/smallCut.png"), 
                tr("&Cut"), Qt::ToolButtonTextBesideIcon);
            actionCut->setShortcut(QKeySequence::Cut);

            QAction* actionCopy = groupClipboard->addAction(QIcon(":/res/smallCopy.png"), 
                tr("&Copy"), Qt::ToolButtonTextBesideIcon);
            actionCopy->setShortcut(QKeySequence::Copy);

            groupClipboard->addAction(QIcon(":/res/smallFormatPainter.png"), 
                tr("F&ormat Pointer"), Qt::ToolButtonTextBesideIcon);

            groupClipboard->setEnabled(false);
        }

        if (Qtitan::RibbonGroup* groupView = pageButtons->addGroup(tr("View"))) 
        {
            QCheckBox* checkStatusBar = new QCheckBox(tr("Status Bar"));
            checkStatusBar->setToolTip(tr("CheckBox 1"));
            checkStatusBar->setCheckState(Qt::Checked);
            groupView->addWidget(checkStatusBar);
            connect(checkStatusBar, SIGNAL(stateChanged(int)), this, SLOT(hideStatusBar(int)));
        }
    }

    if (Qtitan::RibbonPage* pageWindow = ribbonBar()->addPage(tr("&Window")))
    {
        Q_UNUSED(pageWindow);
    }

    if (Qtitan::RibbonPage* pageContext = ribbonBar()->addPage(tr("&Context Tab")))
    {
        pageContext->setContextColor(RibbonPage::ContextColorGreen);
    }

    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Styles"), this);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);

    QWidget* styleWidget = new QWidget();

    QWidget* styleGroup = new QWidget();
    QVBoxLayout* listLayout = new QVBoxLayout();
    listLayout->setMargin(0);

    QListWidget* customerList = new QListWidget();
    for (int i = 0; 10 > i; i++)
        customerList->addItems(QStringList(QObject::tr("Item %1").arg(i+1)));

    listLayout->addWidget(customerList);
    styleGroup->setLayout(listLayout);

    QGroupBox* optionsGroup = new QGroupBox(tr("Preview"));
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    QCheckBox* showPreviewBox = new QCheckBox(tr("Show Preview"));
    showPreviewBox->setEnabled(false);
    QCheckBox* linkedStylesBox = new QCheckBox(tr("Disable Linked Styles"));
    optionsLayout->addWidget(showPreviewBox);
    optionsLayout->addWidget(linkedStylesBox);
    optionsGroup->setLayout(optionsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(styleGroup);
    mainLayout->addWidget(optionsGroup);
    styleWidget->setLayout(mainLayout);

    dock->setWidget(styleWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QList<QDockWidget*> docks; docks << dock;
    QList<int> sizes; sizes << 120;
    resizeDocks(docks, sizes, Qt::Vertical);
#endif
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"));
    if (!fileName.isEmpty()) 
    {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File loaded"), 2000);
    }
}

bool MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),tr("Document"), QLatin1Char('*') + QLatin1String(".txt"));
    if (!fileName.isEmpty())
    {
        setCurrentFile(fileName);
        statusBar()->showMessage(tr("File saved"), 2000);
        return true;
    }
    return false;
}

void MainWindow::openRecentFile(const QString& file)
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Open Recent File.") + QLatin1String(" ") + file);
    messageBox.exec();
}

void MainWindow::print()
{
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted)
    {
    }
}

void MainWindow::printSetup()
{
}

void MainWindow::hideStatusBar(int state)
{
    statusBar()->setVisible(state == Qt::Checked);
}

void MainWindow::pressButton()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Press button."));
    messageBox.exec();
}

void MainWindow::showBackstage()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar())
        quickAccessBar->setEnabled(false);

    m_actionRibbonMinimize->setEnabled(false);
    m_menuOptions->setEnabled(false);
}

void MainWindow::hideBackstage()
{
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar())
        quickAccessBar->setEnabled(true);

    m_actionRibbonMinimize->setEnabled(true);
    m_menuOptions->setEnabled(true);
}

void MainWindow::setDPIToggled(bool on)
{
    m_ribbonStyle->setDPIAware(on);
}

void MainWindow::setCurrentFile(const QString& fileName)
{
    QString curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);

    while (files.size() > 9)
        files.removeLast();

    settings.setValue("recentFileList", files);

    emit updateRecentFileActions(files);
}
