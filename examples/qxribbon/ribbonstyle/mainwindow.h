#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonwindow.h"

class MainWindow : public QxRibbon::RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QAction *addStyleAction(QAction *action, int styleId);

private slots:
    void onActionChangeStyleTriggered();
};
#endif   // MAINWINDOW_H
