#include <QApplication>
#include <QMessageBox>
#include <QTextStream>
#include <QClipboard>
#include <QFileDialog>
#include <QFontComboBox>
#include <QTextList>

#include <QtitanRibbon.h>

#include "texteditchild.h"


/* TextEditChildPrivate */
class TextEditChildPrivate
{
public:
    TextEditChildPrivate(TextEditChild* p);

public:
    Qtitan::RibbonPage* pPageHome_;

public:
    static int currentIndexPage_;

private:
    TextEditChild* pParent_;
};

int TextEditChildPrivate::currentIndexPage_ = -1;
TextEditChildPrivate::TextEditChildPrivate(TextEditChild* p)
    : pParent_(p)
{
}



#undef THIS
#define THIS this->pimpl

/* TextEditChild */
TextEditChild::TextEditChild()
{
    setWindowIcon(QIcon(":/res/editorchild.png"));
    pimpl = new TextEditChildPrivate(this);
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;

    createActions();

    connect(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)), this, SLOT(currentCharFormatChanged(QTextCharFormat)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(cursorPositionChanged()));

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);

    connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));

    connect(this, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

#ifndef QT_NO_CLIPBOARD
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));
#endif
}

TextEditChild::~TextEditChild()
{
    delete pimpl;
}

void TextEditChild::buildUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome)
{
    Q_UNUSED(pStatusBar);
    Q_UNUSED(pageHome);
    THIS->pPageHome_ = pRib->insertPage(0, tr("&Home"));
    if(THIS->pPageHome_)
    {
        createGroupClipboard(THIS->pPageHome_);
        createGroupFont(THIS->pPageHome_);
        createGroupParagraph(THIS->pPageHome_);
//        createGroupEditing(page);

        fontChanged(font());
        colorChanged(this->QTextEdit::textColor());
        alignmentChanged(alignment());
    }

    if (!pRib->isMinimized())
    {
        int indexPage = THIS->currentIndexPage_ != -1 ? THIS->currentIndexPage_ : 0;
        pRib->setCurrentPage(indexPage);
    }
}

void TextEditChild::removeUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome)
{
    Q_UNUSED(pStatusBar);
    Q_UNUSED(pageHome);
    if (THIS->pPageHome_)
    {
        THIS->currentIndexPage_ = pRib->currentIndexPage();
        pRib->removePage(THIS->pPageHome_);
        THIS->pPageHome_ = Q_NULL;
    }
}

void TextEditChild::newFile()
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("document%1.txt").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");

    connect(document(), SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
}

bool TextEditChild::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), SIGNAL(contentsChanged()),this, SLOT(documentWasModified()));

    return true;
}

bool TextEditChild::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool TextEditChild::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
                                                    curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool TextEditChild::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) 
    {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    return true;
}

QString TextEditChild::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

bool TextEditChild::hasSelection() const
{
    return textCursor().hasSelection();
}

void TextEditChild::createActions()
{
    actionPaste = new QAction(QIcon(":/res/paste.png"), tr("&Paste"), this);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionCut = new QAction(QIcon(":/res/smallcut.png"), tr("&Cut"), this);
    actionCut->setShortcut(QKeySequence::Cut);
    actionCopy = new QAction(QIcon(":/res/smallcopy.png"), tr("&Copy"), this);
    actionCopy->setShortcut(QKeySequence::Copy);
    actionFormatPointer = new QAction(QIcon(":/res/smallformatpainter.png"), tr("&Format Pointer"), this);

    actionTextBold = new QAction(QIcon(":/res/smalltextbold.png"), tr("&Bold"), this);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    connect(actionTextBold, SIGNAL(triggered()), this, SLOT(textBold()));
    actionTextBold->setCheckable(true);

    actionTextItalic = new QAction(QIcon(":/res/smalltextitalic.png"), tr("&Italic"), this);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    connect(actionTextItalic, SIGNAL(triggered()), this, SLOT(textItalic()));
    actionTextItalic->setCheckable(true);

    actionTextUnderline = new QAction(QIcon(":/res/smalltextunder.png"), tr("&Underline"), this);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    connect(actionTextUnderline, SIGNAL(triggered()), this, SLOT(textUnderline()));
    actionTextUnderline->setCheckable(true);

    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    actionAlignLeft = new QAction(QIcon(":/res/smallalignleft.png"), tr("Align Left"), this);
    actionAlignLeft->setActionGroup(grp);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);

    actionAlignCenter = new QAction(QIcon(":/res/smallcenter.png"), tr("Center"), this);
    actionAlignCenter->setActionGroup(grp);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);

    actionAlignRight = new QAction(QIcon(":/res/smallalignright.png"), tr("Align Right"), this);
    actionAlignRight->setActionGroup(grp);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);

    actionAlignJustify = new QAction(QIcon(":/res/smalljustify.png"), tr("Justify"), this);
    actionAlignJustify->setActionGroup(grp);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);
}

