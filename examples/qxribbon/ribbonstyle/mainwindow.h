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
    QAction *addThemeAction(QAction *action, int themeId);

private slots:
    void onActionChangeStyleTriggered();
    void onActionChangeThemeTriggered();
};

#endif   // MAINWINDOW_H
