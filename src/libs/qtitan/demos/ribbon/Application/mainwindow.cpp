#include <qglobal.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QClipboard>
#include <QDesktopServices>
#include <QTextEdit>
#include <QTextCodec>
#include <QLineEdit>
#include <QTextDocumentWriter>
#include <QAbstractTextDocumentLayout>
#include <QFileDialog>
#include <QFile>
#include <QPrinter>
#include <QSpinBox>
#include <QPrintDialog>
#include <QFontComboBox>
#include <QTextDocument>
#include <QLabel>
#include <QToolBar>
#include <QDockWidget>
#include <QListWidget>
#include <QPrintPreviewDialog>
#include <QTextList>
#include <QColorDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QDockWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QListWidget>
#include <QBuffer>
#include <QUrl>

#include <QtnRibbonStyle.h>
#include <QtnRibbonCustomizePage.h>

#include "aboutdialog.h"
#include "ribbonpopularpage.h"

#include "mainwindow.h"
#include "ui_ribbonsample.h"

using namespace Qtitan;

static QString categoryPopularCommands = QObject::tr("Popular Commands");
static QString categoryAllCommands     = QObject::tr("All Commands");
static QString categoryMainPages       = QObject::tr("Main Pages");


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : RibbonWindow(parent)
{
    m_comboFont = NULL;
    setRibbonBackground(QString(":/res/calligraphy.png"));

    createAction();
    createMenuFile();
    createQuickAccessBar();
 
    createRibbonBar();
    createStatusBar();
    createDockWindows();

    m_textEdit = new QTextEdit(this);

    connect(m_textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

    setCentralWidget(m_textEdit);
    m_textEdit->setFocus();

    fontChanged(m_textEdit->font());
    colorChanged(m_textEdit->textColor());
    alignmentChanged(m_textEdit->alignment());

    connect(m_textEdit->document(), SIGNAL(modificationChanged(bool)), m_actionSaveFile, SLOT(setEnabled(bool)));
    connect(m_textEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));

    connect(m_textEdit->document(), SIGNAL(undoAvailable(bool)), m_actionUndo, SLOT(setEnabled(bool)));
    connect(m_textEdit->document(), SIGNAL(redoAvailable(bool)), m_actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(m_textEdit->document()->isModified());
    m_actionSaveFile->setEnabled(m_textEdit->document()->isModified());
    m_actionUndo->setEnabled(m_textEdit->document()->isUndoAvailable());
    m_actionRedo->setEnabled(m_textEdit->document()->isRedoAvailable());

    connect(m_actionUndo, SIGNAL(triggered()), m_textEdit, SLOT(undo()));
    connect(m_actionRedo, SIGNAL(triggered()), m_textEdit, SLOT(redo()));

    m_actionCut->setEnabled(false);
    m_actionCopy->setEnabled(false);

    connect(m_actionCut, SIGNAL(triggered()), m_textEdit, SLOT(cut()));
    connect(m_actionCopy, SIGNAL(triggered()), m_textEdit, SLOT(copy()));
    connect(m_actionPaste1, SIGNAL(triggered()), m_textEdit, SLOT(paste()));
    connect(m_actionPaste2, SIGNAL(triggered()), m_textEdit, SLOT(paste()));

    connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_actionCut, SLOT(setEnabled(bool)));
    connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_actionCopy, SLOT(setEnabled(bool)));

#ifndef QT_NO_CLIPBOARD
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
#endif

    connect(m_textEdit, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    connect(ribbonBar(), SIGNAL(showRibbonContextMenu(QMenu*, QContextMenuEvent*)), this, SLOT(showRibbonContextMenu(QMenu*, QContextMenuEvent*)));
    ribbonBar()->setFrameThemeEnabled();
    m_stateWindow = windowState();

    QString initialFile = ":/res/example.html";
    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
        initialFile = args.at(1);

    customization();

    if (!load(initialFile))
        fileNew();
    readSettings();
}

MainWindow::~MainWindow()
{
    delete m_itemsStyles;
}

QString MainWindow::ribbonBackground() const
{
    return m_curRibbonBackground;
}

void MainWindow::setRibbonBackground(const QString& name)
{
    if (m_curRibbonBackground != name)
    {
        m_curRibbonBackground = name;
        updateRibbonBackground();
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave())
        event->accept();
    else
        event->ignore();
    writeSettings();
}

