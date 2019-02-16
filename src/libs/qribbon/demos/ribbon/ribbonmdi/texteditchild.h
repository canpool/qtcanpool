#ifndef TEXTEDITCHILD_H
#define TEXTEDITCHILD_H

#include <QTextEdit>
#include "childinterface.h"

namespace Qtitan 
{
    class RibbonBar; 
    class RibbonPage;
    class RibbonGroup;
    class PopupColorButton;
}
class TextEditChildPrivate;
class QFontComboBox;
class QComboBox;

/* TextEditChild */
class TextEditChild : public QTextEdit, 
                      public ChildInterface
{
    Q_OBJECT
    Q_INTERFACES(ChildInterface)
public:
    TextEditChild();
    virtual ~TextEditChild();

public:
    virtual TypeDocument typeDocument() const { return td_Text; }
    virtual void buildUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome);
    virtual void removeUserInterface(Qtitan::RibbonBar* pRib, QStatusBar* pStatusBar, Qtitan::RibbonPage* pageHome);

    void newFile();
    bool loadFile(const QString& fileName);
    virtual bool save();
    virtual bool saveAs();
    bool saveFile(const QString& fileName);
    virtual QString userFriendlyCurrentFile();
    virtual QString currentFile() const { return curFile; }

    virtual bool hasSelection() const;

protected:
    void createActions();

    // [Home]
    void createGroupClipboard(Qtitan::RibbonPage* page);
    void createGroupFont(Qtitan::RibbonPage* page); 
    void createGroupParagraph(Qtitan::RibbonPage* page);
    //    void createGroupEditing(Qtitan::RibbonPage* page);

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString& f);
    void textSize(const QString& p);
    void textStyle(int styleIndex);
    void textColor(const QColor& col);
    void setColorText();
    void textAlign(QAction* a);
    void selectAll();

    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();

    void documentWasModified();
    void clipboardDataChanged();

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);

    bool maybeSave();
    void setCurrentFile(const QString& fileName);
    QString strippedName(const QString& fullFileName);

protected:
    QString curFile;
    bool isUntitled;

    QAction* actionTextBold;
    QAction* actionTextUnderline;
    QAction* actionTextItalic;
    QAction* actionTextColor;
    QAction* actionAlignLeft;
    QAction* actionAlignCenter;
    QAction* actionAlignRight;
    QAction* actionAlignJustify;
    QAction* actionUndo;
    QAction* actionRedo;

    QAction* actionPaste;
    QAction* actionCut;
    QAction* actionCopy;
    QAction* actionFormatPointer;

    Qtitan::PopupColorButton* pColorButton;
    QFontComboBox*  comboFont;
    QComboBox*      comboSize;

private:
    friend class TextEditChildPrivate;
    TextEditChildPrivate* pimpl;
};

#endif // TEXTEDITCHILD_H
