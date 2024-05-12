#include <QApplication>
#include <QDesktopWidget>
#include <QSignalMapper>
#include <QTabBar>
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QFileDialog>
#include <QPainter>
#include <QCheckBox>
#include <QRadioButton>
#include <QSettings>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QtitanRibbon.h>

#include "mainwindow.h"
#include "texteditchild.h"
#include "sketchchild.h"
#include "aboutdialog.h"


/* MdiArea */
MdiArea::MdiArea(QWidget* parent) 
    : QMdiArea(parent) 
{
    setTabsClosable(true);
}

MdiArea::~MdiArea() 
{
}

void MdiArea::paintEvent(QPaintEvent* paintEvent)
{
    Q_UNUSED(paintEvent);
    QPainter painter(viewport());
    QStyleOption opt;
    opt.init(this);
    if (paintEvent->rect().intersects(opt.rect))
        style()->drawPrimitive((QStyle::PrimitiveElement)CommonStyle::PE_Workspace, &opt, &painter, this);
}


/* ManagerRibbon */
MainWindow::MainWindow()
    : RibbonWindow(Q_NULL)
{
    m_typeDocCurrent = ChildInterface::td_None;
    m_childCurrent = Q_NULL;
    m_mdiArea = new MdiArea(this);
    m_mdiArea->setLineWidth(3);
    m_mdiArea->setFrameShape(QFrame::Panel);
    m_mdiArea->setFrameShadow(QFrame::Sunken);

    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    qApp->installEventFilter(this);

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);

    connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));
    connect(m_mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subWindowActivated(QMdiSubWindow*)));

    m_windowMapper = new QSignalMapper(this);
    connect(m_windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenuFile();
    createRibbon();
    createStatusBar();
    updateMenus();

    setWindowTitle(tr("Ribbon MDI"));
    setUnifiedTitleAndToolBarOnMac(true);

    ribbonBar()->setTitleBarVisible(false);
    switchFrameTheme(true);

    m_stateWindow = windowState();

    QRect geom = QApplication::desktop()->availableGeometry();
    resize( 2 * geom.width() / 3, 2 * geom.height() / 3 );
    readSettings();
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    switch(event->type())
    {
        case QEvent::ChildRemoved :
            if (QChildEvent* pEvent= static_cast<QChildEvent*>(event))
            {
                if (qobject_cast<QMdiSubWindow*>(pEvent->child()) && m_childCurrent && m_mdiArea->subWindowList().size() == 1)
                {
                    if (ChildInterface* ifOldChild = qobject_cast<ChildInterface*>(m_childCurrent))
                        ifOldChild->removeUserInterface(ribbonBar(), statusBar(), Q_NULL);

                    m_childCurrent = Q_NULL;
                    m_typeDocCurrent = ChildInterface::td_None;
                }
            }
            break;
        default:
            break;
    }
    return Qtitan::RibbonMainWindow::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_mdiArea->closeAllSubWindows();
    if (m_mdiArea->currentSubWindow()) 
    {
        event->ignore();
    } 
    else 
    {
        writeSettings();
        event->accept();
    }
}

void MainWindow::newTextFile()
{
    TextEditChild* child = createMdiTextChild();
    child->newFile();
    child->show();
}

void MainWindow::newSketchFile()
{
    SketchChild* child = createSketchChild();
    child->newFile();
    child->show();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty()) 
    {
        QMdiSubWindow* existing = findMdiChild(fileName);
        if (existing) 
        {
            m_mdiArea->setActiveSubWindow(existing);
            return;
        }

        TextEditChild* child = createMdiTextChild();
        if (child->loadFile(fileName)) 
        {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
        } 
        else 
        {
            child->close();
        }
    }
}