void MainWindow::createAction()
{
    m_actionFileNew = new QAction(::qtnIcon(Image_New), tr("&New"), this);
    m_actionFileNew->setPriority(QAction::LowPriority);
    m_actionFileNew->setShortcut(QKeySequence::New);
    m_actionFileNew->setStatusTip(tr("Create a new document"));
    m_actionFileNew->setToolTip(tr("New"));
    connect(m_actionFileNew, SIGNAL(triggered()), this, SLOT(fileNew()));

    m_actionOpenFile = new QAction(::qtnIcon(Image_Open), tr("&Open..."), this);
    m_actionOpenFile->setShortcut(QKeySequence::Open);
    m_actionOpenFile->setToolTip(tr("Open"));
    m_actionOpenFile->setStatusTip(tr("Open an existing document"));
    connect(m_actionOpenFile, SIGNAL(triggered()), this, SLOT(fileOpen()));

    m_actionSaveFile = new QAction(::qtnIcon(Image_Save), tr("&Save"), this);
    m_actionSaveFile->setShortcut(QKeySequence::Save);
    m_actionSaveFile->setToolTip(tr("Save"));
    m_actionSaveFile->setStatusTip(tr("Save the active document"));
    m_actionSaveFile->setEnabled(false);
    connect(m_actionSaveFile, SIGNAL(triggered()), this, SLOT(fileSave()));

    m_actionPrint = new QAction(::qtnIcon(Image_Print), tr("&Print..."), this);
    m_actionPrint->setPriority(QAction::LowPriority);    
    m_actionPrint->setShortcut(QKeySequence::Print);
    m_actionPrint->setToolTip(tr("Select a printer, number of copies, and other printing options before printing"));
    connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

    m_actionPrintSetup = new QAction(QIcon(":/res/largePrintSetup.png"), tr("P&rint Setup"), this);
    m_actionPrintSetup->setToolTip(tr("Change the printer and printing options"));

    m_actionPrintPreview = new QAction(QIcon(":/res/largePrintPreview.png"), tr("Print Pre&view..."), this);
    m_actionPrintPreview->setToolTip(tr("Preview and make changes to pages before printing"));
    connect(m_actionPrintPreview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
//    iconLogo.addPixmap(QPixmap(":/res/file.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitanlogo32x32.png"));
    if(QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) 
    {
        actionFile->setToolTip(tr("Click here to see everything you can do with your document"));
        RibbonSystemPopupBar* popupBar = qobject_cast<RibbonSystemPopupBar*>(actionFile->menu());

        popupBar->addAction(m_actionFileNew);
        popupBar->addAction(m_actionOpenFile);
        popupBar->addAction(m_actionSaveFile);

        QAction* actionSaveAsFile = popupBar->addAction(::qtnIcon(Image_SaveAs), tr("Save &As..."));
        actionSaveAsFile->setPriority(QAction::LowPriority);
        actionSaveAsFile->setToolTip(tr("Save As"));
        actionSaveAsFile->setStatusTip(tr("Save the active document with a new name"));
        connect(actionSaveAsFile, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

        popupBar->addSeparator();

        QAction* actionPagePrint = new QAction(m_actionPrint->icon(), m_actionPrint->text(), this);
        popupBar->addAction(actionPagePrint);
        if (RibbonPageSystemPopup* pageSystemPopup = popupBar->addPageSystemPopup(tr("Preview and print the document"), actionPagePrint, true))
        {
            connect(actionPagePrint, SIGNAL(triggered()), m_actionPrint, SIGNAL(triggered()));
            pageSystemPopup->setMinimumWidth(296);
            pageSystemPopup->addAction(m_actionPrint);
            pageSystemPopup->addAction(m_actionPrintSetup);
            pageSystemPopup->addAction(m_actionPrintPreview);
        }

        QAction* actionPrepare = popupBar->addAction(QIcon(":/res/largePrepare.png"), tr("Pr&epare"));
        popupBar->addAction(actionPrepare);
        
        QAction* actionSend = popupBar->addAction(QIcon(":/res/largeSend.png"), tr("Sen&d"));
        popupBar->addAction(actionSend);
        if (RibbonPageSystemPopup* pageSystemPopup = popupBar->addPageSystemPopup(tr("Preview and print the document"), actionSend, false))
        {
            pageSystemPopup->setMinimumWidth(296);
            QAction* actionMail = pageSystemPopup->addAction(QIcon(":/res/largeMail.png"), tr("E-Mail"));
            actionMail->setToolTip(tr("Send the active document by e-mail"));
            pageSystemPopup->addAction(actionMail);

            QAction* actionIntenetFax = pageSystemPopup->addAction(QIcon(":/res/largeInternetfix.png"), tr("Intenet Fax"));
            actionIntenetFax->setToolTip(tr("Use an Internet fax service to fax the document"));
            pageSystemPopup->addAction(actionIntenetFax);
        }

        popupBar->addSeparator();
        QAction* actionClose = popupBar->addAction(::qtnIcon(Image_Close), tr("&Close"));
        actionClose->setShortcut(tr("Ctrl+C"));
        actionClose->setStatusTip(tr("Exit"));
        actionClose->setEnabled(false);

        QAction* actionExit =  popupBar->addPopupBarAction(tr("Exit Sample"));
        connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

        QAction* actionOption = popupBar->addPopupBarAction(tr("Option"));
        actionOption->setEnabled(false);

        if (RibbonPageSystemRecentFileList* pageRecentFile = popupBar->addPageRecentFile(tr("Recent Documents")))
        {
            Q_UNUSED(pageRecentFile);
        }
    }
}

void MainWindow::createQuickAccessBar()
{
    if (RibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        quickAccessBar->addAction(m_actionFileNew);
        quickAccessBar->setActionVisible(m_actionFileNew, false);

        quickAccessBar->addAction(m_actionOpenFile);
        quickAccessBar->setActionVisible(m_actionOpenFile, false);

        quickAccessBar->addAction(m_actionSaveFile);

        m_actionUndo = quickAccessBar->addAction(::qtnIcon(Image_Undo), tr("&Undo"));
        m_actionUndo->setShortcut(QKeySequence::Undo);
        m_actionRedo = quickAccessBar->addAction(::qtnIcon(Image_Redo), tr("&Redo"));
        m_actionRedo->setShortcut(QKeySequence::Redo);
        quickAccessBar->addAction(m_actionPrint);
        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbonBar()
{
    //RibbonBarAutoUpdater updater(ribbonBar());

    m_pageHome = ribbonBar()->addPage(tr("&Home"));
    if (m_pageHome)
    {
        createGroupClipboard(m_pageHome);
        createGroupFont(m_pageHome);
        createGroupParagraph(m_pageHome);
        createGroupStyles(m_pageHome);
        createGroupEditing(m_pageHome);
    }

    m_pageInsert = ribbonBar()->addPage(tr("&Insert"));
    if (m_pageInsert)
    {
        createGroupPages(m_pageInsert);
        createGroupTables(m_pageInsert);
        createGroupIllustrations(m_pageInsert);
        createGroupLinks(m_pageInsert);
        createGroupHeaderFooter(m_pageInsert);
        createGroupText(m_pageInsert);
        createGroupSymbols(m_pageInsert);
    }

    m_pageLayout = ribbonBar()->addPage(tr("&Page Layout"));
    if (m_pageLayout)
    {
        createGroupThemes(m_pageLayout->addGroup(tr("Themes")));
        createGroupPageSetup(m_pageLayout->addGroup(tr("Page Setup")));
        createGroupPageBackground(m_pageLayout->addGroup(tr("Page Background")));
        createGroupParagraphLayout(m_pageLayout->addGroup(tr("Paragraph")));
    }

    m_pageRef = ribbonBar()->addPage(tr("Reference&s"));
    if (m_pageRef)
    {
        createGroupTableOfContents(m_pageRef->addGroup(tr("Table of Contents")));
        createGroupFootnotes(m_pageRef->addGroup(tr("&Footnotes")));
        createGroupCaptions(m_pageRef->addGroup(tr("Captions")));
        createGroupIndex(m_pageRef->addGroup(tr("Index")));
    }

    m_pageView = ribbonBar()->addPage(tr("View"));
    if (m_pageView)
    {
        createGroupDocumentViews(m_pageView->addGroup(tr("Document Views")));
        createGroupShow_Hide(m_pageView->addGroup(tr("Show/Hide")));
        createGroupZoom(m_pageView->addGroup(tr("Zoom")));
    }

    m_pagePictureFormat = ribbonBar()->addPage(tr("Picture Format"));
    if (m_pagePictureFormat)
    {
        m_pagePictureFormat->setContextTitle(tr("Picture Tools"));
        m_pagePictureFormat->setContextColor(RibbonPage::ContextColorRed);
        m_pagePictureFormat->setVisible(false);
    }
}

void MainWindow::createGroupClipboard(RibbonPage* page)
{
    if (RibbonGroup* groupClipboard = page->addGroup(::qtnIcon(Image_Clipboard), tr("Clipboard"))) 
    {
        groupClipboard->setOptionButtonVisible();
        QAction* act = groupClipboard->optionButtonAction();
        act->setText(tr("Clipboard"));
        act->setIcon(QIcon(":/res/clipboardToolTip.png"));
        act->setToolTip(tr("Show the Office clipboard Task Pane"));
        act->setStatusTip(tr("Show the Office clipboard Task Pane"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionClipboard()));

        QMenu* editPaste = new QMenu(this);
        m_actionPaste1 = editPaste->addAction(::qtnIcon(Image_Clipboard), tr("Paste"));
        m_actionPaste1->setPriority(QAction::LowPriority);
        m_actionPaste1->setShortcut(QKeySequence::Paste);

        editPaste->addAction(tr("Paste Special"));

        m_actionPaste2 = groupClipboard->addAction(::qtnIcon(Image_Clipboard), tr("&Paste"), Qt::ToolButtonTextUnderIcon, editPaste);
        RibbonControl* control = groupClipboard->controlByAction(m_actionPaste2);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        m_actionPaste2->setPriority(QAction::LowPriority);
        m_actionPaste2->setShortcut(QKeySequence::Paste);
        m_actionPaste2->setToolTip(tr("Insert Clipboard contents"));
        m_actionPaste1->setEnabled(!QApplication::clipboard()->text().isEmpty());
        m_actionPaste2->setEnabled(!QApplication::clipboard()->text().isEmpty());

        m_actionCut = groupClipboard->addAction(QIcon(":/res/smallcut.png"), tr("&Cut"), Qt::ToolButtonTextBesideIcon);
        control = groupClipboard->controlByAction(m_actionCut);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        m_actionCut->setShortcut(QKeySequence::Cut);
        m_actionCut->setToolTip(tr("Cut the selection and put it on the Clipboard"));

        m_actionCopy = groupClipboard->addAction(QIcon(":/res/smallcopy.png"), tr("&Copy"), Qt::ToolButtonTextBesideIcon);
        control = groupClipboard->controlByAction(m_actionCopy);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        m_actionCopy->setShortcut(QKeySequence::Copy);

        m_actionFormatPointerAction = groupClipboard->addAction(QIcon(":/res/smallformatpainter.png"), tr("F&ormat Pointer"), Qt::ToolButtonTextBesideIcon);
        control = groupClipboard->controlByAction(m_actionFormatPointerAction);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    }
}

void MainWindow::createGroupFont(RibbonPage* page)
{
    if (RibbonGroup* groupFont = page->addGroup(QIcon(":/res/smallfontgrow.png"), tr("Font"))) 
    {
        groupFont->setOptionButtonVisible();
        QAction* act = groupFont->optionButtonAction();
        act->setText(tr("F&ont"));
        act->setIcon(QIcon(":/res/fontToolTip.png"));
        act->setToolTip(tr("Show the Font dialog box"));
        act->setStatusTip(tr("Show the Font dialog box"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionFont()));

        RibbonToolBarControl* toolBarControl = new RibbonToolBarControl(groupFont);
        m_comboFont = new QFontComboBox();
        m_comboFont->setMaximumWidth(130);
        toolBarControl->addWidget(m_comboFont);
        connect(m_comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

        m_comboSize = new QComboBox();
        m_comboSize->setMaximumWidth(45);
        m_comboSize->setObjectName("m_comboSize");
        toolBarControl->addWidget(m_comboSize);
        m_comboSize->setEditable(false);

        QFontDatabase db;
        foreach(int size, db.standardSizes())
            m_comboSize->addItem(QString::number(size));

        connect(m_comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
        m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(QApplication::font().pointSize())));

        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smallfontgrow.png"), tr("Grow Font"));
        toolBarControl->addAction(QIcon(":/res/smallfontshrink.png"), tr("Shrink Font"));
        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smallfontclear.png"), tr("&Clear Formatting"));
        toolBarControl->addSeparator();
        m_actionTextBold = toolBarControl->addAction(QIcon(":/res/smalltextbold.png"), tr("&Bold"));
        m_actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
        m_actionTextBold->setPriority(QAction::LowPriority);
        QFont bold;
        bold.setBold(true);
        m_actionTextBold->setFont(bold);
        connect(m_actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
        m_actionTextBold->setCheckable(true);

        m_actionTextItalic = toolBarControl->addAction(QIcon(":/res/smalltextitalic.png"), tr("&Italic"));
        m_actionTextItalic->setPriority(QAction::LowPriority);
        m_actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
        QFont italic;
        italic.setItalic(true);
        m_actionTextItalic->setFont(italic);
        connect(m_actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
        m_actionTextItalic->setCheckable(true);

        m_actionTextUnderline = toolBarControl->addAction(QIcon(":/res/smalltextunder.png"), tr("&Underline"));
        m_actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
        m_actionTextUnderline->setPriority(QAction::LowPriority);
        QFont underline;
        underline.setUnderline(true);
        m_actionTextUnderline->setFont(underline);
        connect(m_actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
        m_actionTextUnderline->setCheckable(true);
        QAction* charStrikethrough = toolBarControl->addAction(QIcon(":/res/smallstrikethrough.png"), tr("Strikethrough"));
        charStrikethrough->setEnabled(false);
        toolBarControl->addSeparator();
        QAction* textSubscript = toolBarControl->addAction(QIcon(":/res/smallsubscript.png"), tr("Subscript"));
        textSubscript->setEnabled(false);
        QAction* textSuperscript = toolBarControl->addAction(QIcon(":/res/smallsuperscript.png"), tr("Superscript"));
        textSuperscript->setEnabled(false);
        QAction* textChangecase = toolBarControl->addAction(QIcon(":/res/smallchangecase.png"), tr("Change Case"));
        textChangecase->setEnabled(false);

        toolBarControl->addSeparator();

        PopupColorButton* highlightColorButton = new PopupColorButton();
        QAction* textHighlightcolor = toolBarControl->addWidget(QIcon(":/res/smallcolor.png"), tr("Highlight Color"), highlightColorButton);
        highlightColorButton->setDefaultAction(textHighlightcolor);
        textHighlightcolor->setEnabled(false);

        m_colorButton = new PopupColorButton();
        m_actionTextColor = toolBarControl->addWidget(QIcon(":/res/smallcolor.png"), tr("Color"), m_colorButton);
        m_colorButton->setDefaultAction(m_actionTextColor);
        connect(m_colorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(textColor(const QColor&)));
        connect(m_actionTextColor, SIGNAL(triggered()), this, SLOT(setColorText()));

        groupFont->addControl(toolBarControl);
    }
}

void MainWindow::createGroupParagraph(RibbonPage* page)
{
    if (RibbonGroup* groupParagraph = page->addGroup(QIcon(":/res/smallcenter.png"), tr("&Paragraph"))) 
    {
        groupParagraph->setOptionButtonVisible();
        QAction* act = groupParagraph->optionButtonAction();
        act->setText(tr("Paragraph"));
        act->setIcon(QIcon(":/res/paragraphToolTip.png"));
        act->setToolTip(tr("Show the Paragraph dialog box"));
        act->setStatusTip(tr("Show the Paragraph dialog box"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionParagraph()));

        RibbonToolBarControl* toolBarControl = new RibbonToolBarControl(groupParagraph);

        toolBarControl->addAction(QIcon(":/res/smallbullets.png"),
            tr("Bullets"), Qt::ToolButtonIconOnly, new QMenu(this), QToolButton::InstantPopup);
        toolBarControl->addAction(QIcon(":/res/smallnumbering.png"),
            tr("Numbering"), Qt::ToolButtonIconOnly, new QMenu(this), QToolButton::InstantPopup);
        toolBarControl->addAction(QIcon(":/res/smallmultilevellist.png"),
            tr("Multilevel List"), Qt::ToolButtonIconOnly, new QMenu(this), QToolButton::InstantPopup);
        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smalldecreaseindent.png"),
            tr("Decrease Indent"), Qt::ToolButtonIconOnly);
        toolBarControl->addAction(QIcon(":/res/smallincreaseindent.png"),
            tr("Increase Indent"), Qt::ToolButtonIconOnly);
        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smallsort.png"),
            tr("Sort"), Qt::ToolButtonIconOnly);
        toolBarControl->addSeparator();
        QAction* action = toolBarControl->addAction(QIcon(":/res/smallshow_hide_marks.png"),
            tr("Show/Hide Marks"), Qt::ToolButtonIconOnly);
        action->setCheckable(false);
        connect(action, SIGNAL(triggered()), this, SLOT(showHideMarks()));

        toolBarControl->addSeparator();
        QActionGroup*grp = new QActionGroup(this);
        connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

        m_actionAlignLeft = toolBarControl->addAction(QIcon(":/res/smallalignleft.png"),
            tr("Align Left"), Qt::ToolButtonIconOnly);
        m_actionAlignLeft->setActionGroup(grp);
        m_actionAlignCenter = toolBarControl->addAction(QIcon(":/res/smallcenter.png"),
            tr("Center"), Qt::ToolButtonIconOnly);
        m_actionAlignCenter->setActionGroup(grp);
        m_actionAlignRight = toolBarControl->addAction(QIcon(":/res/smallalignright.png"),
            tr("Align Right"), Qt::ToolButtonIconOnly);
        m_actionAlignRight->setActionGroup(grp);
        m_actionAlignJustify = toolBarControl->addAction(QIcon(":/res/smalljustify.png"),
            tr("Justify"), Qt::ToolButtonIconOnly);
        m_actionAlignJustify->setActionGroup(grp);

        m_actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
        m_actionAlignLeft->setCheckable(true);
        m_actionAlignLeft->setPriority(QAction::LowPriority);
        m_actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
        m_actionAlignCenter->setCheckable(true);
        m_actionAlignCenter->setPriority(QAction::LowPriority);
        m_actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
        m_actionAlignRight->setCheckable(true);
        m_actionAlignRight->setPriority(QAction::LowPriority);
        m_actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
        m_actionAlignJustify->setCheckable(true);
        m_actionAlignJustify->setPriority(QAction::LowPriority);

        toolBarControl->addSeparator();

        QMenu* menuLineSpacing = new QMenu(this);
        act = new QAction("1.0", this);
        act->setCheckable(true); act->setChecked(true);
        menuLineSpacing->addAction(act);
        act = new QAction("1.15", this); act->setCheckable(true);
        menuLineSpacing->addAction(act);
        act = new QAction("1.5", this); act->setCheckable(true);
        menuLineSpacing->addAction(act);
        act = new QAction("2.0", this); act->setCheckable(true);
        menuLineSpacing->addAction(act);
        act = new QAction("2.5", this); act->setCheckable(true);
        menuLineSpacing->addAction(act);
        act = new QAction("3.0", this); act->setCheckable(true);
        menuLineSpacing->addAction(act);
        toolBarControl->addMenu(QIcon(":/res/smalllinespacing.png"), tr("Line spacing"), menuLineSpacing, QToolButton::InstantPopup);

        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smallshading.png"),
            tr("Shading"), Qt::ToolButtonIconOnly, new QMenu(this), QToolButton::InstantPopup);
        toolBarControl->addSeparator();
        toolBarControl->addAction(QIcon(":/res/smallnoborder.png"),
            tr("No Border"), Qt::ToolButtonIconOnly, new QMenu(this), QToolButton::InstantPopup);

        groupParagraph->addControl(toolBarControl);
    }
}

void MainWindow::createGroupStyles(RibbonPage* page)
{
    if (RibbonGroup* groupStyles = page->addGroup(QIcon(":/res/smallcenter.png"), tr("&Styles"))) 
    {
        RibbonGalleryControl* galleryControl = new RibbonGalleryControl();
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMaximumItemCount(16);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setMinimumItemCount(16);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMaximumItemCount(15);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setMinimumItemCount(4);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMaximumItemCount(3);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setMinimumItemCount(3);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMaximumItemCount(4);
        galleryControl->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setMinimumItemCount(4);
        groupStyles->addControl(galleryControl);

        m_itemsStyles = RibbonGalleryGroup::createGalleryGroup();
        m_itemsStyles->setSize(QSize(72, 56));

        QPixmap px(":/res/galleryStyles.png");

        for (int numStyle = 0; numStyle < 16; numStyle++)
        {
            RibbonGalleryItem* item = m_itemsStyles->addItemFromMap(QString(), numStyle, px, QSize(64, 48));
            QString str = QObject::tr("Style, %1").arg(numStyle + 1);
            item->setToolTip(str);
        }

        RibbonGallery* widgetStyleFont = galleryControl->widget();
        widgetStyleFont->setLabelsVisible(false);
        widgetStyleFont->setBorderVisible(true);
        widgetStyleFont->setGalleryGroup(m_itemsStyles);
        widgetStyleFont->setCheckedIndex(0);

        OfficePopupMenu* popup = OfficePopupMenu::createPopupMenu(this);
        widgetStyleFont->setPopupMenu(popup)->setText(tr("&Styles"));

        RibbonGallery* popupStyleFont = new RibbonGallery;
        popup->setMinimumSize(QSize(307, 168));
        popupStyleFont->resize(QSize(307, 168));

        popupStyleFont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        popupStyleFont->setGalleryGroup(m_itemsStyles);
        popupStyleFont->setCheckedIndex(0);
//        connect(popupStyleFont, SIGNAL(itemPressed(RibbonGalleryItem*)), this, SLOT(itemStyleFontPressed(RibbonGalleryItem*)));
        popup->addWidget(popupStyleFont);

        popup->addSeparator();
        popup->addAction(tr("&Save Style"));
        popup->addAction(tr("&Apply Style"));
    }
}

void MainWindow::createGroupEditing(RibbonPage* page)
{
    if (RibbonGroup* groupEditing = page->addGroup(QIcon(":/res/smallfind.png"), tr("Editing"))) 
    {
        QAction* actionFind = new QAction(QIcon(":/res/smallfind.png"), tr("Find"), this);
        actionFind->setShortcut(Qt::CTRL + Qt::Key_F);
        actionFind->setStatusTip(tr("Find the specified text"));

        QAction* actionGoto = new QAction(QIcon(":/res/smallgoto.png"), tr("Go To"), this);
        actionGoto->setStatusTip(tr("Navigate to a specific page, line, numer, footnote, comment, or other object"));
        actionGoto->setEnabled(false);

        QMenu* findMenu = new QMenu(this);
        findMenu->addAction(actionFind);
        findMenu->addAction(actionGoto);
        RibbonControl* control = groupEditing->controlByAction(groupEditing->addAction(actionFind, Qt::ToolButtonTextBesideIcon, findMenu));
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);

        QAction* actionReplace = groupEditing->addAction(QIcon(":/res/smallreplace.png"), tr("Replace"), Qt::ToolButtonTextBesideIcon);
        actionReplace->setShortcut(Qt::CTRL + Qt::Key_H);
        actionReplace->setStatusTip(tr("Replace specific text with different text"));
        actionReplace->setEnabled(false);
        control = groupEditing->controlByAction(actionReplace);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
            
        QMenu* selectMenu = groupEditing->addMenu(QIcon(":/res/smallselect.png"), tr("Select"), Qt::ToolButtonTextBesideIcon);
        control = groupEditing->controlByAction(selectMenu->menuAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);

        m_actionSelectAll = new QAction(tr("Select All"), this);
        m_actionSelectAll->setShortcut(Qt::CTRL + Qt::Key_A);
        m_actionSelectAll->setStatusTip(tr("Select the entire document"));
        selectMenu->addAction(m_actionSelectAll);

        connect(m_actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

        QAction* actionSelectObjects = new QAction(tr("Select Objects"), this);
        actionSelectObjects->setEnabled(false);
        selectMenu->addAction(actionSelectObjects);

        QAction* pActionSelectMultipleObjects = new QAction(tr("Select Multiple Objects"), this);
        pActionSelectMultipleObjects->setEnabled(false);
        selectMenu->addAction(pActionSelectMultipleObjects);
    }
}

void MainWindow::createGroupPages(RibbonPage* page)
{
    if (RibbonGroup* groupPages = page->addGroup(QIcon(":/res/pagesgroup16x16.png"), tr("Pages"))) 
    {
        QMenu* menu = groupPages->addMenu(qtnIcon(":/res/coverpage16x16.png", ":/res/coverpage32x32.png"), tr("Cover Page"));
        RibbonControl* control = groupPages->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        QAction* act = groupPages->addAction(qtnIcon(":/res/blankpage16x16.png", ":/res/blankpage32x32.png"), tr("Blank Page"), Qt::ToolButtonTextBesideIcon);
        control = groupPages->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupPages->addAction(qtnIcon(":/res/pagebreak16x16.png", ":/res/pagebreak32x32.png"), tr("Page Break"), Qt::ToolButtonTextBesideIcon);
        control = groupPages->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    }
}

void MainWindow::createGroupTables(RibbonPage* page)
{
    if (RibbonGroup* groupTables = page->addGroup(QIcon(":/res/tablegroup16x16.png"), tr("Tables"))) 
    {
        QMenu* menu = groupTables->addMenu(qtnIcon(":/res/table16x16.png", ":/res/table32x32.png"), tr("Table"));
        RibbonControl* control = groupTables->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    }
}

void MainWindow::createGroupIllustrations(RibbonPage* page)
{
    if (RibbonGroup* groupIllustrations = page->addGroup(QIcon(":/res/illustrationsgroup16x16.png"), tr("Illustrations"))) 
    {
        QAction* act = groupIllustrations->addAction(qtnIcon(":/res/picture16x16.png", ":/res/picture32x32.png"), tr("Picture"), Qt::ToolButtonTextBesideIcon);
        RibbonControl* control = groupIllustrations->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupIllustrations->addAction(qtnIcon(":/res/clipart16x16.png", ":/res/clipart32x32.png"), tr("Clip Art"), Qt::ToolButtonTextBesideIcon);
        control = groupIllustrations->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        QMenu* menu = groupIllustrations->addMenu(qtnIcon(":/res/shapes16x16.png", ":/res/shapes32x32.png"), tr("Shapes"));
        control = groupIllustrations->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupIllustrations->addAction(qtnIcon(":/res/smartart16x16.png", ":/res/smartart32x32.png"), tr("Smart Art"), Qt::ToolButtonTextBesideIcon);
        control = groupIllustrations->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupIllustrations->addAction(qtnIcon(":/res/chart16x16.png", ":/res/chart32x32.png"), tr("Chart"), Qt::ToolButtonTextBesideIcon);
        control = groupIllustrations->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    }
}

void MainWindow::createGroupLinks(RibbonPage* page)
{
    if (RibbonGroup* groupLinks = page->addGroup(QIcon(":/res/linkgroup16x16.png"), tr("Links"))) 
    {
        QAction* act = groupLinks->addAction(qtnIcon(":/res/hyperlink16x16.png", ":/res/hyperlink32x32.png"), tr("Hyperlink"), Qt::ToolButtonTextBesideIcon);
        RibbonControl* control = groupLinks->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupLinks->addAction(qtnIcon(":/res/bookmark16x16.png", ":/res/bookmark32x32.png"), tr("Bookmark"), Qt::ToolButtonTextBesideIcon);
        control = groupLinks->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        act = groupLinks->addAction(qtnIcon(":/res/crossref16x16.png", ":/res/crossref32x32.png"), tr("Cross-reference"), Qt::ToolButtonTextBesideIcon);
        control = groupLinks->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    }
}

void MainWindow::createGroupHeaderFooter(RibbonPage* page)
{
    if (RibbonGroup* groupHeaderFooter = page->addGroup(QIcon(":/res/headerfootergroup16x16.png"), tr("Header/Footer"))) 
    {
        QMenu* menu = groupHeaderFooter->addMenu(qtnIcon(":/res/header16x16.png", ":/res/header32x32.png"), tr("Header"));
        RibbonControl* control = groupHeaderFooter->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);

        menu = groupHeaderFooter->addMenu(qtnIcon(":/res/footer16x16.png", ":/res/footer32x32.png"), tr("Footer"));
        control = groupHeaderFooter->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);

        menu = groupHeaderFooter->addMenu(qtnIcon(":/res/pagenumber16x16.png", ":/res/pagenumber32x32.png"), tr("Page Number"));
        control = groupHeaderFooter->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    }
}

void MainWindow::createGroupText(RibbonPage* page)
{
    if (RibbonGroup* groupText = page->addGroup(QIcon(":/res/textgroup16x16.png"), tr("Text"))) 
    {
        QMenu* menu = groupText->addMenu(qtnIcon(":/res/textbox16x16.png", ":/res/textbox32x32.png"), tr("Text Box"));
        RibbonControl* control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        menu = groupText->addMenu(qtnIcon(":/res/quickparts16x16.png", ":/res/quickparts32x32.png"), tr("Quick Parts"));
        control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        menu = groupText->addMenu(qtnIcon(":/res/wordart16x16.png", ":/res/wordart32x32.png"), tr("WordArt"));
        control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        menu = groupText->addMenu(qtnIcon(":/res/dropcap16x16.png", ":/res/dropcap32x32.png"), tr("Drop Cap"));
        control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        menu = groupText->addMenu(qtnIcon(":/res/signatureline16x16.png", ""), tr("Signature line"));
        control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);

        QAction* act = groupText->addAction(qtnIcon(":/res/datatime16x16.png", ""), tr("Data / Time"), Qt::ToolButtonTextUnderIcon);
        control = groupText->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);

        menu = groupText->addMenu(qtnIcon(":/res/object16x16.png", ""), tr("Object"));
        control = groupText->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);
    }
}

