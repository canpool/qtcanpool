#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxapp/ribbonappwindow.h"

class MainWindow : public QxApp::RibbonAppWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif   // MAINWINDOW_H
