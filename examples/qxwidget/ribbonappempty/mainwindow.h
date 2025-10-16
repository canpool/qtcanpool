#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxwidget/ribbonappwindow.h"

class MainWindow : public QxWidget::RibbonAppWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif   // MAINWINDOW_H