void MainWindow::createGroupSymbols(RibbonPage* page)
{
    if (RibbonGroup* groupSymbols = page->addGroup(QIcon(":/res/symbolsgroup16x16.png"), tr("Symbols")))
    {
        QAction* act = groupSymbols->addAction(qtnIcon(":/res/equation16x16.png", ":/res/equation32x32.png"), tr("Equation"), Qt::ToolButtonTextBesideIcon);
        RibbonControl* control = groupSymbols->controlByAction(act);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

        QMenu* menu = groupSymbols->addMenu(qtnIcon(":/res/symbol16x16.png", ":/res/symbol32x32.png"), tr("Symbol"));
        control = groupSymbols->controlByAction(menu->defaultAction());
        control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        control->sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    }
}

void MainWindow::createGroupThemes(RibbonGroup* group)
{
    QMenu* themesMenu = new QMenu(this);
    QAction* action = group->addAction(QIcon(":/res/largeThemes.png"), tr("Themes"),
        Qt::ToolButtonTextUnderIcon, themesMenu);
    RibbonControl* control = group->controlByAction(action);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    QMenu* colors = new QMenu(this);
    group->addAction(QIcon(":/res/smallcolors.png"), tr("Colors"), Qt::ToolButtonTextBesideIcon, colors)->setEnabled(false);
    QMenu* fonts = new QMenu(this);
    group->addAction(QIcon(":/res/smallfonts.png"), tr("Fonts"), Qt::ToolButtonTextBesideIcon, fonts);
    QMenu* effects = new QMenu(this);
    group->addAction(QIcon(":/res/smalleffects.png"), tr("Effects"), Qt::ToolButtonTextBesideIcon, effects);
}

