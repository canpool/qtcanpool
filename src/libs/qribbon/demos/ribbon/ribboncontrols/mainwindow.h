#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QtitanRibbon.h>

class QProgressBar;

/* MainWindow */
class MainWindow : public Qtitan::RibbonMainWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();

private:
    void createMenuFile();
    void createQuickAccessBar();
    void createRibbon();
    void createStatusBar();
    void createOptions();

Q_SIGNALS:
    void updateRecentFileActions(const QStringList& files);

protected slots:
    void open();
    bool save();

    void openRecentFile(const QString& file);

#ifndef QT_NO_PRINTER
    void print();
    void printSetup();
#endif
    void about();
    void options(QAction*);
    void pressButton();
    void startProgressBar(int);
    void animateProgressBar();
    void buttonColorClicked(QAbstractButton*);
    void maximizeToggle();
    void minimizationChanged(bool minimized);
    void optionsFont(QAction* act);
    void setDPIToggled(bool);

private:
    void setCurrentFile(const QString &fileName);

private:
    QAction* actionRibbonMinimize;
    QAction* m_actionDefault;
    QAction* m_actionNormal;
    QAction* m_actionLarge;
    QAction* m_actionExLarge;

    QTimer m_timerProgressBar;
    QProgressBar* m_progressBar;
    QProgressBar* m_progressBar1;
    QProgressBar* m_progressBar2;

    int m_valueProgress;
    int m_defaultFont;

    Qtitan::RibbonPage* m_pageContext;
    Qtitan::RibbonStyle* m_ribbonStyle;
};

#endif // MAINWINDOW_H