void MainWindow::save()
{
    ChildInterface* ifChild = qobject_cast<ChildInterface*>(activeMdiChild());
    if (ifChild && ifChild->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
    ChildInterface* ifChild = qobject_cast<ChildInterface*>(activeMdiChild());
    if (ifChild && ifChild->saveAs())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::cut()
{
    TextEditChild* txtChild = qobject_cast<TextEditChild*>(activeMdiChild());
    if (txtChild)
        txtChild->cut();
}

void MainWindow::copy()
{
    TextEditChild* txtChild = qobject_cast<TextEditChild*>(activeMdiChild());
    if (txtChild)
        txtChild->copy();
}

void MainWindow::paste()
{
    TextEditChild* txtChild = qobject_cast<TextEditChild*>(activeMdiChild());
    if (txtChild)
        txtChild->paste();
}

void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    m_saveAct->setEnabled(hasMdiChild);
    m_saveAsAct->setEnabled(hasMdiChild);
    m_closeAct->setEnabled(hasMdiChild);
    m_closeAllAct->setEnabled(hasMdiChild);
    m_tileAct->setEnabled(hasMdiChild);
    m_cascadeAct->setEnabled(hasMdiChild);
    m_nextAct->setEnabled(hasMdiChild);
    m_previousAct->setEnabled(hasMdiChild);
}

void MainWindow::updateWindowMenu()
{
    m_windowMenu->clear();
    QList<QMdiSubWindow*> windows = m_mdiArea->subWindowList();

    for (int i = 0; i < windows.size(); ++i) 
    {
        QWidget* child = windows.at(i)->widget();
        if(!child)
            continue;

        ChildInterface* ifChild = qobject_cast<ChildInterface*>(child);
        if(!ifChild)
            continue;

        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(ifChild->userFriendlyCurrentFile());
        else 
            text = tr("%1 %2").arg(i + 1).arg(ifChild->userFriendlyCurrentFile());

        QAction* action  = m_windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), m_windowMapper, SLOT(map()));
        m_windowMapper->setMapping(action, windows.at(i));
    }
}

TextEditChild* MainWindow::createMdiTextChild()
{
    TextEditChild* child = new TextEditChild;
    m_mdiArea->addSubWindow(child);
    return child;
}

SketchChild* MainWindow::createSketchChild()
{
    SketchChild* pChild = new SketchChild();
    Q_ASSERT(pChild != Q_NULL);
    QMdiSubWindow* subChild = m_mdiArea->addSubWindow(pChild);
    Q_ASSERT(subChild != Q_NULL);
    Q_UNUSED(subChild);
    return pChild;
}

