#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtitanRibbon.h>

class QTextEdit;
class QTextCharFormat;
class QFontComboBox;
class QComboBox;
class QPrinter;

namespace Qtitan
{
    class PopupColorButton;
}
/* MainWindow */
class MainWindow : public Qtitan::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void createAction();
    void createMenuFile();
    void createQuickAccessBar();
    void createRibbon();
    // [Home]
    void createGroupClipboard(Qtitan::RibbonPage* page);
    void createGroupFont(Qtitan::RibbonPage* page); 
    void createGroupParagraph(Qtitan::RibbonPage* page);
    void createGroupEditing(Qtitan::RibbonPage* page);
    // [Page Layout]
    void createGroupThemes(Qtitan::RibbonGroup& page);
    void createGroupPageSetup(Qtitan::RibbonGroup& page);
    void createGroupPageBackground(Qtitan::RibbonGroup& page);
    void createGroupParagraphLayout(Qtitan::RibbonGroup& page);
    // [References]
    void createGroupTableOfContents(Qtitan::RibbonGroup& page);
    void createGroupFootnotes(Qtitan::RibbonGroup& page);
    void createGroupCaptions(Qtitan::RibbonGroup& page);
    void createGroupIndex(Qtitan::RibbonGroup& page);
    // [View]
    void createGroupDocumentViews(Qtitan::RibbonGroup& page);
    void createGroupShow_Hide(Qtitan::RibbonGroup& page);
    void createGroupZoom(Qtitan::RibbonGroup& page);

    void createStatusBar();
    void createOptions();
    void createDockWindows();

    bool load(const QString& f);
    bool maybeSave();
    void setCurrentFileName(const QString& m_fileName);

protected slots:
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
    void textColor(const QColor& col);
    void setColorText();
    void textAlign(QAction* a);
    void selectAll();
    void optionParagraph();
    void optionClipboard();
    void showHideMarks();
    void optionFont();
    void maximizeToggle();
    void minimizationChanged(bool minimized);
    void fullScreen(bool checked);
    void stateStatusBar(int state);

    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void about();
    void printPreview(QPrinter*);
    void options(QAction* action);
    void includingAnimation(bool);
#ifdef Q_OS_WIN
    void frameTheme(bool);
#endif // Q_OS_WIN
    void selectionChanged();
    void setDPIToggled(bool);
    void zoomSliderScroll(int value);

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);
    void readSettings();
    void writeSettings();
    void updateActionsTheme();

protected:
    QAction* m_actionFileNew;
    QAction* m_actionOpenFile;
    QAction* m_actionSave;
    QAction* m_actionPrint;
    QAction* m_actionPrintSetup;
    QAction* m_actionPrintPreview;

    QAction* m_actionTextBold;
    QAction* m_actionTextUnderline;
    QAction* m_actionTextItalic;
    QAction* m_actionTextColor;
    QAction* m_actionAlignLeft;
    QAction* m_actionAlignCenter;
    QAction* m_actionAlignRight;
    QAction* m_actionAlignJustify;
    QAction* m_actionUndo;
    QAction* m_actionRedo;

    QAction* m_actionCut;
    QAction* m_actionCopy;
//    QAction *m_pActionPaste;
    QAction* m_actionPaste1;
    QAction* m_actionPaste2;
    QAction* m_actionFormatPointerAction;
    QAction* m_actionSelectAll;
    QAction* m_actionFullScreen;
    QAction* m_actionPrintLayout;
    
    QActionGroup* m_actionsStyle;
    QAction* m_actionRibbonMinimize;

    Qtitan::PopupColorButton* m_colorButton;
    QTextEdit* m_textEdit;
    QString m_fileName;
    QFontComboBox* m_comboFont;
    QComboBox* m_comboSize;
    QToolButton* m_zoomPercentage;

    Qtitan::RibbonPage* m_pagePictureFormat;
    Qtitan::RibbonStyle* m_ribbonStyle;

private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
