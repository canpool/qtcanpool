#include <qglobal.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QClipboard>
#include <QCloseEvent>
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

#include <QtnOfficeDefines.h>
#include <QtnRibbonStyle.h>
#include <QtnOfficePopupColorButton.h>
#include <QtnRibbonQuickAccessBar.h>

#include "aboutdialog.h"

#include "mainwindow.h"
#include "ui_ribbonsample.h"


/* MainWindow */
MainWindow::MainWindow(QWidget* parent)
    : Qtitan::RibbonMainWindow(parent)
{
    m_ribbonStyle = qobject_cast<Qtitan::RibbonStyle*>(qApp->style());
    Q_ASSERT(m_ribbonStyle != Q_NULL);
    RibbonToolTip::setWrapMode(RibbonToolTip::AutoWrap);

    createAction();
    createMenuFile();
    createQuickAccessBar();
    createRibbon();
    createStatusBar();
//    createDockWindows();

    QAction* actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), "About", Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    createOptions();

    m_actionRibbonMinimize = ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), "Minimize the Ribbon", Qt::ToolButtonIconOnly);
    m_actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    m_actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(m_actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChanged(bool)));

    m_textEdit = new QTextEdit(this);

    connect(m_textEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(m_textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

    setCentralWidget(m_textEdit);
    m_textEdit->setFocus();
//    setCurrentFileName(QString());

    fontChanged(m_textEdit->font());
    colorChanged(m_textEdit->textColor());
    alignmentChanged(m_textEdit->alignment());

    connect(m_textEdit->document(), SIGNAL(modificationChanged(bool)), m_actionSave, SLOT(setEnabled(bool)));
    connect(m_textEdit->document(), SIGNAL(modificationChanged(bool)), this, SLOT(setWindowModified(bool)));

    connect(m_textEdit->document(), SIGNAL(undoAvailable(bool)), m_actionUndo, SLOT(setEnabled(bool)));
    connect(m_textEdit->document(), SIGNAL(redoAvailable(bool)), m_actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(m_textEdit->document()->isModified());
    m_actionSave->setEnabled(m_textEdit->document()->isModified());
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

    ribbonBar()->setFrameThemeEnabled();

    QString initialFile = ":/res/example.html";
    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
        initialFile = args.at(1);

    if (!load(initialFile))
        fileNew();

    readSettings();
}

MainWindow::~MainWindow()
{
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
    QIcon iconNew;
    iconNew.addPixmap(QPixmap(":/res/largeNewFile.png"));
    iconNew.addPixmap(QPixmap(":/res/smallnew.png"));
    m_actionFileNew = new QAction(iconNew, tr("&New"), this);
    m_actionFileNew->setPriority(QAction::LowPriority);
    m_actionFileNew->setShortcut(QKeySequence::New);
    m_actionFileNew->setStatusTip(tr("Create a new document"));
    m_actionFileNew->setToolTip(tr("New"));
    connect(m_actionFileNew, SIGNAL(triggered()), this, SLOT(fileNew()));

    QIcon iconOpen;
    iconOpen.addPixmap(QPixmap(":/res/largeOpenFile.png"));
    iconOpen.addPixmap(QPixmap(":/res/smallOpen.png"));
    m_actionOpenFile = new QAction(iconOpen, tr("&Open..."), this);
    m_actionOpenFile->setShortcut(QKeySequence::Open);
    m_actionOpenFile->setToolTip(tr("Open"));
    m_actionOpenFile->setStatusTip(tr("Open an existing document"));
    connect(m_actionOpenFile, SIGNAL(triggered()), this, SLOT(fileOpen()));

    QIcon iconSave;
    iconSave.addPixmap(QPixmap(":/res/largeSaveFile.png"));
    iconSave.addPixmap(QPixmap(":/res/smallSave.png"));
    m_actionSave = new QAction(iconSave, tr("&Save"), this);
    m_actionSave->setShortcut(QKeySequence::Save);
    m_actionSave->setToolTip(tr("Save"));
    m_actionSave->setStatusTip(tr("Save the active document"));
    m_actionSave->setEnabled(false);
    connect(m_actionSave, SIGNAL(triggered()), this, SLOT(fileSave()));

#ifndef QT_NO_PRINTER
    QIcon iconPrint;
    iconPrint.addPixmap(QPixmap(":/res/largePrint.png"));
    iconPrint.addPixmap(QPixmap(":/res/smallPrint.png"));
    m_actionPrint = new QAction(iconPrint, tr("&Print..."), this);
    m_actionPrint->setPriority(QAction::LowPriority);    
    m_actionPrint->setShortcut(QKeySequence::Print);
    m_actionPrint->setToolTip(tr("Select a printer, number of copies, and other printing options before printing"));
    connect(m_actionPrint, SIGNAL(triggered()), this, SLOT(filePrint()));

    m_actionPrintSetup = new QAction(QIcon(":/res/largePrintSetup.png"), tr("P&rint Setup"), this);
    m_actionPrintSetup->setToolTip(tr("Change the printer and printing options"));

    m_actionPrintPreview = new QAction(QIcon(":/res/largePrintPreview.png"), tr("Print Pre&view..."), this);
    m_actionPrintPreview->setToolTip(tr("Preview and make changes to pages before printing"));
    connect(m_actionPrintPreview, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
#endif
}

void MainWindow::createMenuFile()
{
    QIcon iconLogo;
    iconLogo.addPixmap(QPixmap(":/res/file.png"));
    iconLogo.addPixmap(QPixmap(":/shared/res/qtitanlogo32x32.png"));
    if(QAction* actionFile = ribbonBar()->addSystemButton(iconLogo, tr("&File"))) 
    {
        actionFile->setToolTip(tr("Click here to see everything you can do with your document"));
        Qtitan::RibbonSystemPopupBar* popupBar = qobject_cast<Qtitan::RibbonSystemPopupBar*>(actionFile->menu());

        popupBar->addAction(m_actionFileNew);
        popupBar->addAction(m_actionOpenFile);
        popupBar->addAction(m_actionSave);

        QAction* actionSaveAsFile = popupBar->addAction(QIcon(":/res/largeSaveAsFile.png"), tr("Save &As..."));
        actionSaveAsFile->setPriority(QAction::LowPriority);
        actionSaveAsFile->setToolTip(tr("Save As"));
        actionSaveAsFile->setStatusTip(tr("Save the active document with a new name"));
        connect(actionSaveAsFile, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

        popupBar->addSeparator();

#ifndef QT_NO_PRINTER
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
#endif
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
        QAction* actionClose = popupBar->addAction(QIcon(":/res/largeClose.png"), tr("&Close"));
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
    if (Qtitan::RibbonQuickAccessBar* quickAccessBar = ribbonBar()->getQuickAccessBar())
    {
        QAction* action = quickAccessBar->actionCustomizeButton();
        action->setToolTip(tr("Customize Quick Access Bar"));

        quickAccessBar->addAction(m_actionFileNew);
        quickAccessBar->setActionVisible(m_actionFileNew, false);

        quickAccessBar->addAction(m_actionOpenFile);
        quickAccessBar->setActionVisible(m_actionOpenFile, false);

        quickAccessBar->addAction(m_actionSave);

        m_actionUndo = quickAccessBar->addAction(QIcon(":/res/smallUndo.png"), tr("&Undo"));
        m_actionUndo->setShortcut(QKeySequence::Undo);
        m_actionRedo = quickAccessBar->addAction(QIcon(":/res/smallRedo.png"), tr("&Redo"));
        m_actionRedo->setShortcut(QKeySequence::Redo);
#ifndef QT_NO_PRINTER
        quickAccessBar->addAction(m_actionPrint);
#endif
        ribbonBar()->showQuickAccess(true);
    }
}

void MainWindow::createRibbon()
{
    if (Qtitan::RibbonPage* pageHome = ribbonBar()->addPage(tr("&Home")))
    {
        createGroupClipboard(pageHome);
        createGroupFont(pageHome);
        createGroupParagraph(pageHome);
        createGroupEditing(pageHome);
    }
    if(Qtitan::RibbonPage* pageLayout = ribbonBar()->addPage(tr("&Page Layout")))
    {
        createGroupThemes(*pageLayout->addGroup(tr("Themes")));
        createGroupPageSetup(*pageLayout->addGroup(tr("Page Setup")));
        createGroupPageBackground(*pageLayout->addGroup(tr("Page Background")));
        createGroupParagraphLayout(*pageLayout->addGroup(tr("Paragraph")));
    }
    if(Qtitan::RibbonPage* pageRef = ribbonBar()->addPage(tr("Reference&s")))
    {
        createGroupTableOfContents(*pageRef->addGroup(tr("Table of Contents")));
        createGroupFootnotes(*pageRef->addGroup(tr("&Footnotes")));
        createGroupCaptions(*pageRef->addGroup(tr("Captions")));
        createGroupIndex(*pageRef->addGroup(tr("Index")));
    }

    if(Qtitan::RibbonPage* pageMailings = ribbonBar()->addPage(tr("Mailings")))
    {
        Q_UNUSED(pageMailings);
    }

    if(Qtitan::RibbonPage* pageReview = ribbonBar()->addPage(tr("Review")))
    {
        pageReview->setContextPage(Qtitan::ContextColorPurple);
    }

    if(Qtitan::RibbonPage* pageView = ribbonBar()->addPage(tr("View")))
    {
        createGroupDocumentViews(*pageView->addGroup(tr("Document Views")));
        createGroupShow_Hide(*pageView->addGroup(tr("Show/Hide")));
        createGroupZoom(*pageView->addGroup(tr("Zoom")));
    }

    m_pagePictureFormat = ribbonBar()->addPage(tr("Picture Format"));
    if (m_pagePictureFormat)
    {
        m_pagePictureFormat->setContextTitle(tr("Picture Tools"));
        m_pagePictureFormat->setContextPage(Qtitan::ContextColorRed);
        m_pagePictureFormat->setVisible(false);
    }
}

void MainWindow::createGroupClipboard(Qtitan::RibbonPage* page)
{
    if(Qtitan::RibbonGroup* groupClipboard = page->addGroup(QIcon(":/res/smallpaste.png"), tr("Clipboard"))) 
    {
        groupClipboard->setOptionButtonVisible();
        QAction* act = groupClipboard->getOptionButtonAction();
        act->setText(tr("Clipboard"));
        act->setIcon(QIcon(":/res/clipboardToolTip.png"));
        act->setToolTip(tr("Show the Office clipboard Task Pane"));
        act->setStatusTip(tr("Show the Office clipboard Task Pane"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionClipboard()));

        QMenu* editPaste = new QMenu(this);
        m_actionPaste1 = editPaste->addAction(QIcon(":/res/smallpaste.png"), tr("Paste"));
        m_actionPaste1->setPriority(QAction::LowPriority);
        m_actionPaste1->setShortcut(QKeySequence::Paste);

        editPaste->addAction(tr("Paste Special"));

        m_actionPaste2 = groupClipboard->addAction(QIcon(":/res/largepaste.png"), 
            tr("&Paste"), Qt::ToolButtonTextUnderIcon, editPaste);
        m_actionPaste2->setPriority(QAction::LowPriority);
        m_actionPaste2->setShortcut(QKeySequence::Paste);
        m_actionPaste2->setToolTip(tr("Insert Clipboard contents"));
#ifndef QT_NO_CLIPBOARD
        m_actionPaste1->setEnabled(!QApplication::clipboard()->text().isEmpty());
        m_actionPaste2->setEnabled(!QApplication::clipboard()->text().isEmpty());
#endif

        m_actionCut = groupClipboard->addAction(QIcon(":/res/smallcut.png"), 
            tr("&Cut"), Qt::ToolButtonTextBesideIcon);
        m_actionCut->setShortcut(QKeySequence::Cut);
        m_actionCut->setToolTip(tr("Cut the selection and put it on the Clipboard"));

        m_actionCopy = groupClipboard->addAction(QIcon(":/res/smallcopy.png"), 
            tr("&Copy"), Qt::ToolButtonTextBesideIcon);
        m_actionCopy->setShortcut(QKeySequence::Copy);

        m_actionFormatPointerAction = groupClipboard->addAction(QIcon(":/res/smallformatpainter.png"), 
            tr("F&ormat Pointer"), Qt::ToolButtonTextBesideIcon);
    }
}

void MainWindow::createGroupFont(Qtitan::RibbonPage* page)
{
    if(Qtitan::RibbonGroup* groupFont = page->addGroup(QIcon(":/res/smallfontgrow.png"), tr("Font"))) 
    {
        groupFont->setOptionButtonVisible();
        QAction* act = groupFont->getOptionButtonAction();
        act->setText(tr("F&ont"));
        act->setIcon(QIcon(":/res/fontToolTip.png"));
        act->setToolTip(tr("Show the Font dialog box"));
        act->setStatusTip(tr("Show the Font dialog box"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionFont()));

        groupFont->setControlsGrouping();
        groupFont->setControlsCentering();

        m_comboFont = new QFontComboBox();
        m_comboFont->setMaximumWidth(130);
        groupFont->addWidget(m_comboFont);
        connect(m_comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

        m_comboSize = new QComboBox();
        m_comboSize->setMaximumWidth(45);
        m_comboSize->setObjectName("m_comboSize");
        groupFont->addWidget(m_comboSize);
        m_comboSize->setEditable(true);

        QFontDatabase db;
        foreach(int size, db.standardSizes())
            m_comboSize->addItem(QString::number(size));

        connect(m_comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
        m_comboSize->setCurrentIndex(m_comboSize->findText(QString::number(QApplication::font().pointSize())));

        groupFont->addSeparator();
        groupFont->addAction(QIcon(":/res/smallfontgrow.png"), tr("Grow Font"), Qt::ToolButtonIconOnly);
        groupFont->addAction(QIcon(":/res/smallfontshrink.png"), tr("Shrink Font"), Qt::ToolButtonIconOnly);
        groupFont->addSeparator();
        groupFont->addAction(QIcon(":/res/smallfontclear.png"), tr("&Clear Formatting"), Qt::ToolButtonIconOnly);
        groupFont->addSeparator();
        m_actionTextBold = groupFont->addAction(QIcon(":/res/smalltextbold.png"), tr("&Bold"), Qt::ToolButtonIconOnly);
        m_actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
        m_actionTextBold->setPriority(QAction::LowPriority);
        QFont bold;
        bold.setBold(true);
        m_actionTextBold->setFont(bold);
        connect(m_actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
        m_actionTextBold->setCheckable(true);

        m_actionTextItalic = groupFont->addAction(QIcon(":/res/smalltextitalic.png"), 
            tr("&Italic"), Qt::ToolButtonIconOnly);
        m_actionTextItalic->setPriority(QAction::LowPriority);
        m_actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
        QFont italic;
        italic.setItalic(true);
        m_actionTextItalic->setFont(italic);
        connect(m_actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
        m_actionTextItalic->setCheckable(true);

        m_actionTextUnderline = groupFont->addAction(QIcon(":/res/smalltextunder.png"), 
            tr("&Underline"), Qt::ToolButtonIconOnly);
        m_actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
        m_actionTextUnderline->setPriority(QAction::LowPriority);
        QFont underline;
        underline.setUnderline(true);
        m_actionTextUnderline->setFont(underline);
        connect(m_actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
        m_actionTextUnderline->setCheckable(true);
        QAction* charStrikethrough = groupFont->addAction(QIcon(":/res/smallstrikethrough.png"), 
            tr("Strikethrough"), Qt::ToolButtonIconOnly);
        charStrikethrough->setEnabled(false);
        groupFont->addSeparator();
        QAction* textSubscript = groupFont->addAction(QIcon(":/res/smallsubscript.png"), 
            tr("Subscript"), Qt::ToolButtonIconOnly);
        textSubscript->setEnabled(false);
        QAction* textSuperscript = groupFont->addAction(QIcon(":/res/smallsuperscript.png"), 
            tr("Superscript"), Qt::ToolButtonIconOnly);
        textSuperscript->setEnabled(false);
        QAction* textChangecase = groupFont->addAction(QIcon(":/res/smallchangecase.png"), 
            tr("Change Case"), Qt::ToolButtonIconOnly);
        textChangecase->setEnabled(false);
        groupFont->addSeparator();

        Qtitan::PopupColorButton* highlightColorButton = new Qtitan::PopupColorButton(groupFont);
        QAction* textHighlightcolor= groupFont->addWidget(QIcon(":/res/smallcolor.png"), 
            tr("Highlight Color"), highlightColorButton);
        textHighlightcolor->setEnabled(false);

        m_colorButton = new Qtitan::PopupColorButton(groupFont);
        m_actionTextColor = groupFont->addWidget(QIcon(":/res/smallcolor.png"), tr("Color"), m_colorButton);
        connect(m_colorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(textColor(const QColor&)));
        connect(m_actionTextColor, SIGNAL(triggered()), this, SLOT(setColorText()));
    }
}

void MainWindow::createGroupParagraph(Qtitan::RibbonPage* page)
{
    if(Qtitan::RibbonGroup* groupParagraph = page->addGroup(QIcon(":/res/smallcenter.png"), tr("&Paragraph"))) 
    {
        groupParagraph->setOptionButtonVisible();
        QAction* act = groupParagraph->getOptionButtonAction();
        act->setText(tr("Paragraph"));
        act->setIcon(QIcon(":/res/paragraphToolTip.png"));
        act->setToolTip(tr("Show the Paragraph dialog box"));
        act->setStatusTip(tr("Show the Paragraph dialog box"));
        connect(act, SIGNAL(triggered()), this, SLOT(optionParagraph()));

        groupParagraph->setControlsGrouping();
        groupParagraph->setControlsCentering();

        QMenu* menu = new QMenu(this);
        groupParagraph->addAction(QIcon(":/res/smallbullets.png"), 
            tr("Bullets"), Qt::ToolButtonIconOnly, menu);
        groupParagraph->addAction(QIcon(":/res/smallnumbering.png"), 
            tr("Numbering"), Qt::ToolButtonIconOnly, menu);
        groupParagraph->addAction(QIcon(":/res/smallmultilevellist.png"), 
            tr("Multilevel List"), Qt::ToolButtonIconOnly, menu);
        groupParagraph->addSeparator();
        groupParagraph->addAction(QIcon(":/res/smalldecreaseindent.png"), 
            tr("Decrease Indent"), Qt::ToolButtonIconOnly);
        groupParagraph->addAction(QIcon(":/res/smallincreaseindent.png"), 
            tr("Increase Indent"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        groupParagraph->addAction(QIcon(":/res/smallsort.png"), 
            tr("Sort"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        QAction* action = groupParagraph->addAction(QIcon(":/res/smallshow_hide_marks.png"), 
            tr("Show/Hide Marks"), Qt::ToolButtonIconOnly);
        action->setCheckable(false);
        connect(action, SIGNAL(triggered()), this, SLOT(showHideMarks()));

        groupParagraph->addSeparator();
        QActionGroup*grp = new QActionGroup(this);
        connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

        m_actionAlignLeft = groupParagraph->addAction(QIcon(":/res/smallalignleft.png"), 
            tr("Align Left"), Qt::ToolButtonIconOnly);
        m_actionAlignLeft->setActionGroup(grp);
        m_actionAlignCenter = groupParagraph->addAction(QIcon(":/res/smallcenter.png"), 
            tr("Center"), Qt::ToolButtonIconOnly);
        m_actionAlignCenter->setActionGroup(grp);
        m_actionAlignRight = groupParagraph->addAction(QIcon(":/res/smallalignright.png"), 
            tr("Align Right"), Qt::ToolButtonIconOnly);
        m_actionAlignRight->setActionGroup(grp);
        m_actionAlignJustify = groupParagraph->addAction(QIcon(":/res/smalljustify.png"), 
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

        groupParagraph->addSeparator();
        groupParagraph->addAction(QIcon(":/res/smalllinespacing.png"), 
            tr("Line spacing"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        groupParagraph->addAction(QIcon(":/res/smallshading.png"), 
            tr("Shading"), Qt::ToolButtonIconOnly, menu);
        groupParagraph->addSeparator();
        groupParagraph->addAction(QIcon(":/res/smallnoborder.png"), 
            tr("No Border"), Qt::ToolButtonIconOnly, menu);
    }
}

void MainWindow::createGroupEditing(Qtitan::RibbonPage* page)
{
    if (Qtitan::RibbonGroup* groupEditing = page->addGroup(QIcon(":/res/smallfind.png"), tr("Editing"))) 
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

        groupEditing->addAction(actionFind, Qt::ToolButtonTextBesideIcon, findMenu);

        QAction* actionReplace = groupEditing->addAction(QIcon(":/res/smallreplace.png"), tr("Replace"), Qt::ToolButtonTextBesideIcon);
        actionReplace->setShortcut(Qt::CTRL + Qt::Key_H);
        actionReplace->setStatusTip(tr("Replace specific text with different text"));
        actionReplace->setEnabled(false);

        QMenu* selectMenu = groupEditing->addMenu(QIcon(":/res/smallselect.png"), tr("Select"));

        m_actionSelectAll = new QAction(tr("Select All"), this);
        m_actionSelectAll->setShortcut(Qt::CTRL + Qt::Key_A);
        m_actionSelectAll->setStatusTip(tr("Select the entire document"));
        selectMenu->addAction(m_actionSelectAll);
        connect(m_actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

        QAction* pActionSelectObjects = new QAction(tr("Select Objects"), this);
        pActionSelectObjects->setEnabled(false);
        selectMenu->addAction(pActionSelectObjects);

        QAction* pActionSelectMultipleObjects = new QAction(tr("Select Multiple Objects"), this);
        pActionSelectMultipleObjects->setEnabled(false);
        selectMenu->addAction(pActionSelectMultipleObjects);
    }
}

void MainWindow::createGroupThemes(Qtitan::RibbonGroup& page)
{
    QMenu* themesMenu = new QMenu(this);
    page.addAction(QIcon(":/res/largeThemes.png"), tr("Themes"), 
        Qt::ToolButtonTextUnderIcon, themesMenu);
    QMenu* colors = new QMenu(this);
    page.addAction(QIcon(":/res/smallColors.png"), 
        tr("Colors"), Qt::ToolButtonTextBesideIcon, colors)->setEnabled(false);
    QMenu* fonts = new QMenu(this);
    page.addAction(QIcon(":/res/smallfonts.png"), 
        tr("Fonts"), Qt::ToolButtonTextBesideIcon, fonts);
    QMenu* effects = new QMenu(this);
    page.addAction(QIcon(":/res/smalleffects.png"), 
        tr("Effects"), Qt::ToolButtonTextBesideIcon, effects);
}

void MainWindow::createGroupPageSetup(Qtitan::RibbonGroup& page)
{
    page.addMenu(QIcon(":/res/largeMargins.png"), tr("Margins"));
    page.addMenu(QIcon(":/res/largeOrientation.png"), tr("Orientation"));
    page.addMenu(QIcon(":/res/largeSize.png"), tr("Size"));
    page.addMenu(QIcon(":/res/largeColumns.png"), tr("Columns"));
    page.addMenu(QIcon(":/res/smallBreaks.png"), tr("Breaks"));
    page.addMenu(QIcon(":/res/smalllinenumbers.png"), tr("Line Numbers"));
    page.addMenu(QIcon(":/res/smallhyphenation.png"), tr("Hyphenation"));
}

void MainWindow::createGroupPageBackground(Qtitan::RibbonGroup& page)
{
    page.addMenu(QIcon(":/res/largeWatermark.png"), tr("Watermark"));
    page.addMenu(QIcon(":/res/largePageColor.png"), tr("Page Color"));
    page.addAction(QIcon(":/res/largePageBorders.png"), tr("Page Borders"), Qt::ToolButtonTextUnderIcon);
}

void MainWindow::createGroupParagraphLayout(Qtitan::RibbonGroup& page)
{
    page.addWidget(new QLabel(tr("Indent")));
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox;
    doubleSpinBox->setValue(0.3);
    doubleSpinBox->setDecimals(1);
    doubleSpinBox->setSuffix(tr("\""));
    doubleSpinBox->setRange(0.0, 22.0);
    doubleSpinBox->setMinimumWidth(120);
    page.addWidget(QIcon(":/res/smallleft.png"), tr("Left:"), true, doubleSpinBox);
    doubleSpinBox = new QDoubleSpinBox;
    doubleSpinBox->setValue(0.3);
    doubleSpinBox->setDecimals(1);
    doubleSpinBox->setSuffix(tr("\""));
    doubleSpinBox->setRange(-11.0, 22.0);
    doubleSpinBox->setMinimumWidth(120);
    page.addWidget(QIcon(":/res/smallright.png"), tr("Right:"), true, doubleSpinBox);

    page.addSeparator();

    page.addWidget(new QLabel(tr("Spacing")));
    QSpinBox* spinBox = new QSpinBox;
    spinBox->setValue(0);
    spinBox->setSuffix(tr("pt"));
    spinBox->setRange(0, 10000);
    spinBox->setMinimumWidth(120);
    page.addWidget(QIcon(":/res/smallbefore.png"), tr("Before:"), true, spinBox);
    spinBox = new QSpinBox;
    spinBox->setValue(3);
    spinBox->setSuffix(tr("pt"));
    spinBox->setRange(0, 10000);
    spinBox->setMinimumWidth(120);
    page.addWidget(QIcon(":/res/smallafter.png"), tr("After:"), true, spinBox);
}

void MainWindow::createGroupTableOfContents(Qtitan::RibbonGroup& page)
{
    QMenu* editPaste = new QMenu(this);
    page.addAction(QIcon(":/res/largetablecontents.png"), tr("Table of Contents"), 
        Qt::ToolButtonTextUnderIcon, editPaste);
    page.addAction(QIcon(":/res/smalladdtext.png"), tr("Add Text"), 
        Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smallupdatetable.png"), tr("Update Table"), 
        Qt::ToolButtonTextBesideIcon);
}

void MainWindow::createGroupFootnotes(Qtitan::RibbonGroup& page)
{
    page.addAction(QIcon(":/res/largeInsertFootnote.png"), 
        tr("Insert Footnote"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/smallinsertendnote.png"), 
        tr("&Insert Endnote"), Qt::ToolButtonTextBesideIcon);
    QMenu* nextFootnote = new QMenu(this);
    page.addAction(QIcon(":/res/smallnextfootnote.png"), 
        tr("Next Footnote"), Qt::ToolButtonTextBesideIcon, nextFootnote);
    page.addAction(QIcon(":/res/smallshownotes.png"), 
        tr("Show Notes"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupCaptions(Qtitan::RibbonGroup& page)
{
    page.addAction(QIcon(":/res/largeInsertCaption.png"), 
        tr("Insert Caption"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/smallinserttablefigures.png"), 
        tr("Insert Table of Figures"), Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smallupdatetable.png"), 
        tr("Update Table"), Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smallcrossreference.png"), 
        tr("Cross-reference"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupIndex(Qtitan::RibbonGroup& page)
{
    page.addAction(QIcon(":/res/largeMarkEntry.png"), 
        tr("Mark Entry"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/smallinserttablefigures.png"), 
        tr("Insert Index"), Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smallupdatetable.png"), 
        tr("Update Index"), Qt::ToolButtonTextBesideIcon)->setEnabled(false);
}

void MainWindow::createGroupDocumentViews(Qtitan::RibbonGroup& page)
{
    QIcon printLayoutIcons;
    printLayoutIcons.addPixmap(QPixmap(":/res/largeprintlayout.png"));
    printLayoutIcons.addPixmap(QPixmap(":/res/smallprintlayout.png"));
    m_actionPrintLayout = page.addAction(printLayoutIcons, tr("Print Layout"), Qt::ToolButtonTextUnderIcon);
    m_actionPrintLayout->setCheckable(true);
    m_actionPrintLayout->setChecked(true);

    QIcon fullScreenIcons;
    fullScreenIcons.addPixmap(QPixmap(":/res/largefullscreen.png"));
    fullScreenIcons.addPixmap(QPixmap(":/res/smallfullscreen.png"));
    m_actionFullScreen = page.addAction(fullScreenIcons, tr("Full Screen Reading"), Qt::ToolButtonTextUnderIcon);

    m_actionFullScreen->setCheckable(true);
    m_actionFullScreen->setChecked(false);
    connect(m_actionFullScreen, SIGNAL(triggered(bool)), this, SLOT(fullScreen(bool)));
    page.addAction(QIcon(":/res/largeweblayout.png"), tr("Web Layout"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/largeoutline.png"), tr("Outline"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/largedraft.png"), tr("Draft"), Qt::ToolButtonTextUnderIcon);
}

void MainWindow::createGroupShow_Hide(Qtitan::RibbonGroup& page)
{
    QCheckBox* ruler = new QCheckBox(tr("Ruler"));
    ruler->setToolTip(tr("Ruler"));
//    check1->setCheckState(Qt::Checked);
    page.addWidget(ruler);

    QCheckBox* gridlines = new QCheckBox(tr("Gridlines"));
    gridlines->setToolTip(tr("Gridlines"));
    page.addWidget(gridlines);

    QCheckBox* properties = new QCheckBox(tr("Properties"));
    properties->setToolTip(tr("Properties"));
    page.addWidget(properties);

    QCheckBox* documentMap = new QCheckBox(tr("Document Map"));
    documentMap->setToolTip(tr("Document Map"));
    page.addWidget(documentMap);

    QCheckBox* statusBar = new QCheckBox(tr("Status Bar"));
    statusBar->setCheckState(Qt::Checked);
    statusBar->setToolTip(tr("Status Bar"));
    statusBar->setStatusTip(tr("Show or hide the status bar"));
    page.addWidget(statusBar);
    connect(statusBar, SIGNAL(stateChanged(int)), this, SLOT(stateStatusBar(int)));

    QCheckBox* messageBar = new QCheckBox(tr("Message Bar"));
    messageBar->setToolTip(tr("Message Bar"));
    page.addWidget(messageBar);
}

void MainWindow::createGroupZoom(Qtitan::RibbonGroup& page)
{
    page.addAction(QIcon(":/res/largezoom.png"), tr("Zoom"), Qt::ToolButtonTextUnderIcon);
    page.addAction(QIcon(":/res/smallonepage.png"), tr("One Page"), Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smalltwopage.png"), tr("Two Page"), Qt::ToolButtonTextBesideIcon);
    page.addAction(QIcon(":/res/smallpagewidth.png"), tr("Page Width"), Qt::ToolButtonTextBesideIcon);
}

void MainWindow::createStatusBar()
{
    Qtitan::RibbonStatusBar* statusBar = new Qtitan::RibbonStatusBar();
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

    m_zoomPercentage = new QToolButton(this);
    m_zoomPercentage->setText(tr("100%"));
    m_zoomPercentage->setToolTip(tr("Zoom level. Click to open the Zoom dialog box."));
    statusBar->addPermanentWidget(m_zoomPercentage);
    QFontMetrics mt(statusBar->font());
    int minimumWidth = mt.boundingRect(QString("0000%")).width();
    m_zoomPercentage->setMinimumWidth(minimumWidth);

    Qtitan::RibbonSliderPane* sliderPane = new Qtitan::RibbonSliderPane();
    sliderPane->setScrollButtons(true);
    sliderPane->setRange(0, 200); // range
    sliderPane->setMaximumWidth(130);
    sliderPane->setSingleStep(10);
    sliderPane->setSliderPosition(100);
    statusBar->addPermanentWidget(sliderPane, 1);
    connect(sliderPane, SIGNAL(valueChanged(int)), this, SLOT(zoomSliderScroll(int)));
}

void MainWindow::createOptions()
{
    QMenu* menu = ribbonBar()->addMenu(tr("Options"));
    QAction* actionStyle = menu->addAction(tr("Style"));

    QMenu* menuStyle = new QMenu(this);
    m_actionsStyle = new QActionGroup(this);

    Qtitan::RibbonStyle::Theme themeId = Qtitan::RibbonStyle::Office2007Blue;
    if (m_ribbonStyle)
        themeId = m_ribbonStyle->getTheme();

    QAction* actionBlue = menuStyle->addAction(tr("Office 2007 Blue"));
    actionBlue->setCheckable(true);
    actionBlue->setChecked(themeId == Qtitan::RibbonStyle::Office2007Blue);
    actionBlue->setObjectName("Office2007Blue");

    QAction* actionBlack = menuStyle->addAction(tr("Office 2007 Black"));
    actionBlack->setObjectName("Office2007Black");
    actionBlack->setCheckable(true);
    actionBlack->setChecked(themeId == Qtitan::RibbonStyle::Office2007Black);

    QAction* actionSilver = menuStyle->addAction(tr("Office 2007 Silver"));
    actionSilver->setObjectName("Office2007Silver");
    actionSilver->setCheckable(true);
    actionSilver->setChecked(themeId == Qtitan::RibbonStyle::Office2007Silver);

    QAction* actionAqua = menuStyle->addAction(tr("Office 2007 Aqua"));
    actionAqua->setObjectName("Office2007Aqua");
    actionAqua->setCheckable(true);
    actionAqua->setChecked(themeId == Qtitan::RibbonStyle::Office2007Aqua);

    QAction* actionScenic = menuStyle->addAction(tr("Windows 7 Scenic"));
    actionScenic->setObjectName("Windows7Scenic");
    actionScenic->setCheckable(true);
    actionScenic->setChecked(themeId == Qtitan::RibbonStyle::Windows7Scenic);

    QAction* action2010Blue = menuStyle->addAction(tr("Office 2010 Blue"));
    action2010Blue->setObjectName("Office2010Blue");
    action2010Blue->setCheckable(true);
    action2010Blue->setChecked(themeId == Qtitan::RibbonStyle::Office2010Blue);

    QAction* action2010Silver = menuStyle->addAction(tr("Office 2010 Silver"));
    action2010Silver->setObjectName("Office2010Silver");
    action2010Silver->setCheckable(true);
    action2010Silver->setChecked(themeId == Qtitan::RibbonStyle::Office2010Silver);

    QAction* action2010Black = menuStyle->addAction(tr("Office 2010 Black"));
    action2010Black->setObjectName("Office2010Black");
    action2010Black->setCheckable(true);
    action2010Black->setChecked(themeId == Qtitan::RibbonStyle::Office2010Black);

    m_actionsStyle->addAction(actionBlue);
    m_actionsStyle->addAction(actionBlack);
    m_actionsStyle->addAction(actionSilver);
    m_actionsStyle->addAction(actionAqua);
    m_actionsStyle->addAction(actionScenic);
    m_actionsStyle->addAction(action2010Blue);
    m_actionsStyle->addAction(action2010Silver);
    m_actionsStyle->addAction(action2010Black);

    actionStyle->setMenu(menuStyle);
    connect(m_actionsStyle, SIGNAL(triggered(QAction*)), this, SLOT(options(QAction*)));

    menu->addSeparator();

    QAction* actionAnimation = menu->addAction(tr("Animation"));
    actionAnimation->setCheckable(true);
    actionAnimation->setChecked(true);
    connect(actionAnimation, SIGNAL(triggered(bool)), this, SLOT(includingAnimation(bool)));

#ifdef Q_WIN
    QAction* actionFrameTheme = menu->addAction(tr("Frame Theme"));
    actionFrameTheme->setCheckable(true);
    actionFrameTheme->setChecked(true);
    connect(actionFrameTheme, SIGNAL(triggered(bool)), this, SLOT(frameTheme(bool)));
#endif // Q_WIN

    menu->addSeparator();
    QAction* actionDPI_100_percent = menu->addAction(tr("DPI Aware (new)"));
    actionDPI_100_percent->setCheckable(true);
    actionDPI_100_percent->setChecked(true);
    connect(actionDPI_100_percent, SIGNAL(toggled(bool)), this, SLOT(setDPIToggled(bool)));

    menu->addSeparator();

    QAction* actionCusomize = menu->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);
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
    QCheckBox* linkedStylesBox = new QCheckBox(tr("Disable Linked Styles"));
    optionsLayout->addWidget(showPreviewBox);
    optionsLayout->addWidget(linkedStylesBox);
    optionsGroup->setLayout(optionsLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(styleGroup);
    mainLayout->addWidget(optionsGroup);
    //    mainLayout->addStretch(1);
    styleWidget->setLayout(mainLayout);

    dock->setWidget(styleWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

bool MainWindow::load(const QString& f)
{
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

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

    setCurrentFileName(f);
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

void MainWindow::setCurrentFileName(const QString &m_fileName)
{
    this->m_fileName = m_fileName;

    QString shownName;
    if (m_fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(m_fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Qtitan Ribbon")));
    m_textEdit->document()->setModified(false);
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
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog* dlg = new QPrintDialog(&printer, this);
    if (m_textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        m_textEdit->print(&printer);
    }
    delete dlg;
#endif
}

void MainWindow::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
#endif
}

void MainWindow::printPreview(QPrinter* printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    m_textEdit->print(printer);
#endif
}


void MainWindow::filePrintPdf()
{
#ifndef QT_NO_PRINTER
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
#endif
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

void MainWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void MainWindow::minimizationChanged(bool minimized)
{
    m_actionRibbonMinimize->setChecked(minimized);
    m_actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png") :  QIcon(":/shared/res/ribbonMinimize.png"));
}

void MainWindow::fullScreen(bool checked)
{
    if (checked)
    {
        m_actionFullScreen->setChecked(true);
        ribbonBar()->setFrameThemeEnabled(false);
        ribbonBar()->setVisible(false);
        showFullScreen();
    }
    else
    {
        ribbonBar()->setFrameThemeEnabled(true);
        ribbonBar()->setVisible(true);
        showNormal();
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

void MainWindow::about()
{
    Qtitan::AboutDialog::show(this, tr("About Qtitan Ribbon Controls Sample"), 
        tr("QtitanRibbon"), QLatin1String(QTN_VERSION_RIBBON_STR));
}

void MainWindow::options(QAction* action)
{
    if (m_ribbonStyle)
    {
        Qtitan::OfficeStyle::Theme themeId = Qtitan::OfficeStyle::Office2007Blue;
        if (action->objectName() == "Office2007Black")
            themeId = Qtitan::OfficeStyle::Office2007Black;
        else if (action->objectName() == "Office2007Silver")
            themeId = Qtitan::OfficeStyle::Office2007Silver;
        else if (action->objectName() == "Office2007Aqua")
            themeId = Qtitan::OfficeStyle::Office2007Aqua;
        else if (action->objectName() == "Windows7Scenic")
            themeId = Qtitan::OfficeStyle::Windows7Scenic;
        else if (action->objectName() == "Office2010Blue")
            themeId = Qtitan::OfficeStyle::Office2010Blue;
        else if (action->objectName() == "Office2010Silver")
            themeId = Qtitan::OfficeStyle::Office2010Silver;
        else if (action->objectName() == "Office2010Black")
            themeId = Qtitan::OfficeStyle::Office2010Black;

        if (QToolButton* button = ribbonBar()->getSystemButton())
        {
            if (themeId == Qtitan::OfficeStyle::Windows7Scenic || 
                themeId == Qtitan::OfficeStyle::Office2010Blue ||
                themeId == Qtitan::OfficeStyle::Office2010Silver ||
                themeId == Qtitan::OfficeStyle::Office2010Black)
                button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            else
                button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
        }

        m_ribbonStyle->setTheme(themeId);
    }
}

void MainWindow::updateActionsTheme()
{
    Qtitan::OfficeStyle* st = (Qtitan::OfficeStyle*)style();
    Qtitan::OfficeStyle::Theme theme = st->getTheme();

    QString strTheme = "Office2007Blue";
    if (theme == Qtitan::OfficeStyle::Office2007Black)
        strTheme = "Office2007Black";
    else if (theme == Qtitan::OfficeStyle::Office2007Silver)
        strTheme = "Office2007Silver";
    else if (theme == Qtitan::OfficeStyle::Office2007Aqua)
        strTheme = "Office2007Aqua";
    else if (theme == Qtitan::OfficeStyle::Windows7Scenic)
        strTheme = "Windows7Scenic";
    else if (theme == Qtitan::OfficeStyle::Office2010Silver)
        strTheme = "Office2010Silver";
    else if (theme == Qtitan::OfficeStyle::Office2010Blue)
        strTheme = "Office2010Blue";
    else if (theme == Qtitan::OfficeStyle::Office2010Black)
        strTheme = "Office2010Black";

    QList<QAction*> list = m_actionsStyle->actions();
    for (int i = 0, count = list.count(); count > i; i++)
        list[i]->setChecked(list[i]->objectName() == strTheme);
}

void MainWindow::includingAnimation(bool checked)
{
    if (Qtitan::OfficeStyle* st = qobject_cast<Qtitan::OfficeStyle*>(style()))
        st->setAnimationEnabled(checked);
}

#ifdef Q_OS_WIN
void MainWindow::frameTheme(bool checked)
{
    ribbonBar()->setFrameThemeEnabled(checked);
}
#endif // Q_WIN

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

void MainWindow::setDPIToggled(bool on)
{
    m_ribbonStyle->setDPIAware(on);
}

void MainWindow::zoomSliderScroll(int value)
{
    QString str;
    str += QObject::tr("%1%").arg(value);
    m_zoomPercentage->setText(str);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    Qtitan::RibbonMainWindow::keyPressEvent(event);
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
    m_comboFont->setCurrentIndex(m_comboFont->findText(QFontInfo(f).family()));
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

void MainWindow::readSettings()
{
    QSettings settings("MainWindow", "Qtitan Ribbon Sample");

    Qtitan::OfficeStyle* st = (Qtitan::OfficeStyle*)style();
    Qtitan::OfficeStyle::Theme theme = (Qtitan::OfficeStyle::Theme)settings.value("theme", 
        (int)Qtitan::OfficeStyle::Office2007Blue).toInt();

    if (theme == Qtitan::OfficeStyle::Windows7Scenic   || 
        theme == Qtitan::OfficeStyle::Office2010Silver || 
        theme == Qtitan::OfficeStyle::Office2010Blue   ||
        theme == Qtitan::OfficeStyle::Office2010Black)
        ribbonBar()->getSystemButton()->setToolButtonStyle(Qt::ToolButtonTextOnly);
    else
        ribbonBar()->getSystemButton()->setToolButtonStyle(Qt::ToolButtonFollowStyle);

    if (theme != st->getTheme())
        st->setTheme(theme);

    updateActionsTheme();

    QRect geom = QApplication::desktop()->availableGeometry();
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize sz = settings.value("size", QSize(2 * geom.width() / 3, 2 * geom.height() / 3)).toSize();
    move(pos);
    resize(sz);

    bool minimized = settings.value("ribbonMinimized", false).toBool();
    ribbonBar()->setMinimized(minimized);
}

void MainWindow::writeSettings()
{
    QSettings settings("MainWindow", "Qtitan Ribbon Sample");
    settings.setValue("pos", pos());
    settings.setValue("size", size());

    Qtitan::OfficeStyle* st = (Qtitan::OfficeStyle*)style();
    int theme = (int)st->getOptionStyle();
    settings.setValue("theme", theme);

    bool minimized = ribbonBar()->isMinimized();
    settings.setValue("ribbonMinimized", minimized);
}
