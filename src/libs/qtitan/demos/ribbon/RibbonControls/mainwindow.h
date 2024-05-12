#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include "ribbonwindow.h"

class QProgressBar;

/* MainWindow */
class MainWindow : public RibbonWindow
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

Q_SIGNALS:
    void updateRecentFileActions(const QStringList& files);

protected Q_SLOTS:
    void open();
    bool save();

    void openRecentFile(const QString& file);

    void print();
    void printSetup();
    void pressButton();
    void startProgressBar(int);
    void animateProgressBar();
    void buttonColorClicked(QAbstractButton*);

private:
    void setCurrentFile(const QString &fileName);

private:
    QTimer m_timerProgressBar;
    QProgressBar* m_progressBar;
    QProgressBar* m_progressBar1;
    QProgressBar* m_progressBar2;

    int m_valueProgress;

    RibbonPage* m_contextPage;
};

#endif // MAINWINDOW_H
