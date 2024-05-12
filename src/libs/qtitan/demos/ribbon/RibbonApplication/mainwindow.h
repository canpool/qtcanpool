#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"

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
class MainWindow : public RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    virtual ~MainWindow();

public:
    QString ribbonBackground() const;
    void setRibbonBackground(const QString& name);

protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void createAction();
    void createMenuFile();
    void createQuickAccessBar();
    void createRibbonBar();
    // [Home]
    void createGroupClipboard(RibbonPage* page);
    void createGroupFont(RibbonPage* page);
    void createGroupParagraph(RibbonPage* page);
    void createGroupStyles(RibbonPage* page);
    void createGroupEditing(RibbonPage* page);
    // [Insert]
    void createGroupPages(RibbonPage* page);
    void createGroupTables(RibbonPage* page);
    void createGroupIllustrations(RibbonPage* page);
    void createGroupLinks(RibbonPage* page);
    void createGroupHeaderFooter(RibbonPage* page);
    void createGroupText(RibbonPage* page);
    void createGroupSymbols(RibbonPage* page);

    // [Page Layout]
    void createGroupThemes(RibbonGroup* group);
    void createGroupPageSetup(RibbonGroup* group);
    void createGroupPageBackground(RibbonGroup* group);
    void createGroupParagraphLayout(RibbonGroup* group);
    // [References]
    void createGroupTableOfContents(RibbonGroup* group);
    void createGroupFootnotes(RibbonGroup* group);
    void createGroupCaptions(RibbonGroup* group);
    void createGroupIndex(RibbonGroup* group);
    // [View]
    void createGroupDocumentViews(RibbonGroup* group);
    void createGroupShow_Hide(RibbonGroup* group);
    void createGroupZoom(RibbonGroup* group);

    void createStatusBar();
    void createOptions();
    void createDockWindows();

    bool load(const QString& f);
    bool maybeSave();
    void setCurrentFileName(const QString& m_fileName);

protected Q_SLOTS:
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
    void fullScreen(bool checked);
    void stateStatusBar(int state);

    void currentCharFormatChanged(const QTextCharFormat& format);
    void cursorPositionChanged();

    void clipboardDataChanged();
    void printPreview(QPrinter*);
    void selectionChanged();
    void zoomSliderScroll(int value);
    virtual void showRibbonContextMenu(QMenu* menu, QContextMenuEvent* event);
    void showCompanyWebSite();
protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    void mergeFormatOnWordOrSelection(const QTextCharFormat& format);
    void fontChanged(const QFont& f);
    void colorChanged(const QColor& c);
    void alignmentChanged(Qt::Alignment a);
    void customization();
    void readSettings();
    void writeSettings();
    virtual void updateActionsTheme();
    void updateRibbonBackground();
    void updateImageStyles();

protected:
    QAction* m_actionFileNew;
    QAction* m_actionOpenFile;
    QAction* m_actionSaveFile;
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
    QAction* m_actionPaste1;
    QAction* m_actionPaste2;
    QAction* m_actionFormatPointerAction;
    QAction* m_actionSelectAll;
    QAction* m_actionFullScreen;
    QAction* m_actionPrintLayout;

    RibbonPage* m_pageHome;
    RibbonPage* m_pageInsert;
    RibbonPage* m_pageLayout;
    RibbonPage* m_pageRef;
    RibbonPage* m_pageMailings;
    RibbonPage* m_pageReview;
    RibbonPage* m_pageView;

    RibbonGalleryGroup* m_itemsStyles;
    PopupColorButton* m_colorButton;
    QTextEdit* m_textEdit;
    QString m_fileName;
    QFontComboBox* m_comboFont;
    QComboBox* m_comboSize;
    QToolButton* m_zoomPercentage;

    RibbonPage* m_pagePictureFormat;
    Qt::WindowStates m_stateWindow;
    QString m_curRibbonBackground;

private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