void MainWindow::createGroupPageSetup(RibbonGroup* group)
{
    QMenu* menu = group->addMenu(QIcon(":/res/largeMargins.png"), tr("Margins"));
    RibbonControl* control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    menu = group->addMenu(qtnIcon(":/res/orientation16x16.png", ":/res/orientation32x32.png"), tr("Orientation"));
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);

    menu = group->addMenu(qtnIcon(":/res/size16x16.png", ":/res/size32x32.png"), tr("Size"));
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);

    menu = group->addMenu(qtnIcon(":/res/columns16x16.png", ":/res/columns32x32.png"), tr("Columns"));
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);

    menu = group->addMenu(QIcon(":/res/smallBreaks.png"), tr("Breaks"), Qt::ToolButtonTextBesideIcon);
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);

    menu = group->addMenu(QIcon(":/res/smalllinenumbers.png"), tr("Line Numbers"), Qt::ToolButtonTextBesideIcon);
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);

    menu = group->addMenu(QIcon(":/res/smallhyphenation.png"), tr("Hyphenation"), Qt::ToolButtonTextBesideIcon);
    control = group->controlByAction(menu->defaultAction());
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
}

void MainWindow::createGroupPageBackground(RibbonGroup* group)
{
    group->addMenu(qtnIcon(":/res/watermark16x16.png", ":/res/watermark32x32.png"), tr("Watermark"));
    group->addMenu(qtnIcon(":/res/pagecolor16x16.png", ":/res/pagecolor32x32.png"), tr("Page Color"));
    group->addAction(qtnIcon(":/res/pageborders16x16.png", ":/res/pageborders32x32.png"), tr("Page Borders"), Qt::ToolButtonTextUnderIcon);
}

