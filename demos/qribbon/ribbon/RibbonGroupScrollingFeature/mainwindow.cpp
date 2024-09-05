#include "mainwindow.h"
#include "qribbon/ribbonquickaccessbar.h"
#include <QApplication>
#include <QDesktopWidget>

/* MainWindow */
MainWindow::MainWindow(QWidget *parent) : RibbonWindow(parent)
{
    setWindowTitle(tr("Ribbon Group Scrolling"));
    createQuickAccessBar();
    createRibbon();
    createStatusBar();

    m_actionLogoRight = ribbonBar()->addAction(QIcon(":/res/smallRight.png"), tr("Logo Right"), Qt::ToolButtonIconOnly);
    m_actionLogoRight->setCheckable(true);
    connect(m_actionLogoRight, SIGNAL(triggered()), this, SLOT(pressLogoRight()));
    m_actionLogoLeft = ribbonBar()->addAction(QIcon(":/res/smallLeft.png"), tr("Logo Left"), Qt::ToolButtonIconOnly);
    m_actionLogoLeft->setCheckable(true);
    connect(m_actionLogoLeft, SIGNAL(triggered()), this, SLOT(pressLogoLeft()));

    move(QPoint(200, 200));
    QRect geom = QApplication::desktop()->availableGeometry();
    resize(2 * geom.width() / 4, 2 * geom.height() / 4);
}

void MainWindow::createQuickAccessBar()
{
    if (QRibbon::RibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar()) {
        QAction *action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        QAction *smallButton = quickAccessBar->addAction(QIcon(":/res/smallNew.png"), tr("New"));
        smallButton->setToolTip(tr("Create a new document"));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallOpen.png"), tr("Open"));
        smallButton->setToolTip(tr("Open an existing document"));
        quickAccessBar->setActionVisible(smallButton, false);

        smallButton = quickAccessBar->addAction(QIcon(":/res/smallSave.png"), tr("Save"));
        smallButton->setToolTip(tr("Save the active document"));

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
    if (RibbonPage *pageButtons = ribbonBar()->addPage(tr("&Buttons"))) {
        QIcon iconButtton(":/res/scroll.png");
        for (int i = 0; 18 > i; ++i) {
            if (RibbonGroup *ribbonGroup = pageButtons->addGroup(tr("Buttons number%1").arg(i + 1))) {
                ribbonGroup->setIcon(iconButtton);
                ribbonGroup->setContentAlignment(Qt::AlignVCenter);
                ribbonGroup->addAction(iconButtton, tr("&Button 1"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 2"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 3"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 4"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 5"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 6"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 7"), Qt::ToolButtonTextUnderIcon);
                ribbonGroup->addAction(iconButtton, tr("&Button 8"), Qt::ToolButtonTextUnderIcon);
            }
        }
    }
    //    ribbonBar()->setLogoPixmap(Qt::AlignRight, QPixmap(":/res/logo.png"));
    ribbonBar()->setFrameThemeEnabled();
}

void MainWindow::createStatusBar()
{
    QRibbon::RibbonSliderPane *sliderPane = new QRibbon::RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 100);   // Range
    sliderPane->setMaximumWidth(130);
    sliderPane->setSingleStep(10);
    sliderPane->setSliderPosition(50);
    statusBar()->addPermanentWidget(sliderPane);
    statusBar();
}

void MainWindow::pressLogoRight()
{
    ribbonBar()->setLogoPixmap(Qt::AlignRight, QPixmap(":/res/logo.png"));
    m_actionLogoRight->setChecked(true);
    m_actionLogoLeft->setChecked(false);
}

void MainWindow::pressLogoLeft()
{
    ribbonBar()->setLogoPixmap(Qt::AlignLeft, QPixmap(":/res/logo.png"));
    m_actionLogoRight->setChecked(false);
    m_actionLogoLeft->setChecked(true);
}