void MainWindow::createActions()
{
    m_newTextAct = new QAction(QIcon(":/res/textedit.png"), tr("&Text Edit"), this);
    m_newTextAct->setStatusTip(tr("Create a new file"));
    connect(m_newTextAct, SIGNAL(triggered()), this, SLOT(newTextFile()));

    m_newSketchAct = new QAction(QIcon(":/res/sketch.png"), tr("&Skcetch Edit"), this);
    m_newSketchAct->setStatusTip(tr("Create a new file"));
    connect(m_newSketchAct, SIGNAL(triggered()), this, SLOT(newSketchFile()));

    m_openAct = new QAction(QIcon(":/res/open.png"), tr("&Open..."), this);
    m_openAct->setShortcuts(QKeySequence::Open);
    m_openAct->setStatusTip(tr("Open an existing file"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct = new QAction(QIcon(":/res/save.png"), tr("&Save"), this);
    m_saveAct->setShortcuts(QKeySequence::Save);
    m_saveAct->setStatusTip(tr("Save the document to disk"));
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save &As..."), this);
    m_saveAsAct->setShortcuts(QKeySequence::SaveAs);
    m_saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    //! [0]
    m_exitAct = new QAction(tr("E&xit"), this);
    m_exitAct->setShortcuts(QKeySequence::Quit);
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    m_closeAct = new QAction(QIcon(":/res/close.png"), tr("Cl&ose"), this);
    m_closeAct->setStatusTip(tr("Close the active window"));
    connect(m_closeAct, SIGNAL(triggered()), m_mdiArea, SLOT(closeActiveSubWindow()));

    m_closeAllAct = new QAction(tr("Close &All"), this);
    m_closeAllAct->setStatusTip(tr("Close all the windows"));
    connect(m_closeAllAct, SIGNAL(triggered()), m_mdiArea, SLOT(closeAllSubWindows()));

    m_tileAct = new QAction(QIcon(":/res/arrangeicons.png"), tr("&Arrange Icons"), this);
//    m_tileAct->setStatusTip(tr("Tile the windows"));
    m_tileAct->setStatusTip(tr("Arrange icons at the bottom of the window"));
    connect(m_tileAct, SIGNAL(triggered()), m_mdiArea, SLOT(tileSubWindows()));

    m_cascadeAct = new QAction(tr("&Cascade"), this);
    m_cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(m_cascadeAct, SIGNAL(triggered()), m_mdiArea, SLOT(cascadeSubWindows()));

    m_nextAct = new QAction(tr("Ne&xt"), this);
    m_nextAct->setShortcuts(QKeySequence::NextChild);
    m_nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(m_nextAct, SIGNAL(triggered()), m_mdiArea, SLOT(activateNextSubWindow()));

    m_previousAct = new QAction(tr("Pre&vious"), this);
    m_previousAct->setShortcuts(QKeySequence::PreviousChild);
    m_previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(m_previousAct, SIGNAL(triggered()), m_mdiArea, SLOT(activatePreviousSubWindow()));
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitan.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/sysbutton.png"));
    if (QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) 
    {
        if (QToolButton* fileButton = ribbonBar()->getSystemButton())
            fileButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

        actionFile->setToolTip(tr("Click here to see everything<br />you can do with your<br />document"));
        if (Qtitan::RibbonSystemPopupBar* pPopupBar = qobject_cast<Qtitan::RibbonSystemPopupBar*>(actionFile->menu()))
        {
            pPopupBar->addAction(m_newTextAct);
            pPopupBar->addAction(m_newSketchAct);
            pPopupBar->addAction(m_openAct);
            pPopupBar->addAction(m_saveAct);
            pPopupBar->addAction(m_saveAsAct);
            pPopupBar->addSeparator();

            pPopupBar->addSeparator();
            pPopupBar->addAction(m_closeAct);

            m_appCloseAct = pPopupBar->addPopupBarAction(tr("Exit Sample") );
            connect(m_appCloseAct, SIGNAL(triggered()), this, SLOT(close()));

            QAction* option = pPopupBar->addPopupBarAction(tr("Option") );
            option->setEnabled(false );
        }
    }
}

void MainWindow::createRibbon()
{
    if (Qtitan::RibbonPage* pageView = ribbonBar()->addPage(tr("&View"))) 
    {
        if (Qtitan::RibbonGroup* groupDocument = pageView->addGroup(tr("Document Views")))
        {
            groupDocument->addAction(QIcon(":/res/normal.png"), tr("&Normal"), Qt::ToolButtonTextUnderIcon);
            groupDocument->addAction(QIcon(":/res/printpreview.png"), tr("&Print Preview"), Qt::ToolButtonTextUnderIcon);
            m_actionFullScreen = groupDocument->addAction(QIcon(":/res/fullscreen.png"), tr("&Full Screen"), Qt::ToolButtonTextUnderIcon);
            m_actionFullScreen->setCheckable(true);
            m_actionFullScreen->setChecked(false);
            connect(m_actionFullScreen, SIGNAL(triggered(bool)), this, SLOT(fullScreen(bool)));
        }

        Qt::CheckState stateWorkspace = Qt::Checked;
        if (Qtitan::RibbonGroup* groupShow_Hide = pageView->addGroup(tr("Show/Hide")))
        {
            QCheckBox* checkStatusBar = new QCheckBox(tr("StatusBar"));
            checkStatusBar->setCheckState(Qt::Checked);
            groupShow_Hide->addWidget(checkStatusBar);
            connect(checkStatusBar, SIGNAL(stateChanged(int)), this, SLOT(hideStatusBar(int)));

            QCheckBox* checkWorkspace = new QCheckBox(tr("Workspace"));
            checkWorkspace->setCheckState(stateWorkspace);
            groupShow_Hide->addWidget(checkWorkspace);
            connect(checkWorkspace, SIGNAL(stateChanged(int)), this, SLOT(switchViewMode(int)));

            QCheckBox* checkFrameTheme = new QCheckBox(tr("FrameTheme"));
            checkFrameTheme->setCheckState(Qt::Checked);
            m_actFrameTheme = groupShow_Hide->addWidget(checkFrameTheme);
            QObject::connect(checkFrameTheme, SIGNAL(toggled(bool)), m_actFrameTheme, SIGNAL(toggled(bool)));
            connect(m_actFrameTheme, SIGNAL(toggled(bool)), this, SLOT(switchFrameTheme(bool)));
        }

        m_groupMdiTabs = pageView->addGroup(tr("Option MDI Tabs"));
        if (m_groupMdiTabs)
        {
            m_posTop = new QRadioButton(tr("Position Top"));
            connect(m_posTop, SIGNAL(toggled(bool)), this, SLOT(setTabPositionTop(bool)));
            m_posTop->setChecked(true);
            m_groupMdiTabs->addWidget(m_posTop);

            m_posBottom = new QRadioButton(tr("Position Bottom"));
            connect(m_posBottom, SIGNAL(toggled(bool)), this, SLOT(setTabPositionBottom(bool)));
            m_groupMdiTabs->addWidget(m_posBottom);

            m_posLeft = new QRadioButton(tr("Position Left"));
            connect(m_posLeft, SIGNAL(toggled(bool)), this, SLOT(setTabPositionLeft(bool)));
            m_groupMdiTabs->addWidget(m_posLeft);

            m_posRight = new QRadioButton(tr("Position Right"));
            connect(m_posRight, SIGNAL(toggled(bool)), this, SLOT(setTabPositionRight(bool)));
            m_groupMdiTabs->addWidget(m_posRight);
            m_groupMdiTabs->setEnabled(stateWorkspace == Qt::Checked);
        }
    }
    if (Qtitan::RibbonPage* pageWindow = ribbonBar()->addPage(tr("&Window"))) 
    {
        if (Qtitan::RibbonGroup* groupWindow = pageWindow->addGroup(tr("Window")))
        {
            QAction* actionnewWindow = groupWindow->addAction(QIcon(":/res/newwindow.png"), tr("&New Window"), Qt::ToolButtonTextUnderIcon);
            actionnewWindow->setStatusTip(tr("Open another window for the active document"));
            groupWindow->addAction(m_tileAct, Qt::ToolButtonTextUnderIcon);
            m_windowMenu = groupWindow->addMenu(QIcon(":/res/windowswitch.png"), tr("&Switch Windows"));
            updateWindowMenu();
            connect(m_windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
        }
    }
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(this);
    QRect geom = QApplication::desktop()->availableGeometry();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(2 * geom.width() / 3, 2 * geom.height() / 3)).toSize();
    move(pos); resize(size);
}

void MainWindow::writeSettings()
{
    QSettings settings(this);
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

QWidget* MainWindow::activeMdiChild()
{
    if (QMdiSubWindow* activeSubWindow = m_mdiArea->activeSubWindow())
        return activeSubWindow->widget();
    return Q_NULL;
}

QMdiSubWindow* MainWindow::findMdiChild(const QString& fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    foreach (QMdiSubWindow* window, m_mdiArea->subWindowList()) 
    {
        ChildInterface* ifChild = qobject_cast<ChildInterface*>(window->widget());
        if (ifChild && ifChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}

void MainWindow::setActiveSubWindow(QWidget* window)
{
    if (!window)
        return;
    m_mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(window));
}

void MainWindow::hideStatusBar(int state)
{
    statusBar()->setVisible(state == Qt::Checked);
}

void MainWindow::switchViewMode(int state)
{
    m_mdiArea->setLineWidth(state == Qt::Checked ? 3 : 0);
    m_mdiArea->setViewMode(state == Qt::Checked ? QMdiArea::TabbedView : QMdiArea::SubWindowView);
    m_groupMdiTabs->setEnabled(state == Qt::Checked);
}

void MainWindow::switchFrameTheme(bool checked)
{
//    showNormal();
    ribbonBar()->setFrameThemeEnabled(checked);
}

void MainWindow::setTabPositionLeft(bool state)
{
    if (state)
        m_mdiArea->setTabPosition(QTabWidget::West);
}

void MainWindow::setTabPositionTop(bool state)
{
    if (state)
        m_mdiArea->setTabPosition(QTabWidget::North);
}

void MainWindow::setTabPositionBottom(bool state)
{
    if (state)
        m_mdiArea->setTabPosition(QTabWidget::South);
}

void MainWindow::setTabPositionRight(bool state)
{
    if (state)
        m_mdiArea->setTabPosition(QTabWidget::East);
}

void MainWindow::subWindowActivated(QMdiSubWindow* subWindow)
{
    if (subWindow)
    {
        QWidget* activatWidget = subWindow->widget();
        if (m_childCurrent == activatWidget)
            return;

        if (ChildInterface* ifChild = qobject_cast<ChildInterface*>(activatWidget))
        {
            setUpdatesEnabled(false);
            if (ChildInterface* ifOldChild = qobject_cast<ChildInterface*>(m_childCurrent))
                ifOldChild->removeUserInterface(ribbonBar(), statusBar(), Q_NULL);

            m_childCurrent = activatWidget;
            m_typeDocCurrent = ifChild->typeDocument();

            ifChild->buildUserInterface(ribbonBar(), statusBar(), Q_NULL);
            setUpdatesEnabled(true);
        }
    }
}

void MainWindow::fullScreen(bool checked)
{
    if (checked)
    {
        m_stateWindow = windowState();
        m_actionFullScreen->setChecked(true);
        ribbonBar()->setVisible(false);
        setWindowState(Qt::WindowFullScreen);
    }
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    Qtitan::RibbonMainWindow::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape && m_actionFullScreen->isChecked())
    {
        ribbonBar()->setVisible(true);
        setWindowState(m_stateWindow);
        m_actionFullScreen->setChecked(false);
    }
}
