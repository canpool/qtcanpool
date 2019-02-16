#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QMainWindow>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QPrinter)

namespace Qtitan
{
    class PopupColorButton;
}

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget* parent = 0);

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void setupFileActions();
    void setupEditActions();
    void setupViewActions();
    void setupTextActions();
    bool load(const QString& f);
    bool maybeSave();
    void setCurrentFileName(const QString& fileName);

private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void textBold();
    void textUnderline();
    void textItalic();
    void textFamily(const QString& f);
    void textSize(const QString& p);
    void textStyle(int styleIndex);
    void setColorText();
    void textColor(const QColor& col);
    void textAlign(QAction* a);

    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter*);
    void hideStatusBar();
    void standardBar();
    void formattingBar();
    void options(QAction* action);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);

    QAction *actionSave,
        *actionTextBold,
        *actionTextUnderline,
        *actionTextItalic,
        *actionTextColor,
        *actionAlignLeft,
        *actionAlignCenter,
        *actionAlignRight,
        *actionAlignJustify,
        *actionUndo,
        *actionRedo,
        *actionCut,
        *actionCopy,
        *actionPaste, 
        *actionStatusBar,
        *actionStandardBar,
        *actionFormattingBar;

    QComboBox* comboStyle;
    QFontComboBox* comboFont;
    QComboBox* comboSize;
    Qtitan::PopupColorButton* colorButton;

    QToolBar* standardToolBar;
    QToolBar* formattingToolBar;
    QString fileName;
    QTextEdit* textEdit;
};

#endif
