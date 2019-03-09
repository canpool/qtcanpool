#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "litewindow.h"

class LiteModeBar;
class LiteTabWidget;
class LiteTabBar;

class QStackedWidget;

class MainWindow : public LiteWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createWindow();

private slots:
    void slotLiteDialog();

private:
    LiteModeBar *m_pModeBar;
    QStackedWidget *m_pStackedWidget;

    LiteTabWidget *m_pTabWidget;
    LiteTabBar *m_pTabBar;
};

#endif // MAINWINDOW_H