void MainWindow::createGroupParagraphLayout(RibbonGroup* group)
{
    group->addControl(new RibbonLabelControl(tr("Indent")));
    RibbonDoubleSpinBoxControl* spinBoxControl = new RibbonDoubleSpinBoxControl();
    spinBoxControl->setDefaultAction(new QWidgetAction(this));
    spinBoxControl->defaultAction()->setIcon(QIcon(":/res/smallleft.png"));
    spinBoxControl->defaultAction()->setText(QObject::tr("Left:"));
    group->addControl(spinBoxControl);

    QDoubleSpinBox* doubleSpinBox = spinBoxControl->widget();
    doubleSpinBox->setValue(0.3);
    doubleSpinBox->setDecimals(1);
    doubleSpinBox->setSuffix(tr("\""));
    doubleSpinBox->setRange(0.0, 22.0);
    doubleSpinBox->setMinimumWidth(60);
    doubleSpinBox->setMaximumWidth(60);

    spinBoxControl = new RibbonDoubleSpinBoxControl();
    spinBoxControl->setDefaultAction(new QWidgetAction(this));
    spinBoxControl->defaultAction()->setIcon(QIcon(":/res/smallright.png"));
    spinBoxControl->defaultAction()->setText(QObject::tr("Right:"));
    group->addControl(spinBoxControl);

    doubleSpinBox = spinBoxControl->widget();
    doubleSpinBox->setValue(0.3);
    doubleSpinBox->setDecimals(1);
    doubleSpinBox->setSuffix(tr("\""));
    doubleSpinBox->setRange(-11.0, 22.0);
    doubleSpinBox->setMinimumWidth(60);
    doubleSpinBox->setMaximumWidth(60);

    group->addSeparator();

    group->addControl(new RibbonLabelControl(tr("Spacing")));

    spinBoxControl = new RibbonDoubleSpinBoxControl();
    spinBoxControl->setDefaultAction(new QWidgetAction(this));
    spinBoxControl->defaultAction()->setIcon(QIcon(":/res/smallbefore.png"));
    spinBoxControl->defaultAction()->setText(QObject::tr("Before:"));
    group->addControl(spinBoxControl);

    QDoubleSpinBox* spinBox = spinBoxControl->widget();
    spinBox->setValue(0);
    spinBox->setSuffix(tr("pt"));
    spinBox->setRange(0, 10000);
    spinBox->setMinimumWidth(60);
    spinBox->setMaximumWidth(60);

    spinBoxControl = new RibbonDoubleSpinBoxControl();
    spinBoxControl->setDefaultAction(new QWidgetAction(this));
    spinBoxControl->defaultAction()->setIcon(QIcon(":/res/smallafter.png"));
    spinBoxControl->defaultAction()->setText(QObject::tr("After:"));
    group->addControl(spinBoxControl);

    spinBox = spinBoxControl->widget();
    spinBox->setValue(3);
    spinBox->setSuffix(tr("pt"));
    spinBox->setRange(0, 10000);
    spinBox->setMinimumWidth(60);
    spinBox->setMaximumWidth(60);
}