void TextEditChild::createGroupClipboard(Qtitan::RibbonPage* page)
{
    if(Qtitan::RibbonGroup * groupClipboard = page->addGroup(tr("Clipboard"))) 
    {
#ifndef QT_NO_CLIPBOARD
        actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
        actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
#endif
        groupClipboard->addAction(actionPaste, Qt::ToolButtonTextUnderIcon);
        groupClipboard->addAction(actionCut, Qt::ToolButtonTextBesideIcon);
        groupClipboard->addAction(actionCopy, Qt::ToolButtonTextBesideIcon);
        groupClipboard->addAction(actionFormatPointer, Qt::ToolButtonTextBesideIcon);
        actionFormatPointer->setEnabled(false);
    }
}

void TextEditChild::createGroupFont(Qtitan::RibbonPage* page)
{
    if(Qtitan::RibbonGroup* groupFont = page->addGroup(tr("Font"))) 
    {
        groupFont->setControlsGrouping();
        groupFont->setControlsCentering();

        comboFont = new QFontComboBox(groupFont);
        comboFont->setMaximumWidth(130);
        groupFont->addWidget(comboFont);
        connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

        comboSize = new QComboBox(groupFont);
        comboSize->setMaximumWidth(45);
        comboSize->setObjectName("comboSize");
        groupFont->addWidget(comboSize);
        comboSize->setEditable(true);
        QFontDatabase db;
        foreach(int size, db.standardSizes())
            comboSize->addItem(QString::number(size));

        connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
        comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));

        groupFont->addSeparator();
        groupFont->addAction( QIcon(":/res/smallfontgrow.png"), tr("Grow Font"), Qt::ToolButtonIconOnly);
        groupFont->addAction( QIcon(":/res/smallfontshrink.png"), tr("Shrink Font"), Qt::ToolButtonIconOnly);
        groupFont->addSeparator();
        groupFont->addAction( QIcon(":/res/smallfontclear.png"), tr("&Clear Formatting"), Qt::ToolButtonIconOnly );
        groupFont->addSeparator();

        groupFont->addAction(actionTextBold, Qt::ToolButtonIconOnly);
        groupFont->addAction(actionTextItalic, Qt::ToolButtonIconOnly);
        groupFont->addAction(actionTextUnderline, Qt::ToolButtonIconOnly);

        QAction* pCharStrikethrough = groupFont->addAction( QIcon(":/res/smallstrikethrough.png"), tr("Strikethrough"), Qt::ToolButtonIconOnly);
        pCharStrikethrough->setEnabled(false);
        groupFont->addSeparator();
        QAction* pTextSubscript = groupFont->addAction( QIcon(":/res/smallsubscript.png"), tr("Subscript"), Qt::ToolButtonIconOnly);
        pTextSubscript->setEnabled(false);
        QAction* pTextSuperscript = groupFont->addAction( QIcon(":/res/smallsuperscript.png"), tr("Superscript"), Qt::ToolButtonIconOnly);
        pTextSuperscript->setEnabled(false);
        QAction* pTextChangecase = groupFont->addAction( QIcon(":/res/smallchangecase.png"), tr("Change Case"), Qt::ToolButtonIconOnly);
        pTextChangecase->setEnabled(false);
        groupFont->addSeparator();

        Qtitan::PopupColorButton* pHighlightColorButton = new Qtitan::PopupColorButton(groupFont);
        QAction* pTextHighlightcolor= groupFont->addWidget(QIcon(":/res/smallcolor.png"), tr("Highlight Color"), pHighlightColorButton);
        pTextHighlightcolor->setEnabled(false);

        pColorButton = new Qtitan::PopupColorButton(groupFont);
        actionTextColor = groupFont->addWidget(QIcon(":/res/smallcolor.png"), tr("Color"), pColorButton);
        connect(pColorButton, SIGNAL(colorChanged(const QColor&)), this, SLOT(textColor(const QColor&)));
        connect(actionTextColor, SIGNAL(triggered()), this, SLOT(setColorText()));
    }
}

