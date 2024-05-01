#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"

/* MainWindow */
class MainWindow : public RibbonWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = Q_NULL);
    virtual ~MainWindow();
private:
    void createRibbon();
    void createStatusBar();
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif   // MAINWINDOW_H
