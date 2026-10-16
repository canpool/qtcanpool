#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

#include <QScopedPointer>

QX_RIBBON_BEGIN_NAMESPACE
class RibbonMdiArea;
class RibbonPage;
QX_RIBBON_END_NAMESPACE

class QTextEdit;

class MainWindow : public QxRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void createFileGroup(QxRibbon::RibbonPage *page);
    void createWindowGroup(QxRibbon::RibbonPage *page);
    void createDocumentPages();
    void newDocument(bool sketch);
    void updateStatus();

    QTextEdit *activeEditor() const;

    QxRibbon::RibbonMdiArea *m_mdiArea;
    QxRibbon::RibbonPage *m_textPage;
    QxRibbon::RibbonPage *m_sketchPage;
    int m_textCount;
    int m_sketchCount;
};
#endif   // MAINWINDOW_H