void MainWindow::createGroupTableOfContents(RibbonGroup* group)
{
    QMenu* editPaste = new QMenu(this);
    QAction* action = group->addAction(QIcon(":/res/largetablecontents.png"), tr("Table of Contents"),
        Qt::ToolButtonTextUnderIcon, editPaste);
    RibbonControl* control = group->controlByAction(action);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    group->addAction(QIcon(":/res/smalladdtext.png"), tr("Add Text"),
        Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smallupdatetable.png"), tr("Update Table"),
        Qt::ToolButtonTextBesideIcon);
}

void MainWindow::createGroupFootnotes(RibbonGroup* group)
{
    QAction* action = group->addAction(QIcon(":/res/largeInsertFootnote.png"),
        tr("Insert Footnote"), Qt::ToolButtonTextUnderIcon);
    RibbonControl* control = group->controlByAction(action);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    group->addAction(QIcon(":/res/smallinsertendnote.png"),
        tr("&Insert Endnote"), Qt::ToolButtonTextBesideIcon);
    QMenu* nextFootnote = new QMenu(this);
    group->addAction(QIcon(":/res/smallnextfootnote.png"),
        tr("Next Footnote"), Qt::ToolButtonTextBesideIcon, nextFootnote);
    group->addAction(QIcon(":/res/smallshownotes.png"),
        tr("Show Notes"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupCaptions(RibbonGroup* group)
{
    QAction* action = group->addAction(QIcon(":/res/largeInsertCaption.png"),
        tr("Insert Caption"), Qt::ToolButtonTextUnderIcon);
    RibbonControl* control = group->controlByAction(action);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    group->addAction(QIcon(":/res/smallinserttablefigures.png"),
        tr("Insert Table of Figures"), Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smallupdatetable.png"),
        tr("Update Table"), Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smallcrossreference.png"),
        tr("Cross-reference"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupIndex(RibbonGroup* group)
{
    QAction* action = group->addAction(QIcon(":/res/largeMarkEntry.png"),
        tr("Mark Entry"), Qt::ToolButtonTextUnderIcon);
    RibbonControl* control = group->controlByAction(action);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageLarge);
    control->sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageLarge);

    group->addAction(QIcon(":/res/smallinserttablefigures.png"),
        tr("Insert Index"), Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smallupdatetable.png"),
        tr("Update Index"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupDocumentViews(RibbonGroup* group)
{
    m_actionPrintLayout = group->addAction(qtnIcon(":/res/printlayout16x16.png", ":/res/printlayout32x32.png"), tr("Print Layout"), Qt::ToolButtonTextUnderIcon);
    m_actionPrintLayout->setCheckable(true);
    m_actionPrintLayout->setChecked(true);

    QIcon fullScreenIcons;
    fullScreenIcons.addPixmap(QPixmap(":/res/largefullscreen.png"));
    fullScreenIcons.addPixmap(QPixmap(":/res/smallfullscreen.png"));
    m_actionFullScreen = group->addAction(fullScreenIcons, tr("Full Screen Reading"), Qt::ToolButtonTextUnderIcon);

    m_actionFullScreen->setCheckable(true);
    m_actionFullScreen->setChecked(false);
    connect(m_actionFullScreen, SIGNAL(triggered(bool)), this, SLOT(fullScreen(bool)));
    group->addAction(QIcon(":/res/largeweblayout.png"), tr("Web Layout"), Qt::ToolButtonTextUnderIcon);
    group->addAction(QIcon(":/res/largeoutline.png"), tr("Outline"), Qt::ToolButtonTextUnderIcon);
    group->addAction(QIcon(":/res/largedraft.png"), tr("Draft"), Qt::ToolButtonTextUnderIcon);
}

void MainWindow::createGroupShow_Hide(RibbonGroup* group)
{
    QCheckBox* ruler = new QCheckBox(tr("Ruler"));
    ruler->setToolTip(tr("Ruler"));
    group->addWidget(ruler);

    QCheckBox* gridlines = new QCheckBox(tr("Gridlines"));
    gridlines->setToolTip(tr("Gridlines"));
    group->addWidget(gridlines);

    QCheckBox* properties = new QCheckBox(tr("Properties"));
    properties->setToolTip(tr("Properties"));
    group->addWidget(properties);

    QCheckBox* documentMap = new QCheckBox(tr("Document Map"));
    documentMap->setToolTip(tr("Document Map"));
    group->addWidget(documentMap);

    QCheckBox* statusBar = new QCheckBox(tr("Status Bar"));
    statusBar->setCheckState(Qt::Checked);
    statusBar->setToolTip(tr("Status Bar"));
    statusBar->setStatusTip(tr("Show or hide the status bar"));
    group->addWidget(statusBar);
    connect(statusBar, SIGNAL(stateChanged(int)), this, SLOT(stateStatusBar(int)));

    QCheckBox* messageBar = new QCheckBox(tr("Message Bar"));
    messageBar->setToolTip(tr("Message Bar"));
    group->addWidget(messageBar);
}

void MainWindow::createGroupZoom(RibbonGroup* group)
{
    group->addAction(QIcon(":/res/largezoom.png"), tr("Zoom"), Qt::ToolButtonTextUnderIcon);
    group->addAction(QIcon(":/res/smallonepage.png"), tr("One Page"), Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smalltwopage.png"), tr("Two Page"), Qt::ToolButtonTextBesideIcon);
    group->addAction(QIcon(":/res/smallpagewidth.png"), tr("Page Width"), Qt::ToolButtonTextBesideIcon);
}

void MainWindow::createStatusBar()
{
    RibbonStatusBar* statusBar = new RibbonStatusBar();
    setStatusBar(statusBar);

    statusBar->addAction(tr("Page 1 of 1"));
    statusBar->addSeparator();
    statusBar->addAction(tr("Words: 1"));
    statusBar->addSeparator();
    statusBar->addAction(tr("English (U.S.)"));
    statusBar->addSeparator();
    statusBar->addAction(tr("Track Changes: Off"));
    statusBar->addSeparator();
    statusBar->addAction(tr("Insert"));

    RibbonStatusBarSwitchGroup* switchGroup = new RibbonStatusBarSwitchGroup;
    QList<QAction*> actions;
    actions << m_actionPrintLayout;
    actions << m_actionFullScreen;
    QAction* actionWebLayout = new QAction(QIcon(":/res/smallweblayout.png"), tr("Web Layout"), this);
    actions << actionWebLayout;
    QAction* actionOutLine = new QAction(QIcon(":/res/smalloutline.png"), tr("Outline"), this);
    actions << actionOutLine;
    QAction* actionDraft = new QAction(QIcon(":/res/smalldraft.png"), tr("Draft"), this);
    actions << actionDraft;
    switchGroup->addActions(actions);
    statusBar->addPermanentWidget(switchGroup);

    m_zoomPercentage = new QToolButton(statusBar);
    m_zoomPercentage->setText(tr("100%"));
    m_zoomPercentage->setToolTip(tr("Zoom level. Click to open the Zoom dialog box."));
    statusBar->addPermanentWidget(m_zoomPercentage);
    QFontMetrics mt(statusBar->font());
    int minimumWidth = mt.boundingRect(QString("0000%")).width();
    m_zoomPercentage->setMinimumWidth(minimumWidth);

    RibbonSliderPane* sliderPane = new RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 200); // range
    sliderPane->setMaximumWidth(130);
    sliderPane->setSingleStep(10);
    sliderPane->setSliderPosition(100);
    statusBar->addPermanentWidget(sliderPane, 1);
    connect(sliderPane, SIGNAL(valueChanged(int)), this, SLOT(zoomSliderScroll(int)));
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

#if (QT_VERSION >= QT_VERSION_CHECK(5,6,0))
    QList<QDockWidget*> docks; docks << dock;
    QList<int> sizes; sizes << 120;
    resizeDocks(docks, sizes, Qt::Vertical);
#endif
}

bool MainWindow::load(const QString& f)
{
    if (!QFile::exists(f))
        return false;

    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    setCurrentFileName(f);

    QByteArray data = file.readAll();
    QTextCodec* codec = Qt::codecForHtml(data);
    QString str = codec->toUnicode(data);
    if (Qt::mightBeRichText(str)) 
    {
        m_textEdit->setHtml(str);
    } 
    else 
    {
        str = QString::fromLocal8Bit(data);
        m_textEdit->setPlainText(str);
    }

    return true;
}

bool MainWindow::maybeSave()
{
    if (!m_textEdit->document()->isModified())
        return true;
    if (m_fileName.startsWith(QLatin1String(":/")))
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
        tr("The document has been modified.\n"
        "Do you want to save your changes?"),
        QMessageBox::Save | QMessageBox::Discard
        | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void MainWindow::setCurrentFileName(const QString& fileName)
{
    this->m_fileName = fileName;
    m_textEdit->document()->setModified(false);

    QString shownName;
    if (m_fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(m_fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Qtitan Ribbon Office")));
    setWindowModified(false);
    setWindowFilePath(shownName);
}

void MainWindow::fileNew()
{
    if (maybeSave()) 
    {
        m_textEdit->clear();
        setCurrentFileName(QString());
    }
}

void MainWindow::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
        QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty())
        load(fn);
}

bool MainWindow::fileSave()
{
    if (m_fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(m_fileName);
    bool success = writer.write(m_textEdit->document());
    if (success)
    {
        m_textEdit->document()->setModified(false);
        setWindowModified(false);
    }
    return success;
}

bool MainWindow::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
        QString(), tr("ODF files (*.odt);;HTML-Files (*.htm *.html);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    if (! (fn.endsWith(".odt", Qt::CaseInsensitive) || 
           fn.endsWith(".htm", Qt::CaseInsensitive) || 
           fn.endsWith(".html", Qt::CaseInsensitive)))
        fn += ".odt"; // default
    setCurrentFileName(fn);
    return fileSave();
}

void MainWindow::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog* dlg = new QPrintDialog(&printer, this);
    if (m_textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        m_textEdit->print(&printer);
    }
    delete dlg;
}

void MainWindow::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MainWindow::printPreview(QPrinter* printer)
{
    m_textEdit->print(printer);
}


void MainWindow::filePrintPdf()
{
    QString m_fileName = QFileDialog::getSaveFileName(this, "Export PDF",
        QString(), "*.pdf");
    if (!m_fileName.isEmpty()) {
        if (QFileInfo(m_fileName).suffix().isEmpty())
            m_fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(m_fileName);
        m_textEdit->document()->print(&printer);
    }
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(m_actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(m_actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(m_actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) 
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void MainWindow::textStyle(int styleIndex)
{
    QTextCursor cursor = m_textEdit->textCursor();

    if (styleIndex != 0) 
    {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) 
        {
            case 1:
                style = QTextListFormat::ListDisc;
                break;
            case 2:
                style = QTextListFormat::ListCircle;
                break;
            case 3:
                style = QTextListFormat::ListSquare;
                break;
            case 4:
                style = QTextListFormat::ListDecimal;
                break;
            case 5:
                style = QTextListFormat::ListLowerAlpha;
                break;
            case 6:
                style = QTextListFormat::ListUpperAlpha;
                break;
            case 7:
                style = QTextListFormat::ListLowerRoman;
                break;
            case 8:
                style = QTextListFormat::ListUpperRoman;
                break;
            default:
                break;
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) 
        {
            listFmt = cursor.currentList()->format();
        } 
        else 
        {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } 
    else 
    {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void MainWindow::textColor(const QColor& col)
{
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void MainWindow::setColorText()
{
    textColor(m_colorButton->color());
}

void MainWindow::textAlign(QAction* a)
{
    if (a == m_actionAlignLeft)
        m_textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == m_actionAlignCenter)
        m_textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == m_actionAlignRight)
        m_textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == m_actionAlignJustify)
        m_textEdit->setAlignment(Qt::AlignJustify);
}

void MainWindow::selectAll()
{
    m_textEdit->selectAll();
}

void MainWindow::optionParagraph()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Paragraph dialog."));
    messageBox.exec();
}

void MainWindow::optionClipboard()
{
    QMessageBox messageBox(QMessageBox::Information, windowTitle(), 
        QLatin1String(""), QMessageBox::Ok, this);
    messageBox.setInformativeText(QLatin1String("Clipboard dialog."));
    messageBox.exec();
}

void MainWindow::showHideMarks()
{
    if (QTextDocument* document = m_textEdit->document())
    {
        QTextOption txtOption = document->defaultTextOption();
        txtOption.setFlags(txtOption.flags() & QTextOption::ShowLineAndParagraphSeparators ? 
            QTextOption::IncludeTrailingSpaces : QTextOption::ShowLineAndParagraphSeparators);
        document->setDefaultTextOption(txtOption);
    }
}

void MainWindow::optionFont()
{
    bool ok = false;
    QFont font = QFontDialog::getFont(&ok, m_textEdit->currentFont(), this);
    if (ok) 
        m_textEdit->setCurrentFont(font);
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
    else
    {
        ribbonBar()->setVisible(true);
        setWindowState(m_stateWindow);
        m_actionFullScreen->setChecked(false);
    }
}

void MainWindow::stateStatusBar(int state)
{
    statusBar()->setVisible(state == Qt::Checked);
}

void MainWindow::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
//    colorChanged(format.foreground().color());
}

void MainWindow::cursorPositionChanged()
{
    alignmentChanged(m_textEdit->alignment());
}

void MainWindow::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    m_actionPaste1->setEnabled(!QApplication::clipboard()->text().isEmpty());
    m_actionPaste2->setEnabled(!QApplication::clipboard()->text().isEmpty());
#endif
}

void MainWindow::selectionChanged()
{
    if (m_pagePictureFormat)
    {
        QTextFormat format = m_textEdit->textCursor().charFormat(); 
        if (format.isImageFormat())
            m_pagePictureFormat->setVisible(true);
        else
            m_pagePictureFormat->setVisible(false);
    }
}

void MainWindow::zoomSliderScroll(int value)
{
    QString str;
    str += QObject::tr("%1%").arg(value);
    m_zoomPercentage->setText(str);
}

void MainWindow::showRibbonContextMenu(QMenu* menu, QContextMenuEvent* event)
{
    Q_UNUSED(menu);
    Q_UNUSED(event);
}

void MainWindow::showCompanyWebSite()
{
    QDesktopServices::openUrl(QUrl("http://www.devmachines.com"));
}


void MainWindow::keyPressEvent(QKeyEvent* event)
{
    RibbonMainWindow::keyPressEvent(event);
    if (event->key() == Qt::Key_Escape && m_actionFullScreen->isChecked())
        fullScreen(false);
}

void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
    QTextCursor cursor = m_textEdit->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    m_textEdit->mergeCurrentCharFormat(format);
}

void MainWindow::fontChanged(const QFont& f)
{
    QString famaly = QFontInfo(f).family();
    if (famaly.isEmpty())
        return;
    int index = m_comboFont ? m_comboFont->findText(famaly) : -1;
    if (index == -1)
        return;

    m_comboFont->setCurrentIndex(index);
    m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(f.pointSize())));
    m_actionTextBold->setChecked(f.bold());
    m_actionTextItalic->setChecked(f.italic());
    m_actionTextUnderline->setChecked(f.underline());
}

void MainWindow::colorChanged(const QColor& c)
{
    m_colorButton->setColor(c);
}

void MainWindow::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) 
        m_actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter) 
        m_actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight) 
        m_actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify) 
        m_actionAlignJustify->setChecked(true);
}

