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
    QAction *addThemeAction(QAction *action, const QString &themeId);

private slots:
    void onActionChangeStyleTriggered();
    void onActionChangeThemeTriggered();
    void onThemeChanged(const QString &id);
};

#endif   // MAINWINDOW_H
