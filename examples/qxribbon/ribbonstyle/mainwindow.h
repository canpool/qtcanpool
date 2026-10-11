#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

class MainWindow : public QxRibbon::RibbonMainWindow
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