void MainWindow::customization()
{
    RibbonCustomizeManager* customizeManager = ribbonBar()->customizeManager();
    customizeManager->addAllActionsCategory(categoryAllCommands);

    customizeManager->addDefaultStateQAccessBar();
    customizeManager->addDefaultStateRibbonBar();

    // customization for ribbonQuickAccessBar
    customizeManager->addToCategory(categoryPopularCommands, m_actionFileNew);
    customizeManager->addToCategory(categoryPopularCommands, m_actionOpenFile);
    customizeManager->addToCategory(categoryPopularCommands, m_actionSaveFile);
    customizeManager->addToCategory(categoryPopularCommands, m_actionPrint);
    customizeManager->addToCategory(categoryPopularCommands, m_actionPrintPreview);
    customizeManager->addToCategory(categoryPopularCommands, m_actionCut);

    for (int i = 0, count = m_pageHome->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageHome->title(), m_pageHome->getGroup(i));
    for (int i = 0, count = m_pageLayout->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageLayout->title(), m_pageLayout->getGroup(i));
    for (int i = 0, count = m_pageRef->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageRef->title(), m_pageRef->getGroup(i));
    for (int i = 0, count = m_pageView->groupCount(); count > i; ++i)
        customizeManager->addToCategory(m_pageView->title(), m_pageView->getGroup(i));

    // customization for ribbon
    customizeManager->addToCategory(categoryMainPages, m_pageHome);
    customizeManager->addToCategory(categoryMainPages, m_pageLayout);
    customizeManager->addToCategory(categoryMainPages, m_pageRef);
    customizeManager->addToCategory(categoryMainPages, m_pageView);

    // Adding custom page to the standard ribbon customization dialog.
    RibbonCustomizeDialog* dialog = ribbonBar()->customizeDialog();
    RibbonPopularPage* popularPage = new RibbonPopularPage(this);
    dialog->insertPage(0, popularPage);

    RibbonQuickAccessBarCustomizePage* pCustomizeQuickAccessBar = qobject_cast<RibbonQuickAccessBarCustomizePage*>(dialog->pageByIndex(1));
    Q_ASSERT(pCustomizeQuickAccessBar != NULL);
    pCustomizeQuickAccessBar->addCustomCategory(categoryPopularCommands);
    pCustomizeQuickAccessBar->addCustomCategory(categoryAllCommands);
    pCustomizeQuickAccessBar->addSeparatorCategory();
    pCustomizeQuickAccessBar->addCustomCategory(m_pageHome->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageLayout->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageRef->title());
    pCustomizeQuickAccessBar->addCustomCategory(m_pageView->title());

    RibbonBarCustomizePage* pCustomizeRibbonBar = qobject_cast<RibbonBarCustomizePage*>(dialog->pageByIndex(2));
    Q_ASSERT(pCustomizeRibbonBar != NULL);
    pCustomizeRibbonBar->addCustomCategory(categoryPopularCommands);
    pCustomizeRibbonBar->addCustomCategory(categoryAllCommands);
    pCustomizeRibbonBar->addCustomCategory(categoryMainPages);
}