void TextEditChild::createGroupParagraph(Qtitan::RibbonPage* page)
{
    Qtitan::RibbonGroup * groupParagraph = page->addGroup(tr("&Paragraph"));
    if(groupParagraph) 
    {
        groupParagraph->setControlsGrouping();
        groupParagraph->setControlsCentering();

        QMenu * pMenu = new QMenu(this);
        groupParagraph->addAction( QIcon(":/res/smallbullets.png"), tr("Bullets"), Qt::ToolButtonIconOnly, pMenu);
        groupParagraph->addAction( QIcon(":/res/smallnumbering.png"), tr("Numbering"), Qt::ToolButtonIconOnly, pMenu);
        groupParagraph->addAction( QIcon(":/res/smallmultilevellist.png"), tr("Multilevel List"), Qt::ToolButtonIconOnly, pMenu);
        groupParagraph->addSeparator();
        groupParagraph->addAction( QIcon(":/res/smalldecreaseindent.png"), tr("Decrease Indent"), Qt::ToolButtonIconOnly);
        groupParagraph->addAction( QIcon(":/res/smallincreaseindent.png"), tr("Increase Indent"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        groupParagraph->addAction( QIcon(":/res/smallsort.png"), tr("Sort"), Qt::ToolButtonIconOnly);
        groupParagraph->addAction( QIcon(":/res/smallshow_hide_marks.png"), tr("Show/Hide Marks"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        QActionGroup *grp = new QActionGroup(this);
        connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

        groupParagraph->addAction(actionAlignLeft, Qt::ToolButtonIconOnly);
        groupParagraph->addAction(actionAlignCenter, Qt::ToolButtonIconOnly);
        groupParagraph->addAction(actionAlignRight, Qt::ToolButtonIconOnly);
        groupParagraph->addAction(actionAlignJustify, Qt::ToolButtonIconOnly);

        groupParagraph->addSeparator();
        groupParagraph->addAction( QIcon(":/res/smalllinespacing.png"), tr("Line spacing"), Qt::ToolButtonIconOnly);
        groupParagraph->addSeparator();
        groupParagraph->addAction( QIcon(":/res/smallshading.png"), tr("Shading"), Qt::ToolButtonIconOnly, pMenu);
        groupParagraph->addSeparator();
        groupParagraph->addAction( QIcon(":/res/smallnoborder.png"), tr("No Border"), Qt::ToolButtonIconOnly, pMenu);
    }
}


void TextEditChild::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void TextEditChild::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEditChild::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEditChild::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEditChild::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEditChild::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void TextEditChild::textStyle(int styleIndex)
{
    QTextCursor cursor = textCursor();

    if (styleIndex != 0) {
        QTextListFormat::Style style = QTextListFormat::ListDisc;

        switch (styleIndex) {
            default:
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
        }

        cursor.beginEditBlock();

        QTextBlockFormat blockFmt = cursor.blockFormat();

        QTextListFormat listFmt;

        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }

        listFmt.setStyle(style);

        cursor.createList(listFmt);

        cursor.endEditBlock();
    } else {
        // ####
        QTextBlockFormat bfmt;
        bfmt.setObjectIndex(-1);
        cursor.mergeBlockFormat(bfmt);
    }
}

void TextEditChild::textColor(const QColor& col)
{
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEditChild::setColorText()
{
    textColor(pColorButton->color());
}

void TextEditChild::textAlign(QAction *a)
{
    if (a == actionAlignLeft)
        setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        setAlignment(Qt::AlignJustify);
}

void TextEditChild::selectAll()
{
    QTextEdit::selectAll();
}

void TextEditChild::currentCharFormatChanged(const QTextCharFormat &format)
{
    fontChanged(format.font());
}

void TextEditChild::cursorPositionChanged()
{
    alignmentChanged(alignment());
}

void TextEditChild::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void TextEditChild::clipboardDataChanged()
{
#ifndef QT_NO_CLIPBOARD
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
#endif
}

void TextEditChild::mergeFormatOnWordOrSelection(const QTextCharFormat& format)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    mergeCurrentCharFormat(format);
}

void TextEditChild::colorChanged(const QColor& c)
{
    pColorButton->setColor(c);
}

void TextEditChild::fontChanged(const QFont& f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

bool TextEditChild::maybeSave()
{
    if (document()->isModified()) 
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("MDI"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
            | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void TextEditChild::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString TextEditChild::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void TextEditChild::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft) {
        actionAlignLeft->setChecked(true);
    } else if (a & Qt::AlignHCenter) {
        actionAlignCenter->setChecked(true);
    } else if (a & Qt::AlignRight) {
        actionAlignRight->setChecked(true);
    } else if (a & Qt::AlignJustify) {
        actionAlignJustify->setChecked(true);
    }
}

#undef THIS