void MainWindow::readSettings()
{
    QSettings settings(this);

    OfficeStyle* st = (OfficeStyle*)qApp->style();
    OfficeStyle::Theme theme = (OfficeStyle::Theme)settings.value("theme", 
        (int)OfficeStyle::Office2013White).toInt();

    if (theme == OfficeStyle::Windows7Scenic   || 
        theme == OfficeStyle::Office2010Silver || 
        theme == OfficeStyle::Office2010Blue   ||
        theme == OfficeStyle::Office2010Black  ||
        theme == OfficeStyle::Office2013White  ||
        theme == OfficeStyle::Office2016Colorful ||
        theme == OfficeStyle::Office2016White ||
        theme == OfficeStyle::Office2016DarkGray ||
        theme == OfficeStyle::Office2016Black)
        ribbonBar()->getSystemButton()->setToolButtonStyle(Qt::ToolButtonTextOnly);
    else
        ribbonBar()->getSystemButton()->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    if (theme != st->getTheme())
        st->setTheme(theme);

    updateActionsTheme();

    QByteArray byteArray = settings.value("stateRibbon").toByteArray();
    if (!byteArray.isEmpty())
    {
        QBuffer buffer(&byteArray);
        if (!buffer.open(QIODevice::ReadOnly))
            Q_ASSERT(false);
        ribbonBar()->customizeManager()->loadStateFromDevice(&buffer);
    }

    QByteArray byte = settings.value("geometry").toByteArray();
    if (!byte.isEmpty())
        restoreGeometry(byte);

    bool minimized = settings.value("ribbonMinimized", false).toBool();
    ribbonBar()->setMinimized(minimized);

    bool onTop = settings.value("quickAccessToolbarOnTop", true).toBool();
    ribbonBar()->setQuickAccessBarPosition(onTop ? RibbonBar::QATopPosition : RibbonBar::QABottomPosition);

    QString strBackground = settings.value("titleBackground", ribbonBackground()).toString();
    setRibbonBackground(strBackground);
}

void MainWindow::writeSettings()
{
    QSettings settings(this);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    if (!buffer.open(QIODevice::WriteOnly))
        Q_ASSERT(false);

    ribbonBar()->customizeManager()->saveStateToDevice(&buffer);
    settings.setValue(("stateRibbon"), QVariant(byteArray));

    QByteArray byte = saveGeometry();
    settings.setValue("geometry", byte);

    OfficeStyle* st = (OfficeStyle*)qApp->style();
    int theme = (int)st->getTheme();
    settings.setValue("theme", theme);

    bool minimized = ribbonBar()->isMinimized();
    settings.setValue("ribbonMinimized", minimized);

    bool onTop = ribbonBar()->quickAccessBarPosition() == RibbonBar::QATopPosition;
    settings.setValue("quickAccessToolbarOnTop", onTop);

    settings.setValue("titleBackground", ribbonBackground());
}

void MainWindow::updateActionsTheme()
{
    RibbonWindow::updateActionsTheme();
    updateImageStyles();
    updateRibbonBackground();
}

void MainWindow::updateRibbonBackground()
{
    OfficeStyle::Theme theme = m_ribbonStyle->getTheme();
    QPixmap px(m_curRibbonBackground);
    if (theme == OfficeStyle::Office2013Dark)
        ribbonBar()->setTitleBackground(QPixmap::fromImage(DrawHelpers::addaptColors(px.toImage(), qRgb(255, 255, 255), qRgb(230, 230, 230))));
    else
        ribbonBar()->setTitleBackground(px);
}

void MainWindow::updateImageStyles()
{
    QPixmap px(":/res/galleryStyles.png");
    if (m_ribbonStyle->getTheme() == OfficeStyle::Office2016Black)
        px = QPixmap::fromImage(DrawHelpers::invertColors(px.toImage()));

    QSize sz(64, 48);
    for (int i = 0, count = m_itemsStyles->itemCount(); count > i; i++)
    {
        RibbonGalleryItem* item = m_itemsStyles->item(i);
        QRect rcImage(i * sz.width(), 0, sz.width(), sz.height());
        QPixmap copyPix = px.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());
        item->setIcon(copyPix);
    }
}
